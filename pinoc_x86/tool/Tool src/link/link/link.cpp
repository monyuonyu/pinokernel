/* ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■
	概要	:	このプログラムは 指定されたファイルをリンクします
	更新	:	2010/05/02 新規作成 [mk-pino]
		:	2010/05/03	Linuxで実行すると日本語が文字化けする問題を解決 [mk-pino]
 ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ */

#include <stdio.h>
#include <stdlib.h>

int main(int argv, char* argc[])
{
	if(argv < 4)
	{
		printf("Not find parameter!!\n");
		printf("Exa... Link.exe input1.o input2.o output.img");
		exit(1);
	}


	/* ファイルを３つ開く */
	FILE* fp1;
	fp1 = fopen(argc[1], "r");
	if(fp1 == NULL)
	{
		printf("input1 -> open error\n");
		exit(1);
	}
	printf("input1 -> %s\n", argc[1]);

	FILE* fp2;
	fp2 = fopen(argc[2], "r");
	if(fp2 == NULL)
	{
		printf("input2 -> open error\n");
		exit(1);
	}
	printf("input2 -> %s\n", argc[2]);

	FILE* fp3;
	fp3 = fopen(argc[3], "w");
	if(fp3 == NULL)
	{
		printf("output -> open error\n");
		exit(1);
	}
	printf("output -> %s\n", argc[3]);


	/* fp1とfp2のデータをfp3へ書き込んでいく */
	int data = 0;

	while(EOF != (data = getc(fp1)))
	{
		putc(data, fp3);
	}

	while(EOF != (data = getc(fp2)))
	{
		putc(data, fp3);
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

	return 0;
}