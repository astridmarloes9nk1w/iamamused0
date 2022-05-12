/*
 * Socket.h
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "ifs/Socket.h"

#ifndef SOCKET_H_
#define SOCKET_H_

namespace fibjs
{

class Socket: public Socket_base
{
public:
	union _sockaddr
	{
		struct sockaddr_in addr4;
		struct sockaddr_in6 addr6;
	};

public:
	Socket();
	virtual ~Socket();

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t write(obj_ptr<Buffer_base> data);
	virtual result_t flush();
	virtual result_t close();

public:
	// Socket_base
	virtual result_t get_family(int32_t& retVal);
	virtual result_t get_type(int32_t& retVal);
	virtual result_t connect(const char* addr, int32_t port);
	virtual result_t bind(const char* addr, int32_t port, bool onlyIPv6);
	virtual result_t bind(int32_t port, bool onlyIPv6);
	virtual result_t listen(int32_t backlog);
	virtual result_t accept(obj_ptr<Socket_base>& retVal);
	virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t send(obj_ptr<Buffer_base> data);
	virtual result_t sendto(obj_ptr<Buffer_base> data, const char* host,
			int32_t port);

public:
	result_t create(int32_t family, int32_t type);
	result_t send(const char* p, int sz);

private:
	result_t getAddrInfo(const char* addr, int32_t port, _sockaddr& addr_info);

private:
	SOCKET m_sock;
	int32_t m_family;
	int32_t m_type;
};

}

#endif /* SOCKET_H_ */
