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

uint8_t elfParser_loadElfFile(uint8_t data[], ElfFileInfo_t* fileInfo, uint32_t* pAddress, uint32_t vMemoryStartAddress)
{
    Elf32_Ehdr* header = (Elf32_Ehdr*)data;

    uint8_t valid = checkIfFileIsValid(header);

    if (valid)
    {
        fileInfo->entryPoint = header->e_entry;
        Elf32_Half nrOfSections = header->e_shnum;
        Elf32_Half i;
        for (i = 0; i < nrOfSections - 1; i++)
        {
            Elf32_Shdr* pSectionHeader = getSectionHeader(header, i);
            char* sectionName = getSectionName(header, pSectionHeader->sh_name);

            if (pSectionHeader->sh_type == SHT_PROGBITS)
            {
                uint8_t* section = (uint8_t*)header + pSectionHeader->sh_offset;

                if (pSectionHeader->sh_size > 0)
                {
                    if (strcmp(sectionName, ".data") == 0 || strcmp(sectionName, ".text") == 0 ||
                            strcmp(sectionName, ".const") == 0)
                    {
                        uint32_t* addressToCopy = (uint32_t*)((uint32_t)pAddress +(uint32_t)pSectionHeader->sh_addr - vMemoryStartAddress);;
                        memcpy(addressToCopy, section, pSectionHeader->sh_size);
                    }
                }
            }
            else
            {
                if (strcmp(sectionName, ".bss") == 0)
                {
                    // clear section data
                    uint8_t* currAddress = (uint8_t*)((uint32_t)pAddress + (uint32_t)pSectionHeader->sh_addr - vMemoryStartAddress);
                    uint8_t* endAddress = (uint8_t*)currAddress + pSectionHeader->sh_size;

                    while (currAddress != endAddress)
                    {
                        *currAddress++ = 0;
                    }
                }
                else if (strcmp(sectionName, ".stack") == 0)
                {
                    fileInfo->stackPointer = (uint32_t)((uint8_t*)pSectionHeader->sh_addr + pSectionHeader->sh_size);
                }
                else if (strcmp(sectionName, ".cinit") == 0)
                {
                    // do nothing
                }
                else if (strcmp(sectionName, ".sysmem") == 0)
                {
                    // do nothing
                }
            }
        }
        return ELF_FILE_LOADED;
    }
    return ELF_FILE_NOT_LOADED;
}

uint32_t elfParser_getNrOfBytesNecessary(uint8_t data[], uint32_t vMemoryStartAddress)
{
    Elf32_Ehdr* header = (Elf32_Ehdr*)data;
    uint8_t valid = checkIfFileIsValid(header);
    uint32_t endAddress = 0x0000000;

    if (valid)
    {
        Elf32_Half nrOfSections = header->e_shnum;
        Elf32_Half i;
        for (i = 0; i < nrOfSections - 1; i++)
        {
            Elf32_Shdr* pSectionHeader = getSectionHeader(header, i);

            if (pSectionHeader->sh_size > 0)
            {
                char* sectionName = getSectionName(header, pSectionHeader->sh_name);

                if (strcmp(sectionName, ".text") == 0 || strcmp(sectionName, ".data") == 0 ||
                        strcmp(sectionName, ".bss") == 0 || strcmp(sectionName, ".stack") == 0)
                {
                    if (pSectionHeader->sh_addr >= endAddress)
                    {
                        endAddress = (uint32_t)((uint8_t*)pSectionHeader->sh_addr + pSectionHeader->sh_size);
                    }
                }
            }
        }
    }

    return endAddress - vMemoryStartAddress;
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
    uint8_t* pSectionHeaderTable = ((uint8_t*)header + header->e_shoff);
    uint32_t offset = header->e_shentsize*index;
    return (Elf32_Shdr*)(pSectionHeaderTable + offset);
}

static char* getSectionName(Elf32_Ehdr* header, uint8_t index)
{
    Elf32_Shdr* pSectionNameTableHeader = getSectionHeader(header, header->e_shstrndx);
    //uint8_t* pSectionNameTable = (uint8_t*)pSectionNameTableHeader + pSectionNameTableHeader->sh_offset;
    uint8_t* pSectionNameTable = (uint8_t*)header + pSectionNameTableHeader->sh_offset;
    char* name = (char*)pSectionNameTable + index;
    return name;
}
