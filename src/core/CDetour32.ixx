export module CDetour32;

import <cstdint>;

import IHook;

export class CDetour32 final : public IHook {
public:
	[[nodiscard]] explicit CDetour32(
		[[maybe_unused]] _In_ std::nullptr_t = nullptr
	) noexcept;
	[[nodiscard]] explicit CDetour32(
		_In_ void* const vpHookAddress,
		_In_ const size_t hookLength
	) noexcept;
	CDetour32(
		const CDetour32&
	) = delete;
	CDetour32(
		_Inout_ CDetour32&&
	) noexcept;
public:
	CDetour32& operator=(
		const CDetour32&
	) = delete;
	CDetour32& operator=(
		_Inout_ CDetour32&&
	) noexcept;
public:
	virtual ~CDetour32(void) noexcept;
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const operator()(
		_In_ const void* const vpNewFunction
	) noexcept override;

	_Success_(return == true)
	virtual bool operator~(void) noexcept override;
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const attach(
		_In_ const void* const vpNewFunction
	) noexcept override;

	_Success_(return == true)
	virtual bool detach(void) noexcept override;
public:
	std::uint8_t* m_bypHookAddress = nullptr;
	size_t m_HookLength = NULL;
	std::uint8_t* m_byArrStolenBytes = nullptr;
};