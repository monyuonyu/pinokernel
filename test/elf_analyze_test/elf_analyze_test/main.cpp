#include <iostream>
#include <fstream>
using namespace std;
//#include <stdio.h>

// 修正BSDライセンス使用

//Elf32_Addr	32bit符号無し整数
//Elf32_Half	16bit符号無し整数
//Elf32_Off		32bit符号無し整数
//Elf32_Sword	32bit符号付き整数
//Elf32_Word	32bit符号無し整数
//
//Elf64_Addr	64bit符号無し整数
//Elf64_Half	16bit符号無し整数
//Elf64_Off		64bit符号無し整数
//Elf64_Sword	32bit符号付き整数
//Elf64_Word	32bit符号無し整数
//Elf64_Xword	64bit符号無し整数
//Elf64_Sxword	64bit符号付き整数


/********************************************************************************
	ELFヘッダ
********************************************************************************/
// 52byte
struct Elf32_Ehdr {
   unsigned char	chare_ident[16];/* 16byteのマジックコード */
   unsigned short	e_type;			/* 再配置可能や実行可能等のELFの形式を表す。 */
   unsigned short   e_machine;		/* ファイルで要求されるアーキテクチャ */
   unsigned long    e_version;		/* ELFフォーマットのバージョンで現在は1 */
   unsigned long    e_entry;		/* プログラムのエントリポイント。指定無しなら0。 */
   unsigned long    e_phoff;		/* プログラムヘッダテーブルのファイル先頭からのオフセット */
   unsigned long    e_shoff;		/* 実行時には使われない */
   unsigned long    e_flags;		/* プロセッサ固有のフラグ */
   unsigned short   e_ehsize;		/* ELFヘッダのサイズ */
   unsigned short   e_phentsize;	/* プログラムヘッダテーブルの1要素あたりのサイズ */
   unsigned short   e_phnum;		/* プログラムヘッダテーブルの要素数 */
   unsigned short   e_shentsize;	/* 実行時には使われない */
   unsigned short   e_shnum;		/* 実行時には使われない */
   unsigned short   e_shstrndx;		/* 実行時には使われない */
};

// 64byte
struct Elf64_Ehdr {
   unsigned char e_ident[16];
   unsigned short    e_type;
   unsigned short    e_machine;
   unsigned long    e_version;
   unsigned long long    e_entry;
   unsigned long long     e_phoff;
   unsigned long long     e_shoff;
   unsigned long    e_flags;
   unsigned short    e_ehsize;
   unsigned short    e_phentsize;
   unsigned short    e_phnum;
   unsigned short    e_shentsize;
   unsigned short    e_shnum;
   unsigned short    e_shstrndx;
};

/********************************************************************************
	プログラムヘッダテーブル
********************************************************************************/
enum {
   PT_NULL,    /* 使われないエントリで、他のメンバの値の意味は未定義 */
   PT_LOAD,    /* 実行時にロードされるセグメント */
   PT_DYNAMIC, /* 動的構造体配列を保持するセグメント */
   PT_INTERP,  /* ファイルの解釈に使われるインタプリタのパスを保持するセグメント */
   PT_NOTE,    /* ファイルの解釈には使われない情報を保持するセグメント */
   PT_SHLIB,   /* 予約 */
   PT_PHDR,    /* プログラムヘッダテーブル（プログラムのメモリイメージの一部である場合のみ存在） */
   PT_TLS,     /* スレッド局所記憶領域のテンプレート */

   PT_LOOS = 0x60000000,
   /* OS固有のセマンティクスの為に予約された領域 */
   PT_HIOS = 0x6fffffff,

   PT_LOPROC = 0x70000000,
   /* プロセッサ固有のセマンティクスの為に予約された領域 */
   PT_HIPROC = 0x7fffffff
};

// 32byte
struct Elf32_Phdr {
	unsigned long p_type;   /* エントリのタイプ */
	unsigned long  p_offset; /* 対応するセグメントのファイル先頭からのオフセット */
	unsigned long p_vaddr;  /* メモリ上でのセグメントの第一バイトの仮想アドレス */
	unsigned long p_paddr;  /* 物理番地指定が適切なシステムの為に予約。 */
	unsigned long p_filesz; /* 対応するセグメントのファイルでのサイズ。0も可。 */
	unsigned long p_memsz;  /* 対応するセグメントのメモリ上に展開された時のサイズ。0も可。 */
	unsigned long p_flags;  /* 対応するセグメントに適切なフラグ。 */
	unsigned long p_align;  /* p_offsetとp_vaddrをこの値で割った余りは等しい */
	struct Elf32_Phdr* next;
};

