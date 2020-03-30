#include "junhasl.h"
#include <iostream>

using namespace std;

struct Cat 
{
	int age;
	float weight;
	string name;
	tuple<int, int> data;

	YSL_NAMES_S(Cat, age, weight, name, data)
};



int main()
{
	auto names = Cat::ysl_names();

	auto k = ysl::to(tuple{ "fuck", "suck" });

	cout << "Hello CMake." << endl;
	return 0;
}
