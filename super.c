/***************************SUPER START********************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <linux/types.h>
#include <unistd.h>
#include <time.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#define BLKSIZE 1024

char buf[BLKSIZE];
int fd;

int get_block(int fd, int blk, char buf[ ])
{
	lseek(fd, (long)blk*BLKSIZE, 0);
	read(fd, buf, BLKSIZE);
}

int super()
{
	// read SUPER block
	get_block(fd, 1, buf);  
	sp = (SUPER *)buf;

	// check for EXT2 magic number:

	if (sp->s_magic != 0xEF53){
		printf("NOT an EXT2 FS\n");
		exit(1);
	}

	printf("=============== start super.c =====================\n");
	printf("s_inodes_count = %d\n", sp->s_inodes_count);
	printf("s_blocks_count = %d\n", sp->s_blocks_count);

	printf("s_free_inodes_count = %d\n", sp->s_free_inodes_count);
	printf("s_free_blocks_count = %d\n", sp->s_free_blocks_count);

	printf("s_log_block_size = %d\n", sp->s_log_block_size);
	printf("s_blocks_per_group = %d\n", sp->s_blocks_per_group);
	printf("s_inodes_per_group = %d\n", sp->s_inodes_per_group);

	printf("s_mnt_count = %d\n", sp->s_mnt_count);
	printf("s_max_mnt_count = %d\n", sp->s_max_mnt_count);

	printf("s_magic = %x\n", sp->s_magic);

	printf("s_wtime = %s", ctime((time_t *)&sp->s_wtime));
	printf("s_inode_size = %d\n", sp->s_inode_size);
	printf("================ end super.c ==================\n");
}

char *disk = "mydisk";

int main(int argc, char *argv[ ])
{ 
	if (argc > 1)
		disk = argv[1];
	fd = open(disk, O_RDONLY);
	if (fd < 0){
		printf("open failed\n");
		exit(1);
	}
	super();
}
/************************** SUPER END *******************************/
