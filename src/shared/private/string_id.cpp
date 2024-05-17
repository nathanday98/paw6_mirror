#include <shared/string_id.inl>

#include <shared/string.h>

#include <vector>

struct StringIDLookupTable
{
	std::vector<u64> hashes;
	std::vector<char const*> strings;
};

static StringIDLookupTable& get_hash_lookup() {
    static StringIDLookupTable reverse_hash{};
    return reverse_hash;
}

StringIDLookupTable* StringID::debug_values = &get_hash_lookup();

void register_string_id_debug_value(StringID hash, char const* str)
{
	StringIDLookupTable& lookup_table = get_hash_lookup();

	bool found = false;
	for(size_t i = 0; i < lookup_table.hashes.size(); i++)
	{
		if(lookup_table.hashes[i] == hash.value)
		{
			found = true;
			break;
		}
	}
	
	if(!found) 
	{
		lookup_table.hashes.push_back(hash.value);
		lookup_table.strings.push_back(str);
	}
}

char const* get_string_id_debug_value(StringID hash)
{
	StringIDLookupTable& lookup_table = get_hash_lookup();
	for(size_t i = 0; i < lookup_table.hashes.size(); i++)
	{
		if(lookup_table.hashes[i] == hash.value)
		{
			return lookup_table.strings[i];
		}
	}
	return "<Unknown hash>";
}

static void format(byte* out_buffer, usize out_buffer_size_bytes, void const* data)
{
	StringID const* id = static_cast<StringID const*>(data);
	string_format(out_buffer, out_buffer_size_bytes, "%s", get_string_id_debug_value(*id));
}

void string_id_register_formatter()
{
	string_registerFormatter(PAW_STRING_ID("StringID"), &format);
}

#ifdef PAW_TESTS

#include <testing/testing.h>

#define PAW_TEST_MODULE_NAME StringID

PAW_TEST(switch)
{
	constexpr StringID a = PAW_STRING_ID("hey");
	int b = 0;
	switch (a)
	{
		case PAW_STRING_ID("hey"):
			b = 10;
			break;
	}

	PAW_TEST_EXPECT_EQUAL(int, b, 10);
}

#endif