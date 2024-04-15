/*
 * HttpRequest.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: lion
 */

#include "HttpRequest.h"
#include "parse.h"
#include "HttpUploadCollection.h"
#include "Url.h"

namespace fibjs
{

result_t HttpRequest_base::_new(obj_ptr<HttpRequest_base>& retVal)
{
	retVal = new HttpRequest();
	return 0;
}

result_t HttpRequest::get_protocol(std::string& retVal)
{
	return m_message.get_protocol(retVal);
}

result_t HttpRequest::set_protocol(const char* newVal)
{
	return m_message.set_protocol(newVal);
}

result_t HttpRequest::get_headers(obj_ptr<HttpCollection_base>& retVal)
{
	return m_message.get_headers(retVal);
}

result_t HttpRequest::get_body(obj_ptr<SeekableStream_base>& retVal)
{
	return m_message.get_body(retVal);
}

result_t HttpRequest::set_body(SeekableStream_base* newVal)
{
	return m_message.set_body(newVal);
}

result_t HttpRequest::get_contentType(std::string& retVal)
{
	return m_message.get_contentType(retVal);
}

result_t HttpRequest::set_contentType(const char* newVal)
{
	return m_message.set_contentType(newVal);
}

result_t HttpRequest::get_contentLength(int64_t& retVal)
{
	return m_message.get_contentLength(retVal);
}

result_t HttpRequest::get_keepAlive(bool& retVal)
{
	return m_message.get_keepAlive(retVal);
}

result_t HttpRequest::set_keepAlive(bool newVal)
{
	return m_message.set_keepAlive(newVal);
}

result_t HttpRequest::get_value(std::string& retVal)
{
	retVal = m_address;
	return 0;
}

result_t HttpRequest::set_value(const char* newVal)
{
	m_address = newVal;
	return 0;
}

result_t HttpRequest::clear()
{
	m_message.clear();
	m_method.assign("GET", 3);
	m_address.assign("/", 1);
	m_queryString.clear();

	m_response->clear();

	m_cookie.Release();
	m_query.Release();
	m_form.Release();

	return 0;
}

result_t HttpRequest::sendTo(Stream_base* stm, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	std::string strCommand = m_method;
	std::string strProtocol;

	strCommand += ' ';
	strCommand.append(m_address);
	if (!m_queryString.empty())
	{
		strCommand += '?';
		strCommand.append(m_queryString);
	}

	get_protocol(strProtocol);
	strCommand += ' ';
	strCommand.append(strProtocol);

	return m_message.sendTo(stm, strCommand, ac);
}

result_t HttpRequest::asyncSendTo(Stream_base* stm)
{
	acb_sendTo(stm);
	return 0;
}

result_t HttpRequest::onsendto(v8::Handle<v8::Function> func)
{
	return on("sendto", func);
}

result_t HttpRequest::readFrom(BufferedStream_base* stm, exlib::AsyncEvent* ac)
{
	class asyncReadFrom: public asyncState
	{
	public:
		asyncReadFrom(HttpRequest* pThis, BufferedStream_base* stm,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_stm(stm)
		{
			m_pThis->clear();
			set(begin);
		}

		static int begin(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;

			pThis->set(command);
			return pThis->m_stm->readLine(pThis->m_strLine, pThis);
		}

		static int command(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;
			result_t hr;

			int p1, p2;
			_parser p(pThis->m_strLine);

			p1 = p.pos;
			p.skipWord();
			p2 = p.pos;
			if (p1 == p2)
				return CALL_E_INVALID_DATA;

			pThis->m_pThis->m_method.assign(p.string + p1, p2 - p1);

			p.skipSpace();

			p1 = p.pos;
			p.skipWord('?');
			p2 = p.pos;
			if (p1 == p2)
				return CALL_E_INVALID_DATA;

			pThis->m_pThis->m_address.assign(p.string + p1, p2 - p1);

			if (p.get() == '?')
			{
				p.skip();

				p1 = p.pos;
				p.skipWord();
				p2 = p.pos;
				pThis->m_pThis->m_queryString.assign(p.string + p1, p2 - p1);
			}

			p.skipSpace();

			if (p.end())
				return CALL_E_INVALID_DATA;

			hr = pThis->m_pThis->set_protocol(p.string + p.pos);
			if (hr < 0)
				return hr;

			pThis->set(ok);
			return pThis->m_pThis->m_message.readFrom(pThis->m_stm, pThis);
		}

		static int ok(asyncState* pState, int n)
		{
			asyncReadFrom* pThis = (asyncReadFrom*) pState;

			std::string strProtocol;

			pThis->m_pThis->get_protocol(strProtocol);
			pThis->m_pThis->m_response->set_protocol(strProtocol.c_str());

			bool bKeepAlive;

			pThis->m_pThis->get_keepAlive(bKeepAlive);
			pThis->m_pThis->m_response->set_keepAlive(bKeepAlive);

			return pThis->done(0);
		}

	public:
		obj_ptr<HttpRequest> m_pThis;
		obj_ptr<BufferedStream_base> m_stm;
		std::string m_strLine;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncReadFrom(this, stm, ac))->post(0);
}

result_t HttpRequest::asyncReadFrom(BufferedStream_base* stm)
{
	acb_readFrom(stm);
	return 0;
}

result_t HttpRequest::onreadfrom(v8::Handle<v8::Function> func)
{
	return on("readfrom", func);
}

result_t HttpRequest::get_method(std::string& retVal)
{
	retVal = m_method;
	return 0;
}

result_t HttpRequest::set_method(const char* newVal)
{
	m_method = newVal;
	return 0;
}

result_t HttpRequest::get_address(std::string& retVal)
{
	retVal = m_address;
	return 0;
}

result_t HttpRequest::set_address(const char* newVal)
{
	m_address = newVal;
	return 0;
}

result_t HttpRequest::get_queryString(std::string& retVal)
{
	retVal = m_queryString;
	return 0;
}

result_t HttpRequest::set_queryString(const char* newVal)
{
	m_queryString = newVal;
	return 0;
}

result_t HttpRequest::get_response(obj_ptr<HttpResponse_base>& retVal)
{
	retVal = m_response;
	return 0;
}

void HttpRequest::parse(std::string& str, char split,
		obj_ptr<HttpCollection_base>& retVal)
{
	obj_ptr<HttpCollection> c = new HttpCollection();

	const char* pstr = str.c_str();
	int nSize = (int) str.length();
	const char* pstrTemp;
	std::string strKey, strValue;

	while (nSize)
	{
		while (nSize && *pstr == ' ')
		{
			pstr++;
			nSize--;
		}

		pstrTemp = pstr;
		while (nSize && *pstr != '=' && *pstr != split)
		{
			pstr++;
			nSize--;
		}

		if (pstr > pstrTemp)
			Url::decodeURI(pstrTemp, (int) (pstr - pstrTemp), strKey);
		else
			strKey.clear();

		if (nSize && *pstr == '=')
		{
			nSize--;
			pstr++;
		}

		pstrTemp = pstr;
		while (nSize && *pstr != split)
		{
			pstr++;
			nSize--;
		}

		if (!strKey.empty())
			if (pstr > pstrTemp)
				Url::decodeURI(pstrTemp, (int) (pstr - pstrTemp), strValue);
			else
				strValue.clear();

		if (nSize)
		{
			nSize--;
			pstr++;
		}

		if (!strKey.empty())
			c->add(strKey, strValue);
	}

	retVal = c;
}

result_t HttpRequest::get_cookie(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_cookie == NULL)
	{
		std::string strCookie;

		header("cookie", strCookie);
		parse(strCookie, ';', m_cookie);
	}

	retVal = m_cookie;
	return 0;
}

result_t HttpRequest::get_form(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_form == NULL)
	{
		int64_t len = 0;

		get_contentLength(len);
		if (len == 0)
			m_form = new HttpCollection();
		else
		{
			std::string strType;
			bool bUpload = false;

			get_contentType(strType);

			if (!qstricmp(strType.c_str(), "multipart/form-data;", 20))
				bUpload = true;
			else if (qstricmp(strType.c_str(),
					"application/x-www-form-urlencoded"))
				return CALL_E_INVALID_DATA;

			obj_ptr<Buffer_base> buf;
			obj_ptr<SeekableStream_base> _body;

			get_body(_body);
			_body->rewind();
			result_t hr = _body->read((int32_t) len, buf, NULL);
			if (hr < 0)
				return hr;

			std::string strForm;
			buf->toString(strForm);

			if (bUpload)
			{
				obj_ptr<HttpUploadCollection> col = new HttpUploadCollection();
				col->parse(strForm, strType.c_str());
				m_form = col;
			}
			else
				parse(strForm, '&', m_form);
		}
	}

	retVal = m_form;

	return 0;
}

result_t HttpRequest::get_query(obj_ptr<HttpCollection_base>& retVal)
{
	if (m_query == NULL)
		parse(m_queryString, '&', m_query);

	retVal = m_query;
	return 0;
}

} /* namespace fibjs */
