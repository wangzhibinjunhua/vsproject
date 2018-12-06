#include <iostream>
#include <fstream>
using namespace std;



void test()
{
	int count = 0;
	while(1) {
		count++;
		printf("count=%d\n", count);
		if (count > 5) {
			break;
		}
	}
}
void test2()
{

}

int main(int argc, char **argv)
{
	test();
	getchar();
	return 0;

}
