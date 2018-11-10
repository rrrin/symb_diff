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
	virtual Expression * diff_impl(string x) = 0;
	Expression * diff(const string x)
	{
		Expression * d = this->diff_impl(x);
		return d->symplify();
	}
//	virtual expr_p diff_impl() = 0;
	virtual void print() const = 0;
	virtual Expression * symplify() 
	{
		return this;
	}
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
	virtual Number* diff_impl(string x) override
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
	virtual Number* diff_impl(string X) override
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
class Add : public double_oper
{
public:
	Add(Expression *le, Expression *r)
		: double_oper(le, r, " + ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual Add* diff_impl(string x) override
	{
		Expression *le = left->diff_impl(x);
		Expression*r = right->diff_impl(x);
		return new Add(le, r);
	}

	virtual Expression* symplify()  override
	{
		Expression * l = left->symplify();
		Expression * r = right->symplify();
		if (!l->not_zero())
			return r;
		else if (!r->not_zero())
			return l;
		else
			return new Add(l, r);
	}
};

class Mul : public double_oper
{
public:
	Mul(Expression *le, Expression *r)
		: double_oper(le, r, " * ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual Add* diff_impl(string x) override
	{
		Expression *le = left->diff_impl(x);
		Expression*r = right->diff_impl(x);
		Expression *op1 = new Mul(le, right);
		Expression *op2 = new Mul(left, r);
		return new Add(op1, op2);
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
	virtual Expression* symplify()  override
	{
		Expression * l = left->symplify();
		Expression * r = right->symplify();
		if (l->not_zero()&&r->not_zero())
			return new Mul(l, r);
		else
			return new Number(0);
	}

};
class Sub : public double_oper
{
public:
	Sub(Expression *le, Expression *r)
		: double_oper(le, r, " - ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual Add* diff_impl(string x) override
	{
		Expression *le = left->diff_impl(x);
		Expression*r = right->diff_impl(x);
		Expression* r2 = new Mul(new Number(-1.), r);
		return new Add(le, r2);
	}
	virtual Expression* symplify()  override
	{
		Expression * l = left->symplify();
		Expression * r = right->symplify();
		if (!l->not_zero())
			return r;
		else if (!r->not_zero())
			return l;
		else
			return new Sub(l, r);
	}
};

class Div : public double_oper
{
public:
	Div(Expression *a, Expression *b)
		: double_oper(a, b, " / ")
	{
	}
	virtual Div* diff_impl(string x) override
	{
		Expression *a = new Mul(left->diff_impl(x), right);
		Expression *b = new Mul(left, right->diff_impl(x));
		Expression *c = new Sub(a, b);
		Expression *d = new Mul(right, right);
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
	virtual Mul * diff_impl(string x) override
	{
		Expression * y = base->diff_impl(x);
		return new Mul(degree, new Mul(y, new Pow(base, new Sub(degree, new Number(1)))));
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
	virtual Expression * diff_impl(string x) override
	{
		Expression * y = arg->diff_impl(x);
		return new Mul(y, make_cos(arg));
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
	virtual Expression * diff_impl(string x) override
	{
		Expression * y = arg->diff_impl(x);
		return new Mul(new Mul(new Number(-1),y), new Sin(arg));
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
	virtual Expression * diff_impl(string x) override
	{
		Expression * y = arg->diff_impl(x);
		Expression * t = new Pow(new Sin(arg), new Number(2));
		return new Div(new Mul(new Number(-1), y), t);

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
		Add s(&x, &n);
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
		Sub s(&n, &x);
		cout << "s = ";
		s.print();
		cout << endl;

		Expression* s1 = s.diff("x");
		cout << "s1.diff = ";
		s1->print();
		Expression* s2 = s1->symplify();
		cout << "s2.symplify = ";
		s2->print();
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

		cout << "\n p1.symp = ";
		Expression *p2 = p1->symplify();
		p2->print();
		cout << endl;
	}

	_getch();
	return 0;
}