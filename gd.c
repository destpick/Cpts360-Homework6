#include <stdio.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <fcntl.h>
#include <linux/types.h>

#define BLKSIZE 1024
#define GDBLK 2

typedef struct ext2_group_desc GD;

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: gd [disk]");
		return 0;
	}
	int fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		printf("open failed\n");
		return 1;
	}

	char buf[BLKSIZE];
	lseek(fd, (long)GDBLK * BLKSIZE, 0);
	read(fd, buf, BLKSIZE);
	GD *gp = (GD*)buf;

	printf("============ start gd.c ===============\n");
	printf("bg_block_bitmap: %u\n", gp->bg_block_bitmap);
	printf("bg_inode_bitmap: %u\n", gp->bg_inode_bitmap);
	printf("bg_inode_table: %u\n", gp->bg_inode_table);
	printf("bg_free_blocks_count: %u\n", gp->bg_free_blocks_count);
	printf("bg_free_inodes_count: %u\n", gp->bg_free_inodes_count);
	printf("bg_used_dirs_count: %u\n", gp->bg_used_dirs_count);
	printf("============= end gd.c ================\n");
	return 0;
}
