/*
 * X509Cert.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ssl.h"
#include "X509Cert.h"
#include "parse.h"
#include <polarssl/pem.h>

namespace fibjs
{

result_t X509Cert_base::_new(obj_ptr<X509Cert_base> &retVal)
{
    retVal = new X509Cert();
    return 0;
}

X509Cert::X509Cert()
{
    x509_crt_init(&m_crt);
}

X509Cert::~X509Cert()
{
    x509_crt_free(&m_crt);
}

result_t X509Cert::load(Buffer_base *DerCert)
{
    int ret;

    std::string crt;
    DerCert->toString(crt);

    ret = x509_crt_parse_der(&m_crt, (const unsigned char *)crt.c_str(),
                             crt.length());
    if (ret != 0)
        return _ssl::setError(ret);

    return 0;
}

result_t X509Cert::load(const char *txtCert)
{
    int ret;

    if (qstrstr(txtCert, "BEGIN CERTIFICATE"))
    {
        ret = x509_crt_parse(&m_crt, (const unsigned char *)txtCert,
                             qstrlen(txtCert));
        if (ret != 0)
            return _ssl::setError(ret);

        return 0;
    }

    _parser p(txtCert, (int)qstrlen(txtCert));
    bool is_loaded = false;

    while (!p.end())
    {
        std::string cka_value;
        bool in_multiline = false, in_obj = false;
        bool is_cert = false;
        bool is_value = false;

        while (!p.end())
        {
            std::string line;
            std::string cmd, type, value;

            p.getLine(line);
            _parser p1(line);

            p1.skipSpace();
            if (p1.get() == '#')
                continue;

            if (in_multiline)
            {
                if (p1.get() == '\\')
                {
                    if (is_value)
                    {
                        while (p1.get() == '\\')
                        {
                            char ch1, ch2, ch3;

                            p1.skip();

                            ch1 = p1.getChar();
                            if (ch1 < '0' || ch1 > '7')
                                break;

                            ch2 = p1.getChar();
                            if (ch2 < '0' || ch2 > '7')
                                break;

                            ch3 = p1.getChar();
                            if (ch3 < '0' || ch3 > '7')
                                break;

                            ch1 = (ch1 - '0') * 64 + (ch2 - '0') * 8 + (ch3 - '0');
                            cka_value.append(&ch1, 1);
                        }
                    }
                    continue;
                }

                p1.getWord(cmd);
                if (!qstrcmp(cmd.c_str(), "END"))
                    in_multiline = false;

                continue;
            }

            p1.getWord(cmd);

            p1.skipSpace();
            p1.getWord(type);
            if (!qstrcmp(type.c_str(), "MULTILINE_OCTAL"))
            {
                is_value = is_cert && !qstrcmp(cmd.c_str(), "CKA_VALUE");
                if (is_value)
                    cka_value.resize(0);
                in_multiline = true;
                continue;
            }

            p1.skipSpace();
            p1.getLeft(value);

            if (!in_obj)
            {
                if (!qstrcmp(cmd.c_str(), "CKA_CLASS"))
                {
                    in_obj = true;
                    is_cert = !qstrcmp(value.c_str(), "CKO_CERTIFICATE");
                }
                continue;
            }

            if (cmd.empty())
                break;
        }

        if (!cka_value.empty())
        {
            ret = x509_crt_parse_der(&m_crt,
                                     (const unsigned char *)cka_value.c_str(),
                                     cka_value.length());
            if (ret != 0)
                return _ssl::setError(ret);

            is_loaded = true;
        }
    }

    if (!is_loaded)
        return _ssl::setError(POLARSSL_ERR_X509_INVALID_FORMAT);

    return 0;
}

result_t X509Cert::loadFile(const char *filename)
{
    result_t hr;
    std::string data;
    int ret;

    hr = fs_base::ac_readFile(filename, data);
    if (hr < 0)
        return hr;

    if (qstrstr(data.c_str(), "BEGIN CERTIFICATE") ||
            qstrstr(data.c_str(), "CKO_CERTIFICATE"))
        return load(data.c_str());

    ret = x509_crt_parse_der(&m_crt, (const unsigned char *)data.c_str(),
                             data.length());
    if (ret != 0)
        return _ssl::setError(ret);

    return 0;
}

#define PEM_BEGIN_CRT           "-----BEGIN CERTIFICATE-----\n"
#define PEM_END_CRT             "-----END CERTIFICATE-----\n"

result_t X509Cert::dump(v8::Local<v8::Array> &retVal)
{
    retVal = v8::Array::New(isolate);

    const x509_crt *pCert = &m_crt;
    int ret, n = 0;
    std::string buf;
    size_t olen;

    while (pCert)
    {
        if (pCert->raw.len > 0)
        {
            buf.resize(pCert->raw.len * 2 + 64);
            ret = pem_write_buffer(PEM_BEGIN_CRT, PEM_END_CRT,
                                   pCert->raw.p, pCert->raw.len,
                                   (unsigned char *)&buf[0], buf.length(), &olen);
            if (ret != 0)
                return _ssl::setError(ret);

            retVal->Set(n ++, v8::String::NewFromUtf8(isolate, buf.c_str(),
                        v8::String::kNormalString, (int) olen - 1));
        }
        pCert = pCert->next;
    }

    return 0;
}

result_t X509Cert::clear()
{
    x509_crt_free(&m_crt);
    x509_crt_init(&m_crt);
    return 0;
}

}
