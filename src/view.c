// Simple fat32 disk image viewer
// Designed to run on a Mac or raw on a rosco_68k computer

// Note the rosco computer has a 68000 CPU, endianess is an issue ..
// The sbc also has limited stdlib on boot

// Only considers FAT32
// References this article:
// https://eric-lo.gitbook.io/lab9-filesystem/overview-of-fat32#

#include <stdio.h>
#include "msdos_fs.h"

//#define IMAGE_NAME "/Users/malcolm/Downloads/sd8gb.img"
#define IMAGE_NAME "/Users/malcolm/cpm68k-bootstrap/disk/target/rosco.img"

// adapted from https://gist.github.com/ccbrown/9722406
void DumpHex(uint32_t addr, const void* data, size_t size) {
	char ascii[17];

    // for each line, ignore any partial line
    for (size_t k=0; k < (size / 16); k++) {
        ascii[16] = '\0';
        printf("%08X  ", addr);
        addr += 16;
        // for each character on the line
        for (size_t i=(k*16); i < ((k+1)*16); ++i) {
            printf("%02X ", ((unsigned char*)data)[i]);
            if (i == (k*16 + 7)) {
                printf(" ");
            }
            if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
                ascii[i % 16] = ((unsigned char*)data)[i];
            } else {
                ascii[i % 16] = '.';
            }
        }
        printf(" |%s|\n", ascii);
    }
}


int main() {
    FILE *fp = NULL;
    struct fat_boot_sector boot_entry;

    fp = fopen(IMAGE_NAME, "r+");
    if(fp == NULL) {
        printf("Failed to open image %s\n", IMAGE_NAME);
        goto exit;
    }

    // read mbr
    uint32_t numItem = fread(&boot_entry, sizeof(struct fat_boot_sector), 1, fp);
    if(numItem != 1) {
        printf("Failed to read MBR in image %s\n", IMAGE_NAME);
        goto exit;
    }


    uint32_t sector_size = (boot_entry.sector_size[0]) + (boot_entry.sector_size[1]<<8); 
    printf("sector size %d\n", sector_size);
    printf("reserved %d\n", boot_entry.reserved);
    printf("sectors per cluster %d\n", boot_entry.sec_per_clus);
    printf("root directory cluster %d\n", boot_entry.fat32.root_cluster);
    printf("number of fat tables %d\n", boot_entry.fats);
    uint16_t rootDirectorySector = boot_entry.fat_length * boot_entry.fats + boot_entry.reserved;
    printf("(calc) rootDirectorySector %d\n", rootDirectorySector);

    // tidy this up with a better MBR structure ..

    struct msdos_partition part[4];
    
    fseek(fp, 0x1BE, SEEK_SET); // go to partition table start
    uint32_t res = fread(&part, sizeof(struct msdos_partition), 4, fp);

    printf("\nPartition status, type, sector_count\n");
    for (int i=0; i<4; i++) {  
        printf("%02X ", part[i].bootable);
        printf("%02X ", part[i].partition_type);
        printf("%08X   ", part[i].number_sectors);
    }
    printf("\n");
    
    DumpHex(0x1BE, &part, sizeof(struct msdos_partition)*4);
    printf("\n");


    // print out directory table
    fseek(fp, rootDirectorySector * boot_entry.sec_per_clus * sector_size, SEEK_SET); // go to partition table start

    struct msdos_dir_entry dir_entry[16];
    res = fread(&dir_entry, rootDirectorySector * sector_size, 16*sizeof(struct msdos_dir_entry), fp);

    DumpHex(rootDirectorySector * sector_size, &dir_entry, 16*sizeof(struct msdos_dir_entry));

    return 0;
exit:
    return 1;
}
