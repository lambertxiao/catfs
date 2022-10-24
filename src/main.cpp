#include <iostream>
#include "fs/fuse.h"
#include "fs/fs.h"
#include "fs/fsa.h"
#include "flags/cmdline.h"

using namespace catfs::fs;

static const struct fuse_lowlevel_ops catfs_oper = {
	.init = FuseAdapter::init,
	.destroy = FuseAdapter::destroy,
	.lookup = FuseAdapter::lookup,
	.forget = FuseAdapter::forget,
	.getattr = FuseAdapter::getattr,
	.setattr = FuseAdapter::setattr,
	.readlink = FuseAdapter::readlink,
	.mknod = FuseAdapter::mknod,
	.mkdir = FuseAdapter::mkdir,
	.unlink = FuseAdapter::unlink,
	.rmdir = FuseAdapter::rmdir,
	.symlink = FuseAdapter::symlink,
	.rename = FuseAdapter::rename,
	.link = FuseAdapter::link,
	.open = FuseAdapter::open,
	.read = FuseAdapter::read,
	.write = FuseAdapter::write,
	.flush = FuseAdapter::flush,
	.release = FuseAdapter::release,
	.fsync = FuseAdapter::fsync,
	.opendir = FuseAdapter::opendir,
	.readdir = FuseAdapter::readdir,
	.releasedir = FuseAdapter::releasedir,
	.fsyncdir = FuseAdapter::fsyncdir,
	.statfs	= FuseAdapter::statfs,
	.setxattr = FuseAdapter::setxattr,
	.getxattr = FuseAdapter::getxattr,
	.listxattr = FuseAdapter::listxattr,
	.removexattr = FuseAdapter::removexattr,
	.access = FuseAdapter::access,
	.create = FuseAdapter::create,
	.getlk = FuseAdapter::getlk,
	.setlk = FuseAdapter::setlk,
	.bmap = FuseAdapter::bmap,
	.poll = FuseAdapter::poll,
	.write_buf = FuseAdapter::write_buf,
	.retrieve_reply = FuseAdapter::retrieve_reply,
	.forget_multi = FuseAdapter::forget_multi,
	.flock = FuseAdapter::flock,
	.fallocate = FuseAdapter::fallocate,
	.readdirplus = FuseAdapter::readdirplus,
	.copy_file_range = FuseAdapter::copy_file_range,
	.lseek = FuseAdapter::lseek,
};

int main(int argc, char **argv)
{
	cmdline::parser parm;
	parm.add<std::string>("bucket", 'b', "bucket name", true, "");
	parm.add<std::string>("mountpoint", 'm', "mount point", true, "");
	parm.add("foreground", 'f', "running in foreground");
	parm.add("singlethread", '\0', "singlethread");
	parm.add<u_int>("max_idle_threads", '\0', "max_idle_threads", false, 4);

	parm.parse_check(argc, argv);

	const char* mp = parm.get<std::string>("mountpoint").data();

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_session *se;
	int ret = -1;

  args.argc = 1;
  args.argv = (char**)&mp;

  // if (fuse_parse_cmdline(&args, &opts) != 0)
	// 	return 1;

  se = fuse_session_new(&args, &catfs_oper, sizeof(catfs_oper), NULL);

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
