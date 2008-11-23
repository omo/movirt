
#include <cassert>
#include <cstdio>
#include <cstddef>
#include <movirt/movirt.hpp>
#include <string>

namespace mvt = movirt;

struct VariousMemFns
{
public:
	virtual void foo() = 0;
	virtual int  bar(double x) = 0;
	virtual int* baz(double ch, char* str) = 0;
};

namespace {
	template<class Fn>
	const std::type_info& receiver_type(Fn fn)
	{
		return typeid(typename mvt::mem_fn_traits_t<Fn>::receiver_type);
	}

	template<class Fn>
	size_t type_arity(Fn fn)
	{
		return mvt::mem_fn_traits_t<Fn>::arity;
	}
}

void test_mem_fn_traits_hello()
{
	assert(typeid(VariousMemFns) == receiver_type(&VariousMemFns::foo));
	assert(0 == type_arity(&VariousMemFns::foo));
	assert(1 == type_arity(&VariousMemFns::bar));
	assert(2 == type_arity(&VariousMemFns::baz));
}

class Hello
{
public:
	virtual ~Hello() {}
	virtual void foo() = 0;
	virtual void bar() = 0;
};

class TestHello : public Hello
{
public:
	TestHello() : m_nfoo(0), m_nbar(0) {}

	virtual void foo() { m_nfoo++; }
	virtual void bar() { m_nbar++; }

	size_t m_nfoo;
	size_t m_nbar;
};

template<size_t ID>
class CallHelper
{
public:
	CallHelper() { *counter() = 0; }

	static void called() { *counter() = *counter() + 1; }
	static size_t ncalled()  { return *counter(); }
private:
	static size_t* counter() { static size_t s_counter; return &s_counter; }
};

template<size_t ID>
void fake_proc()
{
	FV_PROLOGUE_GUARD();
	CallHelper<ID>::called();
}

void test_hello()
{
	CallHelper<0> ch;

	mvt::faker_t<Hello> faker;
	faker.replace_with_fn(&Hello::bar, &fake_proc<0>);
	Hello* h = faker.ptr();
	h->bar();
	assert(ch.ncalled() == 1);
}

class CountedHello : public Hello
{
public:
	CountedHello() { s_nctor++; }
	virtual ~CountedHello() { s_ndtor++; }

	virtual void foo() {}
	virtual void bar() {}

	static int s_nctor;
	static int s_ndtor;

	static void clear() { s_nctor = s_ndtor = 0; }
};

int CountedHello::s_nctor = 0;
int CountedHello::s_ndtor = 0;

void test_base_dtor_should_be_invoked()
{
	CountedHello::clear();

	assert(CountedHello::s_ndtor == 0);
	assert(CountedHello::s_nctor == 0);

	{
	  mvt::faker_t<Hello, void*, CountedHello> faker;
	}

	assert(CountedHello::s_ndtor == 1);
	assert(CountedHello::s_nctor == 1);
}

void test_given_base_should_not_be_deleted()
{
	{
		CountedHello::clear();
		CountedHello ch;

		assert(CountedHello::s_nctor == 1);
		assert(CountedHello::s_ndtor == 0);

		{
		  mvt::faker_t<Hello, void*, CountedHello> faker(0, &ch);
		}

		assert(CountedHello::s_nctor == 1);
		assert(CountedHello::s_ndtor == 0);
	}

	assert(CountedHello::s_nctor == 1);
	assert(CountedHello::s_ndtor == 1);
}

void test_originate_should_enable_method_in_fake()
{
	TestHello target;
	mvt::faker_t<Hello, void*, TestHello> faker(0, &target);

	faker.replace_with_fn(&Hello::bar, &fake_proc<0>);	
	faker.ref().bar();
	assert(0 ==target.m_nbar);
	
	faker.unfake();
	faker.originate(&Hello::bar);
	faker.ref().bar();
	assert(1 == target.m_nbar);
}

class VirtualOrNot
{
public:
	void non_virtual_mf() {}
	virtual void virtual_mf() {}
};

