
#include "elf.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static unsigned char m_magic[4];
static unsigned char m_fileclass;
static unsigned char m_endian;
static unsigned char m_elf_version;
static unsigned char m_os_abi;
static unsigned char m_abi_version;
static unsigned short m_e_type;
static unsigned short m_e_machine;
static unsigned long m_e_version;
static unsigned long m_e_entry;
static unsigned long m_e_phoff;
static unsigned long m_e_shoff;
static unsigned long m_e_flags;
static unsigned short m_e_ehsize;
static unsigned short m_e_phentsize;
static unsigned short m_e_phnum;
static unsigned short m_e_shentsize;
static unsigned short m_e_shnum;
static unsigned short m_e_shstrndx;
static struct Elf32_Phdr* m_program_hed;

// 32bitまで
// PCではビッグエンディアンのバイナリをリトルエンディアンとして扱う
// H8マイコンはビッグエンディアンのバイナリをビッグエンディアンとして扱う
static long littletobig(unsigned char* buff, char size)
{
	unsigned long tmp = 0;
	unsigned long scale = 1;
	int i;

	for(i = size - 1; i >= 0; i--)
	{
		tmp += buff[i] * scale;
		scale *= 0x100;
	}

	//tmp += buff[0] * 0x1000000;
	//tmp += buff[1] * 0x10000;
	//tmp += buff[2] * 0x100;
	//tmp += buff[3];

	//printf("ltob 0x%x\n", tmp);

	return tmp;
}

void elf_status()
{
	int i;
	struct Elf32_Phdr* elf_program_ent;

	printf("-------- elf_status --------\n");

	printf("---- ELF Header ----\n");
	// マジック
	printf("magic : %x%c%c%c\n", m_magic[0], m_magic[1], m_magic[2], m_magic[3]);
	printf("ファイルクラス : %d\n", m_fileclass);
	printf("エンディアン : %d\n", m_endian);
	printf("elf version : %d\n", m_elf_version);
	printf("OS ABI : %d\n", m_os_abi);
	printf("ABI version : %d\n", m_abi_version);
	// e_ident[9...15]	未使用
	printf("再配置可能や実行可能等のELFの形式を表す。 : 0x%x\n", m_e_type);
	printf("ファイルで要求されるアーキテクチャ : 0x%x\n", m_e_machine);
	printf("ELFフォーマットのバージョンで現在は1 : 0x%lx\n", m_e_version);
	printf("プログラムのエントリポイント。指定無しなら0 : 0x%lx\n", m_e_entry);
	printf("プログラムヘッダテーブルのファイル先頭からのオフセット : %ldbyte\n", m_e_phoff);
	printf("実行時には使われない : 0x%lx\n", m_e_shoff);
	printf("プロセッサ固有のフラグ : 0x%lx\n", m_e_flags);
	printf("ELFヘッダのサイズ : %dbyte\n", m_e_ehsize);
	printf("プログラムヘッダテーブルの1要素あたりのサイズ : %dbyte\n", m_e_phentsize);
	printf("プログラムヘッダテーブルの要素数 : %d個\n", m_e_phnum);
	printf("実行時には使われない : 0x%x\n", m_e_shentsize);
	printf("実行時には使われない : 0x%x\n", m_e_shnum);
	printf("実行時には使われない : 0x%x\n", m_e_shstrndx);

	printf("---- Program Header ----\n");
	elf_program_ent = m_program_hed;
	// プログラムヘッダーの個数分繰り返す
	for(i = 0; i < m_e_phnum; i++)
	{
		printf("■ Program Header %d\n", i+1);

		printf("1.エントリのタイプ : 0x%lx\n", elf_program_ent->p_type);
		printf("2.対応するセグメントのファイル先頭からのオフセット : %ldbyte\n", elf_program_ent->p_offset);
		printf("3.メモリ上でのセグメントの第一バイトの仮想アドレス : 0x%lx\n", elf_program_ent->p_vaddr);
		printf("4.物理番地指定が適切なシステムの為に予約 : 0x%lx\n", elf_program_ent->p_paddr);
		printf("5.対応するセグメントのファイルでのサイズ。0も可 : %ldbyte\n", elf_program_ent->p_filesz);
		printf("6.対応するセグメントのメモリ上に展開された時のサイズ。0も可 : %ldbyte\n", elf_program_ent->p_memsz);
		printf("7.対応するセグメントに適切なフラグ : 0x%lx\n", elf_program_ent->p_flags);
		printf("8.p_offset「2.」とp_vaddr「3.」をこの値で割った余りは等しい : 0x%lx\n", elf_program_ent->p_align);

		elf_program_ent = elf_program_ent->next;
	}
}

int elf_check(struct Elf32_Ehdr* buff)
{
	//e_ident[0]	magic	0x7f
	//e_ident[1]	magic	'E'
	//e_ident[2]	magic	'L'
	//e_ident[3]	magic	'F'
	//e_ident[4]	ファイルのクラス	0=invalid, 1=32bit, 2=64bit
	//e_ident[5]	データの符号化方式（エンディアン等）	0=invalid, 1=LSB, 2=MSB
	//e_ident[6]	ELFヘッダのバージョン	現在は1
	//e_ident[7]	ファイルが対象とするOSとABI
	//e_ident[8]	ファイルが対象とするABIのバージョン
	//e_ident[9...15]	未使用

	int check_flg = 0;

	printf("-------- elf_check --------\n");

	// magicコードが間違っていればELFファイルではないと判断
	if(buff->chare_ident[0] != 0x7F) check_flg = 1;
	if(buff->chare_ident[1] != 'E') check_flg = 1;
	if(buff->chare_ident[2] != 'L') check_flg = 1;
	if(buff->chare_ident[3] != 'F') check_flg = 1;

	if(check_flg)
	{
		printf("正しいELFファイルフォーマットではありません\n");
		return 1;
	}
	printf("正しいELFファイルフォーマットです\n");
	return 0;
}


