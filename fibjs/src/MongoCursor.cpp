/*
 * MongoCursor.cpp
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "MongoCursor.h"
#include "encoding_bson.h"

namespace fibjs
{

MongoCursor::MongoCursor(MongoDB* db, const std::string& ns,
		const std::string& name, v8::Handle<v8::Object> query,
		v8::Handle<v8::Object> projection)
{
	m_state = CUR_NONE;
	m_db = db;

	m_ns = ns;
	m_name = name;

	mongo_cursor_init(&m_cursor, &db->m_conn, ns.c_str());

	m_query = v8::Persistent < v8::Object > ::New(query->Clone());

	bson_init(&m_bbq);
	mongo_cursor_set_query(&m_cursor, &m_bbq);

	bson_init(&m_bbp);
	encodeObject(&m_bbp, projection);
	bson_finish(&m_bbp);

	mongo_cursor_set_fields(&m_cursor, &m_bbp);

	m_bInit = false;
	m_bSpecial = false;
}

MongoCursor::~MongoCursor()
{
	m_query.Dispose();
	mongo_cursor_destroy(&m_cursor);
	bson_destroy(&m_bbq);
	bson_destroy(&m_bbp);
}

void MongoCursor::ensureSpecial()
{
	if (!m_bSpecial)
	{
		v8::Handle < v8::Object > o = v8::Object::New();

		o->Set(v8::String::New("query"), m_query);
		m_query.Dispose();

		m_query = v8::Persistent < v8::Object > ::New(o);
		m_bSpecial = true;
	}
}

result_t MongoCursor::batchSize(int32_t size, obj_ptr<MongoCursor_base>& retVal)
{
	retVal = this;
	return 0;
}

result_t MongoCursor::hint(v8::Handle<v8::Object> opts,
		obj_ptr<MongoCursor_base>& retVal)
{
	return _addSpecial("$hint", opts, retVal);
}

result_t MongoCursor::limit(int32_t size, obj_ptr<MongoCursor_base>& retVal)
{
	if (m_bInit)
		return CALL_E_INVALID_CALL;

	mongo_cursor_set_limit(&m_cursor, size);
	retVal = this;
	return 0;
}

result_t MongoCursor::readPref(const char* mode, v8::Handle<v8::Array> tagSet,
		obj_ptr<MongoCursor_base>& retVal)
{
	v8::Handle < v8::Object > o = v8::Object::New();

	o->Set(v8::String::New("mode"), v8::String::New(mode));

	if (tagSet->Length() > 0)
		o->Set(v8::String::New("tags"), tagSet);

	return _addSpecial("$readPreference", o, retVal);
}

result_t MongoCursor::showDiskLoc(obj_ptr<MongoCursor_base>& retVal)
{
	return _addSpecial("$showDiskLoc", v8::True(), retVal);
}

result_t MongoCursor::count(bool applySkipLimit, int32_t& retVal)
{
	bson bbq;

	bson_init(&bbq);
	bson_append_string(&bbq, "count", m_name.c_str());

	if (m_bSpecial)
		encodeValue(&bbq, "query", m_query->Get(v8::String::New("query")));
	else
		encodeValue(&bbq, "query", m_query);

	if (applySkipLimit)
	{
		if (m_cursor.limit)
			bson_append_int(&bbq, "limit", m_cursor.limit);
		if (m_cursor.skip)
			bson_append_int(&bbq, "skip", m_cursor.skip);
	}

	bson_finish(&bbq);

	v8::Handle < v8::Object > res;

	result_t hr = m_db->run_command(&bbq, res);
	if (hr < 0)
		return hr;

	retVal = res->Get(v8::String::New("n"))->Int32Value();

	return 0;
}

result_t MongoCursor::explain(v8::Handle<v8::Object>& retVal)
{
	return CALL_RETURN_NULL;
}

result_t MongoCursor::forEach(v8::Handle<v8::Function> func)
{
	result_t hr;
	v8::Handle < v8::Object > o;
	v8::Handle < v8::Object > o1 = v8::Object::New();

	while ((hr = next(o)) != CALL_RETURN_NULL)
	{
		v8::Handle < v8::Value > a = o;
		v8::Handle < v8::Value > v = func->Call(o1, 1, &a);

		if (v.IsEmpty())
			return CALL_E_JAVASCRIPT;
	}

	return hr < 0 ? hr : 0;
}

result_t MongoCursor::map(v8::Handle<v8::Function> func,
		v8::Handle<v8::Array>& retVal)
{
	result_t hr;
	v8::Handle < v8::Object > o;
	v8::Handle < v8::Object > o1 = v8::Object::New();
	v8::Handle < v8::Array > as = v8::Array::New();
	int n = 0;

	while ((hr = next(o)) != CALL_RETURN_NULL)
	{
		v8::Handle < v8::Value > a = o;
		v8::Handle < v8::Value > v = func->Call(o1, 1, &a);

		if (v.IsEmpty())
			return CALL_E_JAVASCRIPT;

		as->Set(n, v);
		n++;
	}

	retVal = as;
	return hr < 0 ? hr : 0;
}

result_t MongoCursor::hasNext(bool& retVal)
{
	if (!m_bInit)
	{
		bson_init(&m_bbq);
		encodeObject(&m_bbq, m_query);
		bson_finish(&m_bbq);

		m_bInit = true;
	}

	if (m_state == CUR_NONE)
		m_state =
				mongo_cursor_next(&m_cursor) == MONGO_OK ?
						CUR_DATA : CUR_NODATA;

	retVal = m_state == CUR_DATA;

	return m_db->error();
}

result_t MongoCursor::next(v8::Handle<v8::Object>& retVal)
{
	bool has;
	result_t hr = hasNext(has);

	if (hr < 0)
		return hr;

	if (!has)
		return CALL_RETURN_NULL;

	retVal = decodeObject(mongo_cursor_bson(&m_cursor));
	m_state = CUR_NONE;

	return 0;
}

result_t MongoCursor::size(int32_t& retVal)
{
	return count(true, retVal);
}

result_t MongoCursor::skip(int32_t num, obj_ptr<MongoCursor_base>& retVal)
{
	if (m_bInit)
		return CALL_E_INVALID_CALL;

	mongo_cursor_set_skip(&m_cursor, num);
	retVal = this;

	return 0;
}

result_t MongoCursor::snapshot(obj_ptr<MongoCursor_base>& retVal)
{
	return _addSpecial("$snapshot", v8::True(), retVal);
}

result_t MongoCursor::sort(v8::Handle<v8::Object> opts,
		obj_ptr<MongoCursor_base>& retVal)
{
	return _addSpecial("orderby", opts, retVal);
}

result_t MongoCursor::min(v8::Handle<v8::Object> opts,
		obj_ptr<MongoCursor_base>& retVal)
{
	return _addSpecial("$min", opts, retVal);
}

result_t MongoCursor::max(v8::Handle<v8::Object> opts,
		obj_ptr<MongoCursor_base>& retVal)
{
	return _addSpecial("$max", opts, retVal);
}

result_t MongoCursor::_addSpecial(const char* name, v8::Handle<v8::Value> opts,
		obj_ptr<MongoCursor_base>& retVal)
{
	if (m_bInit)
		return CALL_E_INVALID_CALL;

	ensureSpecial();
	m_query->Set(v8::String::New(name), opts);

	retVal = this;
	return 0;
}

result_t MongoCursor::toArray(v8::Handle<v8::Array>& retVal)
{
	result_t hr;
	v8::Handle < v8::Object > o;
	v8::Handle < v8::Array > as = v8::Array::New();
	int n = 0;

	while ((hr = next(o)) != CALL_RETURN_NULL)
	{
		as->Set(n, o);
		n++;
	}

	if (hr < 0)
		return hr;

	retVal = as;
	return 0;
}

result_t MongoCursor::toJSON(const char* key, v8::Handle<v8::Value>& retVal)
{
	result_t hr;
	v8::Handle < v8::Array > as;

	hr = toArray(as);
	if (hr < 0)
		return hr;

	retVal = as;
	return 0;
}

} /* namespace fibjs */
