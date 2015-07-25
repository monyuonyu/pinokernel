/* �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��
	�T�v	:	���̃v���O������ �w�肳�ꂽ�T�C�Y�܂�0�Ńt�@�C���𖄂߂܂�
			:		���߂�\������A�t�@�C���T�C�Y���傫�������ꍇ�A0�\�萔�̔{���Ŗ��߂܂�
	�X�V	:	2010/05/02 �V�K�쐬 [mk-pino]
			:	2010/05/03	Linux�Ŏ��s����Ɠ�{�ꂪ������������������� [mk-pino]
			:	2010/06/06	�T�C�Y���傫�������ꍇ�A�{���Ŗ��߂�@�\�ǉ� [mk-pino]
 �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� */

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int ui;

int main(int argv, char* argc[])
{
	/* �p���[���[�^���ݒ肳��Ă��Ȃ� */
	if(argv < 3)
	{
		printf("Not find parameter!!\n");
		printf("Exa... FillNop.exe 512 smpl.img\n");
		exit(1);
	}

	int cnt;
	ui size;

	// ���l�֕ϊ� �����łȂ��ꍇ�I��
	cnt = atoi(argc[1]);
	if(cnt == 0)
	{
		printf("please input Figure\n");
		exit(1);
	}
	printf("cnt -> %d\n", cnt);

	// �t�@�C�����J���ăT�C�Y�𓾂�
	FILE* fp;
	fp = fopen(argc[2], "r+");
	if(fp == NULL)
	{
		printf("open error\n");
		exit(1);
	}
	printf("open -> %s\n", argc[2]);

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	// �Ώۂ̃t�@�C���T�C�Y���傫�������ꍇ�ANULL�̔{���Ŗ��߂�
	while(cnt < size)
	{
		cnt += 512;
	}

	// ���݂̃T�C�Y�ƃ��T�C�Y��̃T�C�Y��\������
	printf("%dbyte to %dbyte\n", size, cnt);

	cnt -= size;

	// �󔒂�0�Ŗ��߂�
	for(; cnt > 0; cnt--)
	{
		putc(NULL, fp);
	}

	fclose(fp);

	return 0;
}
