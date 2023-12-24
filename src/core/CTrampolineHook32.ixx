export module CTrampolineHook32;

import<cstdint>;
import<sal.h>;

import IHook;

export class CTrampolineHook32 final : public IHook {
public:
	[[nodiscard]] explicit CTrampolineHook32(
		[[maybe_unused]] _In_ std::nullptr_t = nullptr
	) noexcept;
	[[nodiscard]] explicit CTrampolineHook32(
		_In_ void* const vpHookAddress,
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
	virtual ~CTrampolineHook32( void ) noexcept override;
public:
	[[nodiscard]]
	_Ret_maybenull_
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const attach(
		_In_ const void* const vpNewFunction
	) noexcept override;

	_Success_(return == true)
	virtual bool detach( void ) noexcept override;
private:
	std::uint8_t* m_bypHookAddress = nullptr;
	size_t m_HookLength = NULL;
	std::uint8_t* m_bypGateway = nullptr;
};