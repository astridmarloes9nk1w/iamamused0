/*
 * X509Cert.h
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "ifs/X509Cert.h"
#include <polarssl/x509_crt.h>

#ifndef _fj_X509CERT_H
#define _fj_X509CERT_H

namespace fibjs
{

class X509Cert : public X509Cert_base
{
public:
    X509Cert();
    X509Cert(X509Cert *root, int no);
    ~X509Cert();

public:
    // X509Cert_base
    virtual result_t load(Buffer_base *derCert);
    virtual result_t load(const char *txtCert);
    virtual result_t loadFile(const char *filename);
    virtual result_t dump(v8::Local<v8::Array> &retVal);
    virtual result_t clear();
    virtual result_t get_issuer(std::string &retVal);
    virtual result_t get_subject(std::string &retVal);
    virtual result_t get_notBefore(date_t &retVal);
    virtual result_t get_notAfter(date_t &retVal);
    virtual result_t get_publicKey(obj_ptr<PKey_base> &retVal);
    virtual result_t get_next(obj_ptr<X509Cert_base> &retVal);

private:
    x509_crt *get_crt();

private:
    x509_crt m_crt;
    obj_ptr<X509Cert> m_root;
    int m_no;
};

}

#endif
