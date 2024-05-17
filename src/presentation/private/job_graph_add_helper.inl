#include "job_graph.h"

#include <new>
#include <utility>
#include <tuple>

#include "allocators/frame_allocator.h"

template <typename T>
struct JobResourceTypeInfo<T const&>
{
	static constexpr bool value = true;
	static JobResource get(T const& ref, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(&ref);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + sizeof(T),
			.access = ResourceAccessType::ReadOnly,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<T&>
{
	static constexpr bool value = true;
	static JobResource get(T& ref, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(&ref);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + sizeof(T),
			.access = ResourceAccessType::ReadWrite,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<T const*>
{
	static constexpr bool value = true;
	static JobResource get(T const* ref, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(ref);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + sizeof(T),
			.access = ResourceAccessType::ReadOnly,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<T*>
{
	static constexpr bool value = true;
	static JobResource get(T* ref, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(ref);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + sizeof(T),
			.access = ResourceAccessType::ReadWrite,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<Slice<T>>
{
	static constexpr bool value = true;
	static JobResource get(Slice<T> const& slice, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(slice.ptr);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + slice.calc_total_size_in_bytes(),
			.access = ResourceAccessType::ReadWrite,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<Slice<T const>>
{
	static constexpr bool value = true;
	static JobResource get(Slice<T const> const& slice, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(slice.ptr);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + slice.calc_total_size_in_bytes(),
			.access = ResourceAccessType::ReadOnly,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<Slice<T>&>
{
	static constexpr bool value = true;
	static JobResource get(Slice<T> const& slice, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(slice.ptr);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + slice.calc_total_size_in_bytes(),
			.access = ResourceAccessType::ReadWrite,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<Slice<T const>&>
{
	static constexpr bool value = true;
	static JobResource get(Slice<T const> const& slice, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(slice.ptr);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + slice.calc_total_size_in_bytes(),
			.access = ResourceAccessType::ReadOnly,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<Slice<T>*>
{
	static constexpr bool value = true;
	static JobResource get(Slice<T> const* slice, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(slice->ptr);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + slice.calc_total_size_in_bytes(),
			.access = ResourceAccessType::ReadWrite,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceTypeInfo<Slice<T const>*>
{
	static constexpr bool value = true;
	static JobResource get(Slice<T const> const* slice, u32 arg_index)
	{
		u64 const range_start = reinterpret_cast<u64>(slice->ptr);
		return JobResource{
			.range_start = range_start,
			.range_end = range_start + slice.calc_total_size_in_bytes(),
			.access = ResourceAccessType::ReadOnly,
			.arg_index = arg_index,
		};
	}
};

template <typename T>
struct JobResourceDisable
{
	static constexpr void error_if_needed()
	{
	}
};

template <typename T>
struct JobResourceDisable<Slice<T>*>
{
	static constexpr void error_if_needed()
	{
		static_assert(sizeof(T) == -1, "Pointers to slices aren't allowed, please pass the slice by value");
	}
};

template <typename T>
struct JobResourceDisable<Slice<T>&>
{
	static constexpr void error_if_needed()
	{
		static_assert(sizeof(T) == -1, "References to slices aren't allowed, please pass the slice by value");
	}
};

template <typename T>
struct JobResourceDisable<Slice<T> const*>
{
	static constexpr void error_if_needed()
	{
		static_assert(sizeof(T) == -1, "Pointers to slices aren't allowed, please pass the slice by value");
	}
};

template <typename T>
struct JobResourceDisable<Slice<T> const&>
{
	static constexpr void error_if_needed()
	{
		static_assert(sizeof(T) == -1, "References to slices aren't allowed, please pass the slice by value");
	}
};

template <typename... Args>
using JobGraphFuncPointer = void (*)(JobGraph*, JobHandle, Args...);

template <typename... Args>
using JobGraphFuncArgs = std::tuple<JobGraph*, JobHandle, Args...>;

template <typename... Args>
struct WrappedJob : public JobBase
{
	WrappedJob(JobGraphFuncArgs<Args...>&& args, JobGraphFuncPointer<Args...> func)
		: args(std::move(args)), ptr(func)
	{
	}

	JobGraphFuncArgs<Args...> args;
	JobGraphFuncPointer<Args...> ptr;

	virtual void execute(JobGraph* job_graph, JobHandle job) override
	{
		std::get<0>(args) = job_graph;
		std::get<1>(args) = job;
		std::apply(ptr, args);
	}
};

template <typename... Args, typename... Inputs>
JobHandle job_graph_add_job(JobGraph* self, JobSrc&& src, UTF8StringView const& name, Slice<JobHandle const> const& dependencies, JobGraphFuncPointer<Args...> func, Inputs&&... inputs)
{
	constexpr usize resource_count = (... + [&]
									  {
			JobResourceDisable<Args>::error_if_needed();
			if constexpr (JobResourceTypeInfo<Args>::value) {
				return 1;
			}

			return 0; }());

	JobResource resources[resource_count]{};

	usize write_index = 0;
	u32 arg_index = 0;

	([&]
	 {

        using ResourceInfo = JobResourceTypeInfo<Args>;

        if constexpr(ResourceInfo::value) {
            resources[write_index++] = ResourceInfo::get(inputs, arg_index);
        } 
		arg_index++; }(),
	 ...);

	WrappedJob<Args...> job = WrappedJob<Args...>(JobGraphFuncArgs<Args...>(nullptr, g_null_job, inputs...), func);

	return job_graph_add_internal(self, std::move(src), name, &job, sizeof(job), alignof(decltype(job)), dependencies, {resources, resource_count});
}

template <typename... Args, typename... Inputs>
void job_graph_set_initial_job(JobGraph* self, JobSrc&& src, UTF8StringView const& name, JobGraphFuncPointer<Args...> func, ArenaAllocator& persistent_allocator, ArenaAllocator& debug_persistent_allocator, Inputs&&... inputs)
{
	constexpr usize resource_count = (... + [&]
									  {
			JobResourceDisable<Args>::error_if_needed();
			if constexpr (JobResourceTypeInfo<Args>::value) {
				return 1;
			}

			return 0; }());

	JobResource resources[resource_count]{};

	usize write_index = 0;
	u32 arg_index = 0;

	([&]
	 {

        using ResourceInfo = JobResourceTypeInfo<Args>;

        if constexpr(ResourceInfo::value) {
            resources[write_index++] = ResourceInfo::get(inputs, arg_index);
        } 
		arg_index++; }(),
	 ...);

	WrappedJob<Args...> job = WrappedJob<Args...>(JobGraphFuncArgs<Args...>(nullptr, g_null_job, inputs...), func);

	job_graph_set_initial_job_internal(self, std::move(src), name, &job, sizeof(job), alignof(decltype(job)), {resources, resource_count}, persistent_allocator, debug_persistent_allocator);
}