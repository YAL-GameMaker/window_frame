// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#include <stdint.h>

#ifdef _WINDOWS
	#include "targetver.h"
	
	#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
#endif

#if defined(WIN32)
#define dllx extern "C" __declspec(dllexport)
#elif defined(GNUC)
#define dllx extern "C" __attribute__ ((visibility("default"))) 
#else
#define dllx extern "C"
#endif

#define _trace // requires user32.lib;Kernel32.lib

#ifdef _trace
#ifdef _WINDOWS
void trace(const char* format, ...);
void tracew(const wchar_t* format, ...);
#else
#define trace(...) { printf("[window_frame:%d] ", __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); }
#endif
#endif
void trace_winmsg(const char* name, UINT msg, WPARAM wParam, LPARAM lParam);

void yal_memset(void* at, int fill, size_t len);

wchar_t* yal_strcpy(wchar_t* str, const wchar_t* val, size_t size);
template<size_t size> wchar_t* yal_strcpy(wchar_t(&str)[size], const wchar_t* val) {
	return yal_strcpy(str, val, size);
}

wchar_t* yal_strcat(wchar_t* str, const wchar_t* val, size_t size);
template<size_t size> wchar_t* yal_strcat(wchar_t(&str)[size], const wchar_t* val) {
	return yal_strcat(str, val, size);
}

void* yal_alloc(size_t bytes);
template<typename T> T* yal_alloc_arr(size_t count = 1) {
	return (T*)yal_alloc(sizeof(T) * count);
}
void* yal_realloc(void* thing, size_t bytes);
template<typename T> T* yal_realloc_arr(T* arr, size_t count) {
	return (T*)yal_realloc(arr, sizeof(T) * count);
}
bool yal_free(void* thing);

template<typename T> struct yal_set {
private:
	T* _arr;
	size_t _length;
	size_t _capacity;
public:
	yal_set() {}
	yal_set(size_t capacity) { init(capacity); }
	void init(size_t capacity = 4) {
		_capacity = capacity;
		_length = 0;
		_arr = yal_alloc_arr<T>(_capacity);
	}

	static const size_t npos = MAXSIZE_T;
	size_t find(T val) {
		for (size_t i = 0; i < _length; i++) {
			if (_arr[i] == val) return i;
		}
		return npos;
	}
	inline bool contains(T val) {
		return find(val) != npos;
	}

	bool add(T val) {
		if (find(val) != npos) return false;
		if (_length >= _capacity) {
			_capacity *= 2;
			_arr = yal_realloc_arr(_arr, _capacity);
		}
		_arr[_length++] = val;
		return true;
	}
	bool remove(T val) {
		auto i = find(val);
		if (i == npos) return false;
		_length -= 1;
		for (; i < _length; i++) _arr[i] = _arr[i + 1];
		return true;
	}
	bool set(T val, bool on) {
		if (on) return add(val); else return remove(val);
	}
};

#include "gml_ext.h"

// TODO: reference additional headers your program requires here
