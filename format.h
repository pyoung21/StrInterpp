#include <iterator>

#define FMT(s) [] ()  { \
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
	constexpr size_t count_args(Fmt fmt)
	{
		size_t count = 0;
		for (size_t i = 0; i < fmt.size() - 1; i += 2)
		{
			if (fmt.at(i) == '{')
			{
				if (fmt.at(i + 1) == '}')
					count++;
			}
		}
		return count;
	}
}

template<typename Fmt, typename... Args>
constexpr decltype(auto) sformat(Fmt, Args&&... args)
{
	static_assert(internal::count_args(Fmt{}) == sizeof...(args), "fmt: Arguments don't match format.");
	return 1;
}
