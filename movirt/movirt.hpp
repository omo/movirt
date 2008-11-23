
#ifndef MOVIRT_HPP
#define MOVIRT_HPP

#include <typeinfo>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <map>
#include <movirt/types.hpp>
#include <movirt/untypes.hpp>
#include <movirt/mem_fn_traits.hpp>
#include <movirt/vtable.hpp>
#include <movirt/faker.hpp>

MVT_NAMESPACE_BEGIN

enum { argument_size_limit = 1024 };

MVT_NAMESPACE_END

/*
 * to keep away original caller argument from local variable,
 * replacer should put this macro at the top of the statements.
 */
#define FV_PROLOGUE_GUARD()																\
	char _favta_prologue_guard[movirt::argument_size_limit]

#endif//MOVIRT_HPP

/* -*-
   Local Variables:
   mode: c++
   c-tab-always-indent: t
   c-indent-level: 2
   c-basic-offset: 2
	 tab-width:2
   End:
   -*- */
