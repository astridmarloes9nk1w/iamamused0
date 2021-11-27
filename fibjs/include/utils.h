/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fj_utils_H_
#define _fj_utils_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <v8/v8.h>
#include <string>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#endif

namespace fibjs
{

typedef int result_t;

// Invalid number of parameters.
#define CALL_E_BADPARAMCOUNT    -1
// Parameter not optional.
#define CALL_E_PARAMNOTOPTIONAL -2
// Constructor cannot be called as a function.
#define CALL_E_CONSTRUCTOR      -3
// Object is not an instance of declaring class.
#define CALL_E_NOTINSTANCE      -4
// The input parameter is not a valid type.
#define CALL_E_BADVARTYPE       -5
// An argument is invalid.
#define CALL_E_INVALIDARG       -6
// The argument could not be coerced to the specified type.
#define CALL_E_TYPEMISMATCH     -7
// Value is out of range.
#define CALL_E_OUTRANGE         -8
// Index was out of range.
#define CALL_E_BADINDEX         -9
#define CALL_E_MAX              -10

#define PROPERTY_ENTER() result_t hr = 0;do{

#define METHOD_ENTER(c, o) \
    result_t hr = 0; do{\
    int argc; \
    argc = args.Length(); \
    if((c) >= 0 && argc > (c)){hr = CALL_E_BADPARAMCOUNT;break;} \
    if((o) > 0 && argc < (o)){hr = CALL_E_PARAMNOTOPTIONAL;break;}

#define CONSTRUCT_ENTER(c, o) \
    if (!args.IsConstructCall())return ThrowResult(CALL_E_CONSTRUCTOR); \
    METHOD_ENTER(c, o)

#define METHOD_INSTANCE(cls) \
    obj_ptr<cls> pInst = (cls*)cls::class_info().getInstance(args.This()); \
    if(pInst == NULL){hr = CALL_E_NOTINSTANCE;break;}

#define PROPERTY_INSTANCE(cls) \
    obj_ptr<cls> pInst = (cls*)cls::class_info().getInstance(info.This()); \
    if(pInst == NULL){hr = CALL_E_NOTINSTANCE;break;}


#define PROPERTY_SET_LEAVE() \
    }while(0); \
    if(hr < 0)ThrowResult(hr);

#define METHOD_RETURN() \
    if(hr >= 0)return ReturnValue(vr); \
    }while(0); \
    return ThrowResult(hr);

#define METHOD_VOID() \
    if(hr >= 0)return v8::Undefined(); \
    }while(0); \
    return ThrowResult(hr);

#define CONSTRUCT_RETURN() \
    if(hr >= 0)return vr->wrap(args.This()); \
    }while(0); \
    return ThrowResult(hr);


