/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _profiler_base_H_
#define _profiler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class HeapSnapshot_base;

class profiler_base : public object_base
{
    DECLARE_CLASS(profiler_base);

public:
    // profiler_base
    static result_t takeSnapshot(obj_ptr<HeapSnapshot_base>& retVal);

public:
    static void s_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "HeapSnapshot.h"

namespace fibjs
{
    inline ClassInfo& profiler_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"takeSnapshot", s_takeSnapshot, true}
        };

        static ClassData s_cd = 
        { 
            "profiler", NULL, 
            1, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void profiler_base::s_takeSnapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HeapSnapshot_base> vr;

        METHOD_ENTER(0, 0);

        hr = takeSnapshot(vr);

        METHOD_RETURN();
    }

}

#endif

