#include "CppUTest/TestHarness.h"
#include "Container/List.h"
#include "Container/Array.h"
#include <stdio.h>

using namespace Container;

class MyListItem : public ListItem {
public:
	int m_value;
	MyListItem() : m_value(0) {}
	explicit MyListItem(int v) : m_value(v) {}
	virtual ~MyListItem() {}
	virtual const char* name() const { return "MyListItem"; }
};

class DerivedItem1 : public MyListItem {
public:
	DerivedItem1() : MyListItem() {}
	explicit DerivedItem1(int v) : MyListItem(v) {}
	virtual ~DerivedItem1() {}
	const char* name() const { return "DerivedItem1"; }
};

class DerivedItem2 : public MyListItem {
public:
	DerivedItem2() : MyListItem() {}
	explicit DerivedItem2(int v) : MyListItem(v) {}
	virtual ~DerivedItem2() {}
	const char* name() const { return "DerivedItem2"; }
};


TEST_GROUP(ListTest) {
	typedef List<MyListItem> MyList;
	static const std::size_t MAXSIZE = 100;
	typedef Array<MyListItem*, MAXSIZE> CheckArray;

	void setup()
	{
	}
	void teardown()
	{
	}

	void checkList(const CheckArray& expected, MyList& x)
	{
		std::size_t i = 0;
		for (MyList::iterator it = x.begin(); it != x.end(); ++it, ++i) {
			POINTERS_EQUAL(expected[i], &*it);
		}
	}

	void printList(const char* name, MyList& x)
	{
		printf("%s: term[%p], begin:", name, (void*)&*x.end());
		for (MyList::iterator it = x.begin(); it != x.end(); ++it) {
			printf("%p[%d] -> ", (void*)&*it, it->m_value);
		}
		printf("end, rbegin:");
		for (MyList::iterator it = x.end(); it != x.begin();) {
			--it;
			printf("%p[%d] -> ", (void*)&*it, it->m_value);
		}
		printf("rend\n");
	}

	SimpleString StringFrom(MyList::iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
	SimpleString StringFrom(MyList::const_iterator value)
	{
		return StringFromFormat("%p", (void *)&*value);
	}
};

TEST(ListTest, size_0)
{
	MyList x;
	LONGS_EQUAL(0, x.size());
}

TEST(ListTest, empty_true)
{
	MyList x;
	CHECK(x.empty());
}

TEST(ListTest, empty_false)
{
	MyList x;
	MyListItem a(100);
	x.push_back(a);
	CHECK(!x.empty());
}

TEST(ListTest, push_back)
{
	MyList x;
	MyListItem a(100);
	x.push_back(a);
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(100, x.back().m_value);

	MyListItem b(200);
	x.push_back(b);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(200, x.back().m_value);

	MyListItem c(300);
	x.push_back(c);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(300, x.back().m_value);
}

TEST(ListTest, push_back_pop_front)
{
	MyList x;
	Array<MyListItem, 10> a;

	for (std::size_t i = 0; i < a.size(); ++i) {
		a[i].m_value = i;
		x.push_back(a[i]);
	}
	LONGS_EQUAL(a.size(), x.size());

	int expected = 0;
	while (!x.empty()) {
		LONGS_EQUAL(expected, x.front().m_value);
		x.pop_front();
		expected++;
		LONGS_EQUAL(a.size() - expected, x.size());
	}
	LONGS_EQUAL(0, x.size());
}

TEST(ListTest, push_front)
{
	MyList x;
	MyListItem a(100);
	x.push_front(a);
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(100, x.front().m_value);

	MyListItem b(200);
	x.push_front(b);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(200, x.front().m_value);

	MyListItem c(300);
	x.push_front(c);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(300, x.front().m_value);
}

TEST(ListTest, push_front_pop_back)
{
	MyList x;
	Array<MyListItem, 10> a;

	for (std::size_t i = 0; i < a.size(); ++i) {
		a[i].m_value = i;
		x.push_front(a[i]);
	}
	LONGS_EQUAL(a.size(), x.size());

	int expected = 0;
	while (!x.empty()) {
		LONGS_EQUAL(expected, x.back().m_value);
		x.pop_back();
		expected++;
		LONGS_EQUAL(a.size() - expected, x.size());
	}
	LONGS_EQUAL(0, x.size());
}

TEST(ListTest, insert)
{
	MyList x;
	MyList::iterator it;
	MyListItem a(100);
	it = x.insert(x.end(), a);
	CHECK_EQUAL(x.begin(), it);
	POINTERS_EQUAL(&a, &*it);
	LONGS_EQUAL(100, it->m_value);
	LONGS_EQUAL(1, x.size());

	MyListItem b(200);
	it = x.insert(x.begin(), b);
	CHECK_EQUAL(x.begin(), it);
	POINTERS_EQUAL(&b, &*it);
	LONGS_EQUAL(200, it->m_value);
	LONGS_EQUAL(2, x.size());

	MyListItem c(300);
	it = x.insert(++x.begin(), c);
	CHECK_EQUAL(++x.begin(), it);
	POINTERS_EQUAL(&c, &*it);
	LONGS_EQUAL(300, it->m_value);
	LONGS_EQUAL(3, x.size());

	MyListItem d(400);
	it = x.insert(x.end(), d);
	CHECK_EQUAL(--x.end(), it);
	POINTERS_EQUAL(&d, &*it);
	LONGS_EQUAL(400, it->m_value);
	LONGS_EQUAL(4, x.size());

}

TEST(ListTest, erase)
{
	MyList x;
	MyList::iterator it;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	MyListItem d(400);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	x.push_back(d);

	it = x.erase(++x.begin());
	LONGS_EQUAL(3, x.size());
	POINTERS_EQUAL(&c, &*it);

	it = x.erase(x.begin());
	LONGS_EQUAL(2, x.size());
	POINTERS_EQUAL(&c, &*it);

	it = x.erase(--x.end());
	LONGS_EQUAL(1, x.size());
	CHECK_EQUAL(x.end(), it);

	it = x.erase(x.begin());
	LONGS_EQUAL(0, x.size());
	CHECK_EQUAL(x.end(), it);

}

TEST(ListTest, begin_end)
{
	MyList x;
	MyListItem a(0);
	MyListItem b(1);
	MyListItem c(2);
	MyListItem d(3);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	x.push_back(d);

	std::size_t i = 0;
	for (MyList::iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(i, (*it).m_value);
		LONGS_EQUAL(i, it->m_value);
	}
}

TEST(ListTest, begin_end_const)
{
	MyList z;
	MyListItem a(0);
	MyListItem b(1);
	MyListItem c(2);
	MyListItem d(3);
	z.push_back(a);
	z.push_back(b);
	z.push_back(c);
	z.push_back(d);
	const MyList& x = z;

	std::size_t i = 0;
	for (MyList::const_iterator it = x.begin(); it != x.end(); ++it, ++i) {
		LONGS_EQUAL(i, (*it).m_value);
		LONGS_EQUAL(i, it->m_value);
	}
}

TEST(ListTest, front_read)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	LONGS_EQUAL(100, x.front().m_value);
	POINTERS_EQUAL(&a, &x.front());
}

