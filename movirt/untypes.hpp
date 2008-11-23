
#ifndef MOVIRT_UNTYPES_HPP
#define MOVIRT_UNTYPES_HPP

#include <cstddef>
#include <cassert>
#include <movirt/types.hpp>

MVT_NAMESPACE_BEGIN

template<class T>
inline T* advance_bytes(T* ptr, delta_t d)
{
	return reinterpret_cast<T*>(reinterpret_cast<byte_t*>(ptr) + d);
}

template<class T>
inline const T* advance_bytes(const T* ptr, delta_t d)
{
	return reinterpret_cast<const T*>(reinterpret_cast<const byte_t*>(ptr) + d);
}

struct offset_and_delta_t 
{
	size_t  m_raw_offset;
	delta_t m_delta;

	bool   virtual_p() { return (m_raw_offset & (0x00000001)); }
	size_t offset() const { return (m_raw_offset & ~(0x00000001)); }
	delta_t delta() const { return m_delta; }
};

template<class MemFn>
inline offset_and_delta_t to_offset_and_delta(MemFn fp)
{
	union {
		offset_and_delta_t u_od;
		MemFn  u_fp;
	} u;

	u.u_fp = fp;

	// currently we don't know hwere delta goes non-zero...
	assert(0 == u.u_od.delta()); 

	/*
	 * gcc raises least significant bit. so we fall it.
	 * but i don't know whether it is OK...
	 */
	return u.u_od;
}

template<class MemFn>
inline int to_offset(MemFn fp)
{
	offset_and_delta_t od = to_offset_and_delta(fp);
	assert(od.virtual_p());
	return static_cast<int>(od.offset());
}

template<class To, class From>
inline int vptr_offset_of(const From* from)
{
	const To* to = from;
	return reinterpret_cast<const byte_t*>(to) - reinterpret_cast<const byte_t*>(from);
}

MVT_NAMESPACE_END

#endif//MOVIRT_UNTYPES_HPP

/* -*-
   Local Variables:
   mode: c++
   c-tab-always-indent: t
   c-indent-level: 2
   c-basic-offset: 2
	 tab-width:2
   End:
   -*- */
