/*
 * HttpRequest.h
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "ifs/HttpRequest.h"
#include "HttpCollection.h"

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

namespace fibjs
{

class HttpRequest : public HttpRequest_base
{
public:
	HttpRequest()
	{
		m_headers = new HttpCollection();
	}

	EVENT_SUPPORT();

public:
	// HttpMessage_base
	virtual result_t get_protocol(int32_t& retVal);
	virtual result_t set_protocol(int32_t newVal);
	virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal);
	virtual result_t get_body(obj_ptr<Stream_base>& retVal);
	virtual result_t set_body(obj_ptr<Stream_base>& newVal);
	virtual result_t get_contentType(std::string& retVal);
	virtual result_t set_contentType(const char* newVal);
	virtual result_t get_contentLength(int32_t& retVal);
	virtual result_t set_contentLength(int32_t newVal);
	virtual result_t get_keepAlive(bool& retVal);
	virtual result_t set_keepAlive(bool newVal);
	virtual result_t clear();
	virtual result_t send(obj_ptr<Stream_base>& stm);
	virtual result_t asyncSend(obj_ptr<Stream_base>& stm);
	virtual result_t onsend(v8::Handle<v8::Function> func);
	virtual result_t read(obj_ptr<BufferedStream_base>& stm);
	virtual result_t asyncRead(obj_ptr<BufferedStream_base>& stm);
	virtual result_t onread(v8::Handle<v8::Function> func);

public:
	// HttpRequest_base
	virtual result_t get_method(std::string& retVal);
	virtual result_t set_method(const char* newVal);
	virtual result_t get_address(std::string& retVal);
	virtual result_t set_address(const char* newVal);
	virtual result_t get_queryString(std::string& retVal);
	virtual result_t set_queryString(const char* newVal);
	virtual result_t get_cookies(obj_ptr<HttpCollection_base>& retVal);
	virtual result_t get_form(obj_ptr<HttpCollection_base>& retVal);
	virtual result_t get_query(obj_ptr<HttpCollection_base>& retVal);

private:
	obj_ptr<HttpCollection> m_headers;
};

} /* namespace fibjs */
#endif /* HTTPREQUEST_H_ */
