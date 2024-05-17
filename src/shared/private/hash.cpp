#include <shared/hash.h>

#include <shared/slice.h>
#include <shared/string.h>

// Based on MurmurHash2 by Austin Appleby

//-----------------------------------------------------------------------------
// MurmurHash2, 64-bit versions, by Austin Appleby

// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment
// and endian-ness issues if used across multiple platforms.

// 64-bit hash for 64-bit platforms

static u64 MurmurHash64A(const void* key, int len, u64 seed)
{
	const u64 m = 0xc6a4a7935bd1e995;
	const int r = 47;

	u64 h = seed ^ (len * m);

	const u64* data = (const u64*)key;
	const u64* end = data + (len / 8);

	while (data != end)
	{
		u64 k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char* data2 = (const unsigned char*)data;

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

	return h;
}

u64 murmur64(const byte* data_ptr, usize data_size, u64 seed)
{
	return MurmurHash64A(data_ptr, (int)data_size, seed);
}

u64 murmur64(const UTF8StringView& str, u64 seed)
{
	return MurmurHash64A(str.ptr, (int)str.size_bytes, seed);
}