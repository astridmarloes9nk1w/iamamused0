/*
 * Stat.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#ifndef STAT_H_
#define STAT_H_

#include "ifs/Stat.h"
#include <sys/stat.h>

namespace fibjs
{

class Stat: public Stat_base
{
public:
	result_t getStat(const char* path);
	void fillStat(const char* path, struct stat& st);

#ifdef _WIN32
	void fillStat(WIN32_FIND_DATAW& fd);
#endif

public:
	// Stat_base
	virtual result_t get_name(std::string& retVal);
	virtual result_t get_size(double& retVal);
	virtual result_t get_mtime(int64_t& retVal);
	virtual result_t get_atime(int64_t& retVal);
	virtual result_t get_ctime(int64_t& retVal);
	virtual result_t isWritable(bool& retVal);
	virtual result_t isReadable(bool& retVal);
	virtual result_t isExecutable(bool& retVal);
	virtual result_t isHidden(bool& retVal);
	virtual result_t isDirectory(bool& retVal);
	virtual result_t isFile(bool& retVal);
	virtual result_t isSymbolicLink(bool& retVal);

public:
	std::string name;
	double size;
	int64_t mtime, atime, ctime;
	bool m_isWritable, m_isReadable, m_isExecutable, m_isHidden;
	bool m_isDirectory, m_isFile, m_isSymbolicLink;
};

}

#endif /* STAT_H_ */
