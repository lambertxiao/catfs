#include <iostream>
#include <memory>
#include <exception>

#include "fmtlog/fmtlog.h"
#include "cmdline/cmdline.h"
#include "yaml-cpp/yaml.h"

#include "fs/fuse.h"
#include "fs/fs.h"
#include "fs/fsa.h"
#include "stor/stor.h"
#include "stor/stor_s3.h"
#include "meta/meta.h"
#include "meta/meta_impl.h"
#include "meta/meta_local_mem.h"

using std::string;
using namespace catfs::fs;

using catfs::stor::S3Stor;
using catfs::stor::Stor;
using catfs::stor::StorOpt;

using catfs::meta::LocalMemMeta;
using catfs::meta::LocalMeta;
using catfs::meta::Meta;
using catfs::meta::MetaImpl;
using catfs::meta::MetaOpt;

void create_log_dir(string log_dir)
{
	auto mkdirs = "mkdir -p " + log_dir;
	auto logfile_path = log_dir + "/catfs.log";
	system(mkdirs.c_str());

	std::ofstream logfile;
	logfile.open(logfile_path.c_str(), std::ios_base::app);
	logfile.close();

	fmtlog::setLogFile(logfile_path.c_str());
}

void init_catfs(cmdline::parser &parm)
{
	auto passwd = parm.get<string>("passwd");
	auto passwd_content = YAML::LoadFile(passwd);
	auto public_key = passwd_content["public_key"].as<string>();
	auto private_key = passwd_content["private_key"].as<string>();
	auto endpoint = passwd_content["endpoint"].as<string>();

	if (public_key == "")
	{
		throw "invalid passwd, public_key is required";
	}
	if (private_key == "")
	{
		throw "invalid passwd, private_key is required";
	}
	if (endpoint == "")
	{
		throw "invalid passwd, endpoint is required";
	}

	auto stor_backend = parm.get<string>("stor_backend");
	auto bucket = parm.get<string>("bucket");
	auto bucket_prefix = parm.get<string>("bucket_prefix");

	auto dcache_timeout = parm.get<uint32_t>("dcache_timeout");
	auto gid = parm.get<uint32_t>("gid");
	auto uid = parm.get<uint32_t>("uid");

	auto stor_opt = StorOpt{
		bucket : bucket,
		public_key : public_key,
		private_key : private_key,
		endpoint : endpoint,
	};

	std::shared_ptr<Stor> stor;
	if (stor_backend == "s3")
	{
		stor = std::make_shared<S3Stor>(stor_opt);
	}
	else if (stor_backend == "us3")
	{
	}
	else
	{
		throw fmt::format("not support storage backend: {}", stor_backend);
	}

	auto meta_opt = MetaOpt{
		gid : gid,
		uid : uid,
		dcache_timeout : dcache_timeout,
		bucket : bucket,
		bucket_prefix : bucket_prefix,
	};

	auto lmeta = std::make_shared<LocalMemMeta>(meta_opt);
	std::shared_ptr<Meta> meta = std::make_shared<MetaImpl>(meta_opt, lmeta, stor);

	auto cfs = new CatFS(meta);
	FuseAdapter::init_catfs(cfs);
};

void set_cmdline(cmdline::parser &parm)
{
	parm.add<string>("bucket", 'b', "bucket name", true, "");
	parm.add<string>("mount", 'm', "mount point", true, "");
	parm.add<string>("bucket_prefix", '\0', "bucket key prefix", false, "");
	parm.add<string>("public_key", '\0', "public key", false, "");
	parm.add<string>("private_key", '\0', "private key", false, "");
	parm.add<string>("endpoint", '\0', "storage backend endpoint", false, "");
	parm.add<string>("stor_backend", '\0', "specified storage backend", false, "s3");
	parm.add<string>("passwd", '\0', "specify access file", false, "/etc/catfs/catfs.conf");

	parm.add<uint32_t>("gid", '\0', "gid", false, 0);
	parm.add<uint32_t>("uid", '\0', "uid", false, 0);
	parm.add<uint32_t>("dcache_timeout", '\0', "dentry cache timeout, unit is seconds", false, 60 * 5);
	parm.add<uint32_t>("retry", '\0', "number of times to retry a failed I/O", false, 3);
	parm.add<uint32_t>("parallel", '\0', "number of parallel I/O thread", false, 32);
	parm.add<string>("level", '\0', "set log level: error/warn/info/debug", false, "info");
	parm.add<string>("log_dir", '\0', "set log dir", false, "");

	parm.add("foreground", 'f', "running in foreground");
	parm.add("singlethread", '\0', "singlethread");
	parm.add<u_int>("max_idle_threads", '\0', "max_idle_threads", false, 4);
}

int main(int argc, char **argv)
{
	try
	{
		fmtlog::flushOn(fmtlog::INF);
		fmtlog::startPollingThread(1);

		logi("start catfs");
		cmdline::parser parm;
		set_cmdline(parm);
		parm.parse_check(argc, argv);

		auto log_dir = parm.get<string>("log_dir");
		if (log_dir != "")
		{
			create_log_dir(log_dir);
		}

		init_catfs(parm);

		auto mount = parm.get<string>("mount");
		const char *mp = mount.data();
		struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
		int ret = -1;

		args.argc = 3;
		args.argv[0] = (char*)mp;
		args.argv[1] = (char*)"-o";
		args.argv[2] = (char*)"auto_unmount";

		struct fuse_session *se = fuse_session_new(&args, &catfs_oper, sizeof(catfs_oper), NULL);

		if (se == NULL)
			goto err_out1;

		if (fuse_set_signal_handlers(se) != 0)
			goto err_out2;

		if (fuse_session_mount(se, mp) != 0)
			goto err_out3;

		fuse_daemonize(parm.exist("foreground"));

		/* Block until ctrl+c or fusermount -u */
		if (parm.exist("singlethread"))
			ret = fuse_session_loop(se);
		else
		{
			struct fuse_loop_config config;
			config.max_idle_threads = parm.get<u_int>("max_idle_threads");
			ret = fuse_session_loop_mt(se, &config);
		}

		fuse_session_unmount(se);
	err_out3:
		fuse_remove_signal_handlers(se);
	err_out2:
		fuse_session_destroy(se);
	err_out1:
		fuse_opt_free_args(&args);

		return ret ? 1 : 0;
	}
	catch (std::exception &e)
	{
		loge("start catfs failed, err: {}", e.what());
		fmtlog::poll();
		return 0;
	}
	return 0;
}