// elfファイルを解析して初期化する
// 戻り値1がエラー
int elf_init(char* buff)
{
	int i;
	struct Elf32_Phdr* ent,* ent_tmp;
	struct Elf32_Ehdr* elf_header;
	struct Elf32_Phdr* elf_program;

	printf("-------- elf_init --------\n");
	elf_header = (struct Elf32_Ehdr*)buff;
	if(elf_check(elf_header))
		return 1;

	m_magic[0] = elf_header->chare_ident[0];
	m_magic[0] = elf_header->chare_ident[1];
	m_magic[0] = elf_header->chare_ident[2];
	m_magic[0] = elf_header->chare_ident[3];
	m_fileclass = elf_header->chare_ident[4];
	m_endian =  elf_header->chare_ident[5];
	m_elf_version = elf_header->chare_ident[6];
	m_os_abi = elf_header->chare_ident[7];
	m_abi_version = elf_header->chare_ident[8];
	// e_ident[9...15]	未使用
	m_e_type = elf_header->e_type;
	m_e_machine = elf_header->e_machine;
	m_e_version = elf_header->e_version;
//	printf("elf_header->e_version:%x\n", elf_header->e_version);
//	printf("elf_header->e_version:%u\n", elf_header->e_version);
//	printf("elf_header->e_version:%d\n", elf_header->e_version);
//	printf("elf_header->e_version:%ld\n", elf_header->e_version);
//	printf("elf_header->e_version:m_e_version:%ld\n", m_e_version);
//	printf("elf_header->e_version[0]:%x\n", ((char*)&(elf_header->e_version))[0]);
//	printf("elf_header->e_version[1]:%x\n", ((char*)&(elf_header->e_version))[1]);
//	printf("elf_header->e_version[2]:%x\n", ((char*)&(elf_header->e_version))[2]);
//	printf("elf_header->e_version[3]:%x\n", ((char*)&(elf_header->e_version))[3]);

	m_e_entry = elf_header->e_entry;
	m_e_phoff = elf_header->e_phoff;
	m_e_shoff = elf_header->e_shoff;
	m_e_flags = elf_header->e_flags;
	m_e_ehsize =  elf_header->e_ehsize;
	m_e_phentsize = elf_header->e_phentsize;
	m_e_phnum =  elf_header->e_phnum;
	m_e_shentsize = elf_header->e_shentsize;
	m_e_shnum = elf_header->e_shnum;
	m_e_shstrndx = elf_header->e_shstrndx;

	// プログラムヘッダの数は不定なのでリスト構造で保持する
	ent = (struct Elf32_Phdr*)malloc(sizeof(struct Elf32_Phdr));
	m_program_hed = ent;

	// プログラムヘッダーの個数分繰り返す
	for(i = 0; i < m_e_phnum; i++)
	{
		// プログラムヘッダの開始番地取得
		elf_program = (struct Elf32_Phdr*)&buff[ m_e_phoff + m_e_phentsize * i ];

		// ヘッダの内容を保存
		ent->p_type = elf_program->p_type;
		ent->p_offset = elf_program->p_offset;
		ent->p_vaddr = elf_program->p_vaddr;
		ent->p_paddr = elf_program->p_paddr;
		ent->p_filesz = elf_program->p_filesz;
		ent->p_memsz = elf_program->p_memsz;
		ent->p_flags = elf_program->p_flags;
		ent->p_align = elf_program->p_align;
		ent->next = 0x00; // リストの終端コード 0x00

		// リスト構造で保存する
		ent_tmp = ent;
		ent = (struct Elf32_Phdr*)malloc(sizeof(struct Elf32_Phdr));
		ent_tmp->next = ent;
	}


	return 0;
}

void* elf_devlop(char* buff)
{
	long i, ii;
	long virtual_space_cnt;
	void* entry_point;
	struct Elf32_Phdr* ent;

//	int test_buff = (int)(char*)malloc(1000);	// テストバッファ「visual studio上でのみ必要」

	// elf_ヘッダーにエントリポイントが書いてあった場合は優先的に適応
	entry_point = (void*)m_e_entry;

	ent = m_program_hed;

	// プログラムヘッダの要素を全て展開する
	for(i = 0; i < m_e_phnum; i++)
	{
		printf("program header %ld developint...\n", i);
		virtual_space_cnt = ent->p_memsz - ent->p_filesz;
//		memcpy((void*)(test_buff+(ent->p_vaddr)), &buff[ent->p_offset], ent->p_filesz);
//		memset((void*)(test_buff+(ent->p_vaddr) + ent->p_filesz), 0x00, virtual_space_cnt);
		memcpy((void*)((ent->p_vaddr)), &buff[ent->p_offset], ent->p_filesz);
		memset((void*)((ent->p_vaddr) + ent->p_filesz), 0x00, virtual_space_cnt);

		// 展開できたか確認する
		printf("p_vaddr 0x%lx\n", ent->p_vaddr);
		for(ii = 0; ii < ent->p_memsz; ii++)
			printf("%x", ((char*)ent->p_vaddr)[ii]);
		printf("\n");

		ent = ent->next;

	}



	return entry_point;
}
