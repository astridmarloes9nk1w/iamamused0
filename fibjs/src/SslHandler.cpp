/*
 * SslHandler.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#include "ifs/mq.h"
#include "SslHandler.h"
#include "JSHandler.h"

namespace fibjs
{

result_t SslHandler_base::_new(v8::Local<v8::Array> certs, v8::Local<v8::Value> hdlr,
                               obj_ptr<SslHandler_base> &retVal)
{
    obj_ptr<SslHandler> sslHdlr = new SslHandler();

    result_t hr = sslHdlr->init(certs, hdlr);
    if (hr < 0)
        return hr;

    retVal = sslHdlr;
    return 0;
}

result_t SslHandler_base::_new(X509Cert_base *crt, PKey_base *key,
                               v8::Local<v8::Value> hdlr, obj_ptr<SslHandler_base> &retVal)
{
    obj_ptr<SslHandler> sslHdlr = new SslHandler();

    result_t hr = sslHdlr->init(crt, key, hdlr);
    if (hr < 0)
        return hr;

    retVal = sslHdlr;
    return 0;
}

result_t SslHandler::init(v8::Local<v8::Array> certs, v8::Local<v8::Value> hdlr)
{
    result_t hr;

    hr = JSHandler::New(hdlr, m_hdlr);
    if (hr < 0)
        return hr;

    hr = SslSocket_base::_new(certs, m_socket);
    if (hr < 0)
        return hr;

    m_socket->set_verification(ssl_base::_VERIFY_OPTIONAL);
    return 0;
}
result_t SslHandler::init(X509Cert_base *crt, PKey_base *key, v8::Local<v8::Value> hdlr)
{
    result_t hr;

    hr = JSHandler::New(hdlr, m_hdlr);
    if (hr < 0)
        return hr;

    hr = SslSocket_base::_new(crt, key, m_socket);
    if (hr < 0)
        return hr;

    m_socket->set_verification(ssl_base::_VERIFY_OPTIONAL);
    return 0;
}

result_t SslHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            exlib::AsyncEvent *ac)
{
    class asyncInvoke: public asyncState
    {
    public:
        asyncInvoke(SslHandler *pThis, Stream_base *stm, exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_stm(stm)
        {
            set(accept);
        }

        static int accept(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->set(invoke);
            return pThis->m_pThis->m_socket->accept(pThis->m_stm, pThis->m_socket, pThis);
        }

        static int invoke(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->set(close);
            return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_socket, pThis);
        }

        static int close(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->set(exit);
            return pThis->m_socket->close(pThis);
        }

        static int exit(asyncState *pState, int n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;
            return pThis->done(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<SslHandler> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<SslSocket_base> m_socket;
    };

    if (!ac)
        return CALL_E_NOSYNC;

    obj_ptr<Stream_base> stm = Stream_base::getInstance(v);
    if (stm == NULL)
        return CALL_E_BADVARTYPE;

    return (new asyncInvoke(this, stm, ac))->post(0);
}

result_t SslHandler::get_verification(int32_t &retVal)
{
    return m_socket->get_verification(retVal);
}

result_t SslHandler::set_verification(int32_t newVal)
{
    return m_socket->set_verification(newVal);
}

result_t SslHandler::get_ca(obj_ptr<X509Cert_base> &retVal)
{
    return m_socket->get_ca(retVal);
}

}

