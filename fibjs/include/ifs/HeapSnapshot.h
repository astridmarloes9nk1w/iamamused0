/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HeapSnapshot_base_H_
#define _HeapSnapshot_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class HeapGraphNode_base;
class List_base;

class HeapSnapshot_base : public object_base
{
    DECLARE_CLASS(HeapSnapshot_base);

public:
    // HeapSnapshot_base
    virtual result_t diff(HeapSnapshot_base* before, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t write(const char* fname, AsyncEvent* ac) = 0;
    virtual result_t get_time(date_t& retVal) = 0;
    virtual result_t get_root(obj_ptr<HeapGraphNode_base>& retVal) = 0;
    virtual result_t get_nodes(obj_ptr<List_base>& retVal) = 0;
    virtual result_t get_serialize(std::string& retVal) = 0;

public:
    static void s_diff(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getNodeById(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_time(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_root(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_nodes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_serialize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
    ASYNC_MEMBER1(HeapSnapshot_base, write, const char*);
};

}

#include "HeapGraphNode.h"
#include "List.h"

namespace fibjs
{
    inline ClassInfo& HeapSnapshot_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"diff", s_diff, false},
            {"getNodeById", s_getNodeById, false},
            {"write", s_write, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"time", s_get_time, block_set, false},
            {"root", s_get_root, block_set, false},
            {"nodes", s_get_nodes, block_set, false},
            {"serialize", s_get_serialize, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "HeapSnapshot", NULL, 
            3, s_method, 0, NULL, 4, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void HeapSnapshot_base::s_get_time(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        date_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapSnapshot_base);

        hr = pInst->get_time(vr);

        METHOD_RETURN();
    }

    inline void HeapSnapshot_base::s_get_root(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<HeapGraphNode_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapSnapshot_base);

        hr = pInst->get_root(vr);

        METHOD_RETURN();
    }

    inline void HeapSnapshot_base::s_get_nodes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<List_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapSnapshot_base);

        hr = pInst->get_nodes(vr);

        METHOD_RETURN();
    }

    inline void HeapSnapshot_base::s_get_serialize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(HeapSnapshot_base);

        hr = pInst->get_serialize(vr);

        METHOD_RETURN();
    }

    inline void HeapSnapshot_base::s_diff(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_INSTANCE(HeapSnapshot_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<HeapSnapshot_base>, 0);

        hr = pInst->diff(v0, vr);

        METHOD_RETURN();
    }

    inline void HeapSnapshot_base::s_getNodeById(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapGraphNode_base> vr;

        METHOD_INSTANCE(HeapSnapshot_base);
        METHOD_ENTER(1, 1);

        ARG(int32_t, 0);

        hr = pInst->getNodeById(v0, vr);

        METHOD_RETURN();
    }

    inline void HeapSnapshot_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(HeapSnapshot_base);
        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = pInst->ac_write(v0);

        METHOD_VOID();
    }

}

#endif

