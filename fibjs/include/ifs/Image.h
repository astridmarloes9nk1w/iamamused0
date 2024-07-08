/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Image_base_H_
#define _Image_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class gd_base;
class Buffer_base;
class Stream_base;

class Image_base : public object_base
{
public:
	// Image_base
	virtual result_t get_width(int32_t& retVal) = 0;
	virtual result_t get_height(int32_t& retVal) = 0;
	virtual result_t get_format(int32_t& retVal) = 0;
	virtual result_t get_type(int32_t& retVal) = 0;
	virtual result_t get_colorsTotal(int32_t& retVal) = 0;
	virtual result_t getData(int32_t format, int32_t quality, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t save(Stream_base* stm, int32_t format, int32_t quality, exlib::AsyncEvent* ac) = 0;
	virtual result_t colorAllocate(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorAllocateAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t colorDeallocate(int32_t color) = 0;
	virtual result_t colorClosest(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorClosestHWB(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorClosestAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t colorExact(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorExactAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t colorResolve(int32_t red, int32_t green, int32_t blue, int32_t& retVal) = 0;
	virtual result_t colorResolveAlpha(int32_t red, int32_t green, int32_t blue, int32_t alpha, int32_t& retVal) = 0;
	virtual result_t clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2) = 0;
	virtual result_t getPixel(int32_t x, int32_t y, int32_t& retVal) = 0;
	virtual result_t getTrueColorPixel(int32_t x, int32_t y, int32_t& retVal) = 0;
	virtual result_t setPixel(int32_t x, int32_t y, int32_t color) = 0;
	virtual result_t transparent(int32_t color) = 0;
	virtual result_t setThickness(int32_t thickness) = 0;
	virtual result_t line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color) = 0;
	virtual result_t rectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color) = 0;
	virtual result_t filledRectangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color) = 0;
	virtual result_t polygon(v8::Handle<v8::Array> points, int32_t color) = 0;
	virtual result_t openPolygon(v8::Handle<v8::Array> points, int32_t color) = 0;
	virtual result_t filledPolygon(v8::Handle<v8::Array> points, int32_t color) = 0;
	virtual result_t ellipse(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color) = 0;
	virtual result_t filledEllipse(int32_t x, int32_t y, int32_t width, int32_t height, int32_t color) = 0;
	virtual result_t arc(int32_t x, int32_t y, int32_t width, int32_t height, int32_t start, int32_t end, int32_t color) = 0;
	virtual result_t filledArc(int32_t x, int32_t y, int32_t width, int32_t height, int32_t start, int32_t end, int32_t color, int32_t style) = 0;
	virtual result_t fill(int32_t x, int32_t y, int32_t color) = 0;
	virtual result_t fillToBorder(int32_t x, int32_t y, int32_t borderColor, int32_t color) = 0;
	virtual result_t clone(obj_ptr<Image_base>& retVal) = 0;
	virtual result_t resample(int32_t width, int32_t height, obj_ptr<Image_base>& retVal) = 0;
	virtual result_t copy(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height) = 0;
	virtual result_t copyMerge(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t percent) = 0;
	virtual result_t copyMergeGray(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t percent) = 0;
	virtual result_t copyResized(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW, int32_t srcH) = 0;
	virtual result_t copyResampled(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t dstW, int32_t dstH, int32_t srcW, int32_t srcH) = 0;
	virtual result_t copyRotated(Image_base* source, int32_t dstX, int32_t dstY, int32_t srcX, int32_t srcY, int32_t width, int32_t height, int32_t angle) = 0;
	virtual result_t flip(int32_t dir) = 0;

	DECLARE_CLASSINFO(Image_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(width, int32_t);
		CLONE(height, int32_t);
		CLONE(format, int32_t);
		CLONE(type, int32_t);
		CLONE(colorsTotal, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_width(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_height(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_format(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_colorsTotal(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_getData(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_save(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorAllocate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorAllocateAlpha(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorDeallocate(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorClosest(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorClosestHWB(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorClosestAlpha(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorExact(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorExactAlpha(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorResolve(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_colorResolveAlpha(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_clip(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_getPixel(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_getTrueColorPixel(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_setPixel(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_transparent(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_setThickness(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_line(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rectangle(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_filledRectangle(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_polygon(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_openPolygon(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_filledPolygon(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_ellipse(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_filledEllipse(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_arc(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_filledArc(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_fill(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_fillToBorder(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_clone(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_resample(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_copy(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_copyMerge(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_copyMergeGray(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_copyResized(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_copyResampled(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_copyRotated(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_flip(const v8::Arguments& args);

public:
	ASYNC_MEMBER3(Image_base, getData);
	ASYNC_VALUEBACK2(Image_base, getData, obj_ptr<Buffer_base>);
	ASYNC_MEMBER3(Image_base, save);
	ASYNC_CALLBACK3(Image_base, save);
};

}

#include "gd.h"
#include "Buffer.h"
#include "Stream.h"

namespace fibjs
{
	inline ClassInfo& Image_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"getData", s_getData},
			{"save", s_save},
			{"colorAllocate", s_colorAllocate},
			{"colorAllocateAlpha", s_colorAllocateAlpha},
			{"colorDeallocate", s_colorDeallocate},
			{"colorClosest", s_colorClosest},
			{"colorClosestHWB", s_colorClosestHWB},
			{"colorClosestAlpha", s_colorClosestAlpha},
			{"colorExact", s_colorExact},
			{"colorExactAlpha", s_colorExactAlpha},
			{"colorResolve", s_colorResolve},
			{"colorResolveAlpha", s_colorResolveAlpha},
			{"clip", s_clip},
			{"getPixel", s_getPixel},
			{"getTrueColorPixel", s_getTrueColorPixel},
			{"setPixel", s_setPixel},
			{"transparent", s_transparent},
			{"setThickness", s_setThickness},
			{"line", s_line},
			{"rectangle", s_rectangle},
			{"filledRectangle", s_filledRectangle},
			{"polygon", s_polygon},
			{"openPolygon", s_openPolygon},
			{"filledPolygon", s_filledPolygon},
			{"ellipse", s_ellipse},
			{"filledEllipse", s_filledEllipse},
			{"arc", s_arc},
			{"filledArc", s_filledArc},
			{"fill", s_fill},
			{"fillToBorder", s_fillToBorder},
			{"clone", s_clone},
			{"resample", s_resample},
			{"copy", s_copy},
			{"copyMerge", s_copyMerge},
			{"copyMergeGray", s_copyMergeGray},
			{"copyResized", s_copyResized},
			{"copyResampled", s_copyResampled},
			{"copyRotated", s_copyRotated},
			{"flip", s_flip}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"width", s_get_width},
			{"height", s_get_height},
			{"format", s_get_format},
			{"type", s_get_type},
			{"colorsTotal", s_get_colorsTotal}
		};

		static ClassData s_cd = 
		{ 
			"Image", NULL, 
			39, s_method, 0, NULL, 5, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> Image_base::s_get_width(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_width(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_get_height(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_height(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_get_format(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_format(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_get_colorsTotal(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Image_base);

		hr = pInst->get_colorsTotal(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_getData(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 0);

		OPT_ARG(int32_t, 0, gd_base::_PNG);
		OPT_ARG(int32_t, 1, 85);

		hr = pInst->ac_getData(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_save(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 1);

		ARG(obj_ptr<Stream_base>, 0);
		OPT_ARG(int32_t, 1, gd_base::_PNG);
		OPT_ARG(int32_t, 2, 85);

		hr = pInst->ac_save(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorAllocate(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorAllocate(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorAllocateAlpha(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorAllocateAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorDeallocate(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->colorDeallocate(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorClosest(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorClosest(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorClosestHWB(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorClosestHWB(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorClosestAlpha(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorClosestAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorExact(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorExact(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorExactAlpha(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorExactAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorResolve(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->colorResolve(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_colorResolveAlpha(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->colorResolveAlpha(v0, v1, v2, v3, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_clip(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->clip(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_getPixel(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->getPixel(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_getTrueColorPixel(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->getTrueColorPixel(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_setPixel(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->setPixel(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_transparent(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->transparent(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_setThickness(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->setThickness(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_line(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->line(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_rectangle(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->rectangle(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_filledRectangle(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->filledRectangle(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_polygon(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Handle<v8::Array>, 0);
		ARG(int32_t, 1);

		hr = pInst->polygon(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_openPolygon(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Handle<v8::Array>, 0);
		ARG(int32_t, 1);

		hr = pInst->openPolygon(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_filledPolygon(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Handle<v8::Array>, 0);
		ARG(int32_t, 1);

		hr = pInst->filledPolygon(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_ellipse(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->ellipse(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_filledEllipse(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(5, 5);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);

		hr = pInst->filledEllipse(v0, v1, v2, v3, v4);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_arc(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(7, 7);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);

		hr = pInst->arc(v0, v1, v2, v3, v4, v5, v6);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_filledArc(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 7);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		OPT_ARG(int32_t, 7, gd_base::_ARC);

		hr = pInst->filledArc(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_fill(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);

		hr = pInst->fill(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_fillToBorder(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);

		hr = pInst->fillToBorder(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_clone(const v8::Arguments& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clone(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_resample(const v8::Arguments& args)
	{
		obj_ptr<Image_base> vr;

		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(int32_t, 1);

		hr = pInst->resample(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Image_base::s_copy(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(7, 7);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);

		hr = pInst->copy(v0, v1, v2, v3, v4, v5, v6);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_copyMerge(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 8);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);

		hr = pInst->copyMerge(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_copyMergeGray(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 8);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);

		hr = pInst->copyMergeGray(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_copyResized(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(9, 9);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);
		ARG(int32_t, 8);

		hr = pInst->copyResized(v0, v1, v2, v3, v4, v5, v6, v7, v8);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_copyResampled(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(9, 9);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);
		ARG(int32_t, 8);

		hr = pInst->copyResampled(v0, v1, v2, v3, v4, v5, v6, v7, v8);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_copyRotated(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(8, 8);

		ARG(obj_ptr<Image_base>, 0);
		ARG(int32_t, 1);
		ARG(int32_t, 2);
		ARG(int32_t, 3);
		ARG(int32_t, 4);
		ARG(int32_t, 5);
		ARG(int32_t, 6);
		ARG(int32_t, 7);

		hr = pInst->copyRotated(v0, v1, v2, v3, v4, v5, v6, v7);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Image_base::s_flip(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Image_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, gd_base::_HORIZONTAL);

		hr = pInst->flip(v0);

		METHOD_VOID();
	}

}

#endif

