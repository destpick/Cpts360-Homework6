#include <stdio.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <unistd.h>

#define BLKSIZE 1024

typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;

int get_block(int fd, int blk, char buf[ ])
{
	lseek(fd, (long)blk*BLKSIZE, 0);
	read(fd, buf, BLKSIZE);
}

int tst_bit(char *buf, int bit)
{
	int i, j;
	i = bit / 8;  j = bit % 8;
	if (buf[i] & (1 << j))
		return 1;
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: bmap [drive]");
		return 0;
	}
	int fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		printf("open %s failed\n", argv[1]);
		return 1;
	}

	char buf[BLKSIZE];
	get_block(fd, 1, buf);
	SUPER* sp = (SUPER*)buf;
	printf("============= start bmap =========\n");
	int nblocks = sp->s_blocks_count;
	printf("nblocks = %d\n", nblocks);

	get_block(fd, 2, buf);
	GD* gp = (GD*)buf;

	int bmap = gp->bg_block_bitmap;
	printf("bmap = %d\n", bmap);

	get_block(fd, bmap, buf);

	for (int i = 0; i < nblocks; i++)
	{
		putchar(tst_bit(buf, i) + 48);
		if(i && (i % 8) == 0)
			putchar(' ');
		if(i && (i % 32) == 0)
			putchar('\n');
	}
	putchar('\n');
	printf("============ finish bmap ============= \n");
}
