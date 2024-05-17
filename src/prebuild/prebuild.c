#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

static const char* g_supported_extensions[] = {
	"c",
	"cpp",
	"h",
	"inl",
};

#define IN_BUFFER_SIZE 1024 * 1024
#define OUT_BUFFER_SIZE 1024 * 1024

static uint64_t MurmurHash64A(const void* key, int len, uint64_t seed)
{
	const uint64_t m = 0xc6a4a7935bd1e995;
	const int r = 47;

	uint64_t h = seed ^ (len * m);

	const uint64_t* data = (const uint64_t*)key;
	const uint64_t* end = data + (len / 8);

	while (data != end)
	{
		uint64_t k = *data++;

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
			h ^= (uint64_t)data2[6] << 48;
		case 6:
			h ^= (uint64_t)data2[5] << 40;
		case 5:
			h ^= (uint64_t)data2[4] << 32;
		case 4:
			h ^= (uint64_t)data2[3] << 24;
		case 3:
			h ^= (uint64_t)data2[2] << 16;
		case 2:
			h ^= (uint64_t)data2[1] << 8;
		case 1:
			h ^= (uint64_t)data2[0];
			h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

static void preprocessFile(const char* path, char* buffer, char* out_buffer, bool force)
{
	(void)out_buffer;
	FILE* file = NULL;
	fopen_s(&file, path, "rb");
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	if (size > IN_BUFFER_SIZE)
	{
		fprintf(stderr, "Error: %s is too big for current buffer size, skipping!\n", path);
		fclose(file);
		return;
	}
	fseek(file, 0, SEEK_SET);
	fread(buffer, size, 1, file);
	fclose(file);

	if (size == 0)
	{
		return;
	}

	int string_start = -1;
	int string_len = 0;

	int sid_start = -1;
	int sid_len = 0;

	size_t write_index = 0;
	size_t last_copy_index = 0;

	const char str[] = "PAW_STR(\"";
	const int str_size = sizeof(str) - 1;

	const char sid[] = "PAW_SID(\"";
	const int sid_size = sizeof(str) - 1;

	static char int_to_str_buffer[19];

	bool changed = false;

	for (long char_index = 0; char_index < size; char_index++)
	{
		if (string_start == -1 && char_index < size - str_size)
		{
			bool found = true;
			for (int i = 0; i < str_size; i++)
			{
				if (buffer[char_index + i] != str[i])
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				string_start = char_index + str_size;
				char_index = string_start;
				continue;
			}
		}

		if (sid_start == -1 && char_index < size - str_size)
		{
			bool found = true;
			for (int i = 0; i < sid_size; i++)
			{
				if (buffer[char_index + i] != sid[i])
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				sid_start = char_index + sid_size;
				char_index = sid_start;
				continue;
			}
		}

		const char c = buffer[char_index];

		if (string_start != -1)
		{
			if (c == '"' && buffer[char_index - 1] != '\\')
			{
				string_len++;
				// printf("Found %.*s\n", string_len, buffer + string_start);

				for (; char_index < size && buffer[char_index] != ','; char_index++)
				{
				}

				for (; char_index < size && (buffer[char_index] < '0' || buffer[char_index] > '9'); char_index++)
				{
				}

				size_t num_start_char_index = char_index;

				int existing_size = 0;
				for (; char_index < size && buffer[char_index] != ')'; char_index++)
				{
					const char num_char = buffer[char_index];
					if (num_char < '0' || num_char > '9')
					{
						fprintf(
							stderr,
							"Error: PAW_STR %.*s size argument is not in correct format\n",
							string_len,
							buffer + string_start);
						return;
					}

					existing_size *= 10;
					existing_size += num_char - '0';
				}

				if (existing_size != string_len)
				{
					const size_t copy_size = (num_start_char_index - last_copy_index);

					memcpy_s(
						out_buffer + write_index, OUT_BUFFER_SIZE - write_index, buffer + last_copy_index, copy_size);
					write_index += copy_size;

					int digit_count = 0;
					for (int i = string_len; i != 0; i /= 10)
					{
						const char digit = '0' + i % 10;
						int_to_str_buffer[digit_count++] = digit;
					}

					for (int i = 0; i < digit_count; i++)
					{
						out_buffer[write_index++] = int_to_str_buffer[digit_count - i - 1];
					}

					last_copy_index = char_index;

					fprintf(stdout, "Updated %.*s\n", string_len, buffer + string_start);

					changed = true;
				}

				string_start = -1;
				string_len = 0;
			}
			else
			{

				string_len++;
			}
		}

		if (sid_start != -1)
		{
			if (c == '"' && buffer[char_index - 1] != '\\')
			{
				sid_len++;
				// printf("Found %.*s\n", sid_len, buffer + sid_start);

				for (; char_index < size && buffer[char_index] != ','; char_index++)
				{
				}

				for (; char_index < size && (buffer[char_index] < '0' || buffer[char_index] > '9'); char_index++)
				{
				}

				size_t num_start_char_index = char_index;

				uint64_t existing_hash = 0;
				for (; char_index < size && buffer[char_index] != 'u' && buffer[char_index] != ')'; char_index++)
				{
					const char num_char = buffer[char_index];
					if (num_char < '0' || num_char > '9')
					{
						fprintf(
							stderr,
							"Error: PAW_SID %.*s hash argument is not in correct format\n",
							sid_len,
							buffer + sid_start);
						return;
					}

					existing_hash *= 10;
					existing_hash += num_char - '0';
				}

				const uint64_t hash =
					MurmurHash64A((const void*)(buffer + sid_start), (int)sid_len, 0x0B7D6BC07CD461CD);

				if (existing_hash != hash || force)
				{
					const size_t copy_size = (num_start_char_index - last_copy_index);

					memcpy_s(
						out_buffer + write_index, OUT_BUFFER_SIZE - write_index, buffer + last_copy_index, copy_size);
					write_index += copy_size;

					int digit_count = 0;
					for (uint64_t i = hash; i != 0; i /= 10)
					{
						const char digit = '0' + i % 10;
						int_to_str_buffer[digit_count++] = digit;
					}

					for (int i = 0; i < digit_count; i++)
					{
						out_buffer[write_index++] = int_to_str_buffer[digit_count - i - 1];
					}

					out_buffer[write_index++] = 'u';

					last_copy_index = char_index;

					fprintf(stdout, "Updated %.*s\n", sid_len, buffer + sid_start);

					changed = true;
				}

				sid_start = -1;
				sid_len = 0;
			}
			else
			{

				sid_len++;
			}
		}
	}

	if (changed)
	{
		const size_t copy_size = size - last_copy_index;
		memcpy_s(out_buffer + write_index, OUT_BUFFER_SIZE - write_index, buffer + last_copy_index, copy_size);
		write_index += copy_size;

		FILE* write_file = NULL;
		fopen_s(&write_file, path, "wb");
		fwrite(out_buffer, write_index, 1, write_file);
		fclose(write_file);
		fprintf(stdout, "Written %s\n", path);
	}
}

static void recurseDir(const char* path, char* buffer, char* out_buffer, bool force)
{
	WIN32_FIND_DATAA find_data;

	char path_buffer[MAX_PATH];
	path_buffer[snprintf(path_buffer, sizeof(path_buffer), "%s/*", path)] = 0;

	HANDLE file_handle = FindFirstFile(path_buffer, &find_data);
	do
	{
		path_buffer[snprintf(path_buffer, sizeof(path_buffer), "%s/%s", path, find_data.cFileName)] = 0;
		if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			const int filename_len = (int)strlen(find_data.cFileName);
			int extension_index = -1;
			int extension_len = 0;
			for (int i = filename_len - 1; i > 0; i--)
			{
				if (find_data.cFileName[i] == '.')
				{
					extension_index = i + 1;
					break;
				}

				extension_len++;
			}

			if (extension_index != -1 && extension_len > 0)
			{
				const char* extension = find_data.cFileName + extension_index;
				bool supported = false;
				for (int i = 0; i < (int)(sizeof(g_supported_extensions) / sizeof(g_supported_extensions[0])); i++)
				{
					if (strncmp(g_supported_extensions[i], extension, extension_len) == 0)
					{
						supported = true;
						break;
					}
				}
				if (supported)
				{
					preprocessFile(path_buffer, buffer, out_buffer, force);
				}
			}
		}
		else
		{
			if (strncmp(find_data.cFileName, ".", 1) == 0 || strncmp(find_data.cFileName, "..", 2) == 0)
			{
				continue;
			}
			recurseDir(path_buffer, buffer, out_buffer, force);
		}

	} while (FindNextFile(file_handle, &find_data));

	FindClose(file_handle);
}

int main(int arg_count, char* args[])
{
	LARGE_INTEGER start_time;
	QueryPerformanceCounter(&start_time);

	char* buffer = (char*)malloc(IN_BUFFER_SIZE);
	char* out_buffer = (char*)malloc(OUT_BUFFER_SIZE);

	bool force = false;
	if (arg_count > 1)
	{
		const char* force_arg = "--force";
		const size_t force_len = min(strlen(args[1]), strlen(force_arg));
		force = memcmp(args[1], force_arg, force_len) == 0;
	}
	recurseDir("C:/dev/paw6/code/src/core", buffer, out_buffer, force);
	recurseDir("C:/dev/paw6/code/src/engine", buffer, out_buffer, force);
	recurseDir("C:/dev/paw6/code/src/game", buffer, out_buffer, force);
	recurseDir("C:/dev/paw6/code/src/platform", buffer, out_buffer, force);

	LARGE_INTEGER end_time;
	QueryPerformanceCounter(&end_time);

	double ticks_taken = (double)(end_time.QuadPart - start_time.QuadPart);
	LARGE_INTEGER ticks_per_second;
	QueryPerformanceFrequency(&ticks_per_second);
	double ticks_per_second_f = (double)ticks_per_second.QuadPart;
	double ms_taken = (ticks_taken / ticks_per_second_f) * 1000.0;
	printf("Prebuild took %gms\n", ms_taken);
}