#pragma once

#include <shared/std.h>

typedef struct UTF8StringView UTF8StringView;

struct StringIDLookupTable;

struct StringID
{
	u64 value;
	constexpr operator u64() const
	{
		return value;
	}

	static StringIDLookupTable* debug_values;

};

void string_id_register_formatter();

void register_string_id_debug_value(StringID hash, char const* str);
char const* get_string_id_debug_value(StringID hash);