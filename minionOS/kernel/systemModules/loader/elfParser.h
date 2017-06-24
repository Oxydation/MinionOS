/*
 * elfParser.h
 *
 *  Created on: 24.06.2017
 *      Author: sabrinarosmann
 */

#ifndef KERNEL_SYSTEMMODULES_LOADER_ELFPARSER_H_
#define KERNEL_SYSTEMMODULES_LOADER_ELFPARSER_H_

/* see: http://www.skyfree.org/linux/references/ELF_Format.pdf */

#include <inttypes.h>
#include <string.h>
#include "kernel/common/mmio.h"

#define ELF_FILE_INVALID    0
#define ELF_FILE_VALID      1

#define ELF_FILE_NOT_LOADED 0
#define ELF_FILE_LOADED     1

/* data representation */
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

/* ELF HEADER */
/* identification indexes */
#define EI_MAG0     0       // file identification
#define EI_MAG1     1       // file identification
#define EI_MAG2     2       // file identification
#define EI_MAG3     3       // file identification
#define EI_CLASS    4       // file class
#define EI_DATA     5       // data encoding
#define EI_VERSION  6       // file version
#define EI_PAD      7       // start of padding bytes
#define EI_NIDENT   16      // size of e_ident[]

/* file identification */
#define ELFMAG0     0x7f    // e_ident[EI_MAG0]
#define ELFMAG1     'E'     // e_ident[EI_MAG1]
#define ELFMAG2     'L'     // e_ident[EI_MAG2]
#define ELFMAG3     'F'     // e_ident[EI_MAG3]

/* file class */
#define ELFCLASSNONE    0   // invalid class
#define ELFCLASS32      1   // 32-bit objects
#define ELFCLASS64      2   // 64-bit objects

/* data encoding */
#define ELFDATANONE     0   // invalid data encoding
#define ELFDATA2LSB     1   // specifies 2's complement values with the least significant byte occupying the lowest address
#define ELFDATA2MSB     2   // specifies 2's complement values with the most significant byte occupying the lowest address

/* object file type */
#define ET_NONE     0       // no file type
#define ET_REL      1       // relocatable file
#define ET_EXEC     2       // executable file
#define ET_DYN      3       // shared object file
#define ET_CORE     4       // core file
#define ET_LOPROC   0xff00  // processor-specific
#define ET_HIPROC   0xffff  // processor-specific

/* architecture type */
#define EM_NONE     0       // no machine
#define EM_M32      1       // AT&T WE 32100
#define EM_SPARC    2       // SPARC
#define EM_386      3       // Intel 80386
#define EM_68K      4       // Motorola 68000
#define EM_88K      5       // Motorola 88000
#define EM_860      7       // Intel 80860
#define EM_MIPS     8       // MIPS RS300

/* object file version */
#define EV_NONE     0       // invalid version
#define EV_CURRENT  1       // current version

typedef struct {
    unsigned char   e_ident[EI_NIDENT];     // marks the file as object file and provides machine-independent data
    Elf32_Half      e_type;                 // identifies the object file type
    Elf32_Half      e_machine;              // specifies the required architecture for an individual file
    Elf32_Word      e_version;              // identifies the object file version
    Elf32_Addr      e_entry;                // gives the virtual address to which the system first transfers control, thus starting the process
    Elf32_Off       e_phoff;                // holds the program header table's file offset in bytes (0 if the file has no program header table)
    Elf32_Off       e_shoff;                // holds the section header table's offset in bytes (0 if the file has no section header table)
    Elf32_Word      e_flags;                // holds processor-specific flags associated with the file
    Elf32_Half      e_ehsize;               // holds the ELF header's size in bytes
    Elf32_Half      e_phentsize;            // holds the size in bytes of one entry in the file's program header table (all entries are the same size)
    Elf32_Half      e_phnum;                // holds the number of entries in the program header table
    Elf32_Half      e_shentsize;            // holds a section header's size in bytes. a section header is one entry in the section header table; all entries are the same size
    Elf32_Half      e_shnum;                // holds the number of entries in the section header table
    Elf32_Half      e_shstrndx;             // holds the section header table index of the entry associated with the section name string table
} Elf32_Ehdr;


/* SECTIONS */
/* special section indexes */
#define SHN_UNDEF       0
#define SHN_LORESERVE   0xff00
#define SHN_LOPROC      0xff00
#define SHN_HIPROC      0xff1f
#define SHN_ABS         0xfff1
#define SHN_COMMON      0xfff2
#define SHN_HIRESERVE   0xffff

/* section types */
#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7fffffff
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xffffffff

/* section attribute flags */
#define SHF_WRITE       0x1
#define SHF_ALLOC       0x2
#define SHF_EXECINSTR   0x4
#define SHF_MASKPROC    0xf0000000

typedef struct {
    Elf32_Word  sh_name;        // specifies the name of the section. its value is an index into the section header string table section
    Elf32_Word  sh_type;        // categorizes the section's contents and semantics
    Elf32_Word  sh_flags;       // sections support 1-bit flags that describe miscellaneous attributes
    Elf32_Addr  sh_addr;        // if the section will appear in the memory image of a process, this member gives the address at which the section's first byte should reside (otherwise 0)
    Elf32_Off   sh_offset;      // gives the byte offset from the beginning of the file to the first byte in the section
    Elf32_Word  sh_size;        // gives the section's size in bytes
    Elf32_Word  sh_link;        // holds a section header table index link, whose interpretation depends on the section type
    Elf32_Word  sh_info;        // holds extra information, whose interpretation depends on the section type
    Elf32_Word  sh_addralign;   // some sections have address alignment constraints
    Elf32_Word  sh_entsize;     // some sections hold a table of fixed-size entries, such as a symbol table. for such a section, this member gives the size in bytes of each entry
} Elf32_Shdr;


/* PROGRAM HEADER */
/* segment types*/
#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6
#define PT_LOPROC   0x70000000
#define PT_HIPROC   0x7fffffff

typedef struct {
        Elf32_Word  p_type;     // tells what kind of segment this array element describes
        Elf32_Off   p_offset;   // gives the offset from the beginning of the file at which the first byte of the segment resided
        Elf32_Addr  p_vaddr;    // gives the virtual address at which the first byte of the segment resides in memory
        Elf32_Addr  p_paddr;     // on systems for which physical addressing is relevant, this member is reserved for the segment's physical address
        Elf32_Word  p_filesz;   // gives the number of bytes in the file image of the segment, it may be zero
        Elf32_Word  p_memsz;    // gives the number of bytes in the memory image of the segment, it may be zero
        Elf32_Word  p_flags;    // gives flags relevant to the segment
        Elf32_Word  p_align;    //
} Elf32_Phdr;

/* functions */
uint8_t elfParser_loadElfFile(uint8_t data[]);
void printElf(uint8_t data[]);

#endif /* KERNEL_SYSTEMMODULES_LOADER_ELFPARSER_H_ */