TEST(ListTest, front_read_const)
{
	MyList z;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	z.push_back(a);
	z.push_back(b);
	z.push_back(c);
	const MyList& x = z;
	LONGS_EQUAL(100, x.front().m_value);
	POINTERS_EQUAL(&a, &x.front());
}

TEST(ListTest, front_write)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	x.front().m_value = 200;
	LONGS_EQUAL(200, x.front().m_value);
	POINTERS_EQUAL(&a, &x.front());
}

TEST(ListTest, back_read)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	LONGS_EQUAL(300, x.back().m_value);
	POINTERS_EQUAL(&c, &x.back());
}

TEST(ListTest, back_read_const)
{
	MyList z;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	z.push_back(a);
	z.push_back(b);
	z.push_back(c);
	const MyList& x = z;
	LONGS_EQUAL(300, x.back().m_value);
	POINTERS_EQUAL(&c, &x.back());
}

TEST(ListTest, back_write)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	x.back().m_value = 200;
	LONGS_EQUAL(200, x.back().m_value);
	POINTERS_EQUAL(&c, &x.back());
}

TEST(ListTest, splice_all)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	MyListItem d(400);
	MyListItem e(500);
	y.push_back(d);
	y.push_back(e);

	x.splice(x.begin(), y);
	LONGS_EQUAL(5, x.size());
	LONGS_EQUAL(0, y.size());
	CheckArray expected = {&d, &e, &a, &b, &c};
	checkList(expected, x);

	y.splice(y.end(), x);
	LONGS_EQUAL(0, x.size());
	LONGS_EQUAL(5, y.size());
	checkList(expected, y);

}

