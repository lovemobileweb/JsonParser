
#ifndef CONVERT_H
#define CONVERT_H

#include <windows.h>
#include <string>

namespace Chordia
{

	//-----------------------------------------------------------------------------
	// convert wide to ASCII
	inline std::string convert(const wchar_t* parg,int lcid = CP_UTF8)
	{
		std::string ret;
		size_t length = 0;
		if (parg)
		{
			length = wcslen(parg);
		}
		if (length)
		{
			// wide character string to multibyte character string 
#if (_MSC_VER >= 1400)	
			int chars = WideCharToMultiByte(lcid,0,parg,length,0,0,0,0);
			if (chars > 0)
			{
				ret.assign(chars,0);
				WideCharToMultiByte(lcid,0,parg,length,&ret[0],chars,0,0);
			}
#else		
			ret.assign(length,0);
			wcstombs(&ret[0],parg,length);
#endif		
		}
		return ret;
	}

	//-----------------------------------------------------------------------------
	// convert wide to ASCII
	inline std::string convert(const std::wstring& warg,int lcid = CP_UTF8)
	{
		return convert(warg.c_str(),lcid);
	}

	//-----------------------------------------------------------------------------
	// char to wchar_t
	inline std::wstring convert(const char* parg,int lcid = CP_UTF8)
	{
		std::wstring ret;
		size_t length = 0;

		if (parg)
		{
			length = strlen(parg);
		}
		if (length)
		{
			// multibyte to widechar
			int chars = MultiByteToWideChar(lcid,0,parg,length,0,0);
			if (chars > 0)
			{
				ret.assign(chars,0);
				MultiByteToWideChar(lcid,0,parg,length,&ret[0],chars);
			}
		}
		return ret;
	}

	//-----------------------------------------------------------------------------
	// convert ASCII to wide
	inline std::wstring convert(const std::string& arg)
	{
		return convert(arg.c_str());
	}

}

#endif