#define ARG_CLASS(cls, n) \
    obj_ptr<cls> v##n = (cls*)cls::class_info().getInstance(args[n]); \
    if(v##n == NULL){hr = CALL_E_NOTINSTANCE;break;}


#define ARG_String(n) \
    v8::String::Utf8Value tv##n(args[n]); \
    const char* v##n = *tv##n; \
    if(v##n == NULL){hr = CALL_E_INVALIDARG;break;}

#define OPT_ARG_String(n, d) \
    v8::Local<v8::Value> tvv##n; \
    if(n < argc)tvv##n = args[n]; \
    v8::String::Utf8Value tv##n(tvv##n); \
    const char* v##n = (n) < argc ? *tv##n : (d); \
    if(v##n == NULL){hr = CALL_E_INVALIDARG;break;}

#define PROPERTY_VAL_String() \
    v8::String::Utf8Value tv0(value); \
    const char* v0 = *tv0; \
    if(v0 == NULL){hr = CALL_E_INVALIDARG;break;}

#define ARG(t, n) \
    t v##n; \
    hr = SafeGetValue(args[n], v##n); \
    if(hr < 0)break;

#define OPT_ARG(t, n, d) \
    t v##n; \
    if((n) < argc){ \
        hr = SafeGetValue(args[n], v##n); \
        if(hr < 0)break; \
    }else v##n = (d);

#define PROPERTY_VAL(t) \
    t v0; \
    hr = SafeGetValue(value, v0); \
    if(hr < 0)break;

#ifdef _MSC_VER
#define isnan _isnan
#endif

template<class T>
class obj_ptr
{
public:
    obj_ptr() : p(NULL)
    {}

    obj_ptr(T* lp) : p(NULL)
    {
        operator=(lp);
    }

    obj_ptr(const obj_ptr<T>& lp) : p(NULL)
    {
        operator=(lp);
    }

    ~obj_ptr()
    {
        Release();
    }

    T* operator=(T* lp)
    {
        if (lp != NULL)
            lp->Ref();

        return Attach(lp);
    }

    T* operator=(const obj_ptr<T>& lp)
    {
        return operator=(lp.p);
    }

    operator T*() const
    {
        return p;
    }

    T& operator*() const
    {
        return *p;
    }

    T** operator&()
    {
        return &p;
    }

    bool operator!() const
    {
        return (p == NULL);
    }

    bool operator==(T* pT) const
    {
        return p == pT;
    }

    T* operator->()
    {
        return p;
    }

    void Release()
    {
        T* pTemp = Detach();
        if (pTemp)
            pTemp->Unref();
    }

    T* Attach(T* p2)
    {
        T* p1 = p;
        p = p2;

        if (p1)
            p1->Unref();

        return p2;
    }

    T* Detach()
    {
        T* p1 = p;

        p = NULL;
        return p1;
    }

    T* p;
};

inline result_t SafeGetValue(v8::Handle<v8::Value> v, double& n)
{
    if(v.IsEmpty())
        return CALL_E_INVALIDARG;

    n = v->NumberValue();
    if(isnan(n))
        return CALL_E_INVALIDARG;

    return 0;
}

inline result_t SafeGetValue(v8::Handle<v8::Value> v, int64_t& n)
{
    double num;

    result_t hr = SafeGetValue(v, num);
    if(hr < 0)return hr;

    if(num < -9007199254740992ll || num > 9007199254740992ll)
        return CALL_E_OUTRANGE;

    n = (int64_t)num;

    return 0;
}

inline result_t SafeGetValue(v8::Handle<v8::Value> v, int32_t& n)
{
    double num;

    result_t hr = SafeGetValue(v, num);
    if(hr < 0)return hr;

    if(num < -2147483648ll || num > 2147483647ll)
        return CALL_E_OUTRANGE;

    n = (int32_t)num;

    return 0;
}

inline result_t SafeGetValue(v8::Handle<v8::Value> v, bool& n)
{
    n = v->BooleanValue();
    return 0;
}

inline v8::Handle<v8::Value> ReturnValue(int32_t v)
{
    return v8::Int32::New(v);
}

inline v8::Handle<v8::Value> ReturnValue(bool v)
{
    return v ? v8::True() : v8::False();
}

inline v8::Handle<v8::Value> ReturnValue(double v)
{
    return v8::Number::New(v);
}

inline v8::Handle<v8::Value> ReturnValue(std::string& str)
{
    return v8::String::New(str.c_str(), str.length());
}

inline v8::Handle<v8::Value> ReturnValue(v8::Handle<v8::Object>& obj)
{
    return obj;
}

inline v8::Handle<v8::Value> ReturnValue(v8::Handle<v8::Array>& array)
{
    return array;
}

inline v8::Handle<v8::Value> ReturnValue(v8::Handle<v8::Function>& func)
{
    return func;
}

template<class T>
inline v8::Handle<v8::Value> ReturnValue(obj_ptr<T>& obj)
{
    return obj->JSObject();
}

inline v8::Handle<v8::Value> ThrowError(const char* msg)
{
    return v8::ThrowException(v8::Exception::Error(v8::String::New(msg)));
}

inline v8::Handle<v8::Value> ThrowTypeError(const char* msg)
{
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New(msg)));
}

inline v8::Handle<v8::Value> ThrowRangeError(const char* msg)
{
    return v8::ThrowException(v8::Exception::RangeError(v8::String::New(msg)));
}

inline result_t LastError()
{
#ifdef _WIN32
    return CALL_E_MAX - GetLastError();
#else
    return CALL_E_MAX - errno;
#endif
}

v8::Handle<v8::Value> ThrowResult(result_t hr);
std::string traceInfo();
void ReportException(v8::TryCatch* try_catch, bool rt = false);

}

#endif

