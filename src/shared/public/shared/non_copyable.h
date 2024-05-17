#pragma once

struct NonCopyable
{
	NonCopyable() = default;

	NonCopyable(NonCopyable const&) = delete;
	NonCopyable& operator=(NonCopyable const&) = delete;

	/*NonCopyable(NonCopyable&&) = default;
	NonCopyable& operator=(NonCopyable&&) = default;*/
};
