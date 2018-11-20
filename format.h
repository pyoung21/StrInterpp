#include <iostream>
#include <iterator>
#include <array>

#define FMT(s) [] () constexpr -> auto { \
	struct _ { \
		static constexpr decltype(auto) value() { return s; } \
		static constexpr size_t size() { return std::size(s) - 1; } \
		static constexpr char at(size_t index) { return s[index]; } \
	}; \
	return _{}; \
}()

namespace internal
{
	template<typename Fmt>
	constexpr auto count_args(Fmt fmt) -> size_t
	{
		size_t count = 0;
		for (size_t i = 0; i < fmt.size() - 1;)
		{
			if (fmt.at(i) == '{')
			{
				i++;
				if (fmt.at(i) == '}')
				{
					count++;
					i++;
				}
			}
			i++;
		}
		return count;
	}

	template<typename T>
	constexpr auto get_arg_size()
	{
		return sizeof(T);
	}

	template<typename Fmt, typename... Args>
	constexpr auto get_format_size()
	{
		return (0 + ... + get_arg_size<Args>());
	}
}

template<typename Fmt, typename... Args>
constexpr auto sformat(Fmt, Args&&... args)
{
	static_assert(internal::count_args(Fmt{}) == sizeof...(args), "fmt: Arguments don't match format.");
	std::array<char, internal::get_format_size<Fmt, Args...>()> ret{ 0 };
	constexpr auto t = internal::get_format_size<Fmt, Args...>();
	return ret;
}
