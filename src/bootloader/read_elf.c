/*
 * read_elf.c
 *
 *  Created on: 2011/04/25
 *      Author: mizu
 */

#include "read_elf.h"
#include "3069f_sci.h"
#include "string.h"

char* elf_develop(char* p)
{
	struct elf_header* header;
	header = (struct elf_header*) p;
	struct elf_program_header *program;

	int i;
	for (i = 0; i < header->section_header_num; i++)
	{

		program = (struct elf_program_header*) ((char*) header
				+ header->program_header_offset + header->program_header_size
				* i);

		// ロード可能かどうか
		if (program->type != 1)
			continue;

		// メモリへセクション展開
		memcpy((char*) program->physical_addr,
				(char*) header + program->offset, program->file_size);

		// メモリサイズとファイルサイズの隙間を0で埋める
		memset((char*) program->physical_addr + program->file_size, 0,
				(char*) program->memory_size - program->file_size);

	}

	return (char*)header->entry_point;
}

static int elf_check(struct elf_header *header)
{
	if(memcmp(header->id.magic, "\x7f" "ELF", 4))
		return -1;

	// ヘッダ情報チェック
	if(header->id.class		!= 1) return -1;	// ELF32
	if(header->id.format	!= 2) return -1;	// Big endian
	if(header->id.version	!= 1) return -1;	// version 1
	if(header->type			!= 2) return -1;	// Exe file
	if(header->version		!= 1) return -1;	// version 1

	// H8アーキテクチャ Exe file
	if((header->arch != 46) && (header->arch != 47)) return -1;

	return 0;
}

int elf_analysis(char* p)
{
	struct elf_header* header;

	header = (struct elf_header*) p;

	int i;
	for (i = 0; i < header->section_header_num; i++)
	{

		struct elf_program_header *program =
				(struct elf_program_header*) ((char*) header
						+ header->program_header_offset
						+ header->program_header_size * i);

		// ロード可能かどうか
		if (program->type != 1)
			return -1;

	}

	return 0;


//	sci_write(SCI1, );
}

int elf_read(char* p)
{
	struct elf_header* header;

	header = (struct elf_header*)p;

	// ELFかチェック
	if(elf_check(header))
	{
		return -1;
	}

	return 0;

}
