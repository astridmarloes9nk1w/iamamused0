/*
 * Redis.cpp
 *
 *  Created on: Aug 2, 2014
 *      Author: lion
 */

#include "Redis.h"
#include "ifs/db.h"
#include "Url.h"
#include "Buffer.h"
#include "List.h"

namespace fibjs
{

result_t db_base::openRedis(const char *connString,
                            obj_ptr<Redis_base> &retVal, exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    std::string host;
    int nPort = 6379;

    if (!qstrcmp(connString, "redis:", 6))
    {
        obj_ptr<Url> u = new Url();

        result_t hr = u->parse(connString);
        if (hr < 0)
            return hr;

        host = u->m_hostname;
        connString = host.c_str();

        if (u->m_port.length() > 0)
            nPort = atoi(u->m_port.c_str());
    }

    obj_ptr<Redis> conn = new Redis();
    retVal = conn;

    return conn->connect(connString, nPort, ac);
}

result_t Redis::connect(const char *host, int port, exlib::AsyncEvent *ac)
{
    result_t hr;

    hr = Socket_base::_new(net_base::_AF_INET, net_base::_SOCK_STREAM, m_sock);
    if (hr < 0)
        return hr;

    m_stmBuffered = new BufferedStream(m_sock);
    m_stmBuffered->set_EOL("\r\n");

    return m_sock->connect(host, port, ac);
}

#define REDIS_MAX_LINE 1024
result_t Redis::_command(std::string &req, Variant &retVal, exlib::AsyncEvent *ac)
{
    class asyncCommand: public asyncState
    {
    public:
        asyncCommand(Redis *pThis, std::string &req, Variant &retVal, exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_req(req), m_retVal(retVal)
        {
            m_stmBuffered = pThis->m_stmBuffered;
            set(send);
        }

        static int send(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;

            pThis->m_buffer = new Buffer(pThis->m_req);

            pThis->set(read);
            return pThis->m_stmBuffered->write(pThis->m_buffer, pThis);
        }

        static int read(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;

            pThis->set(read_ok);
            return pThis->m_stmBuffered->readLine(REDIS_MAX_LINE, pThis->m_strLine, pThis);
        }

        int setResult(int hr = 0)
        {
            if (m_list)
            {
                m_list->push(m_retVal);
                m_count --;
                if (m_count)
                {
                    m_retVal.clear();
                    set(read);
                    return 0;
                }

                m_retVal = m_list;
                return done();
            }

            return done(hr);
        }

        static int read_ok(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;
            char ch = pThis->m_strLine[0];

            if (ch == '+')
            {
                pThis->m_retVal = pThis->m_strLine.substr(1);
                return pThis->setResult();
            }

            if (ch == '-')
                return Runtime::setError(pThis->m_strLine.substr(1));

            if (ch == ':')
            {
                pThis->m_retVal = atoi(pThis->m_strLine.c_str() + 1);
                return pThis->setResult();
            }

            if (ch == '$')
            {
                int32_t sz = atoi(pThis->m_strLine.c_str() + 1);

                if (sz < 0)
                    return pThis->setResult(CALL_RETURN_NULL);

                pThis->set(bulk_ok);
                pThis->m_buffer.Release();

                return pThis->m_stmBuffered->read(sz + 2, pThis->m_buffer, pThis);
            }

            if (ch == '*' && !pThis->m_list)
            {
                int32_t sz = atoi(pThis->m_strLine.c_str() + 1);

                if (sz < 0)
                    return pThis->done(CALL_RETURN_NULL);

                pThis->m_list = new List();

                if (sz == 0)
                {
                    pThis->m_retVal = pThis->m_list;
                    return pThis->done();
                }

                pThis->m_count = sz;
                pThis->set(read);
                return 0;
            }

            return Runtime::setError("Invalid response.");
        }

        static int bulk_ok(asyncState *pState, int n)
        {
            asyncCommand *pThis = (asyncCommand *) pState;

            if (n == CALL_RETURN_NULL)
                return pThis->setResult(CALL_RETURN_NULL);

            std::string str;

            pThis->m_buffer->toString(str);
            str.resize(str.length() - 2);
            pThis->m_retVal = str;
            return pThis->setResult();
        }

    protected:
        obj_ptr<Redis> m_pThis;
        std::string &m_req;
        Variant &m_retVal;
        obj_ptr<BufferedStream_base> m_stmBuffered;
        obj_ptr<Buffer_base> m_buffer;
        obj_ptr<List_base> m_list;
        int32_t m_count;
        std::string m_strLine;
    };

    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncCommand(this, req, retVal, ac))->post(0);
}

result_t Redis::command(const char *cmd, const v8::FunctionCallbackInfo<v8::Value> &args,
                        v8::Local<v8::Value> &retVal)
{
    result_t hr;
    _param ps;
    Variant v;

    hr = ps.add(cmd);
    if (hr < 0)
        return hr;

    hr = ps.add(args, 1);
    if (hr < 0)
        return hr;

    hr = ac__command(ps.str(), v);
    if (hr < 0)
        return hr;

    retVal = v;

    return 0;
}

result_t Redis::close(exlib::AsyncEvent *ac)
{
    if (!m_sock)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_sock.Release();
    m_stmBuffered.Release();

    return 0;
}

}
