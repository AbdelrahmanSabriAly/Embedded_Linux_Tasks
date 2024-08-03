#include <stdio.h>
#include <stdint.h>

// Define ELF header constants
#define EI_NIDENT 16

#define EI_MAG0         0               /* File identification byte 0 index */
#define ELFMAG0         0x7f            /* Magic number byte 0 */

#define EI_MAG1         1               /* File identification byte 1 index */
#define ELFMAG1         'E'             /* Magic number byte 1 */

#define EI_MAG2         2               /* File identification byte 2 index */
#define ELFMAG2         'L'             /* Magic number byte 2 */

#define EI_MAG3         3               /* File identification byte 3 index */
#define ELFMAG3         'F'             /* Magic number byte 3 */


// ELF file types
#define ET_NONE         0               /* No file type */
#define ET_REL          1               /* Relocatable file */
#define ET_EXEC         2               /* Executable file */
#define ET_DYN          3               /* Shared object file */
#define ET_CORE         4               /* Core file */
#define ET_NUM          5               /* Number of defined types */
#define ET_LOOS         0xfe00          /* OS-specific range start */
#define ET_HIOS         0xfeff          /* OS-specific range end */
#define ET_LOPROC       0xff00          /* Processor-specific range start */
#define ET_HIPROC       0xffff          /* Processor-specific range end */


// ELF machine types
#define EM_NONE 0
#define EM_M32 1
#define EM_SPARC 2
#define EM_386 3
#define EM_68K 4
#define EM_88K 5
#define EM_860 7
#define EM_MIPS 8
#define EM_S390 22
#define EM_ARM 40
#define EM_ALPHA 41
#define EM_SH 42
#define EM_SPARCV9 43
#define EM_IA_64 50
#define EM_X86_64 62
#define EM_PPC 20
#define EM_PPC64 21
#define EM_RISCV 243

// ELF header structure
typedef struct {
    unsigned char e_ident[EI_NIDENT]; /* ELF identification */
    uint16_t e_type;           /* Object file type */
    uint16_t e_machine;        /* Machine type */
    uint32_t e_version;        /* Object file version */
    uint64_t e_entry;          /* Entry point address */
    uint64_t e_phoff;          /* Program header offset */
    uint64_t e_shoff;          /* Section header offset */
    uint32_t e_flags;          /* Processor-specific flags */
    uint16_t e_ehsize;         /* ELF header size */
    uint16_t e_phentsize;      /* Size of program header entry */
    uint16_t e_phnum;          /* Number of program header entries */
    uint16_t e_shentsize;      /* Size of section header entry */
    uint16_t e_shnum;          /* Number of section header entries */
    uint16_t e_shstrndx;       /* Section header string table index */
} Elf64_Ehdr;


const char* map_type(uint16_t e_type) {
    switch (e_type) {
        case ET_NONE: 			return "NONE (No file type)";
        case ET_REL: 			return "REL (Relocatable file)";
        case ET_EXEC: 			return "EXEC (Executable file)";
        case ET_DYN: 			return "DYN (Shared object file)";
        case ET_CORE: 			return "CORE (Core file)";
        default: 				return "UNKNOWN";
    }
}

const char* map_machine(uint16_t e_machine) {
    switch (e_machine) {
        case EM_NONE: 			return "No machine";
        case EM_M32: 			return "AT&T WE 32100";
        case EM_SPARC: 			return "SPARC";
        case EM_386: 			return "Intel 80386";
        case EM_68K: 			return "Motorola 68000";
        case EM_88K: 			return "Motorola 88000";
        case EM_860: 			return "Intel 80860";
        case EM_MIPS: 			return "MIPS R3000 big-endian";
        case EM_S390: 			return "IBM S/390";
        case EM_ARM: 			return "ARM";
        case EM_ALPHA: 			return "Alpha";
        case EM_SH: 			return "SuperH";
        case EM_SPARCV9: 		return "SPARC v9 64-bit";
        case EM_IA_64: 			return "Intel IA-64";
        case EM_X86_64: 		return "Advanced Micro Devices X86-64";
        case EM_PPC: 			return "PowerPC";
        case EM_PPC64: 			return "PowerPC 64-bit";
        case EM_RISCV: 			return "RISC-V";
        
		default: 				return "UNKNOWN";
    }
}

const char* map_class(unsigned char e_ident) {
    switch (e_ident) {
        case 1: 			return "ELF32";
        case 2: 			return "ELF64";
        
		default: 			return "UNKNOWN";
    }
}

const char* map_data(unsigned char e_ident) {
    switch (e_ident) {
        case 1: 			return "2's complement, little endian";
        case 2: 			return "2's complement, big endian";
        default: 			return "UNKNOWN";
    }
}

void print_elf_header(const Elf64_Ehdr *header) {
    printf("ELF Header:\n");
    printf("  Magic:   ");

    for (int i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", header->e_ident[i]);
    }
    printf("\n");

    printf("  Class:                             %s\n", map_class(header->e_ident[4]));
    printf("  Data:                              %s\n", map_data(header->e_ident[5]));
    printf("  Version:                           %d\n", header->e_ident[6]);
    printf("  OS/ABI:                            %d\n", header->e_ident[7]);
    printf("  ABI Version:                       %d\n", header->e_ident[8]);
    printf("  Type:                              %s\n", map_type(header->e_type));
    printf("  Machine:                           %s\n", map_machine(header->e_machine));
    printf("  Version:                           0x%x\n", header->e_version);
    printf("  Entry point address:               0x%lx\n", header->e_entry);
    printf("  Start of program headers:          %lu (bytes into file)\n", header->e_phoff);
    printf("  Start of section headers:          %lu (bytes into file)\n", header->e_shoff);
    printf("  Flags:                             0x%x\n", header->e_flags);
    printf("  Size of this header:               %u (bytes)\n", header->e_ehsize);
    printf("  Size of program headers:           %u (bytes)\n", header->e_phentsize);
    printf("  Number of program headers:         %u\n", header->e_phnum);
    printf("  Size of section headers:           %u (bytes)\n", header->e_shentsize);
    printf("  Number of section headers:         %u\n", header->e_shnum);
    printf("  Section header string table index: %u\n", header->e_shstrndx);
}

int main(int argc, char *argv[]) {

    FILE *file = fopen(argv[1], "rb");

    Elf64_Ehdr header;
    size_t bytesRead = fread(&header, 1, sizeof(header), file);

    print_elf_header(&header);

    fclose(file);
    return 0;
}