// 56byte
struct Elf64_Phdr {
	unsigned long  p_type;
	unsigned long  p_flags;
	unsigned long long   p_offset;
	unsigned long long  p_vaddr;
	unsigned long long  p_paddr;
	unsigned long long p_filesz;
	unsigned long long p_memsz;
	unsigned long long p_align;
	struct Elf64_Phdr* next;
};
/********************************************************************************
	セクションヘッダテーブル
********************************************************************************/


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
long littletobig(unsigned char* buff, char size)
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
	printf("ELFフォーマットのバージョンで現在は1 : 0x%x\n", m_e_version);
	printf("プログラムのエントリポイント。指定無しなら0 : 0x%x\n", m_e_entry);
	printf("プログラムヘッダテーブルのファイル先頭からのオフセット : %dbyte\n", m_e_phoff);
	printf("実行時には使われない : 0x%x\n", m_e_shoff);
	printf("プロセッサ固有のフラグ : 0x%x\n", m_e_flags);
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
		
		printf("1.エントリのタイプ : 0x%x\n", elf_program_ent->p_type);
		printf("2.対応するセグメントのファイル先頭からのオフセット : %dbyte\n", elf_program_ent->p_offset);
		printf("3.メモリ上でのセグメントの第一バイトの仮想アドレス : 0x%x\n", elf_program_ent->p_vaddr);
		printf("4.物理番地指定が適切なシステムの為に予約 : 0x%x\n", elf_program_ent->p_paddr);
		printf("5.対応するセグメントのファイルでのサイズ。0も可 : %dbyte\n", elf_program_ent->p_filesz);
		printf("6.対応するセグメントのメモリ上に展開された時のサイズ。0も可 : %dbyte\n", elf_program_ent->p_memsz);
		printf("7.対応するセグメントに適切なフラグ : 0x%x\n", elf_program_ent->p_flags);
		printf("8.p_offset「2.」とp_vaddr「3.」をこの値で割った余りは等しい : 0x%x\n", elf_program_ent->p_align);

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
	m_e_type = littletobig((unsigned char*)&elf_header->e_type, 2);
	m_e_machine = littletobig((unsigned char*)&elf_header->e_machine, 2);
	m_e_version = littletobig((unsigned char*)&elf_header->e_version, 4);
	m_e_entry = littletobig((unsigned char*)&elf_header->e_entry, 4);
	m_e_phoff = littletobig((unsigned char*)&elf_header->e_phoff, 4);
	m_e_shoff = littletobig((unsigned char*)&elf_header->e_shoff, 4);
	m_e_flags = littletobig((unsigned char*)&elf_header->e_flags, 4);
	m_e_ehsize =  littletobig((unsigned char*)&elf_header->e_ehsize, 2);
	m_e_phentsize = littletobig((unsigned char*)&elf_header->e_phentsize, 2);
	m_e_phnum =  littletobig((unsigned char*)&elf_header->e_phnum, 2);
	m_e_shentsize = littletobig((unsigned char*)&elf_header->e_shentsize, 2);
	m_e_shnum = littletobig((unsigned char*)&elf_header->e_shnum, 2);
	m_e_shstrndx = littletobig((unsigned char*)&elf_header->e_shstrndx, 2);

	// プログラムヘッダの数は不定なのでリスト構造で保持する
	ent = (struct Elf32_Phdr*)malloc(sizeof(struct Elf32_Phdr));
	m_program_hed = ent;

	// プログラムヘッダーの個数分繰り返す
	for(i = 0; i < m_e_phnum; i++)
	{
		// プログラムヘッダの開始番地取得
		elf_program = (struct Elf32_Phdr*)&buff[ m_e_phoff + m_e_phentsize * i ];
		
		// ヘッダの内容を保存
		ent->p_type = littletobig((unsigned char*)&elf_program->p_type, 4);
		ent->p_offset = littletobig((unsigned char*)&elf_program->p_offset, 4);
		ent->p_vaddr = littletobig((unsigned char*)&elf_program->p_vaddr, 4);
		ent->p_paddr = littletobig((unsigned char*)&elf_program->p_paddr, 4);
		ent->p_filesz = littletobig((unsigned char*)&elf_program->p_filesz, 4);
		ent->p_memsz = littletobig((unsigned char*)&elf_program->p_memsz, 4);
		ent->p_flags = littletobig((unsigned char*)&elf_program->p_flags, 4);
		ent->p_align = littletobig((unsigned char*)&elf_program->p_align, 4);
		ent->next = 0x00; // リストの終端コード 0x00

		// リスト構造で保存する
		ent_tmp = ent;
		ent = (struct Elf32_Phdr*)malloc(sizeof(struct Elf32_Phdr));
		ent_tmp->next = ent;
	}


	return 0;
}

int main()
{
	char* buff = (char*)malloc(1048);

	printf("-------- sizeof --------\n");
	printf("pointer : %t%dbyte\n", sizeof(void*));
	printf("char : %dbyte\n", sizeof(char));
	printf("int : %dbyte\n", sizeof(int));
	printf("short : %dbyte\n", sizeof(short));
	printf("long : %dbyte\n", sizeof(long));
	printf("long long : %dbyte\n", sizeof(long long));


	// elfサンプル読み込み
	ifstream ifile("sample1.abs", ios::binary);
	ifile.read(buff, 1048);
	ifile.close();

	elf_init(buff);
	elf_status();

	return 0;
}