TEST(ListTest, splice_all2)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	MyListItem d(400);
	MyListItem e(500);
	y.push_back(d);
	y.push_back(e);

	MyList z;
	z.splice(z.end(), x);
	LONGS_EQUAL(0, x.size());
	LONGS_EQUAL(2, y.size());
	LONGS_EQUAL(3, z.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, z);

	x.splice(x.end(), y);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(0, y.size());
	LONGS_EQUAL(3, z.size());
	expected.fill(0);
	expected[0] = &d;
	expected[1] = &e;
	checkList(expected, x);

	y.splice(y.end(), z);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(3, y.size());
	LONGS_EQUAL(0, z.size());
	expected.fill(0);
	expected[0] = &a;
	expected[1] = &b;
	expected[2] = &c;
	checkList(expected, y);
}

TEST(ListTest, splice_all_empty)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	x.splice(x.end(), y);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(0, y.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, x);
}

TEST(ListTest, splice_iter)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	MyListItem d(400);
	MyListItem e(500);
	y.push_back(d);
	y.push_back(e);

	x.splice(x.end(), y, y.begin());
	LONGS_EQUAL(4, x.size());
	LONGS_EQUAL(1, y.size());
	CheckArray expected = {&a, &b, &c, &d};
	checkList(expected, x);

	expected.fill(0);
	expected[0] = &e;
	checkList(expected, y);
}

TEST(ListTest, splice_iter_self)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	x.splice(x.begin(), x, ++x.begin());
	LONGS_EQUAL(3, x.size());
	CheckArray expected = {&b, &a, &c};
	checkList(expected, x);

	x.splice(x.end(), x, x.begin());
	LONGS_EQUAL(3, x.size());
	expected.fill(0);
	expected[0] = &a;
	expected[1] = &c;
	expected[2] = &b;
	checkList(expected, x);
}

TEST(ListTest, splice_iter_self_do_nothing)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	x.splice(x.begin(), x, x.begin());
	LONGS_EQUAL(3, x.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, x);
}

TEST(ListTest, splice_iter_self_do_nothing2)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	x.splice(x.end(), x, --x.end());
	LONGS_EQUAL(3, x.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, x);
}

TEST(ListTest, splice_range)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	MyListItem d(400);
	MyListItem e(500);
	y.push_back(d);
	y.push_back(e);

	x.splice(x.end(), y, y.begin(), ++y.begin());
	LONGS_EQUAL(4, x.size());
	LONGS_EQUAL(1, y.size());
	CheckArray expected = {&a, &b, &c, &d};
	checkList(expected, x);
	expected.fill(0);
	expected[0] = &e;
	checkList(expected, y);

	y.splice(y.end(), x, x.begin(), --x.end());
	LONGS_EQUAL(1, x.size());
	LONGS_EQUAL(4, y.size());
	expected.fill(0);
	expected[0] = &d;
	checkList(expected, x);
	expected.fill(0);
	expected[0] = &e;
	expected[1] = &a;
	expected[2] = &b;
	expected[3] = &c;
	checkList(expected, y);

	y.splice(++y.begin(), x, x.begin(), x.end());
	LONGS_EQUAL(0, x.size());
	LONGS_EQUAL(5, y.size());
	expected.fill(0);
	expected[0] = &e;
	expected[1] = &d;
	expected[2] = &a;
	expected[3] = &b;
	expected[4] = &c;
	checkList(expected, y);
}

TEST(ListTest, splice_range_first_equal_last)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	MyListItem d(400);
	MyListItem e(500);
	y.push_back(d);
	y.push_back(e);

	x.splice(x.end(), y, y.begin(), y.begin());
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(2, y.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, x);
	expected.fill(0);
	expected[0] = &d;
	expected[1] = &e;
	checkList(expected, y);
}

