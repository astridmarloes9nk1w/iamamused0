/*
 * HttpFileHandler.cpp
 *
 *  Created on: Aug 27, 2012
 *      Author: lion
 */

#include "HttpFileHandler.h"
#include "HttpRequest.h"
#include "ifs/io.h"
#include "ifs/path.h"

namespace fibjs
{

static const char* s_defType[2] =
{ NULL, "application/octet-stream" };

static const char* s_mimeTypes[][2] =
{
{ "ai", "application/postscript" },
{ "asc", "text/plain" },
{ "au", "audio/basic" },
{ "avi", "video/x-msvideo" },
{ "bmp", "image/bmp" },
{ "cdf", "application/x-netcdf" },
{ "cpio", "application/x-cpio" },
{ "css", "text/css" },
{ "doc", "application/msword" },
{ "eps", "application/postscript" },
{ "etx", "text/x-setext" },
{ "gif", "image/gif" },
{ "htm", "text/html" },
{ "html", "text/html" },
{ "jpe", "image/jpeg" },
{ "jpeg", "image/jpeg" },
{ "jpg", "image/jpeg" },
{ "js", "application/x-javascript" },
{ "json", "application/json" },
{ "latex", "application/x-latex" },
{ "m3u", "audio/x-mpegurl" },
{ "man", "application/x-troff-man" },
{ "mid", "audio/midi" },
{ "midi", "audio/midi" },
{ "mov", "video/quicktime" },
{ "movie", "video/x-sgi-movie" },
{ "mp2", "audio/mpeg" },
{ "mp3", "audio/mpeg" },
{ "mp4", "video/mpeg" },
{ "mpe", "video/mpeg" },
{ "mpeg", "video/mpeg" },
{ "mpg", "video/mpeg" },
{ "mpga", "audio/mpeg" },
{ "pdf", "application/pdf" },
{ "png", "image/png" },
{ "ppt", "application/vnd.ms-powerpoint" },
{ "ps", "application/postscript" },
{ "qt", "video/quicktime" },
{ "ra", "audio/x-realaudio" },
{ "ram", "audio/x-pn-realaudio" },
{ "rar", "application/rar" },
{ "rgb", "image/x-rgb" },
{ "rm", "audio/x-pn-realaudio" },
{ "rtf", "text/rtf" },
{ "rtx", "text/richtext" },
{ "sgm", "text/sgml" },
{ "sgml", "text/sgml" },
{ "src", "application/x-wais-source" },
{ "svg", "image/svg+xml" },
{ "swf", "application/x-shockwave-flash" },
{ "tar", "application/x-tar" },
{ "tcl", "application/x-tcl" },
{ "tex", "application/x-tex" },
{ "texi", "application/x-texinfo" },
{ "texinfo", "application/x-texinfo" },
{ "tif", "image/tiff" },
{ "tiff", "image/tiff" },
{ "txt", "text/plain" },
{ "wav", "audio/x-wav" },
{ "wbmp", "image/vnd.wap.wbmp" },
{ "wbxml", "application/vnd.wap.wbxml" },
{ "wml", "text/vnd.wap.wml" },
{ "wmlc", "application/vnd.wap.wmlc" },
{ "wmls", "text/vnd.wap.wmlscript" },
{ "wmlsc", "application/vnd.wap.wmlscriptc" },
{ "xbm", "image/x-xbitmap" },
{ "xht", "application/xhtml+xml" },
{ "xhtml", "application/xhtml+xml" },
{ "xls", "application/vnd.ms-excel" },
{ "xml", "text/xml" },
{ "xpm", "image/x-xpixmap" },
{ "xsl", "text/xml" },
{ "xwd", "image/x-xwindowdump" },
{ "xyz", "chemical/x-xyz" },
{ "zip", "application/zip" } };

static int mt_cmp(const void *p, const void *q)
{
	return qstricmp(*(const char**) p, *(const char**) q);
}

result_t HttpFileHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(HttpFileHandler* pThis, HttpRequest_base* req,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_req(req)
		{
			req->get_response(m_rep);
			set(start);
		}

		static int start(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			std::string value;

			pThis->m_req->get_value(value);
			if (value.length() > 0 && isPathSlash(value[value.length() - 1]))
				value.append("index.html", 10);
			path_base::normalize((pThis->m_pThis->m_root + value).c_str(),
					pThis->m_path);

			pThis->set(open);
			return io_base::open(pThis->m_path.c_str(), "r", pThis->m_file,
					pThis);
		}

		static int open(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			std::string ext;

			path_base::extname(pThis->m_path.c_str(), ext);

			if (ext.length() > 0)
			{
				const char* pKey = ext.c_str() + 1;
				const char** pMimeType = (const char**) bsearch(&pKey,
						&s_mimeTypes, sizeof(s_mimeTypes) / sizeof(s_defType),
						sizeof(s_defType), mt_cmp);

				if (!pMimeType)
					pMimeType = s_defType;

				pThis->m_rep->addHeader("Content-Type", pMimeType[1]);
			}

			pThis->set(stat);
			return pThis->m_file->stat(pThis->m_stat, pThis);
		}

		static int stat(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			date_t d, d1;
			std::string str;
			Variant v;

			pThis->m_stat->get_mtime(d);

			if (pThis->m_req->firstHeader("If-Modified-Since",
					v) != CALL_RETURN_NULL)
			{
				str = v.string();
				d1.parse(str.c_str(), (int) str.length());

				if (abs(d.diff(d1)) < 1000)
				{
					pThis->m_rep->set_status(304);
					return pThis->done(CALL_RETURN_NULL);
				}
			}

			d.toString(str);

			pThis->m_rep->addHeader("Last-Modified", str);
			pThis->m_rep->set_body(pThis->m_file);

			return pThis->done(CALL_RETURN_NULL);
		}

		virtual int error(int v)
		{
			m_rep->set_status(404);
			return done(CALL_RETURN_NULL);
		}

	private:
		obj_ptr<HttpFileHandler> m_pThis;
		obj_ptr<HttpRequest_base> m_req;
		obj_ptr<HttpResponse_base> m_rep;
		obj_ptr<File_base> m_file;
		obj_ptr<Stat_base> m_stat;
		std::string m_path;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<HttpRequest_base> req = HttpRequest_base::getInstance(v);

	if (req == NULL)
		return CALL_E_BADVARTYPE;

	return (new asyncInvoke(this, req, ac))->post(0);
}

} /* namespace fibjs */
