/*
 * HttpServer.h
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "ifs/HttpServer.h"
#include "TcpServer.h"
#include "HttpHandler.h"

namespace fibjs {

class HttpServer : public HttpServer_base {
    FIBER_FREE();

public:
    // TcpServer_base
    virtual result_t run(AsyncEvent* ac);
    virtual result_t asyncRun();
    virtual result_t stop(AsyncEvent* ac);
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);
    virtual result_t get_stats(obj_ptr<Stats_base>& retVal);

public:
    // HttpServer_base
    virtual result_t onerror(v8::Local<v8::Object> hdlrs);
    virtual result_t enableCrossOrigin(exlib::string allowHeaders);
    virtual result_t get_maxHeadersCount(int32_t& retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_serverName(exlib::string& retVal);
    virtual result_t set_serverName(exlib::string newVal);
    virtual result_t get_httpStats(obj_ptr<Stats_base>& retVal);

public:
    result_t create(exlib::string addr, int32_t port, Handler_base* hdlr);

private:
    obj_ptr<TcpServer_base> m_server;
    obj_ptr<HttpHandler_base> m_hdlr;
};

} /* namespace fibjs */

#endif /* HTTPSERVER_H_ */
