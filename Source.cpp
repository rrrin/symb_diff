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
typedef shared_ptr<Expression> Expr_p;
class Expression : public std::enable_shared_from_this<Expression>

{
public:
	virtual Expr_p diff_impl(const string x) = 0;
	Expr_p diff(const string x)
	{
		Expr_p d = this->diff_impl(x);
		return d->symplify();
	}
//	virtual expr_p diff_impl() = 0;
	virtual void print() const = 0;
	virtual Expr_p symplify() 
	{
		return shared_from_this();
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
	virtual Expr_p diff_impl(const string x) override
	{
		return Expr_p(new Number(0));
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
	virtual Expr_p diff_impl(const string X) override
	{
		if (X==name)
			return Expr_p(new Number(1));
		else 
			return Expr_p(new Number(0));
	}
	virtual void print() const override
	{
		cout << " " << name << " ";
	}

};

class double_oper : public Expression
{
protected:
	Expr_p left, right;
	string name_oper;
public:
	double_oper(Expr_p le, Expr_p r, const string &oper)
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
	Expr_p calc_numb()
	{
		if (left->is_number() && right->is_number())
			return Expr_p(this->calc(left->is_number()->val(), right->is_number()->val()));
		else 
			return shared_from_this();
	}
};
class Add : public double_oper
{
public:
	Add( Expr_p le,  Expr_p r)
		: double_oper(le, r, " + ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual Expr_p diff_impl(const string x) override
	{
		 Expr_p le = left->diff_impl(x);
		 Expr_p r = right->diff_impl(x);
		return Expr_p(new Add(le, r));
	}

	virtual Expr_p symplify()  override
	{
		Expr_p l = left->symplify();
		if (l.get() != left.get()) left = l;
		Expr_p r = right->symplify();
		if (r.get() != right.get()) right = r;
		if (left->is_number() && right->is_number())
			return calc_numb();
		if (!left->not_zero())
			return right;
		else if (!right->not_zero())
			return left;
		else
			return shared_from_this();
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a + b);
	}
};

class Mul : public double_oper
{
public:
	Mul( Expr_p le,  Expr_p r)
		: double_oper(le, r, " * ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual Expr_p diff_impl(const string x) override
	{
		 Expr_p le = left->diff_impl(x);
		 Expr_p r = right->diff_impl(x);
		 Expr_p op1( new Mul(le, right));
		 Expr_p op2( new Mul(left, r));
		return Expr_p(new Add(op1, op2));
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
	virtual  Expr_p  symplify()  override
	{
		left = left->symplify();
		right = right->symplify();
		if (left->not_zero() && right->not_zero())
		{
			if (left->is_number() && right->is_number())
				return calc_numb();
			if (left->not_one() && right->not_one())
				return shared_from_this();
			else if (left->not_one())
				return left;
			else if (right->not_one())
				return right;
			else 
				return Expr_p(new Number(1));
		}
		else
			return Expr_p(new Number(0));
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a * b);
	}
};

class Sub : public double_oper
{
public:
	Sub( Expr_p le,  Expr_p r)
		: double_oper(le, r, " - ")
	{
		assert(left != nullptr);
		assert(right != nullptr);
	}

	virtual Expr_p diff_impl(const string x) override
	{
		 Expr_p le = left->diff_impl(x);
		 Expr_p r = right->diff_impl(x);
		 Expr_p  r2 = Expr_p(new Mul(Expr_p(new Number(-1.)), r));
		return Expr_p(new Add(le, r2));
	}
	virtual  Expr_p  symplify()  override
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
			return Expr_p(new Sub(left, right));
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a - b);
	}
};

class Div : public double_oper
{
public:
	Div( Expr_p a,  Expr_p b)
		: double_oper(a, b, " / ")
	{
	}
	virtual Expr_p diff_impl(const string x) override
	{
		 Expr_p a = Expr_p(new Mul(left->diff_impl(x), right));
		 Expr_p b = Expr_p(new Mul(left, right->diff_impl(x)));
		 Expr_p c = Expr_p(new Sub(a, b));
		 Expr_p d = Expr_p(new Mul(right, right));
		return Expr_p(new Div(c, d));
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
	virtual  Expr_p  symplify()  override
	{
		left = left->symplify();
		right = right->symplify();
		if (left->is_number() && right->is_number())
			return calc_numb();
		if (left->not_zero() && right->not_zero())
		{
			if (left->not_one() && right->not_one())
				return Expr_p(new Div(left, right));
			else if (left->not_one())
			{
				assert(!right->not_one());
				return left;
			}
			else return Expr_p(new Number(1));
		}
		else
		{
			if (!left->not_zero())
				return Expr_p(new Number(0));
			else return Expr_p(new Number(numeric_limits<double>::infinity()));
		}
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(a / b);
	}
};

class Pow : public Expression
{
	 Expr_p base;
	 Expr_p degree;
public:
	Pow( Expr_p a,  Expr_p b)
	{
		base = a;
		degree = b->symplify();
		assert(degree->is_number());
	}
	virtual Expr_p diff_impl(const string x) override
	{
		 Expr_p  y = base->diff_impl(x);
		return Expr_p(Expr_p(new Mul(degree, Expr_p(new Mul(y, Expr_p(new Pow(base, Expr_p(new Sub(degree, Expr_p(new Number(1)))))))))));
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
	virtual  Expr_p  symplify()  override
	{
		base = base->symplify();
		if (base->is_number())
			return Expr_p(calc(base->is_number()->val(), degree->is_number()->val()));
		else if (!degree->not_one())
			return base;
		else
			return shared_from_this();
	}
	virtual Number* calc(double a, double b)
	{
		return new Number(pow(a, b));
	}
};

