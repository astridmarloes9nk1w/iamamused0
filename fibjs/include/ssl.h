/*
 * ssl.h
 *
 *  Created on: Apr 17, 2014
 *      Author: lion
 */

#ifndef _fj_SSL_H_
#define _fj_SSL_H_

#include "ifs/ssl.h"
#include "polarssl/config.h"
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"
#include "polarssl/ssl.h"
#include "polarssl/ssl_cache.h"
#include "X509Cert.h"

namespace fibjs
{

class _ssl
{
public:
    _ssl()
    {
        entropy_init(&entropy);
        ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                      (const unsigned char *) "fibjs", 5);

        ssl_cache_init(&m_cache);

        m_ca = new X509Cert();
        m_authmode = ssl_base::_VERIFY_REQUIRED;
    }

    ~_ssl()
    {
        ssl_cache_free(&m_cache);

        entropy_free(&entropy);
    }

public:
    static result_t setError(int ret);

public:
    ssl_cache_context m_cache;
    entropy_context entropy;
    ctr_drbg_context ctr_drbg;
    obj_ptr<X509Cert> m_ca;
    int32_t m_authmode;
};

extern _ssl g_ssl;

} /* namespace fibjs */

#endif /* _fj_SSL_H_ */
