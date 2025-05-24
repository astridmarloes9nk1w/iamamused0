/*
 * MongoCursor.h
 *
 *  Created on: Oct 31, 2012
 *      Author: lion
 */

#include "ifs/MongoCursor.h"
#include "MongoDB.h"

#ifndef MONGOCURSOR_H_
#define MONGOCURSOR_H_

namespace fibjs
{

class MongoCursor: public MongoCursor_base
{
public:
	MongoCursor(MongoDB* db, const std::string& ns, const std::string& name,
			v8::Handle<v8::Object> query, v8::Handle<v8::Object> projection);
	~MongoCursor();

public:
	// object_base
	virtual result_t toJSON(const char* key, v8::Handle<v8::Value>& retVal);

public:
	// MongoCursor_base
	virtual result_t batchSize(int32_t size, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t hint(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t limit(int32_t size, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t readPref(const char* mode, v8::Handle<v8::Array> tagSet, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t showDiskLoc(obj_ptr<MongoCursor_base>& retVal);
	virtual result_t count(bool applySkipLimit, int32_t& retVal);
	virtual result_t explain(v8::Handle<v8::Object>& retVal);
	virtual result_t forEach(v8::Handle<v8::Function> func);
	virtual result_t map(v8::Handle<v8::Function> func, v8::Handle<v8::Array>& retVal);
	virtual result_t hasNext(bool& retVal);
	virtual result_t next(v8::Handle<v8::Object>& retVal);
	virtual result_t size(int32_t& retVal);
	virtual result_t skip(int32_t num, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t snapshot(obj_ptr<MongoCursor_base>& retVal);
	virtual result_t sort(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t min(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t max(v8::Handle<v8::Object> opts, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t _addSpecial(const char* name, v8::Handle<v8::Value> opts, obj_ptr<MongoCursor_base>& retVal);
	virtual result_t toArray(v8::Handle<v8::Array>& retVal);

private:
	void ensureSpecial();

private:
	obj_ptr<MongoDB> m_db;
	std::string m_ns;
	std::string m_name;
	mongo_cursor m_cursor;
	bson m_bbq, m_bbp;
	bool m_bInit;
	bool m_bSpecial;
	v8::Persistent<v8::Object> m_query;
	enum
	{
		CUR_NONE, CUR_DATA, CUR_NODATA
	} m_state;
};

} /* namespace fibjs */
#endif /* MONGOCURSOR_H_ */
