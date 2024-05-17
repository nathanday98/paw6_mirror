#include <shared/shared.h>

#include <shared/optional.h>
#include <shared/string.h>
#include <shared/log.h>
#include <shared/allocator.h>
#include <shared/math.h>
#include <shared/hash.h>
#include <shared/string_id.h>

void Core::init()
{
	string_registerFormatters();
	math_registerFormatters();
	string_id_register_formatter();
}