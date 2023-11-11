export module gun;

import<cstdint>;

#define PURE = NULL

export class gun {
private:
	virtual void _pad_index_0( void ) noexcept PURE;  // @index 0
	virtual void _pad_index_1( void ) noexcept PURE;  // @index 1
	virtual void _pad_index_2( void ) noexcept PURE;  // @index 2
public:
	virtual bool shoot(float fArrRay[3u]) noexcept PURE; // @index 3
private:
	virtual void _pad_index_4( void ) noexcept PURE;  // @index 4
	virtual void _pad_index_5( void ) noexcept PURE;  // @index 5
	virtual void _pad_index_6( void ) noexcept PURE;  // @index 6
public:
	virtual bool reload(const bool bUnknown = true) noexcept PURE; // @index 7
private:
	virtual void _pad_index_8( void ) noexcept PURE;  // @index 8
	virtual void _pad_index_9( void ) noexcept PURE;  // @index 9
	virtual void _pad_index_10( void ) noexcept PURE; // @index 10
	virtual void _pad_index_11( void ) noexcept PURE; // @index 11
	virtual void _pad_index_12( void ) noexcept PURE; // @index 12
	virtual void _pad_index_13( void ) noexcept PURE; // @index 13
	virtual void _pad_index_14( void ) noexcept PURE; // @index 14
	virtual void _pad_index_15( void ) noexcept PURE; // @index 15
	virtual void _pad_index_16( void ) noexcept PURE; // @index 16
	virtual void _pad_index_17( void ) noexcept PURE; // @index 17
	virtual void _pad_index_18( void ) noexcept PURE; // @index 18
	virtual void _pad_index_19( void ) noexcept PURE; // @index 19
	virtual void _pad_index_20( void ) noexcept PURE; // @index 20
	virtual void _pad_index_21( void ) noexcept PURE; // @index 21
	virtual void _pad_index_22( void ) noexcept PURE; // @index 22
};