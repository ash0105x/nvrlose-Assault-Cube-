#include"win32api.h"

export module CConsole;

import <fstream>;
import <iostream>;

import <assert.h>;

/*
*	Uncomment the following line to enable
*	the more secure functions (CConsole::print_s, ...)
*/
//#define CONSOLE_SECURE_FUNCTIONS

export enum COLORED_TEXT : WORD {
	COLORED_TEXT_RED = (FOREGROUND_RED | FOREGROUND_INTENSITY),
	COLORED_TEXT_GREEN = (FOREGROUND_GREEN | FOREGROUND_INTENSITY),
	COLORED_TEXT_BLUE = (FOREGROUND_BLUE | FOREGROUND_INTENSITY),
	COLORED_TEXT_RESET = (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
};

export class CConsole final {
public:
	// If you need to create a terminal, then call the constructor,
	// else just use the static methods
	[[nodiscard]] explicit CConsole(
		_In_z_ const decltype(__TEXT(' '))* const pTitle
	) noexcept;
	CConsole(const CConsole&) = delete;
	CConsole(CConsole&&) = delete;
public:
	~CConsole(void) noexcept;
public:
	CConsole& operator=(const CConsole&) = delete;
	CConsole& operator=(CConsole&&) = delete;
public:
	void Release(void) noexcept;

	[[nodiscard]]
	_Check_return_
		_Success_(return == true)
		const bool& consoleCreated(void) const noexcept;
public:
	[[nodiscard]]
	_Check_return_
		_Success_(return == true)
		bool isReadable(void) const noexcept;

	[[nodiscard]]
	_Check_return_
		_Success_(return == true)
		bool isWritable(void) const noexcept;
public:
	static void resetInputBuffer(void) noexcept;
public:
	// Just like Python. Raises an exception if the user provides invalid input
	template<typename T, typename ... U>
	[[nodiscard]]
	_Check_return_
		constexpr static T input(
			_In_z_ _Printf_format_string_ const char* const format,
			_In_opt_ U ... args
		)
	{
		assert(format);

		CConsole::print<decltype(args)...>(format, args...);

		std::cin.exceptions(std::ios_base::failbit);

		T returnValue{ };
		std::cin >> returnValue;

		return returnValue;
	}

#ifdef CONSOLE_SECURE_FUNCTIONS
	template<typename T, const size_t t_BufferSize, typename ... U>
	[[nodiscard]]
	_Check_return_
		constexpr static T input_s(
			_In_z_ _Printf_format_string_ const char(&format)[t_BufferSize],
			_In_opt_ U ... args
		)
	{
		static_assert(t_BufferSize > NULL, "Format buffer size must be greater than 0");

		CConsole::print_s<t_BufferSize, decltype(args)...>(format, args...);

		std::cin.exceptions(std::ios_base::failbit);

		T returnValue{ };
		std::cin >> returnValue;

		return returnValue;
	}
#endif // CONSOLE_SECURE_FUNCTIONS
public:
	template<typename ... T>
	_Check_return_opt_
		constexpr static std::int32_t read(
			_In_z_ _Scanf_format_string_ const char* const format,
			_In_opt_ T* const ... args
		) noexcept
	{
		assert(format);

		return std::scanf(format, args...);
	}

#ifdef CONSOLE_SECURE_FUNCTIONS
	template<const std::size_t t_BufferSize, typename ... T>
	_Check_return_opt_
		constexpr static std::int32_t read_s(
			_In_z_ _Scanf_s_format_string_ const char(&format)[t_BufferSize],
			_In_opt_ T* const ... args
		) noexcept
	{
		static_assert(t_BufferSize > NULL, "Format buffer size must be greater than 0");

		return scanf_s(format, args...);
	}
#endif // CONSOLE_SECURE_FUNCTIONS
public:
	template<typename ... T>
	_Check_return_opt_
		constexpr static std::int32_t print(
			_In_z_ _Printf_format_string_ const char* const format,
			_In_opt_ const T ... args
		) noexcept
	{
		assert(format);

		return std::printf(format, args...);
	}

#ifdef CONSOLE_SECURE_FUNCTIONS
	template<const std::size_t t_BufferSize, typename ... T>
	_Check_return_opt_
		constexpr static std::int32_t print_s(
			_In_z_ _Printf_format_string_ const char(&format)[t_BufferSize],
			_In_opt_ const T ... args
		) noexcept
	{
		static_assert(t_BufferSize, "Format buffer size must be greater than 0");

		return printf_s(format, args...);
	}
#endif // CONSOLE_SECURE_FUNCTIONS
public:
	template<const COLORED_TEXT t_Color, typename ... T> constexpr void printColored(
		_In_z_ _Printf_format_string_ const char* const format,
		_In_opt_ const T ... args
	) noexcept
	{
		assert(format);

		const HANDLE& hRefConsole = this->m_hConsole;

		SetConsoleTextAttribute(hRefConsole, t_Color);

		CConsole::print<decltype(args)...>(format, args...);

		SetConsoleTextAttribute(hRefConsole, COLORED_TEXT::COLORED_TEXT_RESET);
	}

#ifdef CONSOLE_SECURE_FUNCTIONS
	template<const COLORED_TEXT t_Color, const std::size_t t_BufferSize, typename ... T> constexpr void printColored_s(
		_In_z_ _Printf_format_string_ const char(&format)[t_BufferSize],
		_In_opt_ const T ... args
	) noexcept
	{
		static_assert(t_BufferSize, "Format buffer size must be greater than 0");

		const HANDLE& hRefConsole = this->m_hConsole;

		SetConsoleTextAttribute(hRefConsole, t_Color);

		CConsole::print_s<t_BufferSize, decltype(args)...>(format, args...);

		SetConsoleTextAttribute(hRefConsole, COLORED_TEXT::COLORED_TEXT_RESET);
	}
#endif // CONSOLE_SECURE_FUNCTIONS
private:
	FILE* m_pOutFile = nullptr;
	FILE* m_pInFile = nullptr;
	bool m_bConsoleAllocated = false;
	HANDLE m_hConsole = nullptr;
};