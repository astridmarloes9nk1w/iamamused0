#include "ifs/File.h"

#ifndef _fj_FILE_H
#define _fj_FILE_H

namespace fibjs
{

class File : public File_base
{
	EVENT_SUPPORT();

public:
    File() : m_data(0), m_file(NULL)
    {
    }

    ~File()
    {
        close(NULL);
    }

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncRead(int32_t bytes);
	virtual result_t write(obj_ptr<Buffer_base> data, exlib::AsyncEvent* ac);
	virtual result_t asyncWrite(obj_ptr<Buffer_base> data);
	virtual result_t flush(exlib::AsyncEvent* ac);
	virtual result_t asyncFlush();
	virtual result_t close(exlib::AsyncEvent* ac);
	virtual result_t asyncClose();

public:
	// File_base
	virtual result_t get_name(std::string& retVal);
	virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncStat();
	virtual result_t size(double& retVal);
	virtual result_t eof(bool& retVal);
	virtual result_t seek(double offset, int32_t whence);
	virtual result_t tell(double& retVal);
	virtual result_t rewind();
	virtual result_t truncate(double bytes, exlib::AsyncEvent* ac);
	virtual result_t asyncTruncate(double bytes);

public:
    result_t Open(const char* fname, const char* mode);
    result_t Write(const char* p, int sz);

private:
    std::string name;
    double m_data;
    FILE* m_file;
};

}

#endif // FILE_H