void test_virtual_p_should_detect_non_virtual_function()
{
	assert( mvt::to_offset_and_delta(&VirtualOrNot::virtual_mf).virtual_p());
	assert(!mvt::to_offset_and_delta(&VirtualOrNot::non_virtual_mf).virtual_p());
};

class BaseA
{
public:
	virtual ~BaseA() {}
	virtual void base_a() = 0;
};

class BaseB
{
public:
	virtual ~BaseB() {}
	virtual void base_b() = 0;
};

class BaseC
{
public:
	virtual ~BaseC() {}
	virtual void base_c() = 0;
};

class DerivedABC : public BaseA, public BaseB, public BaseC
{
};

class DerivedABCImpl : public DerivedABC
{
	virtual ~DerivedABCImpl() {}
	virtual void base_a() {}
	virtual void base_b() {}
	virtual void base_c() {}
};

void test_offst_of_vptr()
{
	DerivedABC* d = new DerivedABCImpl();

	int dtoa = mvt::vptr_offset_of<BaseA>(d);
	int dtob = mvt::vptr_offset_of<BaseB>(d);
	int dtoc = mvt::vptr_offset_of<BaseC>(d);
	assert(sizeof(mvt::word_t*)*0 == dtoa);
	assert(sizeof(mvt::word_t*)*1 == dtob);
	assert(sizeof(mvt::word_t*)*2 == dtoc);
	delete d;
}

void test_faker_should_allow_multiple_inheritance()
{
	CallHelper<0> ch0;
	CallHelper<1> ch1;
	CallHelper<2> ch2;

	mvt::faker_t<DerivedABC> faker;

	faker.replace_with_fn(&DerivedABC::base_a, &fake_proc<0>);	
	faker.replace_with_fn(&DerivedABC::base_b, &fake_proc<1>);	
	faker.replace_with_fn(&DerivedABC::base_c, &fake_proc<2>);	

	assert(ch0.ncalled() == 0);
	assert(ch1.ncalled() == 0);
	assert(ch2.ncalled() == 0);

	faker.ptr()->base_a();	
	assert(ch0.ncalled() == 1);
	assert(ch1.ncalled() == 0);
	assert(ch2.ncalled() == 0);

	faker.ptr()->base_b();
	assert(ch0.ncalled() == 1);
	assert(ch1.ncalled() == 1);
	assert(ch2.ncalled() == 0);

	faker.ptr()->base_c();
	assert(ch0.ncalled() == 1);
	assert(ch1.ncalled() == 1);
	assert(ch2.ncalled() == 1);
}

void test_vtable_pair_hello()
{
	mvt::ptr_t original[] = { mvt::to_ptr(1), mvt::to_ptr(2),
															mvt::to_ptr(3), mvt::to_ptr(4), 
															mvt::to_ptr(5), mvt::to_ptr(6),
															mvt::to_ptr(7), mvt::to_ptr(8) };
	mvt::vtable_pair_t target(original, 0);
	assert(target.fake().body_at(4) != target.original().body_at(4));
	assert(target.fake().body_at(4) == 0);
	target.originate(4);
	assert(target.fake().body_at(4) == target.original().body_at(4));
}

class VirtualA
{
public:
	virtual ~VirtualA() {}
	virtual void  base_a() = 0;
};

class VirtualB : virtual public VirtualA
{
public:
	virtual ~VirtualB() {}
	virtual void  base_b() = 0;
};

class VirtualC : virtual public VirtualA
{
public:
	virtual ~VirtualC() {}
	virtual void  base_c() = 0;
};

class VirtualD : public VirtualB, public VirtualC
{
public:
	virtual ~VirtualD() {}
};

class VirtualDImpl : public VirtualD
{
public:
	virtual void base_a() {}
	virtual void base_b() {}
	virtual void base_c() {}
};

