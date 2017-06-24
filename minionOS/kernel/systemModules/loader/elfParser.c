/*
 * elfParser.c
 *
 *  Created on: 24.06.2017
 *      Author: sabrinarosmann
 */

#include "kernel/systemModules/loader/elfParser.h"

static uint8_t checkIfFileIsValid(Elf32_Ehdr* header);
static Elf32_Shdr* getSectionHeader(Elf32_Ehdr* header, uint8_t index);
static char* getSectionName(Elf32_Ehdr* header, uint8_t index);

static void printElfHeader(const Elf32_Ehdr* elfHeader);
static void printSectionHeader(const Elf32_Ehdr* elfHeader, int index);
static void printSectionHeaderTable(const Elf32_Ehdr* elfHeader);
static void printProgramHeader(const Elf32_Ehdr* elfHeader, int index);
static void printProgramHeaderTable(const Elf32_Ehdr* elfHeader);

uint8_t elfParser_loadElfFile(uint8_t data[])
{
    Elf32_Ehdr* header = (Elf32_Ehdr*)data;

    uint8_t valid = checkIfFileIsValid(header);

    if (valid)
    {
        Elf32_Half nrOfSections = header->e_shnum;
        Elf32_Half i;
        for (i = 0; i < nrOfSections; i++)
        {
            Elf32_Shdr* pSectionHeader = getSectionHeader(header, i);

            if (pSectionHeader->sh_size > 0)
            {
                char* sectionName = getSectionName(header, pSectionHeader->sh_name);
                int j = 0;
            }
        }

        return ELF_FILE_LOADED;
    }
    return ELF_FILE_NOT_LOADED;
}

static uint8_t checkIfFileIsValid(Elf32_Ehdr* header)
{
    if (header == NULLPOINTER)
    {
        return ELF_FILE_INVALID;
    }

    if (header->e_ident[EI_MAG0] != ELFMAG0)
    {
        return ELF_FILE_INVALID;
    }

    if (header->e_ident[EI_MAG1] != ELFMAG1)
    {
        return ELF_FILE_INVALID;
    }

    if (header->e_ident[EI_MAG2] != ELFMAG2)
    {
        return ELF_FILE_INVALID;
    }

    if (header->e_ident[EI_MAG3] != ELFMAG3)
    {
        return ELF_FILE_INVALID;
    }
    return ELF_FILE_VALID;
}

static Elf32_Shdr* getSectionHeader(Elf32_Ehdr* header, uint8_t index)
{
    //uint32_t* pSectionHeaderTable = (uint32_t*)((uint32_t)header + header->e_shoff);
    uint32_t* pSectionHeaderTable = (uint32_t*)((uint8_t*)header + header->e_shoff);
    uint32_t offset = header->e_shentsize*index;
    return (Elf32_Shdr*)((uint8_t*)pSectionHeaderTable + offset);
}

static char* getSectionName(Elf32_Ehdr* header, uint8_t index)
{
    Elf32_Shdr* pSectionNameTableHeader = getSectionHeader(header, header->e_shstrndx);
    //uint8_t* pSectionNameTable = (uint8_t*)pSectionNameTableHeader + pSectionNameTableHeader->sh_offset;
    uint8_t* pSectionNameTable = (uint8_t*)header + pSectionNameTableHeader->sh_offset;
    char* name = (char*)pSectionNameTable + index;
    return name;
}

