/*
 * HttpCookie.cpp
 *
 *  Created on: Aug 23, 2014
 *      Author: lion
 */

#include "HttpCookie.h"
#include "parse.h"
#include "Url.h"

namespace fibjs
{

result_t HttpCookie_base::_new(v8::Local<v8::Object> opts,
                               obj_ptr<HttpCookie_base> &retVal,
                               v8::Local<v8::Object> This)
{
    obj_ptr<HttpCookie> cookie = new HttpCookie();
    cookie->fill(opts, true);

    retVal = cookie;
    return 0;
}

result_t HttpCookie_base::_new(const char *name, const char *value,
                               v8::Local<v8::Object> opts,
                               obj_ptr<HttpCookie_base> &retVal,
                               v8::Local<v8::Object> This)
{
    obj_ptr<HttpCookie> cookie = new HttpCookie();

    cookie->set_name(name);
    cookie->set_value(value);

    cookie->fill(opts, false);

    retVal = cookie;
    return 0;
}

result_t HttpCookie::parse(const char *header)
{
    _parser p(header);
    std::string key, value;
    std::string tmp;

    p.skipSpace();
    p.getWord(tmp, '=');
    if (!p.want('=') || tmp.empty())
        return CHECK_ERROR(Runtime::setError("HttpCookie: bad cookie format."));
    Url::decodeURI(tmp.c_str(), (int32_t)tmp.length(), m_name);

    p.getWord(tmp, ';');
    Url::decodeURI(tmp.c_str(), (int32_t)tmp.length(), m_value);

    while (p.want(';'))
    {
        p.skipSpace();
        p.getWord(key, ';', '=');

        if (p.want('='))
        {
            p.getString(value, ';');

            if (!qstricmp(key.c_str(), "expires"))
                m_expires.parse(value.c_str());
            else if (!qstricmp(key.c_str(), "domain"))
                m_domain = value;
            else if (!qstricmp(key.c_str(), "path"))
                m_path = value;
        }
        else
        {
            if (!qstricmp(key.c_str(), "secure"))
                m_secure = true;
            else if (!qstricmp(key.c_str(), "HttpOnly"))
                m_httpOnly = true;
        }
    }

    return 0;
}

result_t HttpCookie::fill(v8::Local<v8::Object> opts, bool bBase)
{
    if (bBase)
    {
        GetConfigValue(opts, "name", m_name);
        GetConfigValue(opts, "value", m_value);
    }

    GetConfigValue(opts, "expires", m_expires);
    GetConfigValue(opts, "domain", m_domain);
    GetConfigValue(opts, "path", m_path);
    GetConfigValue(opts, "secure", m_secure);
    GetConfigValue(opts, "httpOnly", m_httpOnly);

    return 0;
}

result_t HttpCookie::get_name(std::string &retVal)
{
    retVal = m_name;
    return 0;
}

result_t HttpCookie::set_name(const char *newVal)
{
    m_name = newVal;
    return 0;
}

result_t HttpCookie::get_value(std::string &retVal)
{
    retVal = m_value;
    return 0;
}

result_t HttpCookie::set_value(const char *newVal)
{
    m_value = newVal;
    return 0;
}

result_t HttpCookie::get_domain(std::string &retVal)
{
    retVal = m_domain;
    return 0;
}

result_t HttpCookie::set_domain(const char *newVal)
{
    m_domain = newVal;
    return 0;
}

result_t HttpCookie::get_path(std::string &retVal)
{
    retVal = m_path;
    return 0;
}

result_t HttpCookie::set_path(const char *newVal)
{
    m_path = newVal;
    return 0;
}

result_t HttpCookie::get_expires(date_t &retVal)
{
    retVal = m_expires;
    return 0;
}

result_t HttpCookie::set_expires(date_t newVal)
{
    m_expires = newVal;
    return 0;
}

result_t HttpCookie::get_httpOnly(bool &retVal)
{
    retVal = m_httpOnly;
    return 0;
}

result_t HttpCookie::set_httpOnly(bool newVal)
{
    m_httpOnly = newVal;
    return 0;
}

result_t HttpCookie::get_secure(bool &retVal)
{
    retVal = m_secure;
    return 0;
}

result_t HttpCookie::set_secure(bool newVal)
{
    m_secure = newVal;
    return 0;
}

static const char *CookieNameTable =
    " ! #$%&'()*+ -./0123456789: < >?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";
static const char *CookieTable =
    " ! #$%&'()*+ -./0123456789: <=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[ ]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";

result_t HttpCookie::toString(std::string &retVal)
{
    std::string str;
    std::string tmp;

    Url::encodeURI(m_name.c_str(), (int32_t)m_name.length(), tmp, CookieNameTable);
    str = tmp;
    str += '=';

    Url::encodeURI(m_value.c_str(), (int32_t)m_value.length(), tmp, CookieTable);
    str.append(tmp);

    if (!m_expires.empty())
    {
        m_expires.toGMTString(tmp);

        str.append("; expires=", 10);
        str.append(tmp);
    }

    if (!m_domain.empty())
    {
        str.append("; domain=", 9);
        str.append(m_domain);
    }

    if (!m_path.empty())
    {
        str.append("; path=", 7);
        str.append(m_path);
    }

    if (m_secure)
        str.append("; secure", 8);

    if (m_httpOnly)
        str.append("; HttpOnly", 10);

    retVal = str;
    return 0;
}

}
