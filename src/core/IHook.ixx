export module IHook;

import<sal.h>;

#define PURE = 0

export class IHook {
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const operator()(
		_In_ const void* const vpNewFunction
	) noexcept PURE;

	_Success_(return == true)
	virtual bool operator~(void) noexcept PURE;
public:
	[[nodiscard]]
	_Check_return_
	_Success_(return != nullptr)
	virtual const void* const attach(
		_In_ const void* const vpNewFunction
	) noexcept PURE;

	_Success_(return == true)
	virtual bool detach(void) noexcept PURE;
};