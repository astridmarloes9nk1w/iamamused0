/*
 * HttpHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/mq.h"

namespace fibjs
{

result_t HttpHandler_base::_new(obj_ptr<Handler_base>& hdlr,
		obj_ptr<HttpHandler_base>& retVal)
{
	retVal = new HttpHandler(hdlr);
	return 0;
}

result_t HttpHandler_base::_new(v8::Handle<v8::Function> hdlr,
		obj_ptr<HttpHandler_base>& retVal)
{
	obj_ptr<Handler_base> hdlr1 = new JSHandler(hdlr);
	retVal = new HttpHandler(hdlr1);
	return 0;
}

result_t HttpHandler::invoke(obj_ptr<object_base>& v,
		obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(HttpHandler* pThis, obj_ptr<Stream_base>& stm,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_stm(stm)
		{
			m_stmBuffered = new BufferedStream(stm);
			m_stmBuffered->set_EOL("\r\n");

			m_req = new HttpRequest();
			m_req->get_response(m_rep);
			m_v = m_req;

			set(read);
		}

		static int read(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->set(invoke);
			return pThis->m_req->readFrom(pThis->m_stmBuffered, pThis);
		}

		static int invoke(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->set(send);
			return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_v, pThis);
		}

		static int send(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->set(read);
			return pThis->m_rep->sendTo(pThis->m_stm, pThis);
		}

	private:
		HttpHandler* m_pThis;
		obj_ptr<Stream_base> m_stm;
		obj_ptr<BufferedStream_base> m_stmBuffered;
		obj_ptr<HttpRequest_base> m_req;
		obj_ptr<HttpResponse_base> m_rep;
		obj_ptr<object_base> m_v;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Stream_base> stm = Stream_base::getInstance(v);

	if (stm == NULL)
		return CALL_E_BADVARTYPE;

	return (new asyncInvoke(this, stm, ac))->post(0);
}

}

