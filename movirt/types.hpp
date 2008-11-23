
#ifndef MOVIRT_TYPES_HPP
#define MOVIRT_TYPES_HPP

#define MVT_NAMESPACE_BEGIN namespace movirt {
#define MVT_NAMESPACE_END   }

MVT_NAMESPACE_BEGIN

typedef int delta_t;
typedef unsigned char byte_t;
typedef unsigned int  word_t;
typedef class opaque_handle* opaque_t;
typedef opaque_t* ptr_t;

template<class From>
inline ptr_t to_ptr(From f) { return reinterpret_cast<ptr_t>(f); }


MVT_NAMESPACE_END

#endif//MOVIRT_TYPES_HPP
