/*
 * encoding_iconv.h
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#include "Buffer.h"
#include "utf8.h"

#ifndef ENCODING_ICONV_H_
#define ENCODING_ICONV_H_

namespace fibjs
{

class encoding_iconv
{
public:
    encoding_iconv(const char *charset = NULL);
    ~encoding_iconv();

    void open(const char *charset);

    result_t encode(const char *data, qstring &retVal);
    result_t encode(const char *data, obj_ptr<Buffer_base> &retVal);

    result_t decode(const qstring &data, qstring &retVal);
    result_t decode(Buffer_base *data, qstring &retVal);

    qstring charset() const
    {
        return m_charset;
    }

private:
    void *m_iconv_en;
    void *m_iconv_de;
    qstring m_charset;
};

} /* namespace fibjs */
#endif /* ENCODING_ICONV_H_ */
