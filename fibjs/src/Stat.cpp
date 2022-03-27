/*
 * Stat.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "Stat.h"
#include "ifs/path.h"
#include "utf8.h"

namespace fibjs
{

#ifdef _WIN32

#ifndef S_IRUSR
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#endif

#define S_ISLNK(m) 0

inline int64_t FileTimeToJSTime(FILETIME &ft)
{
	return (*(int64_t*)&ft - 116444736000000000) / 10000;
}

void Stat::fillStat(WIN32_FIND_DATAW& fd)
{
	name = utf16to8String(fd.cFileName);

	size = (int64_t)fd.nFileSizeHigh << 32 | fd.nFileSizeLow;
	mtime = FileTimeToJSTime(fd.ftLastWriteTime);
	atime = FileTimeToJSTime(fd.ftLastAccessTime);
	ctime = FileTimeToJSTime(fd.ftCreationTime);

	m_isDirectory = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) != 0;
	m_isFile = (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) == 0;

	m_isReadable = true;
	m_isWritable = (FILE_ATTRIBUTE_READONLY & fd.dwFileAttributes) == 0;
	m_isExecutable = true;

	m_isSymbolicLink = false;
}

#endif

result_t Stat::getStat(const char* path)
{
	struct stat st;
	if (::stat(path, &st))
		return LastError();

	fillStat(path, st);

	return 0;
}

void Stat::fillStat(const char* path, struct stat& st)
{
	path_base::basename(path, "", name);

	size = st.st_size;
	mtime = st.st_mtime * 1000ll;
	atime = st.st_atime * 1000ll;
	ctime = st.st_ctime * 1000ll;

	m_isReadable = (st.st_mode | S_IRUSR) != 0;
	m_isWritable = (st.st_mode | S_IWUSR) != 0;
	m_isExecutable = (st.st_mode | S_IXUSR) != 0;

	m_isDirectory = (S_IFDIR & st.st_mode) != 0;
	m_isFile = (S_IFREG & st.st_mode) != 0;

	m_isSymbolicLink = S_ISLNK(st.st_mode);
}

result_t Stat::get_name(std::string& retVal)
{
	retVal = name;
	return 0;
}

result_t Stat::get_size(double& retVal)
{
	retVal = size;
	return 0;
}

result_t Stat::get_mtime(int64_t& retVal)
{
	retVal = mtime;
	return 0;
}

result_t Stat::get_atime(int64_t& retVal)
{
	retVal = atime;
	return 0;
}

result_t Stat::get_ctime(int64_t& retVal)
{
	retVal = ctime;
	return 0;
}

result_t Stat::isWritable(bool& retVal)
{
	retVal = m_isWritable;
	return 0;
}

result_t Stat::isReadable(bool& retVal)
{
	retVal = m_isReadable;
	return 0;
}

result_t Stat::isExecutable(bool& retVal)
{
	retVal = m_isExecutable;
	return 0;
}

result_t Stat::isHidden(bool& retVal)
{
	retVal = m_isHidden;
	return 0;
}

result_t Stat::isDirectory(bool& retVal)
{
	retVal = m_isDirectory;
	return 0;
}

result_t Stat::isFile(bool& retVal)
{
	retVal = m_isFile;
	return 0;
}

result_t Stat::isSymbolicLink(bool& retVal)
{
	retVal = m_isSymbolicLink;
	return 0;
}

}
