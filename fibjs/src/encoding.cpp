/*
 * encoding.cpp
 *
 *  Created on: Apr 10, 2012
 *      Author: lion
 */

#include <osconfig.h>

#ifdef _WIN32
#include <win_iconv.h>
#else

#if !defined(FreeBSD) || defined(__clang_major__)
#include <iconv.h>
#else

#include <stddef.h>

typedef void *iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
    return (iconv_t) - 1;
}

int iconv_close(iconv_t cd)
{
    return 0;
}

size_t iconv(iconv_t cd, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    return 0;
}
#endif

#include <dlfcn.h>

#endif

#include "ifs/encoding.h"
#include "Buffer.h"
#include "utf8.h"
#include "Url.h"

namespace fibjs
{

inline void baseEncode(const char *pEncodingTable, int dwBits,
                       Buffer_base *data, std::string &retVal)
{
    std::string strData;
    int i, len = 0, bits = 0;
    int dwData = 0;
    int dwSize = 0;
    char bMask = 0xff >> (8 - dwBits);

    data->toString(strData);

    if (dwBits == 6)
        dwSize = ((int) strData.length() + 2) / 3 * 4;
    else if (dwBits == 5)
        dwSize = ((int) strData.length() + 4) / 5 * 8;

    retVal.resize(dwSize);

    for (i = 0; i < (int) strData.length(); i++)
    {
        dwData <<= 8;
        dwData |= (unsigned char) strData[i];
        bits += 8;

        while (bits >= dwBits)
        {
            retVal[len++] = pEncodingTable[(dwData >> (bits - dwBits)) & bMask];
            bits -= dwBits;
        }
    }

    if (bits)
        retVal[len++] = pEncodingTable[(dwData << (dwBits - bits)) & bMask];

    while (len < dwSize)
        retVal[len++] = '=';

    retVal.resize(len);
}

inline void baseDecode(const char *pdecodeTable, int dwBits,
                       const char *baseString, obj_ptr<Buffer_base> &retVal)
{
    int nWritten = 0, len = (int) qstrlen(baseString);
    std::string strBuf;

    strBuf.resize(len * dwBits / 8);

    int dwCurr = 0;
    int nBits = 0;
    uint32_t ch;

    while ((ch = utf8_getchar(baseString)) != 0)
    {
        int nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

        if (nCh != -1)
        {
            dwCurr <<= dwBits;
            dwCurr |= nCh;
            nBits += dwBits;

            while (nBits >= 8)
            {
                strBuf[nWritten++] = (char) (dwCurr >> (nBits - 8));
                nBits -= 8;
            }
        }
    }

    strBuf.resize(nWritten);

    retVal = new Buffer(strBuf);
}

result_t encoding_base::base32Encode(Buffer_base *data, std::string &retVal)
{
    baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZ234567", 5, data, retVal);
    return 0;
}

result_t encoding_base::base32Decode(const char *data,
                                     obj_ptr<Buffer_base> &retVal)
{
    static const char decodeTable[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 2x  !"#$%&'()*+,-./   */
        14, 11, 26, 27, 28, 29, 30, 31, -1, 6, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 6x `abcdefghijklmno   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    baseDecode(decodeTable, 5, data, retVal);
    return 0;
}

result_t encoding_base::base64Encode(Buffer_base *data, std::string &retVal)
{
    baseEncode(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
        6, data, retVal);
    return 0;
}

result_t encoding_base::base64Decode(const char *data,
                                     obj_ptr<Buffer_base> &retVal)
{
    static const char decodeTable[] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, /* 2x  !"#$%&'()*+,-./   */
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 6x `abcdefghijklmno   */
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    baseDecode(decodeTable, 6, data, retVal);
    return 0;
}

result_t encoding_base::hexEncode(Buffer_base *data, std::string &retVal)
{
    std::string strData;
    static char HexChar[] = "0123456789abcdef";
    int i, pos, len1;

    data->toString(strData);

    i = (int) strData.length() * 2;
    retVal.resize(i);

    len1 = 0;
    pos = 0;

    for (i = 0; i < (int) strData.length(); i++)
    {
        retVal[pos * 2] = HexChar[(unsigned char) strData[i] >> 4];
        retVal[pos * 2 + 1] = HexChar[(unsigned char) strData[i] & 0xf];
        pos++;
        len1 += 2;
    }

    return 0;
}

result_t encoding_base::hexDecode(const char *data,
                                  obj_ptr<Buffer_base> &retVal)
{
    int pos, len = (int) qstrlen(data);
    std::string strBuf;
    uint32_t ch1, ch2;

    strBuf.resize(len / 2);

    pos = 0;
    while ((ch1 = utf8_getchar(data)) != 0)
    {
        if ((ch1 >= 'a' && ch1 <= 'f') || (ch1 >= 'A' && ch1 <= 'F'))
            ch1 = (ch1 & 0xf) + 9;
        else if (ch1 >= '0' && ch1 <= '9')
            ch1 &= 0xf;
        else
            continue;

        ch2 = utf8_getchar(data);
        if (ch2 == 0)
            break;

        if ((ch2 >= 'a' && ch2 <= 'f') || (ch2 >= 'A' && ch2 <= 'F'))
            ch2 = (ch2 & 0xf) + 9;
        else if (ch2 >= '0' && ch2 <= '9')
            ch2 &= 0xf;
        else
        {
            ch2 = ch1;
            ch1 = 0;
        }

        strBuf[pos++] = (ch1 << 4) + ch2;
    }

    retVal = new Buffer(strBuf);

    return 0;
}

#ifdef _WIN32

#define _iconv_open iconv_open
#define _iconv_close iconv_close
#define _iconv iconv

inline void init_iconv()
{
}

#else

static size_t (*_iconv)(iconv_t, const char **, size_t *, char **, size_t *);
static iconv_t (*_iconv_open)(const char *, const char *);
static int (*_iconv_close)(iconv_t);

inline void init_iconv()
{
    static bool _init = false;

    if (!_init)
    {
        _init = true;

#if defined(FreeBSD) && !defined(__clang_major__)
        void *handle = dlopen("libiconv.so", RTLD_LAZY);

        if (handle)
        {
            _iconv = (size_t (*)(iconv_t, const char **, size_t *, char **, size_t *))dlsym(handle, "iconv");
            if (!_iconv)
                _iconv = (size_t (*)(iconv_t, const char **, size_t *, char **, size_t *))dlsym(handle, "libiconv");

            _iconv_open = (iconv_t (*)(const char *, const char *))dlsym(handle, "iconv_open");
            if (!_iconv_open)
                _iconv_open = (iconv_t (*)(const char *, const char *))dlsym(handle, "libiconv_open");

            _iconv_close = (int (*)(iconv_t))dlsym(handle, "iconv_close");
            if (!_iconv_close)
                _iconv_close = (int (*)(iconv_t))dlsym(handle, "libiconv_close");
        }
#endif

        if (!_iconv || !_iconv_open || !_iconv_close)
        {
            _iconv = (size_t (*)(iconv_t, const char **, size_t *, char **, size_t *))iconv;
            _iconv_open = iconv_open;
            _iconv_close = iconv_close;
        }
    }
}

#endif

result_t encoding_base::iconvEncode(const char *charset, const char *data,
                                    obj_ptr<Buffer_base> &retVal)
{
    init_iconv();

    std::string strBuf;

    if (!qstricmp(charset, "utf8") || !qstricmp(charset, "utf-8"))
        strBuf = data;
    else
    {
        iconv_t cd = _iconv_open(charset, "utf-8");
        if (cd == (iconv_t)(-1))
            return Runtime::setError("Unknown charset.");

        size_t sz = qstrlen(data);

        strBuf.resize(sz * 2);
        char *output_buf = &strBuf[0];
        size_t output_size = strBuf.length();

        size_t n = _iconv(cd, &data, &sz, &output_buf, &output_size);
        _iconv_close(cd);

        if (n == (size_t) - 1)
            return Runtime::setError("convert error.");

        strBuf.resize(strBuf.length() - output_size);
    }

    retVal = new Buffer(strBuf);

    return 0;
}

result_t encoding_base::iconvDecode(const char *charset, Buffer_base *data,
                                    std::string &retVal)
{
    init_iconv();

    if (!qstricmp(charset, "utf8") || !qstricmp(charset, "utf-8"))
        data->toString(retVal);
    else
    {
        std::string strData;
        std::string strBuf;

        data->toString(strData);

        iconv_t cd = _iconv_open("utf-8", charset);
        if (cd == (iconv_t)(-1))
            return Runtime::setError("Unknown charset.");

        size_t sz = strData.length();
        const char *ptr = strData.c_str();

        strBuf.resize(sz * 2);
        char *output_buf = &strBuf[0];
        size_t output_size = strBuf.length();

        size_t n = _iconv(cd, &ptr, &sz, &output_buf, &output_size);
        _iconv_close(cd);

        if (n == (size_t) - 1)
            return Runtime::setError("convert error.");

        strBuf.resize(strBuf.length() - output_size);

        retVal = strBuf;
    }

    return 0;
}

result_t encoding_base::jsstr(const char *str, std::string &retVal)
{
    const char *p;
    char *p1;
    int len;
    char ch;
    std::string s;

    if (!*str)
        return 0;

    for (len = 0, p = str; (ch = *p) != 0; p++, len++)
        if (ch == '\\' || ch == '\r' || ch == '\n' || ch == '\t' || ch == '\''
                || ch == '\"')
            len++;

    s.resize(len);

    for (p1 = &s[0], p = str; (ch = *p) != 0; p++)
        switch (ch)
        {
        case '\\':
            *p1++ = '\\';
            *p1++ = '\\';
            break;
        case '\r':
            *p1++ = '\\';
            *p1++ = 'r';
            break;
        case '\n':
            *p1++ = '\\';
            *p1++ = 'n';
            break;
        case '\t':
            *p1++ = '\\';
            *p1++ = 't';
            break;
        case '\'':
            *p1++ = '\\';
            *p1++ = '\'';
            break;
        case '\"':
            *p1++ = '\\';
            *p1++ = '\"';
            break;
        default:
            *p1++ = ch;
            break;
        }

    retVal = s;
    return 0;
}

static const char *URITable =
    " ! #$ &'()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";
static const char *URIComponentTable =
    " !     '()*  -. 0123456789       ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";

result_t encoding_base::encodeURI(const char *url, std::string &retVal)
{
    Url::encodeURI(url, -1, retVal, URITable);
    return 0;
}

result_t encoding_base::encodeURIComponent(const char *url, std::string &retVal)
{
    Url::encodeURI(url, -1, retVal, URIComponentTable);
    return 0;
}

result_t encoding_base::decodeURI(const char *url, std::string &retVal)
{
    Url::decodeURI(url, -1, retVal);
    return 0;
}

static v8::Persistent<v8::Object> s_json;
static v8::Persistent<v8::Function> s_stringify;

inline void initJSON()
{
    if (s_json.IsEmpty())
    {
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::Local<v8::Object> global = context->Global();

        s_json.Reset(isolate, global->Get(v8::String::NewFromUtf8(isolate, "JSON"))->ToObject());
    }
}

result_t encoding_base::jsonEncode(v8::Local<v8::Value> data,
                                   std::string &retVal)
{
    initJSON();

    v8::Local<v8::Object> _json = v8::Local<v8::Object>::New(isolate, s_json);

    if (s_stringify.IsEmpty())
        s_stringify.Reset(isolate,
                          v8::Local<v8::Function>::Cast(_json->Get(v8::String::NewFromUtf8(isolate, "stringify"))));

    v8::Local<v8::Value> str = v8::Local<v8::Function>::New(isolate, s_stringify)->Call(_json, 1, &data);
    if (str.IsEmpty())
        return CALL_E_JAVASCRIPT;

    v8::String::Utf8Value v(str);
    retVal.assign(*v, v.length());

    return 0;
}

result_t encoding_base::jsonDecode(const char *data,
                                   v8::Local<v8::Value> &retVal)
{
    retVal = v8::JSON::Parse(v8::String::NewFromUtf8(isolate, data));
    if (retVal.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}

}