TEST(ListTest, splice_range_self)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	MyListItem d(400);
	MyListItem e(500);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	x.push_back(d);
	x.push_back(e);

	x.splice(x.begin(), x, ++x.begin(), x.end());
	LONGS_EQUAL(5, x.size());
	CheckArray expected = {&b, &c, &d, &e, &a};
	checkList(expected, x);

	x.splice(x.end(), x, x.begin(), ++x.begin());
	LONGS_EQUAL(5, x.size());
	expected.fill(0);
	expected[0] = &c;
	expected[1] = &d;
	expected[2] = &e;
	expected[3] = &a;
	expected[4] = &b;
	checkList(expected, x);

	x.splice(x.end(), x, x.begin(), --x.end());
	LONGS_EQUAL(5, x.size());
	expected.fill(0);
	expected[0] = &b;
	expected[1] = &c;
	expected[2] = &d;
	expected[3] = &e;
	expected[4] = &a;
	checkList(expected, x);
}

TEST(ListTest, splice_range_self_do_nothing)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	x.splice(x.end(), x, x.begin(), x.end());
	LONGS_EQUAL(3, x.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, x);

}

TEST(ListTest, swap)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	MyListItem d(400);
	MyListItem e(500);
	y.push_back(d);
	y.push_back(e);

	x.swap(y);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(3, y.size());
	CheckArray expected = {&d, &e};
	checkList(expected, x);
	expected.fill(0);
	expected[0] = &a;
	expected[1] = &b;
	expected[2] = &c;
	checkList(expected, y);

	y.swap(x);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(2, y.size());
	expected.fill(0);
	expected[0] = &a;
	expected[1] = &b;
	expected[2] = &c;
	checkList(expected, x);
	expected.fill(0);
	expected[0] = &d;
	expected[1] = &e;
	checkList(expected, y);
}

TEST(ListTest, swap_same)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	x.swap(x);
	LONGS_EQUAL(3, x.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, x);

	x.swap(x);
	LONGS_EQUAL(3, x.size());
	checkList(expected, x);

}

TEST(ListTest, swap_nonmember)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList y;
	MyListItem d(400);
	MyListItem e(500);
	y.push_back(d);
	y.push_back(e);

	swap(x, y);
	LONGS_EQUAL(2, x.size());
	LONGS_EQUAL(3, y.size());
	CheckArray expected = {&d, &e};
	checkList(expected, x);
	expected.fill(0);
	expected[0] = &a;
	expected[1] = &b;
	expected[2] = &c;
	checkList(expected, y);

	swap(y, x);
	LONGS_EQUAL(3, x.size());
	LONGS_EQUAL(2, y.size());
	expected.fill(0);
	expected[0] = &a;
	expected[1] = &b;
	expected[2] = &c;
	checkList(expected, x);
	expected.fill(0);
	expected[0] = &d;
	expected[1] = &e;
	checkList(expected, y);
}

TEST(ListTest, swap_nonmember_same)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	swap(x, x);
	LONGS_EQUAL(3, x.size());
	CheckArray expected = {&a, &b, &c};
	checkList(expected, x);

	swap(x, x);
	LONGS_EQUAL(3, x.size());
	checkList(expected, x);

}

TEST(ListTest, iterator_copy_ctor)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	MyList::iterator it = x.begin();
	MyList::iterator it2(it);
	CHECK_EQUAL(it, it2);
	MyList::iterator it3;
	it3 = it;
	CHECK_EQUAL(it, it3);

	MyList::const_iterator cit = x.begin();
	MyList::const_iterator cit2(it);
	CHECK_EQUAL(cit, it2);
	MyList::const_iterator cit3;
	cit3 = it;
	CHECK_EQUAL(cit, cit3);

}

TEST(ListTest, iterator_operator_incremant)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	MyList::iterator it = x.begin();
	MyList::iterator it2 = ++it;
	LONGS_EQUAL(200, it2->m_value);

	MyList::iterator it3 = it++;
	LONGS_EQUAL(200, it3->m_value);
}

TEST(ListTest, iterator_operator_decremant)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	MyList::iterator it = x.end();
	MyList::iterator it2 = --it;
	LONGS_EQUAL(300, it2->m_value);

	MyList::iterator it3 = it--;
	LONGS_EQUAL(300, it3->m_value);
}

