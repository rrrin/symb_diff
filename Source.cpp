#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<string.h>
#include <conio.h>
#include <assert.h>
#include <vector>
#include <exception>

using namespace std;

class expression
{
public:
	expression * diff()
	{

	}
	void print()
	{

	}
};

class number : expression
{
	double x;
public:
	number(double a)
	{
		x = a;
	}
	number *diff(double a)
	{
		number b(0);
		return b;
	}
	void print()
	{

	}

};

class variable : expression
{

};

class add : expression
{

};

class sub : expression
{

};