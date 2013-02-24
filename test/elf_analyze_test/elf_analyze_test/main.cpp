#include <iostream>
#include <fstream>
using namespace std;
//#include <stdio.h>
#include "elf.h"

int main()
{
	char* buff = (char*)malloc(0xFFFFFFF);
	char* test_buff = (char*)malloc(0xFFFFFFF);
	void (*start)(void);

	printf("-------- sizeof --------\n");
	printf("pointer : %t%dbyte\n", sizeof(void*));
	printf("char : %dbyte\n", sizeof(char));
	printf("int : %dbyte\n", sizeof(int));
	printf("short : %dbyte\n", sizeof(short));
	printf("long : %dbyte\n", sizeof(long));
	printf("long long : %dbyte\n", sizeof(long long));


	// elfÉTÉìÉvÉãì«Ç›çûÇ›
	ifstream ifile("sample2.abs", ios::binary);
	ifile.read(buff, 0xFFFFFFF);
	ifile.close();

	elf_init(buff);
	elf_status();
	start = (void(*)(void))elf_devlop(buff);

	//start();


	return 0;
}
