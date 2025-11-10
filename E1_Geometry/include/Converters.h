	#pragma once
#ifndef _CONVERTERS_H_
#define _CONVERTERS_H_
//Copied from: https://stackoverflow.com/questions/77954545/converting-between-string-wstring-and-u16string
#include <Windows.h>
#include <string>
class Converters
{
	template<typename WideCharT>
	inline static std::basic_string<WideCharT> convert_UTF8_to_UTF16(const std::string& str) {
		const char* str_ptr = str.c_str();
		int str_len = static_cast<int>(str.size());
		int wstr_len = MultiByteToWideChar(CP_UTF8, 0, str_ptr, str_len, nullptr, 0);
		std::basic_string<WideCharT> wstr(wstr_len, WideCharT{});
		MultiByteToWideChar(CP_UTF8, 0, str_ptr, str_len, reinterpret_cast<wchar_t*>(wstr.data()), wstr_len);
		return wstr;
	}

	template<typename WideCharT>
	inline static std::string convert_UTF16_to_UTF8(const std::basic_string<WideCharT>& wstr) {
		const wchar_t* wstr_ptr = reinterpret_cast<const wchar_t*>(wstr.c_str());
		int wstr_len = static_cast<int>(wstr.size());
		int str_len = WideCharToMultiByte(CP_UTF8, 0, wstr_ptr, wstr_len, nullptr, 0, nullptr, nullptr);
		std::string str(str_len, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wstr_ptr, wstr_len, str.data(), str_len, nullptr, nullptr);
		return str;
	}
public:
	inline static std::wstring convert_to_wstring(const std::string& str) {
		return convert_UTF8_to_UTF16<wchar_t>(str);
	}

	inline static std::u16string convert_to_u16string(const std::string& str) {
		return convert_UTF8_to_UTF16<char16_t>(str);
	}

	inline static std::string convert_from_wstring(const std::wstring& wstr) {
		return convert_UTF16_to_UTF8(wstr);
	}

	inline static std::string convert_from_u16string(const std::u16string& ustr) {
		return convert_UTF16_to_UTF8(ustr);
	}
};
#endif

