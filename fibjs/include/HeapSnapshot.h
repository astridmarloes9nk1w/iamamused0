/*
 * HeapSnapshot.h
 *
 *  Created on: Aug 9, 2015
 *      Author: lion
 */

#include "ifs/HeapSnapshot.h"
#include <v8/include/v8-profiler.h>
#include "HeapGraphEdge.h"
#include "HeapGraphNode.h"
#include "List.h"
#include <map>

#ifndef _fj_HEAPSNAPSHOT_H
#define _fj_HEAPSNAPSHOT_H

namespace fibjs
{

class HeapSnapshot : public HeapSnapshot_base
{
public:
    HeapSnapshot(const v8::HeapSnapshot* snapshot)
        : m_snapshot(snapshot)
    {
        m_time.now();
    }

public:
    // HeapSnapshot_base
    virtual result_t diff(HeapSnapshot_base* before, v8::Local<v8::Object>& retVal);
    virtual result_t getNodeById(int32_t id, obj_ptr<HeapGraphNode_base>& retVal);
    virtual result_t serialize(std::string& retVal);
    virtual result_t get_time(date_t& retVal);
    virtual result_t get_root(obj_ptr<HeapGraphNode_base>& retVal);
    virtual result_t get_nodesCount(int32_t& retVal);
    virtual result_t get_nodes(obj_ptr<List_base>& retVal);

private:
    const v8::HeapSnapshot* m_snapshot;
    date_t m_time;
    obj_ptr<List> m_nodes;
};

}

#endif // _fj_HEAPSNAPSHOT_H
