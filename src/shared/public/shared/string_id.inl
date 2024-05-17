#pragma once

#include <shared/string_id.h>

#include <shared/std.h>
#include <shared/string.h>

// This is mumurhash2, but customized a bit so it works at compile time
// I would prefer for this to not be in a header, but it needs to be for
// the consteval to work
inline constexpr StringID hashCStringToStringID(const char* str, usize len)
{
	// Based on mumurhash - see hash.cpp
	const u64 m = 0xc6a4a7935bd1e995;
	const int r = 47;

	u64 h = 0x0B7D6BC07CD461CD ^ (len * m);

	const char* data = str;
	const char* end = data + (len / 8) * 8;

	while (data != end)
	{

		u64 k = 0;
		k |= data[7];
		k <<= 8;
		k |= data[6];
		k <<= 8;
		k |= data[5];
		k <<= 8;
		k |= data[4];
		k <<= 8;
		k |= data[3];
		k <<= 8;
		k |= data[2];
		k <<= 8;
		k |= data[1];
		k <<= 8;
		k |= data[0];

		data += 8;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const char* data2 = (const char*)data;

	switch (len & 7)
	{
		case 7:
			h ^= (u64)data2[6] << 48;
		case 6:
			h ^= (u64)data2[5] << 40;
		case 5:
			h ^= (u64)data2[4] << 32;
		case 4:
			h ^= (u64)data2[3] << 24;
		case 3:
			h ^= (u64)data2[2] << 16;
		case 2:
			h ^= (u64)data2[1] << 8;
		case 1:
			h ^= (u64)data2[0];
			h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;
	return {
		.value = h,
	};
}

consteval StringID operator""_sid(const char* str, usize len)
{
	return hashCStringToStringID(str, len);
}

inline StringID hashUTF8StringViewToStringID(const UTF8StringView& view)
{
	return hashCStringToStringID(view.ptr, view.size_bytes);
}

template<size_t N>
struct StringLiteral {
    consteval StringLiteral(const char (&str)[N]) {
        for(int i = 0; i < N; i++) {
            value[i] = str[i];
        }
    }
    
    char value[N];
};

template<StringID i, StringLiteral str>
struct StaticRegister {
    StaticRegister() {
        register_string_id_debug_value(i, str.value);
    } 
};

template<StringID i, StringLiteral str>
struct RegisterWrapper {
    inline static StaticRegister<i, str> thing{};
};

// Wrap in a function so it becomes an expression and can be used with the comma operator in a macro
template<StringID i, StringLiteral str>
constexpr void register_wrapper() {
    RegisterWrapper<i, str> a{};
	// This seems to be required for clang, but not msvc. It causes a compile error when using constexpr ids on msvc
   // (void)a.thing;
}

#define PAW_STRING_ID(str) (register_wrapper<PAW_CONCAT(str,_sid), str>(), PAW_CONCAT(str,_sid))