 Expr_p  make_cos( Expr_p p);//декларация функции возвращает косинус, определим позже
class Sin : public Expression
{
	 Expr_p arg;
public:
	Sin( Expr_p a)
		: arg(a)
	{
		assert(arg != nullptr);
	}
	virtual  Expr_p  diff_impl(const string x) override
	{
		 Expr_p  y = arg->diff_impl(x);
		return Expr_p(new Mul(y, make_cos(arg)));
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
	virtual  Expr_p  symplify()
	{
		arg = arg->symplify();
		if (arg->is_number())
			return Expr_p(calc(arg->is_number()->val()));
		else
			return shared_from_this();
	}
};
class Cos : public Expression
{
	 Expr_p arg;
public:
	Cos ( Expr_p a)
		: arg(a)
	{
		assert(arg != nullptr);
	}
	virtual  Expr_p  diff_impl(const string x) override
	{
		 Expr_p  y = arg->diff_impl(x);
		return Expr_p(new Mul(Expr_p(new Mul(Expr_p(new Number(-1)),y)), Expr_p(new Sin(arg))));
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
	virtual  Expr_p  symplify()
	{
		arg = arg->symplify();
		if (arg->is_number())
			return Expr_p(calc(arg->is_number()->val()));
		else
			return shared_from_this();
	}
};

 Expr_p  make_cos( Expr_p p)
{
	return Expr_p(new Cos(p));
}

class Ctg : public Expression
{
	 Expr_p arg;
public:
	Ctg( Expr_p a)
		: arg(a)
	{
		assert(arg != nullptr);
	}
	virtual  Expr_p  diff_impl(const string x) override
	{
		 Expr_p  y = arg->diff_impl(x);
		 Expr_p  t = Expr_p(new Pow(Expr_p(new Sin(arg)), Expr_p(new Number(2))));
		return Expr_p(new Div(Expr_p(new Mul(Expr_p(new Number(-1)), y)), t));

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
	virtual  Expr_p  symplify()
	{
		arg = arg->symplify();
		if (arg->is_number())
			return Expr_p(calc(arg->is_number()->val()));
		else
			return shared_from_this();
	}
};

void test(const Expression &e)
{
	cout << "test - ";
	e.print();
	cout << endl;
}

void test1(Expr_p &p, const string& X)
{
	p->print();
	cout << endl;
	 Expr_p p1 = p->diff(X);
	p1->print();
	cout << endl;
}
Expr_p make_number(double a)
{
	return Expr_p(new Number(a));
}
int main()
{
	{
		// x+2
		cout << "**** test  X + 2 " << endl;
		Expr_p x(new variable("x"));
		Expr_p n (new Number(2));
//		Expr_p n = make_number(2);
		Expr_p s(new Add (x, n));
		test1(s, "x");
	}
	{
		// 5 - X
		Expr_p x1(new variable("x"));
		cout << "***test  5 - X " << endl;
		Expr_p x(new variable ("x"));
		Expr_p n(new Number (5));
		Expr_p s(new Sub (n, x));
		test1(s, "x");
	}
	//x^3
	{
		cout << "*** x^3 " << endl;
		Expr_p n(new Number (3));
		Expr_p x(new variable ("x"));
		Expr_p p(new Pow (x, n));
		test1(p, "x");
	}
	{
		cout << "***diff by y" << endl;
		Expr_p n(new Number (3));
		Expr_p x( new variable ("x"));
		Expr_p p(new Pow (x, n));
		test1(p, "y");
	}
	//5*x
	{
		cout << "***5*x" << endl;
		Expr_p n(new Number (5));
		Expr_p x(new variable ("x"));
		Expr_p p(new Mul (n, x));
		test1(p, "x");
	}
#if 0
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
#endif
	_getch();
	return 0;
}