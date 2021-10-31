#include "Buffer.h"

namespace fibjs
{

result_t Buffer::_indexed_getter(uint32_t index, int32_t& retVal)
{
    if(index >= m_data.length())
        return CALL_E_BADINDEX;

    retVal = (unsigned char)m_data[index];
    return 0;
}

result_t Buffer::_indexed_setter(uint32_t index, int32_t newVal)
{
    if(index >= m_data.length())
        return CALL_E_BADINDEX;

    if(newVal < 0 || newVal > 255)
        return CALL_E_OUTRANGE;

    m_data[index] = newVal;
    return 0;
}

result_t Buffer::get_length(int32_t& retVal)
{
    retVal = m_data.length();
    return 0;
}

result_t Buffer::resize(int32_t sz)
{
    if(sz < 0)
        return CALL_E_INVALIDARG;

    m_data.resize(sz);

    return 0;
}

result_t Buffer::write(const char* str)
{
    m_data.append(str);
    return 0;
}

result_t Buffer::slice(int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal)
{
    if(start < 0)
        return CALL_E_INVALIDARG;

    if(end < 0)
        end = m_data.length();

    obj_ptr<Buffer> pNew = new Buffer();

    pNew->m_data.append(m_data, start, end - start);
    retVal = pNew;

    return 0;
}

result_t Buffer::toString(std::string& retVal)
{
    retVal = m_data;
    return 0;
}

result_t Buffer_base::_new(const char* str, obj_ptr<Buffer_base>& retVal)
{
    retVal = new Buffer();
    retVal->write(str);

    return 0;
}

}
