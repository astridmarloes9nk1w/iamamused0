/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _TextColor_base_H_
#define _TextColor_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class TextColor_base : public object_base
{
    DECLARE_CLASS(TextColor_base);

public:
    // TextColor_base
    virtual result_t get_notice(qstring& retVal) = 0;
    virtual result_t set_notice(const char* newVal) = 0;
    virtual result_t get_warn(qstring& retVal) = 0;
    virtual result_t set_warn(const char* newVal) = 0;
    virtual result_t get_error(qstring& retVal) = 0;
    virtual result_t set_error(const char* newVal) = 0;
    virtual result_t get_highLight(qstring& retVal) = 0;
    virtual result_t set_highLight(const char* newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_notice(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_notice(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_warn(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_warn(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_error(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_error(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_highLight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_highLight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

namespace fibjs
{
    inline ClassInfo& TextColor_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"notice", s_get_notice, s_set_notice, false},
            {"warn", s_get_warn, s_set_warn, false},
            {"error", s_get_error, s_set_error, false},
            {"highLight", s_get_highLight, s_set_highLight, false}
        };

        static ClassData s_cd = 
        { 
            "TextColor", s__new, NULL, 
            0, NULL, 0, NULL, 4, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void TextColor_base::s_get_notice(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        qstring vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        hr = pInst->get_notice(vr);

        METHOD_RETURN();
    }

    inline void TextColor_base::s_set_notice(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_notice(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void TextColor_base::s_get_warn(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        qstring vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        hr = pInst->get_warn(vr);

        METHOD_RETURN();
    }

    inline void TextColor_base::s_set_warn(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_warn(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void TextColor_base::s_get_error(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        qstring vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        hr = pInst->get_error(vr);

        METHOD_RETURN();
    }

    inline void TextColor_base::s_set_error(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_error(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void TextColor_base::s_get_highLight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        qstring vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        hr = pInst->get_highLight(vr);

        METHOD_RETURN();
    }

    inline void TextColor_base::s_set_highLight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(TextColor_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_highLight(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

