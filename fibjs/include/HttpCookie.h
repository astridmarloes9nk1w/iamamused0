/*
 * HttpCookie.h
 *
 *  Created on: Aug 23, 2014
 *      Author: lion
 */

#include "ifs/HttpCookie.h"

#ifndef HTTPCOOKIE_H_
#define HTTPCOOKIE_H_

namespace fibjs
{

class HttpCookie: public HttpCookie_base
{
public:
    HttpCookie() : m_httpOnly(false), m_secure(false)
    {}

public:
    // HttpCookie_base
    virtual result_t parse(const char *header);
    virtual result_t match(const char *url, bool &retVal);
    virtual result_t get_name(std::string &retVal);
    virtual result_t set_name(const char *newVal);
    virtual result_t get_value(std::string &retVal);
    virtual result_t set_value(const char *newVal);
    virtual result_t get_domain(std::string &retVal);
    virtual result_t set_domain(const char *newVal);
    virtual result_t get_path(std::string &retVal);
    virtual result_t set_path(const char *newVal);
    virtual result_t get_expires(date_t &retVal);
    virtual result_t set_expires(date_t newVal);
    virtual result_t get_httpOnly(bool &retVal);
    virtual result_t set_httpOnly(bool newVal);
    virtual result_t get_secure(bool &retVal);
    virtual result_t set_secure(bool newVal);

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    result_t fill(v8::Local<v8::Object> opts, bool bBase);

private:
    std::string m_name;
    std::string m_value;
    std::string m_domain;
    std::string m_path;
    date_t m_expires;
    bool m_httpOnly;
    bool m_secure;
};

} /* namespace fibjs */
#endif /* HTTPCOOKIE_H_ */
