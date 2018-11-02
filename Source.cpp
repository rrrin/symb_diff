#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <conio.h>
#include <assert.h>
#include <vector>
#include <exception>
#include <memory>
using namespace std;
//
//
//
class Expression;
typedef shared_ptr<Expression> expr_p;
class Expression
{
public:
	virtual Expression * diff() = 0;
//	virtual expr_p diff() = 0;
	virtual void print() const = 0;
	virtual bool not_zero() const
	{
		return true;
	}
	virtual ~Expression() {};
};


class number : public  Expression
{
	double x;
public:
	number(double a)
	{
		x = a;
	}
	number(const number& b)
	{
		x = b.x;
	}
	virtual number* diff() override
	{
		return new number(0);
	}
	virtual void print() const override
	{
		cout << " " << x << " " ;
	}
	virtual bool not_zero() const override
	{
		return x!=0;
	}
};

class variable : public Expression
{
	string name;
public:
	variable()
		: name("X")
	{
	}
	virtual number* diff() override
	{
		return new number(1);
	}
	virtual void print() const override
	{
		cout << " " << name << " ";
	}

};

class double_oper : public Expression
{
protected:
	Expression *left, *right;
	string name_oper;
public:
	double_oper(Expression *le, Expression *r, const string &oper)
		: left(le), right(r)
		, name_oper(oper)
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual void print() const override
	{
		cout << " (";
		if (left->not_zero())
			left->print();
		if (left->not_zero() && right->not_zero())
			cout << name_oper;
		if (right->not_zero())
			right->print();
		cout << ") ";
	}

};
class add : public double_oper
{
public:
	add(Expression *le, Expression *r)
		: double_oper(le, r, " + ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual add* diff() override
	{
		Expression *le = left->diff();
		Expression*r = right->diff();
		return new add(le, r);
	}

};

class mul : public double_oper
{
public:
	mul(Expression *le, Expression *r)
		: double_oper(le, r, " * ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual add* diff() override
	{
		Expression *le = left->diff();
		Expression*r = right->diff();
		Expression *op1 = new mul(le, right);
		Expression *op2 = new mul(left, r);
		return new add(op1, op2);
	}
};

class sub : public double_oper
{
public:
	sub(Expression *le, Expression *r)
		: double_oper(le, r, " - ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual add* diff() override
	{
		Expression *le = left->diff();
		Expression*r = right->diff();
		Expression* r2 = new mul(new number(-1.), r);
		return new add(le, r2);
	}
};

class div : public double_oper
{
public:
	div(Expression *a, Expression *b)
		: double_oper(a, b, " / ")
	{
	}
	virtual div* diff() override
	{
		Expression *a = new mul(left->diff(), right);
		Expression *b = new mul(left, right->diff());
		Expression *c = new sub(a, b);
		Expression *d = new mul(right, right);
		return new div(c, d);
	}
};

class pov : public Expression
{
	Expression *base;
	Expression *degree;
public:
	pov(Expression *a, Expression *b)
	{
		base = a;
		degree = b;
	}
	virtual mul * diff() override
	{
		Expression * y = base->diff();
		return new mul(degree, new mul(y, new pov(base, new sub(degree, new number(1)))));
	}
	virtual void print() const override
	{
			cout << " (";
			if (base->not_zero())
				base->print();
			if (base->not_zero() && degree->not_zero())
				cout << "^";
			if (degree->not_zero())
				degree->print();
			cout << ") ";
	}
};

class tg : public Expression
{
	char name;
public:
	Expression Sin(Expression& x)
	{

	}
};

void test(const Expression &e)
{
	cout << "test - ";
	e.print();
	cout << endl;
}
int main()
{
	number t(2.);
	variable y;
	test(t);
	test(y);
	{
		// x+2
		cout << "test  X + 2 " << endl;
		variable x;
		number n(2);
		add s(&x, &n);
		cout << "s = ";
		s.print();
		cout << endl;

		Expression* s1 = s.diff();
		cout << "s.diff = ";
		s1->print();
		cout << endl;
	}
	{
		// 5 - X
		expr_p x1(new variable);
		cout << "test  5 - X " << endl;
		variable x;
		number n(5);
		sub s(&n, &x);
		cout << "s = ";
		s.print();
		cout << endl;

		Expression* s1 = s.diff();
		cout << "s.diff = ";
		s1->print();
		cout << endl;
	}
	//x^3
	{
		number n(3);
		variable x;
		pov p(&x, &n);
		p.print();
		Expression*p1 = p.diff();
		p1->print();
		cout << endl;
	}

	_getch();
	return 0;
}