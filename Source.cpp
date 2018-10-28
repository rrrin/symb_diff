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
//	virtual Expression * diff() = 0;
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
	//number *diff(double a)
	//{
	//	number b(0);
	//	return b;
	//}
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
	//number *diff(double a)
	//{
	//	number b(0);
	//	return b;
	//}
	virtual void print() const override
	{
		cout << "class variable name = " << name << endl;
	}

};

class add : Expression
{

};

class sub : Expression
{

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
}