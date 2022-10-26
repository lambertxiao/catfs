#include <iostream>
#include <memory>
#include "fs/fuse.h"
#include "fs/fs.h"
#include "fs/fsa.h"
#include "stor/stor.h"
#include "stor/stor_s3.h"
#include "cmdline/cmdline.h"
#include "fmtlog/fmtlog.h"
#include "meta/meta.h"
#include "meta/meta_impl.h"

using std::string;
using namespace catfs::fs;

using catfs::stor::Stor;
using catfs::stor::S3Stor;
using catfs::stor::StorOpt;

using catfs::meta::Meta;
using catfs::meta::MetaImpl;
using catfs::meta::MetaOpt;
using catfs::meta::LocalMeta;

void init_catfs(cmdline::parser parm);

int main(int argc, char **argv)
{
	cmdline::parser parm;
	parm.add<string>("bucket", 'b', "bucket name", true, "");
	parm.add<string>("bucket_prefix", '\0', "bucket key prefix", true, "");
	parm.add<string>("public_key", '\0', "public key", true, "");
	parm.add<string>("private_key", '\0', "private key", true, "");
	parm.add<string>("mountpoint", 'm', "mount point", true, "");
	parm.add<string>("endpoint", '\0', "storage backend endpoint", true, "");
	parm.add<string>("stor_backend", '\0', "specified storage backend", false, "s3");
	parm.add<uint32_t>("gid", '\0', "gid", false, 0);
	parm.add<uint32_t>("uid", '\0', "uid", false, 0);
	parm.add<uint32_t>("dcache_timeout", '\0', "dentry cache timeout, unit is seconds", false, 60*5);

	parm.add("foreground", 'f', "running in foreground");
	parm.add("singlethread", '\0', "singlethread");
	parm.add<u_int>("max_idle_threads", '\0', "max_idle_threads", false, 4);

	parm.parse_check(argc, argv);

	init_catfs(parm);

	const char* mp = parm.get<string>("mountpoint").data();
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	int ret = -1;

  args.argc = 1;
  args.argv = (char**)&mp;

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
	else {
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

void init_catfs(cmdline::parser parm) {
	auto sto_backend = parm.get<string>("sto_backend");
	auto bucket = parm.get<string>("bucket");
	auto bucket_prefix = parm.get<string>("bucket_prefix");
	auto public_key = parm.get<string>("public_key");
	auto private_key = parm.get<string>("private_key");
	auto endpoint = parm.get<string>("endpoint");

	auto dcache_timeout = parm.get<uint32_t>("dcache_timeout");
	auto gid = parm.get<uint32_t>("gid");
	auto uid = parm.get<uint32_t>("uid");
	
	auto stor_opt = StorOpt{
		bucket: bucket,
		public_key: public_key,
		private_key: private_key,
		endpoint: endpoint,
	};

	std::shared_ptr<Stor> stor;
	if (sto_backend == "s3") {
		stor = std::make_shared<S3Stor>(stor_opt);
	} else {
		throw fmt::format("not support storage backend: {}", sto_backend);
	}

	auto meta_opt = MetaOpt{
		gid: gid,
		uid: uid,
		dcache_timeout: dcache_timeout,
		bucket: bucket,
		bucket_prefix: bucket_prefix,
	};

	auto lmeta = std::make_shared<LocalMeta>(meta_opt);
	std::shared_ptr<Meta> meta = std::make_shared<MetaImpl>(meta_opt, lmeta, stor);

	auto cfs = new CatFS(meta);
	FuseAdapter::init_catfs(cfs);
};
