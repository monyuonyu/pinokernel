/* ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■
	概要	:	このプログラムは 指定されたファイルの511byte512byteへブート署名します
	更新	:	2010/05/02 新規作成 [mk-pino]
			2010/05/03	Linuxで実行すると日本語が文字化けする問題を解決 [mk-pino]
 ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ */

#include <stdio.h>
#include <stdlib.h>

int main(int argv, char* argc[])
{
	if(argv < 2)
	{
		printf("No input parameter!!\n");
		printf("Exa... BootSigned.exe bootimg.img\n");
		exit(1);
	}

	FILE* fp;
	fp = fopen(argc[1], "r+");
	if(fp == NULL)
	{
		printf("open error\n");
		exit(1);
	}

	/* 511byte移動 */
	fseek(fp, 510, SEEK_SET);
	putc(0x55, fp);
	putc(0xaa, fp);

	fclose(fp);
	printf("Signed!!\n");

	return 0;
}