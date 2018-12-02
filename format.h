#include <array>

#define FMT(s) [] () constexpr -> auto { \
	struct _ { \
		static constexpr decltype(auto) value() { return s; } \
		static constexpr size_t size() { return std::size(s) - 1; } \
		static constexpr char at(size_t index) { return s[index]; } \
	}; \
	return _{}; \
}()

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

template<size_t N>
constexpr bool is_string(const char(&)[N])
{
	return true;
}

template<typename T>
constexpr auto get_arg_size(T arg)
{
	if constexpr (is_string(arg))
		return sizeof(arg) - 1;
	return get_represented_size(arg);
}

template<typename Fmt, size_t N>
constexpr auto get_format_size()
{
	return (Fmt::size() - (N * 2));
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

template<typename T>
constexpr size_t get_represented_size(T arg)
{
	int i = 0;
	bool sign = arg < 0;
	for (; arg != 0; arg /= 10, i++) {}
	return sign ? i + 1 : i;
}

constexpr size_t get_represented_size(char arg)
{
	return 1;
}

constexpr size_t get_represented_size(double arg)
{
	return get_represented_size(static_cast<int64_t>(arg)) + 0x10;
}

template<typename... Args>
constexpr size_t get_args_size(Args... args)
{
	return (0 + ... + get_arg_size(args));
}

template<typename ArgFmt, typename Fmt, typename... Args>
constexpr auto myFormat(ArgFmt, Fmt, Args&&... args)
{
	static_assert(count_args(Fmt{}) == sizeof...(args), "fmt: Arguments don't match format.");

	std::array<char, Fmt::size() + ArgFmt::size() - (sizeof...(args)*2) + 1> data{ 0 };
	auto *pData = data.data();

	//int l[] = { writeFormat(pData, Fmt{}), formatArg(pData, args, data.size())... };

	return data;
}

#define ARGFMT(str) [] () constexpr -> auto { \
	struct _ { \
		static constexpr size_t size() { \
			size_t ret = 0; \
			for (int i = 0; str[i] != 0; i++) { \
				if (str[i] == '"') { \
					i++; \
					while (str[i] != '\"' && i++) { ret++; } \
				} \
				else if (str[i] != ',' && str[i] != ' ') { ret++; } \
			} \
			return ret; \
		} \
	}; \
	return _{}; \
}()

#define $(fmt,...) (myFormat(ARGFMT(#__VA_ARGS__), FMT(fmt), __VA_ARGS__))

int main()
{
	constexpr auto str = $("abcdefg {} {} {}", 1, 2, "abcdef");
}
