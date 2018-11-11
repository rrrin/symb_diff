#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <conio.h>
#include <assert.h>
#include <vector>
#include <exception>
#include <memory>
#include <math.h>
#include <limits>
using namespace std;
//
//
//
class Expression;
class Number;
typedef shared_ptr<Expression> expr_p;
class Expression
{
public:
	virtual Expression * diff_impl(const string x) = 0;
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
	virtual bool not_one() const
	{
		return true;
	}
	virtual Number* is_number() 
	{
		return nullptr;
	}
	virtual ~Expression() {};
};


class Number : public  Expression
{
	double x;
//	double value;
public:
	Number(double a)
	{
		x = a;
	}
	Number(const Number& b)
	{
		x = b.x;
	}
	virtual Number* diff_impl(const string x) override
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
	virtual bool not_one() const override
	{
		return x != 1.;
	}
	virtual Number* is_number() override
	{
		return this;
	}
	double val() const
	{
		return x;
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
	virtual Number* diff_impl(const string X) override
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
	virtual Number* calc(double a, double b) = 0;
	Expression* calc_numb()
	{
		if (left->is_number() && right->is_number())
			return this->calc(left->is_number()->val(), right->is_number()->val());
		else return this;
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

	virtual Add* diff_impl(const string x) override
	{
		Expression *le = left->diff_impl(x);
		Expression*r = right->diff_impl(x);
		return new Add(le, r);
	}

	virtual Expression* symplify()  override
	{
		left= left->symplify();
		right = right->symplify();
		if (left->is_number() && right->is_number())
			return calc_numb();
		if (!left->not_zero())
			return right;
		else if (!right->not_zero())
			return left;
		else
			return this;
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a + b);
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

	virtual Add* diff_impl(const string x) override
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
			if (left->not_one())
				left->print();
			if (left->not_one() && right->not_one())
				cout << name_oper;
			if (right->not_one())
				right->print();
		}
		else cout << " 0 ";
		cout << ") ";
	}
	virtual Expression* symplify()  override
	{
		left = left->symplify();
		right = right->symplify();
		if (left->not_zero() && right->not_zero())
		{
			if (left->is_number() && right->is_number())
				return calc_numb();
			if (left->not_one() && right->not_one())
				return this;
			else if (left->not_one())
				return left;
			else if (right->not_one())
				return right;
			else return new Number(1);
		}
		else
			return new Number(0);
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a * b);
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

	virtual Add* diff_impl(const string x) override
	{
		Expression *le = left->diff_impl(x);
		Expression*r = right->diff_impl(x);
		Expression* r2 = new Mul(new Number(-1.), r);
		return new Add(le, r2);
	}
	virtual Expression* symplify()  override
	{
		left = left->symplify();
		right = right->symplify();
		if (left->is_number() && right->is_number())
			return calc_numb();
		if (!left->not_zero())
			return right;
		else if (!right->not_zero())
			return left;
		else
			return new Sub(left, right);
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a - b);
	}
};

class Div : public double_oper
{
public:
	Div(Expression *a, Expression *b)
		: double_oper(a, b, " / ")
	{
	}
	virtual Div* diff_impl(const string x) override
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
	virtual Expression* symplify()  override
	{
		left = left->symplify();
		right = right->symplify();
		if (left->is_number() && right->is_number())
			return calc_numb();
		if (left->not_zero() && right->not_zero())
		{
			if (left->not_one() && right->not_one())
				return new Div(left, right);
			else if (left->not_one())
			{
				assert(!right->not_one());
				return left;
			}
			else return new Number(1);
		}
		else
		{
			if (!left->not_zero())
				return new Number(0);
			else return new Number(numeric_limits<double>::infinity());
		}
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a / b);
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
		degree = b->symplify();
		assert(degree->is_number());
	}
	virtual Mul * diff_impl(const string x) override
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
	virtual Expression* symplify()  override
	{
		base = base->symplify();
		if (base->is_number())
			return calc(base->is_number()->val(), degree->is_number()->val());
		else if (!degree->not_one())
			return base;
		else
			return this;
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(pow(a, b));
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
	virtual Expression * diff_impl(const string x) override
	{
		Expression * y = arg->diff_impl(x);
		return new Mul(y, make_cos(arg));
	}
	virtual void print() const override
	{
		cout << "sin (";
		arg->print();
		cout<< ") ";
	}
	Number *calc(double y)
	{
		return new Number(sin(y));
	}
	virtual Expression* symplify()
	{
		arg = arg->symplify();
		if (arg->is_number())
			return calc(arg->is_number()->val());
		else
			return this;
	}
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
	virtual Expression * diff_impl(const string x) override
	{
		Expression * y = arg->diff_impl(x);
		return new Mul(new Mul(new Number(-1),y), new Sin(arg));
	}
	virtual void print() const override
	{
		cout << "cos (";
		arg->print();
		cout << ") ";
	}
	Number *calc(double y)
	{
		return new Number(cos(y));
	}
	virtual Expression* symplify()
	{
		arg = arg->symplify();
		if (arg->is_number())
			return calc(arg->is_number()->val());
		else
			return this;
	}
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
	virtual Expression * diff_impl(const string x) override
	{
		Expression * y = arg->diff_impl(x);
		Expression * t = new Pow(new Sin(arg), new Number(2));
		return new Div(new Mul(new Number(-1), y), t);

	}
	virtual void print() const override
	{
		cout << "ctg (";
		arg->print();
		cout << ") "; 
	}
	Number *calc(double y)
	{
		return new Number(1./tan(y));
	}
	virtual Expression* symplify()
	{
		arg = arg->symplify();
		if (arg->is_number())
			return calc(arg->is_number()->val());
		else
			return this;
	}
};

void test(const Expression &e)
{
	cout << "test - ";
	e.print();
	cout << endl;
}

void test1(Expression &p, const string& X)
{
	p.print();
	cout << endl;
	Expression*p1 = p.diff(X);
	p1->print();
	cout << endl;
}
int main()
{
	{
		// x+2
		cout << "**** test  X + 2 " << endl;
		variable x("x");
		Number n(2);
		Add s(&x, &n);
		test1(s, "x");
	}
	{
		// 5 - X
		expr_p x1(new variable("x"));
		cout << "***test  5 - X " << endl;
		variable x("x");
		Number n(5);
		Sub s(&n, &x);
		test1(s, "x");
	}
	//x^3
	{
		cout << "*** x^3 " << endl;
		Number n(3);
		variable x("x");
		Pow p(&x, &n);
		test1(p, "x");
	}
	{
		cout << "***diff by y" << endl;
		Number n(3);
		variable x("x");
		Pow p(&x, &n);
		test1(p, "y");
	}
	//5*x
	{
		cout << "***5*x" << endl;
		Number n(5);
		variable x("x");
		Mul p(&n, &x);
		test1(p, "x");
	}
	//3x^3+2*x^2+x+10
	{
		cout << "*** 3x^3+2*x^2+x+10" << endl;
		Number a1(3), a2(2), a3(10);
		variable x("x");
		Pow p1(&x, &a1), p2(&x, &a2);
		Mul q1(&a1, &p1), q2(&a2, &p2);
		Add t(&q1, new Add(&q2, new Add(&x, &a3)));
		test1(t, "x");
	}
	//ctg^3(x)
	{
		cout << "*** ctg ^ 3(x)" << endl;
		variable x("x");
		Ctg c(&x);
		Pow p(&c, new Number(3));
		test1(p, "x");
	}

	_getch();
	return 0;
}