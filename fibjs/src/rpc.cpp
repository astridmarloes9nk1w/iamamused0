/*
 * rpc.cpp
 *
 *  Created on: Sep 3, 2012
 *      Author: lion
 */

#include "ifs/rpc.h"
#include "JsonRpcHandler.h"
#include "JSHandler.h"

namespace fibjs
{

result_t rpc_base::json(v8::Handle<v8::Value> hdlr,
                        obj_ptr<Handler_base> &retVal)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;

    retVal = new JsonRpcHandler(hdlr1);
    return 0;
}

}
