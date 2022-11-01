#define ASYNC_STATIC0(cls, m) \
	static result_t ac_##m(AsyncQueue& q) { \
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	return AsyncCall(q, NULL, _t::_stub).wait();}

#define ASYNC_MEMBER0(cls, m) \
	result_t ac_##m(AsyncQueue& q) { \
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[0])->m( \
				ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC1(cls, m) \
template<typename T0> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER1(cls, m) \
template<typename T0> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[1])->m( \
				*(T0*) ac->args[0], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC2(cls, m) \
template<typename T0, typename T1> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER2(cls, m) \
template<typename T0, typename T1> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[2])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC3(cls, m) \
template<typename T0, typename T1, typename T2> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER3(cls, m) \
template<typename T0, typename T1, typename T2> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[3])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC4(cls, m) \
template<typename T0, typename T1, typename T2, typename T3> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER4(cls, m) \
template<typename T0, typename T1, typename T2, typename T3> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[4])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC5(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER5(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[5])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC6(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER6(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[6])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC7(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER7(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[7])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC8(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER8(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[8])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_STATIC9(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	static result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = cls::m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], *(T8*) ac->args[8], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}; \
	return AsyncCall(q, args, _t::_stub).wait();}

#define ASYNC_MEMBER9(cls, m) \
template<typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> \
	result_t ac_##m(AsyncQueue& q, \
		T0& v0, T1& v1, T2& v2, T3& v3, T4& v4, T5& v5, T6& v6, T7& v7, T8& v8) {\
	class _t { public: \
		static void _stub(AsyncCall* ac) { \
			result_t hr = ((cls*)ac->args[9])->m( \
				*(T0*) ac->args[0], *(T1*) ac->args[1], *(T2*) ac->args[2], *(T3*) ac->args[3], *(T4*) ac->args[4], *(T5*) ac->args[5], *(T6*) ac->args[6], *(T7*) ac->args[7], *(T8*) ac->args[8], ac); \
			if(hr != CALL_E_PENDDING)ac->post(hr); } }; \
	void* args[] = {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, this}; \
	return AsyncCall(q, args, _t::_stub).wait();}
