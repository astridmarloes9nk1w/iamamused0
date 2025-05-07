/*
 * SandBox_root.cpp
 *
 *  Created on: Oct 22, 2012
 *      Author: lion
 */

#include "SandBox.h"

#include "ifs/assert.h"
#include "ifs/path.h"
#include "ifs/os.h"
#include "ifs/coroutine.h"
#include "ifs/encoding.h"

#include "ifs/process.h"

#include "ifs/io.h"
#include "ifs/fs.h"
#include "ifs/net.h"
#include "ifs/zmq.h"
#include "ifs/vm.h"

#include "ifs/mq.h"

#include "ifs/db.h"
#include "ifs/hash.h"
#include "ifs/zlib.h"

#include "ifs/gd.h"

#include "ifs/uuid.h"
#include "ifs/re.h"
#include "ifs/collection.h"

#include "ifs/http.h"

#include "ifs/rpc.h"

namespace fibjs
{

void SandBox::initRoot()
{
	InstallNativeModule("encoding", encoding_base::class_info());
	InstallNativeModule("uuid", uuid_base::class_info());

	InstallNativeModule("re", re_base::class_info());
	InstallNativeModule("collection", collection_base::class_info());

	InstallNativeModule("assert", assert_base::class_info());
	InstallNativeModule("path", path_base::class_info());

	InstallNativeModule("coroutine", coroutine_base::class_info());
	InstallNativeModule("process", process_base::class_info());

	InstallNativeModule("vm", vm_base::class_info());
	InstallNativeModule("io", io_base::class_info());
	InstallNativeModule("fs", fs_base::class_info());
	InstallNativeModule("os", os_base::class_info());
	InstallNativeModule("net", net_base::class_info());
	InstallNativeModule("zmq", zmq_base::class_info());

	InstallNativeModule("mq", mq_base::class_info());

	InstallNativeModule("http", http_base::class_info());
	InstallNativeModule("rpc", rpc_base::class_info());

	InstallNativeModule("db", db_base::class_info());
	InstallNativeModule("hash", hash_base::class_info());
	InstallNativeModule("zlib", zlib_base::class_info());

	InstallNativeModule("gd", gd_base::class_info());
}

}