TEST(ListTest, iterator_operator_arrow)
{
	MyList x;
	MyListItem a(100);
	DerivedItem1 b(200);
	DerivedItem2 c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList::iterator it = x.begin();
	STRCMP_EQUAL("MyListItem", it->name());
	LONGS_EQUAL(100, it->m_value);
	it->m_value = 1000;
	LONGS_EQUAL(1000, it->m_value);

	MyList::const_iterator cit(it);
	STRCMP_EQUAL("MyListItem", cit->name());
	LONGS_EQUAL(1000, cit->m_value);

	++it;
	STRCMP_EQUAL("DerivedItem1", it->name());

	++it;
	STRCMP_EQUAL("DerivedItem2", it->name());
}

TEST(ListTest, iterator_operator_asterisk)
{
	MyList x;
	MyListItem a(100);
	DerivedItem1 b(200);
	DerivedItem2 c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);

	MyList::iterator it = x.begin();
	STRCMP_EQUAL("MyListItem", (*it).name());
	LONGS_EQUAL(100, (*it).m_value);
	(*it).m_value = 1000;
	LONGS_EQUAL(1000, (*it).m_value);

	MyList::const_iterator cit(it);
	STRCMP_EQUAL("MyListItem", (*cit).name());
	LONGS_EQUAL(1000, (*cit).m_value);

	++it;
	STRCMP_EQUAL("DerivedItem1", (*it).name());

	++it;
	STRCMP_EQUAL("DerivedItem2", (*it).name());
}

TEST(ListTest, iterator_operator_equal_true)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	MyList::iterator it = x.begin();
	MyList::iterator it2 = x.begin();
	MyList::const_iterator cit(it);
	CHECK_TRUE(it == it2);
	CHECK_TRUE(cit == it);
	// CHECK_TRUE(it == cit); // compile error

	it = ++x.begin();
	it2 = ++x.begin();
	CHECK_TRUE(it == it2);
	cit = it;
	CHECK_TRUE(cit == it);

	it = x.end();
	it2 = x.end();
	CHECK_TRUE(it == it2);
	cit = it;
	CHECK_TRUE(cit == it);

}

TEST(ListTest, iterator_operator_equal_false)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	MyList::iterator it = x.begin();
	MyList::iterator it2 = ++x.begin();
	CHECK_FALSE(it == it2);
}

TEST(ListTest, iterator_operator_notequal_true)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	MyList::iterator it = x.begin();
	MyList::iterator it2 = ++x.begin();
	CHECK_TRUE(it != it2);

	it = x.begin();
	it2 = x.end();
	CHECK_TRUE(it != it2);
}

TEST(ListTest, iterator_operator_notequal_false)
{
	MyList x;
	MyListItem a(100);
	MyListItem b(200);
	MyListItem c(300);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	MyList::iterator it = x.begin();
	MyList::iterator it2 = x.begin();
	CHECK_FALSE(it != it2);

	it = x.end();
	it2 = x.end();
	CHECK_FALSE(it != it2);
}

TEST(ListTest, new_delete)
{
	MyList* x = new MyList();
	MyListItem* a = new MyListItem(100);
	MyListItem* b = new DerivedItem1(200);
	MyListItem* c = new DerivedItem2(300);
	x->push_back(*a);
	x->push_back(*b);
	x->push_back(*c);
	LONGS_EQUAL(3, x->size());
	while (!x->empty()) {
		MyListItem& tmp = x->front();
		x->pop_front();
		tmp.name();
		delete &tmp;
	}
	delete x;
}

#ifndef NO_STD_ITERATOR
TEST(ListTest, rbegin_rend)
{
	MyList x;
	MyListItem a(0);
	MyListItem b(1);
	MyListItem c(2);
	MyListItem d(3);
	x.push_back(a);
	x.push_back(b);
	x.push_back(c);
	x.push_back(d);

	std::size_t i = 3;
	for (MyList::reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(i, (*it).m_value);
		LONGS_EQUAL(i, it->m_value);
	}
}

TEST(ListTest, rbegin_rend_const)
{
	MyList z;
	MyListItem a(0);
	MyListItem b(1);
	MyListItem c(2);
	MyListItem d(3);
	z.push_back(a);
	z.push_back(b);
	z.push_back(c);
	z.push_back(d);
	const MyList& x = z;

	std::size_t i = 3;
	for (MyList::const_reverse_iterator it = x.rbegin(); it != x.rend(); ++it, --i) {
		LONGS_EQUAL(i, (*it).m_value);
		LONGS_EQUAL(i, it->m_value);
	}
}
#endif