void test_faker_should_allow_virtual_inheritancne()
{
	CallHelper<0> ch0;
	CallHelper<1> ch1;
	CallHelper<2> ch2;

	mvt::faker_t<VirtualD> faker;

	mvt::vtable_pair_t* vt = faker.vtable_for<VirtualA>();

	faker.replace_with_fn(&VirtualD::base_a, &fake_proc<0>);	
	faker.replace_with_fn(&VirtualD::base_b, &fake_proc<1>);	
	faker.replace_with_fn(&VirtualD::base_c, &fake_proc<2>);	

	assert(ch0.ncalled() == 0);
	assert(ch1.ncalled() == 0);
	assert(ch2.ncalled() == 0);

	faker.ptr()->base_a();	
	assert(ch0.ncalled() == 1);
	assert(ch1.ncalled() == 0);
	assert(ch2.ncalled() == 0);


	faker.ptr()->base_b();
	assert(ch0.ncalled() == 1);
	assert(ch1.ncalled() == 1);
	assert(ch2.ncalled() == 0);

	faker.ptr()->base_c();
	assert(ch0.ncalled() == 1);
	assert(ch1.ncalled() == 1);
	assert(ch2.ncalled() == 1);
}

class HelloReturner
{
public:
	virtual ~HelloReturner() {}
	virtual std::string say() = 0;
};

std::string say_hello()
{
	return "hello";
}

void test_replaced_function_should_be_able_to_return_ins()
{
	mvt::faker_t<HelloReturner> faker;
	faker.replace_with_fn(&HelloReturner::say, &say_hello);

	HelloReturner* target = faker.ptr();
	std::string actual = target->say();

	assert("hello" == actual);
}

class HelloReceiver
{
public:
	virtual ~HelloReceiver() {}
	virtual void hear(std::string message) = 0;
};

void hear_bye(void* self, std::string message)
{
	fake_proc<0>();
	assert("bye" == message);
}

void test_replaced_function_should_receive_parameters()
{
	CallHelper<0> ch0;
	assert(ch0.ncalled() == 0);

	mvt::faker_t<HelloReceiver> faker;
	faker.replace_with_fn(&HelloReceiver::hear, &hear_bye);
	HelloReceiver* target = faker.ptr();
	target->hear("bye");

	assert(ch0.ncalled() == 1);
}

class HelloDestructable
{
public:
	virtual ~HelloDestructable() {}
};

void test_faker_should_allow_replace_dtor()
{
	// NOT IMPLEMENTED YET.
	assert(false);
	HelloDestructable* d;
	d->~HelloDestructable();
}

void test_vptr_offset_of_with_virtual()
{
	VirtualD* d = new VirtualDImpl();
	int off = mvt::vptr_offset_of<VirtualA>(d);
	delete d;
}

struct ToGetFaker
{
public:
  virtual mvt::faker_t<ToGetFaker>* get_faker() = 0;
};

mvt::faker_t<ToGetFaker>*
get_faker_impl(ToGetFaker* self)
{
  return mvt::faker_t<ToGetFaker>::backroom(self);
}

void test_faker_should_be_available_from_self()
{
  mvt::faker_t<ToGetFaker> faker;
  
  faker.replace_with_fn(&ToGetFaker::get_faker, &get_faker_impl);
  ToGetFaker* target = faker.ptr();

  mvt::faker_t<ToGetFaker>* expected = &faker;
  mvt::faker_t<ToGetFaker>* actual = target->get_faker();
  assert(expected == actual);
}

int main(int argc, char* argv[])
{
	test_hello();
	test_mem_fn_traits_hello();
	test_base_dtor_should_be_invoked();
	test_given_base_should_not_be_deleted();
	test_originate_should_enable_method_in_fake();
	test_virtual_p_should_detect_non_virtual_function();
	test_offst_of_vptr();
	test_vtable_pair_hello();
	test_faker_should_allow_multiple_inheritance();
	test_vptr_offset_of_with_virtual();
	test_faker_should_allow_virtual_inheritancne();
	test_replaced_function_should_be_able_to_return_ins();
	test_replaced_function_should_receive_parameters();
	//test_faker_should_allow_replace_dtor();
	test_faker_should_be_available_from_self();
	return 0;
}
