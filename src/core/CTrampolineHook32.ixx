export module CTrampolineHook32;

import <cstdint>;
import <sal.h>;

typedef unsigned char BYTE;
static_assert(sizeof(BYTE) == 1u);

export class CTrampolineHook32 final {
public:
	[[nodiscard]] explicit CTrampolineHook32(
		_In_ void* vpHookAddress,
		_In_ const size_t hookLength
	) noexcept;
	CTrampolineHook32(
		const CTrampolineHook32&
	) = delete;
	CTrampolineHook32(
		_Inout_ CTrampolineHook32&&
	) noexcept;
public:
	CTrampolineHook32& operator=(
		const CTrampolineHook32&
	) = delete;
	CTrampolineHook32& operator=(
		_Inout_ CTrampolineHook32&&
	) noexcept;
public:
	~CTrampolineHook32(void) noexcept;
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return == true)
	const void* const operator()(
		_In_ const void* const vpNewFunction
	) noexcept;

	_Success_(return == true)
	bool operator~(void) noexcept;
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return == true)
	const void* const attach(
		_In_ const void* const vpNewFunction
	) noexcept;

	_Success_(return == true)
	bool detach(void) noexcept;
private:
	[[nodiscard]]
	_Check_return_
	_Success_(return == true)
	bool jumpFromHookAddressToNewFunction(
		_In_ const void* const vpNewFunction
	) noexcept;
private:
	BYTE* m_bpHookAddress = nullptr;
	size_t m_HookLength = NULL;
	BYTE* m_bpGateway = nullptr;
};