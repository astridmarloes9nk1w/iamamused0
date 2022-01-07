/*
 * path.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "ifs/path.h"
#include <sstream>

namespace fibjs
{

#ifdef _WIN32

inline bool isSeparator(char ch)
{
	return ch == '/' || ch == '\\';
}

#define pathcmp qstricmp
#define PATH_CHAR	'\\'

#else

inline bool isSeparator(char ch)
{
	return ch == '/';
}

#define pathcmp qstrcmp
#define PATH_CHAR	'/'

#endif

result_t path_base::basename(const char* path, const char* ext,
		std::string& retVal)
{
	char ch;
	const char *p1 = path;
	int extlen = qstrlen(ext);

	while (*path)
	{
		ch = *path++;
		if (isSeparator(ch))
			p1 = path;
	}

	if (extlen && ((int) (path - p1) >= extlen)
			&& !pathcmp(ext, path - extlen, extlen))
		path -= extlen;

	retVal.append(p1, (int) (path - p1));

	return 0;
}

result_t path_base::extname(const char* path, std::string& retVal)
{
	char ch;
	const char *p1 = NULL;

	if (*path == '.')
		path++;

	while (*path)
	{
		ch = *path++;
		if (isSeparator(ch))
		{
			if (*path == '.')
				path++;
			p1 = NULL;
		}
		else if (ch == '.')
			p1 = path - 1;
	}

	if (p1)
		retVal.append(p1, (int) (path - p1));

	return 0;
}

result_t path_base::dirname(const char* path, std::string& retVal)
{
	char ch;
	const char *p1 = path;
	const char *p2 = NULL;

#ifdef _WIN32
	if (path[0] != 0 && path[1] == ':')
	{
		path += 2;
		if (isSeparator(*path))
			path++;
		p2 = path;
	}
	else if (isSeparator(path[0]) && isSeparator(path[1]))
	{
		path += 2;
		while (*path && !isSeparator(*path))
			path++;

		if (*path)
		{
			path++;

			while (*path && !isSeparator(*path))
				path++;

			if (*path)
				path++;
		}

		p2 = path;
	}
#endif

	while (*path)
	{
		ch = *path++;
		if (isSeparator(ch) && *path)
			p2 = path - 1;
	}

	if (p2 == NULL)
		p2 = path;

	if (isSeparator(*p2) && p2 == p1)
		p2++;

	retVal.append(p1, (int) (p2 - p1));

	return 0;
}

inline char fixChar(char ch)
{
	return isSeparator(ch) ? PATH_CHAR : ch;
}

result_t path_base::normalize(const char* path, std::string& retVal)
{
	std::string str;
	const char *p1 = path;
	char *pstr;
	int pos = 0;
	int root = 0;

	str.resize(4096);
	pstr = &str[0];

#ifdef _WIN32
	if (p1[0] != 0 && p1[1] == ':')
	{
		pstr[pos++] = p1[0];
		pstr[pos++] = ':';
		p1 += 2;

		if (isSeparator(*p1))
		{
			pstr[pos++] = PATH_CHAR;
			p1++;
		}
	}
	else if (isSeparator(p1[0]) && isSeparator(p1[1]))
	{
		pstr[pos++] = PATH_CHAR;
		pstr[pos++] = PATH_CHAR;
		p1 += 2;

		while (*p1 && !isSeparator(*p1))
			pstr[pos++] = *p1++;

		if (*p1)
		{
			pstr[pos++] = PATH_CHAR;
			p1++;

			while (*p1 && !isSeparator(*p1))
				pstr[pos++] = *p1++;

			if (*p1)
			{
				pstr[pos++] = PATH_CHAR;
				*p1++;
			}
		}
	}
	else
#endif
	if (isSeparator(p1[0]))
	{
		pstr[pos++] = PATH_CHAR;
		p1++;
	}

	root = pos;

	while (*p1)
	{
		if (isSeparator(p1[0]))
		{
			p1++;
		}
		else if (p1[0] == '.' && (!p1[1] || isSeparator(p1[1])))
		{
			p1 += p1[1] ? 2 : 1;
		}
		else if (p1[0] == '.' && p1[1] == '.' && (!p1[2] || isSeparator(p1[2])))
		{
			if (pos > root)
			{
				if (pstr[pos - 2] == '.' && pstr[pos - 3] == '.'
						&& (pstr[pos - 4] == PATH_CHAR || root == pos - 3))
				{
					pstr[pos++] = '.';
					pstr[pos++] = '.';
					pstr[pos++] = PATH_CHAR;
				}
				else
				{
					pos--;
					while (pos > root && !isSeparator(pstr[pos - 1]))
						pos--;
				}
			}
			else if (root == 0)
			{
				pstr[pos++] = '.';
				pstr[pos++] = '.';
				pstr[pos++] = PATH_CHAR;
			}

			p1 += p1[2] ? 3 : 2;
		}
		else
		{
			while (*p1 && !isSeparator(*p1))
				pstr[pos++] = *p1++;
			if (*p1)
			{
				p1++;
				pstr[pos++] = PATH_CHAR;
			}
		}
	}

	if (pos > root && isSeparator(pstr[pos - 1]))
		pos--;

	str.resize(pos);

	retVal = str;

	return 0;
}

result_t path_base::join(const v8::Arguments& args, std::string& retVal)
{
	std::stringstream strBuffer;
	int argc = args.Length();
	int i;
	bool bRoot;

	for (i = 0; i < argc; i++)
	{
		const char *p = *v8::String::Utf8Value(args[i]);

		if (p && *p)
		{
#ifdef _WIN32
			if ((p[0] != 0 && p[1] == ':')
					|| (isSeparator(p[0]) && isSeparator(p[1])))
				bRoot = true;
			else
#endif
			if (isSeparator(p[0]))
				bRoot = true;
			else
				bRoot = false;

			if (bRoot)
				strBuffer.str("");

			strBuffer << p;
		}
		else
			strBuffer << '.';
		if (i < argc - 1)
			strBuffer << PATH_CHAR;
	}

	return normalize(strBuffer.str().c_str(), retVal);
}

}
