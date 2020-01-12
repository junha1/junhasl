#include "../YSL/YSL.h"

using namespace std;

//#define Fuck

void myprint(const ysl::yobject& x)
{
	for (int i = 0; i < x.x.size(); i++)
	{
		cout << int(x.x[i]) << " " << x.x[i] << endl;
	}
}

int main(void)
{

#ifndef Fuck
	{
		variant<int, optional<float>> k = optional<float>{3.0f};
		auto x = ysl::to(k);
		myprint(x);
		decltype(k) result;
		ysl::from(x, result);
	}
#endif

	{
		variant<int, float, vector<int>, char> x{ vector<int>{1,3} };
		auto q = ysl::to(x);
		
	}

	{
		vector<tuple<string, variant<int, vector<int>, float>>> x = { {string("hi"), 1}, {string("bye"), vector<int>{}}, {string("fuck"), 4.3f} };
		auto q = ysl::to(x);
		ysl::from(q, x);
	}

	{
		tuple<int, float> x;
		auto q = ysl::to(x);
		ysl::from(q, x);
	}

}