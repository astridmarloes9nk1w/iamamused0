/*
 * Url.cpp
 *
 *  Created on: Jul 14, 2012
 *      Author: lion
 */

#include "Url.h"

namespace fibjs
{

result_t Url_base::_new(const char* url, obj_ptr<Url_base>& retVal)
{
	obj_ptr<Url> u = new Url();

	result_t hr = u->parse(url);
	if (hr < 0)
		return hr;

	retVal = u;

	return 0;
}

result_t Url_base::_new(v8::Handle<v8::Object> args, obj_ptr<Url_base>& retVal)
{
	obj_ptr<Url> u = new Url();

	result_t hr = u->format(args);
	if (hr < 0)
		return hr;

	retVal = u;

	return 0;
}

void Url::clear()
{
	m_protocol.resize(0);
	m_slashes = true;
	m_defslashes = true;
	m_username.resize(0);
	m_password.resize(0);
	m_hostname.resize(0);
	m_port.resize(0);
	m_pathname.resize(0);
	m_query.resize(0);
	m_hash.resize(0);
	m_ipv6 = false;
}

void Url::parseProtocol(const char*& url)
{
	const char* p = url;
	char ch;

	while ((ch = *p)
			&& (qisascii(ch) || qisnumber(ch) || ch == '.' || ch == '+'
					|| ch == '-'))
		p++;

	if (ch == ':')
	{
		std::string str;

		p++;
		str.assign(url, p - url);
		put_protocol(str);
		url = p;
	}
}

void Url::parseAuth(const char*& url)
{
	const char* p1 = url;
	const char* p2 = NULL;
	char ch;

	while ((ch = *p1)
			&& (ch != '/' && ch != '@' && ch != '?' && ch != '#' && ch != ':'))
		p1++;

	if (ch == ':')
	{
		p2 = p1 + 1;

		while ((ch = *p2) && (ch != '/' && ch != '@' && ch != '?' && ch != '#'))
			p2++;
	}

	if (ch == '@')
	{
		m_username.assign(url, p1 - url);
		if (p2)
			m_password.assign(p1 + 1, p2 - p1 - 1);

		url = p2 ? p2 + 1 : p1 + 1;
	}
}

void Url::parseHost(const char*& url)
{
	const char* p1 = url;
	const char* p2 = NULL;
	char ch;

	if (*p1 == '[')
	{
		p1++;
		while ((ch = *p1) && (qishex(ch) || ch == ':' || ch == '.'))
			p1++;
		if (ch == ']')
			ch = *++p1;
		else
			url++;
	}
	else
	{
		while ((ch = *p1)
				&& (qisascii(ch) || qisnumber(ch) || ch == '.' || ch == '_'
						|| ch == '-'))
			p1++;
	}

	if (ch == ':')
	{
		p2 = p1 + 1;

		while ((ch = *p2) && qisnumber(ch))
			p2++;
	}

	if (*url == '[')
	{
		m_hostname.assign(url + 1, p1 - url - 2);
		m_ipv6 = true;
	}
	else
		m_hostname.assign(url, p1 - url);

	qstrlwr(&m_hostname[0]);
	if (p2)
		m_port.assign(p1 + 1, p2 - p1 - 1);

	url = p2 ? p2 : p1;
}

void Url::parsePath(const char*& url)
{
	const char* p = url;
	char ch;

	while ((ch = *p) && ch != '?' && ch != '#')
		p++;

	if (*url == '/' || m_hostname.length() == 0 || !m_defslashes)
		m_pathname.assign(url, p - url);
	else
	{
		m_pathname.assign(1, '/');
		m_pathname.append(url, p - url);
	}

	url = p;
}

void Url::parseQuery(const char*& url)
{
	if (*url != '?')
		return;

	const char* p = ++url;
	char ch;

	while ((ch = *p) && ch != '#')
		p++;

	m_query.assign(url, p - url);
	url = p;
}

void Url::parseHash(const char*& url)
{
	if (*url != '#')
		return;

	m_hash.assign(url);
}

result_t Url::parse(const char* url)
{
	bool bHost;
	clear();

	parseProtocol(url);

	bHost = !m_slashes;
	if (m_slashes = (*url == '/' && url[1] == '/'))
		url += 2;

	if (!m_protocol.compare("javascript:"))
	{
		m_slashes = false;
		bHost = false;
	}
	else if (m_slashes || bHost)
	{
		parseAuth(url);
		parseHost(url);
	}

	parsePath(url);
	parseQuery(url);
	parseHash(url);

	return 0;
}

std::string getValue(v8::Handle<v8::Object>& args, const char* key)
{
	std::string s;

	v8::Handle<v8::Value> v = args->Get(v8::String::NewSymbol(key));

	if (!v.IsEmpty() && v->IsString())
		s = *v8::String::Utf8Value(v);

	return s;
}

result_t Url::format(v8::Handle<v8::Object> args)
{
	clear();

	put_protocol(getValue(args, "protocol"));
	m_username = getValue(args, "username");
	m_password = getValue(args, "password");
	m_hostname = getValue(args, "hostname");
	m_port = getValue(args, "port");
	m_pathname = getValue(args, "pathname");
	m_query = getValue(args, "query");
	m_hash = getValue(args, "hash");

	if (m_slashes && m_protocol.compare("file:") && m_hostname.length() == 0)
		m_slashes = false;

	v8::Handle<v8::Value> v = args->Get(v8::String::NewSymbol("slashes"));

	if (!v.IsEmpty() && !v->IsUndefined())
		m_slashes = v->BooleanValue();

	m_ipv6 = m_hostname.find(':', 0) != std::string::npos;

	return 0;
}

result_t Url::resolve(const char* to, obj_ptr<Url_base>& retVal)
{
	return 0;
}

result_t Url::normalize()
{
	return 0;
}

result_t Url::toString(std::string& retVal)
{
	return get_href(retVal);
}

result_t Url::get_href(std::string& retVal)
{
	if (m_protocol.length() > 0)
		retVal.append(m_protocol);

	if (m_slashes)
		retVal.append("//", 2);

	if (m_username.length() > 0)
	{
		get_auth(retVal);
		retVal.append(1, '@');
	}

	get_host(retVal);
	get_path(retVal);
	retVal.append(m_hash);

	return 0;
}

void Url::put_protocol(std::string str)
{
	static const char* s_slashed[] =
	{ "http:", "https:", "ftp:", "gopher:", "file:", "http:", "https:", "ftp:",
			"gopher:", "file:" };
	int i;

	m_protocol = str;
	qstrlwr(&m_protocol[0]);
	m_defslashes = false;
	if (m_protocol.length() > 0)
	{
		for (i = 0; i < (int) (sizeof(s_slashed) / sizeof(const char*)); i++)
			if (!m_protocol.compare(s_slashed[i]))
			{
				m_defslashes = true;
				break;
			}
	}
	else
		m_defslashes = true;

	m_slashes = m_defslashes;
}

result_t Url::get_protocol(std::string& retVal)
{
	retVal = m_protocol;
	return 0;
}

result_t Url::get_slashes(int32_t& retVal)
{
	retVal = m_slashes;
	return 0;
}

result_t Url::get_auth(std::string& retVal)
{
	retVal.append(m_username);
	if (m_password.length() > 0)
	{
		retVal.append(1, ':');
		retVal.append(m_password);
	}

	return 0;
}

result_t Url::get_username(std::string& retVal)
{
	retVal = m_username;
	return 0;
}

result_t Url::get_password(std::string& retVal)
{
	retVal = m_password;
	return 0;
}

result_t Url::get_host(std::string& retVal)
{
	if (m_hostname.length())
	{
		if (m_ipv6)
			retVal.append(1, '[');

		retVal.append(m_hostname);

		if (m_ipv6)
			retVal.append(1, ']');

		if (m_port.length() > 0)
		{
			retVal.append(1, ':');
			retVal.append(m_port);
		}
	}

	return 0;
}

result_t Url::get_hostname(std::string& retVal)
{
	retVal = m_hostname;
	return 0;
}

result_t Url::get_port(std::string& retVal)
{
	retVal = m_port;
	return 0;
}

result_t Url::get_path(std::string& retVal)
{
	retVal.append(m_pathname);
	get_search(retVal);

	return 0;
}

result_t Url::get_pathname(std::string& retVal)
{
	retVal = m_pathname;
	return 0;
}

result_t Url::get_search(std::string& retVal)
{
	if (m_query.length() > 0)
	{
		retVal.append(1, '?');
		retVal.append(m_query);
	}

	return 0;
}

result_t Url::get_query(std::string& retVal)
{
	retVal = m_query;
	return 0;
}

result_t Url::get_hash(std::string& retVal)
{
	retVal = m_hash;
	return 0;
}

} /* namespace fibjs */
