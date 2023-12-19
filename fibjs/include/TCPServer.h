/*
 * TCPServer.h
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "ifs/net.h"
#include "ifs/TCPServer.h"
#include "Socket.h"

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

namespace fibjs
{

class TCPServer: public TCPServer_base
{
public:
	// TCPServer_base
	virtual result_t run(exlib::AsyncEvent* ac);
	virtual result_t asyncRun();
	virtual result_t get_socket(obj_ptr<Socket_base>& retVal);

public:
	result_t create(const char* addr, int32_t port, v8::Handle<v8::Function> listener);
	result_t create(const char* addr, int32_t port, obj_ptr<Handler_base>& listener);

private:
	obj_ptr<Socket_base> m_socket;
	obj_ptr<Handler_base> m_hdlr;
};

} /* namespace fibjs */
#endif /* TCPSERVER_H_ */
