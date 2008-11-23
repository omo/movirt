
#ifndef MOVIRT_FAKER_HPP
#define MOVIRT_FAKER_HPP

#include <cstddef>
#include <cassert>
#include <movirt/types.hpp>
#include <movirt/untypes.hpp>

MVT_NAMESPACE_BEGIN

template<class T, class Ticket=void*, class Base=bulk_t>
class faker_t
{
public:
  typedef faker_t<T, Ticket, Base> self_type;
  typedef T value_type;
  typedef Base base_type;
  typedef Ticket ticket_type;
  typedef std::map<int, vtable_pair_t*> vtable_map_type;

  explicit faker_t(const ticket_type& ti=ticket_type(), base_type* base=0)
	: m_instance(base ? base : new base_type()), 
	  m_owns_instance(0 == base),
	  m_faking(0), m_ticket(ti)
  {}

  ~faker_t()
  {
	if (m_faking) {
	  unfake();
	}

	for (vtable_map_type::iterator i=m_vtables.begin();
		 i != m_vtables.end(); ++i) {
	  delete i->second;
	  i->second = 0; // for safe
	}

	if (m_owns_instance) {
	  delete m_instance;
	}
  }

  void fake()
  {
	assert (!m_faking);

	ptr_t** vptrs = instance_as_vptr();
	for (vtable_map_type::const_iterator i=m_vtables.begin();
		 i != m_vtables.end(); ++i) {
	  *advance_bytes(vptrs, i->first) = i->second->fake().body();
	}

	// XXX: eliminate reinterpret_cast if base_type is subclass of value_type
	m_faking = reinterpret_cast<value_type*>(m_instance);
  }

  void unfake()
  {
	assert (m_faking);
	m_faking = 0;

	ptr_t** vptrs = instance_as_vptr();
	for (vtable_map_type::const_iterator i=m_vtables.begin();
		 i != m_vtables.end(); ++i) {
	  *advance_bytes(vptrs, i->first) = i->second->original().body();
	}
  }

  template<typename MemFn>
  void originate(MemFn memfn)
  {
	assert(!m_faking); // currently vtable should be immutable on fakeing
	vtable_pair_t* vp = vtable_for<typename mem_fn_traits_t<MemFn>::receiver_type>();
	vp->originate(to_offset(memfn)/sizeof(ptr_t));
  }

  template<typename MemFn, typename FakeFn>
  void replace_with_fn(MemFn memfn, FakeFn fakefn)
  {
	assert(!m_faking); // currently vtable should be immutable on fakeing
	vtable_pair_t* vp = vtable_for<typename mem_fn_traits_t<MemFn>::receiver_type>();
	vp->set_fake_body(to_offset(memfn)/sizeof(ptr_t), to_ptr(fakefn));
  }

  value_type* ptr()
  {
	if (!m_faking) {
	  fake();
	}

	return m_faking;
  }

  value_type& ref()
  {
	return *ptr();
  }

  static self_type* backroom(value_type* self)
  {
	return reinterpret_cast<self_type*>(vtable_t::ticket(vtable_t::as_vptrs(self)[0]));
  }

  const ticket_type& ticket() { return m_ticket; }
  void set_ticket(const ticket_type& ti) { m_ticket = ti; }

public: // export just for testing
  value_type* instance_as_object() const { return reinterpret_cast<value_type*>(m_instance); }
  ptr_t** instance_as_vptr() const { return reinterpret_cast<ptr_t**>(m_instance); }

  template<class Receiver>
  vtable_pair_t* vtable_for()
  {
	/*
	 * currently vtable should be immutable on fakeing
	 * typeinfo for m_instance should be real: vptr_offset_of() use it.
	 */
	assert(!m_faking);

	ptr_t** vptrs = instance_as_vptr();
	int vptr_off = vptr_offset_of<Receiver>(instance_as_object());

	if (vtable_t::head_p(vptr_off)) {
	  assert(typeid(Receiver)  != typeid(value_type)); // ah, we can use specialization but...
	  assert(typeid(base_type) == typeid(bulk_t));
	  int new_vptr_off = reinterpret_cast<bulk_t*>(m_instance)->new_vptr_for(&m_sentinel);
	  vtable_pair_t* base_vp = vtable_for<value_type>();
	  size_t head_index = vtable_t::head_index(vptr_off);
	  ptr_t head_value = ptr_t(new_vptr_off);
	  base_vp->original().set_head(head_index, head_value, true); // we know that vt on bulk_t mutable.
	  base_vp->fake().set_head(head_index, head_value);
	  vptr_off = vptr_offset_of<Receiver>(instance_as_object());
	  assert(vptr_off == new_vptr_off);
	}

	vtable_map_type::iterator i = m_vtables.find(vptr_off);
	if (i != m_vtables.end()) {
	  return i->second;
	}

	vtable_pair_t* vp = new vtable_pair_t(vptrs[vptr_off], reinterpret_cast<ptr_t>(this));
	m_vtables.insert(std::make_pair(vptr_off, vp));
	return vp;
  }

private:
  base_type* m_instance;
  bool m_owns_instance;
  value_type* m_faking;
  vtable_map_type m_vtables;
  vtable_t m_sentinel;
  ticket_type m_ticket;
private:
  faker_t(const faker_t&);
  const faker_t& operator=(const faker_t&);
};

MVT_NAMESPACE_END

#endif//MOVIRT_FAKER_HPP

/* -*-
   Local Variables:
   mode: c++
   c-tab-always-indent: t
   c-indent-level: 2
   c-basic-offset: 2
   tab-width:2
   End:
   -*- */
