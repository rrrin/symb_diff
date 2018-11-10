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
	virtual Expression * diff(string x) = 0;
//	virtual expr_p diff() = 0;
	virtual void print() const = 0;
	virtual bool not_zero() const
	{
		return true;
	}
	virtual ~Expression() {};
};


class Number : public  Expression
{
	double x;
public:
	Number(double a)
	{
		x = a;
	}
	Number(const Number& b)
	{
		x = b.x;
	}
	virtual Number* diff(string x) override
	{
		return new Number(0);
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
	variable(string x)
	{
		name = x;
	}
	virtual Number* diff(string X) override
	{
		if (X==name)
			return new Number(1);
		else return new Number(0);
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

	virtual add* diff(string x) override
	{
		Expression *le = left->diff(x);
		Expression*r = right->diff(x);
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

	virtual add* diff(string x) override
	{
		Expression *le = left->diff(x);
		Expression*r = right->diff(x);
		Expression *op1 = new mul(le, right);
		Expression *op2 = new mul(left, r);
		return new add(op1, op2);
	}
	virtual void print() const override
	{
		cout << " (";
		if (left->not_zero() && right->not_zero())
		{
			left->print();
			cout << name_oper;
			right->print();
		}
		else cout << " 0 ";
		cout << ") ";
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

	virtual add* diff(string x) override
	{
		Expression *le = left->diff(x);
		Expression*r = right->diff(x);
		Expression* r2 = new mul(new Number(-1.), r);
		return new add(le, r2);
	}
};

class Div : public double_oper
{
public:
	Div(Expression *a, Expression *b)
		: double_oper(a, b, " / ")
	{
	}
	virtual Div* diff(string x) override
	{
		Expression *a = new mul(left->diff(x), right);
		Expression *b = new mul(left, right->diff(x));
		Expression *c = new sub(a, b);
		Expression *d = new mul(right, right);
		return new Div(c, d);
	}
	virtual void print() const override
	{
		cout << " (";
		if (!right->not_zero())
			cout << " inf ";

		else if (!left->not_zero())
			cout << " 0 ";
		else
		{
			left->print();
			cout << name_oper;
			right->print();
		}
		cout << ") ";
	}
};

class Pow : public Expression
{
	Expression *base;
	Expression *degree;
public:
	Pow(Expression *a, Expression *b)
	{
		base = a;
		degree = b;
	}
	virtual mul * diff(string x) override
	{
		Expression * y = base->diff(x);
		return new mul(degree, new mul(y, new Pow(base, new sub(degree, new Number(1)))));
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

Expression * make_cos(Expression *p);//декларация функции возвращает косинус, определим позже
class Sin : public Expression
{
	Expression *arg;
public:
	Sin(Expression *a)
		: arg(a)
	{
		assert(arg != nullptr);
	}
	virtual Expression * diff(string x) override
	{
		Expression * y = arg->diff(x);
		return new mul(y, make_cos(arg));
	}
	virtual void print() const override
	{}
};
class Cos : public Expression
{
	Expression *arg;
public:
	Cos (Expression *a)
		: arg(a)
	{
		assert(arg != nullptr);
	}
	virtual Expression * diff(string x) override
	{
		Expression * y = arg->diff(x);
		return new mul(new mul(new Number(-1),y), new Sin(arg));
	}
	virtual void print() const override
	{}
};

Expression * make_cos(Expression *p)
{
	return new Cos(p);
}

class Ctg : public Expression
{
	Expression *arg;
public:
	Ctg(Expression *a)
		: arg(a)
	{
		assert(arg != nullptr);
	}
	virtual Expression * diff(string x) override
	{
		Expression * y = arg->diff(x);
		Expression * t = new Pow(new Sin(arg), new Number(2));
		return new Div(new mul(new Number(-1), y), t);

	}
	virtual void print() const override
	{}
};

void test(const Expression &e)
{
	cout << "test - ";
	e.print();
	cout << endl;
}
int main()
{
	Number t(2.);
	variable y("y");
	test(t);
	test(y);
	{
		// x+2
		cout << "test  X + 2 " << endl;
		variable x("x");
		Number n(2);
		add s(&x, &n);
		cout << "s = ";
		s.print();
		cout << endl;

		Expression* s1 = s.diff("x");
		cout << "s.diff = ";
		s1->print();
		cout << endl;
	}
	{
		// 5 - X
		expr_p x1(new variable("x"));
		cout << "test  5 - X " << endl;
		variable x("x");
		Number n(5);
		sub s(&n, &x);
		cout << "s = ";
		s.print();
		cout << endl;

		Expression* s1 = s.diff("x");
		cout << "s.diff = ";
		s1->print();
		cout << endl;
	}
	//x^3
	{
		Number n(3);
		variable x("x");
		Pow p(&x, &n);
		p.print();
		Expression*p1 = p.diff("x");
		p1->print();
		cout << endl;
	}
	{
		cout << "diff by y" << endl;
		Number n(3);
		variable x("x");
		Pow p(&x, &n);
		p.print();
		Expression*p1 = p.diff("y");
		p1->print();
		cout << endl;
	}

	_getch();
	return 0;
}