static void printElfHeader(const Elf32_Ehdr* elfHeader)
{
    printf("ELF header\n");
    printf("------------------------------------\n");
    printf("e_ident:\n");
    printf("\tEI_MAG0: 0x%02x\n", elfHeader->e_ident[EI_MAG0]);
    printf("\tEI_MAG1: %c\n", elfHeader->e_ident[EI_MAG1]);
    printf("\tEI_MAG2: %c\n", elfHeader->e_ident[EI_MAG2]);
    printf("\tEI_MAG3: %c\n", elfHeader->e_ident[EI_MAG3]);
    printf("\tEI_CLASS: %d\n", elfHeader->e_ident[EI_CLASS]);
    printf("\tEI_DATA: %d\n", elfHeader->e_ident[EI_DATA]);
    printf("\tEI_VERSION: %d\n", elfHeader->e_ident[EI_VERSION]);
    printf("e_type: 0x%04x\n", elfHeader->e_type);
    printf("e_machine: 0x%04x\n", elfHeader->e_machine);
    printf("e_version: 0x%08x\n", elfHeader->e_version);
    printf("e_entry: 0x%08x\n", elfHeader->e_entry);
    printf("e_phoff: 0x%08x\n", elfHeader->e_phoff);
    printf("e_shoff: 0x%08x\n", elfHeader->e_shoff);
    printf("e_flags: 0x%08x\n", elfHeader->e_flags);
    printf("e_ehsize: 0x%04x\n", elfHeader->e_ehsize);
    printf("e_phentsize: 0x%04x\n", elfHeader->e_phentsize);
    printf("e_phnum: 0x%04x\n", elfHeader->e_phnum);
    printf("e_shentsize: 0x%04x\n", elfHeader->e_shentsize);
    printf("e_shnum: 0x%04x\n", elfHeader->e_shnum);
    printf("e_shtrndx: 0x%04x\n", elfHeader->e_shstrndx);
}

static void printSectionHeader(const Elf32_Ehdr* elfHeader, int index)
{
    Elf32_Shdr* sectionHeader = getSectionHeader(elfHeader, index);
    printf("sh_name: %s\n", getSectionName(elfHeader, sectionHeader->sh_name));
    printf("sh_type: 0x%08x\n", sectionHeader->sh_type);
    printf("sh_flags: 0x%08x\n", sectionHeader->sh_flags);
    printf("sh_addr: 0x%08x\n", sectionHeader->sh_addr);
    printf("sh_offset: 0x%08x\n", sectionHeader->sh_offset);
    printf("sh_size: 0x%08x\n", sectionHeader->sh_size);
    printf("sh_link: 0x%08x\n", sectionHeader->sh_link);
    printf("sh_info: 0x%08x\n", sectionHeader->sh_info);
    printf("sh_addralign: 0x%08x\n", sectionHeader->sh_addralign);
    printf("sh_entsize: 0x%08x\n", sectionHeader->sh_entsize);
}

static void printSectionHeaderTable(const Elf32_Ehdr* elfHeader)
{
    printf("ELF section header table\n");
    printf("------------------------------------\n");

    int index = 0;
    for (index = 0; index < elfHeader->e_shnum; ++index)
    {
        printSectionHeader(elfHeader, index);
    }
}

static void printProgramHeader(const Elf32_Ehdr* elfHeader, int index)
{
    /*
    Elf32_Phdr* programHeader = getProgramHeader(elfHeader, index);
    printf("p_type: 0x%08x\n", programHeader->p_type);
    printf("p_offset: 0x%08x\n", programHeader->p_offset);
    printf("p_vaddr: 0x%08x\n", programHeader->p_vaddr);
    printf("p_paddr: 0x%08x\n", programHeader->p_paddr);
    printf("p_filesz: 0x%08x\n", programHeader->p_filesz);
    printf("p_memsz: 0x%08x\n", programHeader->p_memsz);
    printf("p_flags: 0x%08x\n", programHeader->p_flags);
    printf("p_align: 0x%08x\n", programHeader->p_align);
    */
}

static void printProgramHeaderTable(const Elf32_Ehdr* elfHeader)
{
    printf("ELF program header table\n");
    printf("------------------------------------\n");

    int index = 0;
    for (index = 0; index < elfHeader->e_phnum; ++index)
    {
        printProgramHeader(elfHeader, index);
    }
}

void printElf(uint8_t data[])
{
    Elf32_Ehdr* elfHeader = (Elf32_Ehdr*)data;
    printElfHeader(elfHeader);
    printSectionHeaderTable(elfHeader);
    printProgramHeaderTable(elfHeader);
}
