
#ifndef MOVIRT_VTABLE_HPP
#define MOVIRT_VTABLE_HPP

#include <cstddef>
#include <cassert>
#include <movirt/types.hpp>
#include <movirt/untypes.hpp>

MVT_NAMESPACE_BEGIN

/*
 * table layout:
 *   table[0]              : head(ticket)
 *   table[1 - 512)        : head(open)
 *   table[512 - 1024+512) : body
 *
 *
 * head_word has 1 for least sig bit: gcc refer this bit 
 * to distinguish if the word is pointer or offset.
 */
class vtable_t
{
public:
  enum {
	size_head_ticket = 1,
	size_head_open = 511,
	size_head = size_head_open + size_head_ticket,
	size_body = 1024,
	head_word = 0xfefe0001, // XXX: care 64bit case
	head_word_lo_limit = (0x00010000 >> 1),
	sizes
  };

  explicit vtable_t(ptr_t* head0=0)
	: m_table(head0 ? (head0 - size_head) : new ptr_t[size_head + size_body]),
	  m_own(0 == head0)
  {
	static int n = 0;
	m_n = n++;

	if (m_own) {
	  fill_head_words(m_table + size_head_ticket, 
					  m_table + size_head_ticket + size_head_open);
	  std::fill(body(), body() + size_body, ptr_t(0));
	}
  }
	
  ~vtable_t()
  {
	if (m_own) {
	  delete [] m_table;
	}
  }

  ptr_t ticket() const
  {
	assert(m_own);
	return body()[-size_head];
  }

  void set_ticket(ptr_t t)
  {
	assert(m_own);
	body()[-size_head] = t;
  }

  ptr_t* body() const { return m_table + size_head; }

  ptr_t  body_at(size_t i) const
  {
	assert(i < size_body);
	return body()[i];
  }

  void   set_body(size_t i, ptr_t p, bool force=false)
  {
	assert(m_own || force);
	assert(i < size_body);
	body()[i] = p;
  }

  ptr_t  head_at(size_t i) const
  {
	assert(i < size_head_open);
	return body()[-int(i)];
  }

  void   set_head(size_t i, ptr_t p, bool force=false)
  {
	assert(m_own || force);
	assert(i < size_head_open);
	body()[-int(i)] = p;
  }

  template<class T>
  static bool head_p(T ptr)
  {
	return head_word == (word_t(ptr) & word_t(head_word));
  }

  template<class T>
  static size_t head_index(T ptr)
  {
	return (word_t(ptr) & ~word_t(head_word)) >> 1;
  }

  template<class T>
  static ptr_t** as_vptrs(T* self)
  {
	return reinterpret_cast<ptr_t**>(self);
  }

  static ptr_t ticket(ptr_t* vptr)
  {
	return vptr[-size_head];
  }

private:
  static void fill_head_words(ptr_t* beg, ptr_t* end)
  {
	size_t size = std::distance(beg, end);
	assert(size < head_word_lo_limit);
	/*
	 * embed negative index from body(), within lower bits:
	 * hibits are for signature (bitpattern)
	 */
	for (size_t i=0; i<size; ++i) {
	  beg[i] = reinterpret_cast<ptr_t>(head_word | ((size - i) << 1));
	}
  }

private:
  vtable_t(const vtable_t&);
  vtable_t& operator=(const vtable_t&);
private:
  ptr_t* m_table;
  bool   m_own;
  int m_n;
};

class vtable_pair_t
{
public:
  explicit vtable_pair_t(ptr_t* orig, ptr_t ticket)
	: m_original(orig)
  {
	m_fake.set_ticket(ticket);
  }
	
  const vtable_t& original() const { return m_original; }
  vtable_t& original() { return m_original; }
  const vtable_t& fake() const { return m_fake; }
  vtable_t& fake() { return m_fake; }

  void set_fake_body(size_t i, ptr_t ptr) { m_fake.set_body(i, ptr); }
  void originate(size_t i) { m_fake.set_body(i, m_original.body_at(i)); }
private:
  vtable_t m_original;
  vtable_t m_fake;
};

/* placeholder class with vtables */
class bulk_t
{
public:
  enum { vptr_size = 256 };
  enum { vptr_safe_index = 128 };

  bulk_t()
  {
	std::fill(m_body, m_body + vptr_size, ptr_t(0));
	vtable_t::as_vptrs(this)[0] = m_vtable.body();
  }

  /*
   * @return offset of allocated vptr slot
   */
  size_t new_vptr_for(vtable_t* vt)
  {
	ptr_t** vptrs = vtable_t::as_vptrs(this);

	for (size_t i=vptr_safe_index; i<vptr_size; ++i) {
	  if (!vptrs[i]) {
		vptrs[i] = vt->body();
		return i*sizeof(ptr_t);
	  }
	}

	assert(false);
	return 0;
  }

private:
  ptr_t m_body[vptr_size];
  vtable_t m_vtable;
};

MVT_NAMESPACE_END

#endif//MOVIRT_VTABLE_HPP

/* -*-
   Local Variables:
   mode: c++
   c-tab-always-indent: t
   c-indent-level: 2
   c-basic-offset: 2
   tab-width:2
   End:
   -*- */
