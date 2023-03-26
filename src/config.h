#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define viros_TOTAL_INTERRUPTS 512

// 100 MB Heap Size
#define viros_HEAP_SIZE_BYTES 104857600
#define viros_HEAP_BLOCK_SIZE 4096 // 4 KB
#define viros_HEAP_ADDRESS 0x01000000
#define viros_HEAP_TABLE_ADDRESS 0X00007E00

#define viros_SECTOR_SIZE 512

#define viros_MAX_FILESYSTEMS 12
#define viros_MAX_FILE_DESCRIPTORS 512

#define viros_MAX_PATH 108

#define viros_TOTAL_GDT_SEGMENTS 6

#define HARDIKHYPERION_OS_VIRTUAL_ADDRESS 0x400000

#define viros_USER_PROGRAM_STACK_SIZE 1024 * 16
#define viros_PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define viros_PROGRAM_VIRTUAL_STACK_ADDRESS_END viros_PROGRAM_VIRTUAL_STACK_ADDRESS_START - viros_USER_PROGRAM_STACK_SIZE

#define viros_PROGRAM_VIRTUAL_ADDRESS 0x400000

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b

#define viros_MAX_PROGRAM_ALLOCATIONS 1024
#define viros_MAX_PROCESSES 12

#define viros_MAX_ISR80H_COMMANDS 1024

#define viros_KEYBOARD_BUFFER_SIZE 1024

#endif