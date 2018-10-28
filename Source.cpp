#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <conio.h>
#include <assert.h>
#include <vector>
#include <exception>

using namespace std;

class Expression
{
public:
	virtual Expression * diff() = 0;
	virtual void print() const = 0;
};

class number : public  Expression
{
	double x;
public:
	number(double a)
	{
		x = a;
	}
	virtual ~number() {};
	virtual number* diff() override
	{
		return new number(0);
	}
	virtual void print() const override
	{
		cout << "class number x = " << x << endl;
	}

};

class variable : public Expression
{
	string name;
public:
	variable (string a)
	{
		name = a;
	}
	virtual number* diff() override
	{
		return new number(1);
	}
	virtual void print() const override
	{
		cout << "class variable name = " << name << endl;
	}

};

class add : public Expression
{
	Expression *left, *right;
public:
	add(Expression *le, Expression *r)
		: left(le), right(r)
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual void print() const override
	{
		cout << "class add left = " ;
		left->print();
		cout << " right = ";
		right->print();
	}

	virtual add* diff() override
	{
		Expression *le = left->diff();
		Expression*r = right->diff();
		return new add(le, r);
	}

};

class sub : Expression
{
	Expression *left, *right;
public:
	sub(Expression *le, Expression *r)
		: left(le), right(r)
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual void print() const override
	{
		cout << "class sub left = " << left << " right = " << right << endl;
	}

	virtual sub* diff() override
	{
		Expression *le = left->diff();
		Expression*r = right->diff();
		return new sub(le, r);
	}
};

void test(const Expression &e)
{
	e.print();
}
Expression * create(int n)
{
	Expression * ans = nullptr;
	switch (n)
	{
	case 1:
		ans = new number(56.);
		break;

	case 2:
		ans = new variable("rt");
		break;

	}
	return ans;
}
int main()
{
	number t(2.);
	variable y("asd");
	t.print();
	y.print();

	test(t);
	test(y);

	Expression * exp = create(1);
	exp->print();
	delete exp;

// x+2
	variable x("x");
	number n(2);
	add s(&x, &n);
	s.print();
	Expression* s1= s.diff();
	s1->print();
}