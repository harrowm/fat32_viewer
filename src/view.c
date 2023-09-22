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

    //  Bytes per sector. Allowed values include 512, 1024, 2048, and 4096
    // uint16_t bps = boot_entry.sector_size[0] + ((uint16_t) boot_entry.sector_size[1] << 8);
    // off_t root_entry_offset = ( boot_entry.reserved +
    //                             boot_entry.fats * boot_entry.fat32.length) * bps;
    // uint32_t bpc = bps * boot_entry.sec_per_clus;
    // off_t fat_offset = bps * boot_entry.reserved;

    printf("sector size %d\n", boot_entry.sector_size);
    printf("reserved %d\n", boot_entry.reserved);
    printf("sectors per cluster %d\n", boot_entry.sec_per_clus);
    printf("root directory cluster %d\n", boot_entry.fat32.root_cluster);
    printf("number of fat tables %d\n", boot_entry.fats);
    uint16_t rootDirectorySector = boot_entry.fat32.root_cluster * boot_entry.fats + boot_entry.reserved;
    printf("(calc) rootDirectorySector %d\n", rootDirectorySector);
    
//     unsigned int i, start_sector, length_sectors;
//     Parttition tables for primary partition
// 0x1BE 
// 0x1CE
// 0x1DE
// 0x1EE

    // tidy this uop with a better MBR structure ..

    struct msdos_partition part[4];
    
    fseek(fp, 0x1BE, SEEK_SET); // go to partition table start
    uint32_t res = fread(&part, sizeof(struct msdos_partition), 4, fp);

    printf("\nPartition status, type, sector_count\n");
    for (int i=0; i<4; i++) {  
        printf("%02X ", part[i].bootable);
        printf("%02X ", part[i].partition_type);
        printf("%08X\n", part[i].number_sectors);
    }
    
    DumpHex(0x1BE, &part, sizeof(struct msdos_partition)*4);

    printf("\n");
    // print out directory table
    fseek(fp, rootDirectorySector * boot_entry.sec_per_clus * boot_entry.sector_size, SEEK_SET); // go to partition table start

    struct msdos_dir_entry dir_entry[16];
    size_t ret = fread(&dir_entry, rootDirectorySector * boot_entry.sec_per_clus * boot_entry.sector_size, 16*sizeof(struct msdos_dir_entry), fp);
//               
    DumpHex(rootDirectorySector * boot_entry.sec_per_clus * boot_entry.sector_size, &dir_entry, 16*sizeof(struct msdos_dir_entry));



    
// for(uint32_t i = 0; i < bpc / sizeof(struct msdos_dir_entry); i++) {
//               struct msdos_dir_entry dir_entry;
//               size_t ret = fread(&dir_entry, 1, sizeof(struct msdos_dir_entry), fp);
//               if(dir_entry.name[0] != 0x00) {
//                   if(dir_entry.attr != 0x0f) {
                    // Insert your code here
                    /*
                    Read the file name:
                      1. Check whether the file is deleted or not
                      2. Convert to 8.3 format names
                      3. Check whether it is a subdirectory

                    uint32_t dir_first_cluster = get_dir_first_cluster(&dir_entry);
                    printf("%d, ", _);
                    printf("%s, ", _);
                    printf("%d, ", _);
                    printf("%d", _);
                    printf("\n");
                    */
                //   }
                //   if((dir_entry.attr & 0x10) != 0) {
  /* It is a subdirectory */
//  }
            //       else {
            //           printf("%d, LFN entry\n", idx++);
            //       }
            //   }
    return 0;
exit:
    return 1;
}
