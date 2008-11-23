


#include <movirt/types.hpp>

MVT_NAMESPACE_BEGIN

template< typename T >
struct mem_fn_traits_t {};


template< typename R, typename T 
           >
struct mem_fn_traits_t<R (T::*)()>
{
  enum { arity = 0 };
  typedef R fn_type;
  typedef T receiver_type;
};

template< typename R, typename T ,
          typename A1 >
struct mem_fn_traits_t<R (T::*)(A1)>
{
  enum { arity = 1 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
};

template< typename R, typename T ,
          typename A1, typename A2 >
struct mem_fn_traits_t<R (T::*)(A1, A2)>
{
  enum { arity = 2 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
};

template< typename R, typename T ,
          typename A1, typename A2, typename A3 >
struct mem_fn_traits_t<R (T::*)(A1, A2, A3)>
{
  enum { arity = 3 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
  typedef A3 arg3_type; 
};

template< typename R, typename T ,
          typename A1, typename A2, typename A3, typename A4 >
struct mem_fn_traits_t<R (T::*)(A1, A2, A3, A4)>
{
  enum { arity = 4 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
  typedef A3 arg3_type; 
  typedef A4 arg4_type; 
};

template< typename R, typename T ,
          typename A1, typename A2, typename A3, typename A4, typename A5 >
struct mem_fn_traits_t<R (T::*)(A1, A2, A3, A4, A5)>
{
  enum { arity = 5 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
  typedef A3 arg3_type; 
  typedef A4 arg4_type; 
  typedef A5 arg5_type; 
};

template< typename R, typename T ,
          typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct mem_fn_traits_t<R (T::*)(A1, A2, A3, A4, A5, A6)>
{
  enum { arity = 6 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
  typedef A3 arg3_type; 
  typedef A4 arg4_type; 
  typedef A5 arg5_type; 
  typedef A6 arg6_type; 
};

template< typename R, typename T ,
          typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct mem_fn_traits_t<R (T::*)(A1, A2, A3, A4, A5, A6, A7)>
{
  enum { arity = 7 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
  typedef A3 arg3_type; 
  typedef A4 arg4_type; 
  typedef A5 arg5_type; 
  typedef A6 arg6_type; 
  typedef A7 arg7_type; 
};

template< typename R, typename T ,
          typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct mem_fn_traits_t<R (T::*)(A1, A2, A3, A4, A5, A6, A7, A8)>
{
  enum { arity = 8 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
  typedef A3 arg3_type; 
  typedef A4 arg4_type; 
  typedef A5 arg5_type; 
  typedef A6 arg6_type; 
  typedef A7 arg7_type; 
  typedef A8 arg8_type; 
};

template< typename R, typename T ,
          typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct mem_fn_traits_t<R (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
  enum { arity = 9 };
  typedef R fn_type;
  typedef T receiver_type;
  typedef A1 arg1_type; 
  typedef A2 arg2_type; 
  typedef A3 arg3_type; 
  typedef A4 arg4_type; 
  typedef A5 arg5_type; 
  typedef A6 arg6_type; 
  typedef A7 arg7_type; 
  typedef A8 arg8_type; 
  typedef A9 arg9_type; 
};


MVT_NAMESPACE_END
