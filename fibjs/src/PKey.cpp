/*
 * PKey.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "PKey.h"
#include "Cipher.h"
#include "Buffer.h"
#include "polarssl/ecdsa.h"
#include "polarssl/rsa.h"
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"
#include "polarssl/ssl.h"

namespace fibjs
{

result_t PKey_base::_new(obj_ptr<PKey_base> &retVal)
{
    retVal = new PKey();
    return 0;
}

PKey::PKey()
{
    pk_init(&m_key);
}

PKey::~PKey()
{
    pk_free(&m_key);
}

void PKey::clear()
{
    if (pk_get_type(&m_key))
    {
        pk_free(&m_key);
        pk_init(&m_key);
    }
}

class _entropy
{
public:
    _entropy()
    {
        entropy_init(&entropy);
        ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                      (const unsigned char *) "fibjs", 5);
    }

    ~_entropy()
    {
        entropy_free(&entropy);
    }

public:
    entropy_context entropy;
    ctr_drbg_context ctr_drbg;
};

result_t PKey::genRsaKey(int32_t size, exlib::AsyncEvent *ac)
{
    if (size < 128 || size > 8192)
        return Runtime::setError("Invalid key size");

    if (!ac)
        return CALL_E_NOSYNC;

    _entropy entropy;
    int ret;

    clear();

    ret = pk_init_ctx(&m_key, pk_info_from_type(POLARSSL_PK_RSA));
    if (ret != 0)
        return Cipher::setError(ret);

    ret = rsa_gen_key(pk_rsa(m_key), ctr_drbg_random, &entropy.ctr_drbg,
                      size, 65537);

    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

result_t PKey::genEcKey(const char *curve, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    const ecp_curve_info *curve_info;
    curve_info = ecp_curve_info_from_name(curve);
    if (curve_info == NULL)
        return Runtime::setError("Unknown curve");

    _entropy entropy;
    int ret;

    clear();

    ret = pk_init_ctx(&m_key, pk_info_from_type(POLARSSL_PK_ECKEY));
    if (ret != 0)
        return Cipher::setError(ret);

    ret = ecp_gen_key(curve_info->grp_id, pk_ec(m_key),
                      ctr_drbg_random, &entropy.ctr_drbg);

    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

result_t PKey::isPrivate(bool &retVal)
{
    pk_type_t type = pk_get_type(&m_key);

    if (type == POLARSSL_PK_RSA)
    {
        retVal = rsa_check_privkey(pk_rsa(m_key)) == 0;
        return 0;
    }

    if (type == POLARSSL_PK_ECKEY)
    {
        ecp_keypair *ecp = pk_ec(m_key);
        retVal = ecp_check_privkey(&ecp->grp, &ecp->d) == 0;
        return 0;
    }

    return CALL_E_INVALID_CALL;
}

result_t PKey::publicKey(obj_ptr<PKey_base> &retVal)
{
    pk_type_t type = pk_get_type(&m_key);
    int ret;

    if (type == POLARSSL_PK_RSA)
    {
        rsa_context *rsa = pk_rsa(m_key);
        obj_ptr<PKey> pk1 = new PKey();

        ret = pk_init_ctx(&pk1->m_key, pk_info_from_type(POLARSSL_PK_RSA));
        if (ret != 0)
            return Cipher::setError(ret);

        rsa_context *rsa1 = pk_rsa(pk1->m_key);

        rsa1->len = rsa->len;
        rsa1->padding = rsa->padding;
        rsa1->hash_id = rsa->hash_id;

        ret = mpi_copy(&rsa1->N, &rsa->N);
        if (ret != 0)
            return Cipher::setError(ret);

        ret = mpi_copy(&rsa1->E, &rsa->E);
        if (ret != 0)
            return Cipher::setError(ret);

        retVal = pk1;

        return 0;
    }

    if (type == POLARSSL_PK_ECKEY)
    {
        ecp_keypair *ecp = pk_ec(m_key);

        obj_ptr<PKey> pk1 = new PKey();

        ret = pk_init_ctx(&pk1->m_key, pk_info_from_type(POLARSSL_PK_ECKEY));
        if (ret != 0)
            return Cipher::setError(ret);

        ecp_keypair *ecp1 = pk_ec(pk1->m_key);

        ret = ecp_group_copy(&ecp1->grp, &ecp->grp);
        if (ret != 0)
            return Cipher::setError(ret);

        ret = ecp_copy(&ecp1->Q, &ecp->Q);
        if (ret != 0)
            return Cipher::setError(ret);

        retVal = pk1;

        return 0;
    }

    return CALL_E_INVALID_CALL;
}

result_t PKey::import(Buffer_base *DerKey, const char *password)
{
    int ret;

    std::string key;
    DerKey->toString(key);

    clear();

    ret = pk_parse_key(&m_key, (unsigned char *)key.c_str(), key.length(),
                       *password ? (unsigned char *)password : NULL,
                       qstrlen(password));

    if (ret == POLARSSL_ERR_PK_KEY_INVALID_FORMAT)
        ret = pk_parse_public_key(&m_key, (unsigned char *)key.c_str(), key.length());

    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

result_t PKey::import(const char *pemKey, const char *password)
{
    int ret;

    clear();

    ret = pk_parse_key(&m_key, (unsigned char *)pemKey, qstrlen(pemKey),
                       *password ? (unsigned char *)password : NULL,
                       qstrlen(password));

    if (ret == POLARSSL_ERR_PK_KEY_INVALID_FORMAT)
        ret = pk_parse_public_key(&m_key, (unsigned char *)pemKey, qstrlen(pemKey));

    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

result_t PKey::exportPem(std::string &retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int ret;
    std::string buf;

    buf.resize(pk_get_len(&m_key) * 8 + 128);
    if (priv)
        ret = pk_write_key_pem(&m_key, (unsigned char *)&buf[0], buf.length());
    else
        ret = pk_write_pubkey_pem(&m_key, (unsigned char *)&buf[0], buf.length());
    if (ret != 0)
        return Cipher::setError(ret);

    buf.resize(qstrlen(buf.c_str()));
    retVal = buf;

    return 0;
}

result_t PKey::exportDer(obj_ptr<Buffer_base> &retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int ret;
    std::string buf;

    buf.resize(8192);
    if (priv)
        ret = pk_write_key_der(&m_key, (unsigned char *)&buf[0], buf.length());
    else
        ret = pk_write_pubkey_der(&m_key, (unsigned char *)&buf[0], buf.length());
    if (ret < 0)
        return Cipher::setError(ret);

    retVal = new Buffer(buf.substr(buf.length() - ret));

    return 0;
}

result_t PKey::encrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                       exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    int ret;
    _entropy entropy;
    std::string str;
    std::string output;
    size_t olen;

    data->toString(str);
    output.resize(POLARSSL_PREMASTER_SIZE);

    ret = pk_encrypt(&m_key, (const unsigned char *)str.c_str(), str.length(),
                     (unsigned char *)&output[0], &olen, output.length(),
                     ctr_drbg_random, &entropy.ctr_drbg);

    if (ret != 0)
        return Cipher::setError(ret);

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::decrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                       exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CALL_E_INVALID_CALL;

    int ret;
    _entropy entropy;
    std::string str;
    std::string output;
    size_t olen;

    data->toString(str);
    output.resize(POLARSSL_PREMASTER_SIZE * 2);

    ret = pk_decrypt(&m_key, (const unsigned char *)str.c_str(), str.length(),
                     (unsigned char *)&output[0], &olen, output.length(),
                     ctr_drbg_random, &entropy.ctr_drbg);

    if (ret != 0)
        return Cipher::setError(ret);

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::toString(std::string &retVal)
{
    return exportPem(retVal);
}

}
