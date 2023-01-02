/*
 * zmqSocket.h
 *
 *  Created on: May 29, 2012
 *      Author: lion
 */

#include "ifs/zmq_Socket.h"
#include <zmq/zmq.h>

#ifndef ZMQSOCKET_H_
#define ZMQSOCKET_H_

namespace fibjs
{

class zmqSocket : public zmq_Socket_base
{
	EVENT_SUPPORT();

public:
	zmqSocket(int32_t type);
	virtual ~zmqSocket();

public:
	// zmq_Socket_base
	result_t bind(const char* addr);
	result_t connect(const char* addr);
	result_t recv(obj_ptr<Buffer_base>& retVal);
	result_t send(obj_ptr<Buffer_base>& data);
	result_t close();
	result_t get_type(int32_t& retVal);

private:
	void* m_sock;
	int32_t m_type;
};

} /* namespace fibjs */
#endif /* ZMQSOCKET_H_ */
