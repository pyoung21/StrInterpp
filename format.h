// Fmt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>

#define FMT(s) [] () constexpr -> auto { \
	struct _ { \
		static constexpr decltype(auto) value() { return s; } \
		static constexpr size_t size() { return std::size(s) - 1; } \
		static constexpr char at(size_t index) { return s[index]; } \
	}; \
	return _{}; \
}()

auto _fmt_ = FMT("");
using PYSTRING_Type = decltype(_fmt_);

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
constexpr bool is_string(T t)
{
	return false;
}

template<>
constexpr bool is_string(const char*)
{
	return true;
}

template<size_t N>
constexpr bool is_string(const char(&)[N])
{
	return true;
}

template<typename T>
constexpr auto get_arg_size()
{
	if constexpr (is_string(T{}))
		return sizeof(T) - 1;
	return sizeof(T);
}

template<typename Fmt, typename... Args>
constexpr auto get_format_size()
{
	return (0 + ... + get_arg_size<Args>()) + (Fmt::size() - (sizeof...(Args) * 2));
}

template<typename T>
constexpr int formatArg(char *data, T arg, size_t total)
{
	for (int i = 0; i < total; i++)
	{
		if (data[i] == 0)
		{
			for (int i = 0; i < sizeof(T); i++)
				data[i] = ((arg >> (8 * i)) & 0xff);
		}
	}
	return 0;
}

template<typename Fmt>
constexpr int writeFormat(char *data, Fmt fmt)
{
	for (int i = 0; i < fmt.size();)
	{
		if (fmt.at(i) == '{' || fmt.at(i) == '}')
			data[i] = 0;
		else
			data[i] = fmt.at(i);
		i++;
	}
	return 0;
}

template<typename Fmt, typename... Args>
constexpr auto myFormat(Fmt, Args&&... args)
{
	static_assert(count_args(Fmt{}) == sizeof...(args), "fmt: Arguments don't match format.");

	std::array<char, get_format_size<Fmt, Args...>()> data{ 0 };
	auto *pData = data.data();

	int l[] = { writeFormat(pData, Fmt{}), formatArg(pData, args, data.size())... };

	return data;
}

#define $(fmt, ...) (myFormat(FMT(fmt), __VA_ARGS__))

int main()
{
	//constexpr auto t = is_pystring<decltype(_fmt_)>::value;
	constexpr auto str = $("my format {} {} {}", 1, 2, 3);
}
