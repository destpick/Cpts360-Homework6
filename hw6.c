/********* showblock.c code ***************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h> 
#include <sys/stat.h> 

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#define MAXSIZE 1024


char char_buffer[MAXSIZE];
int fd;

int getDiskBlock(int fd, int blk, char char_buffer[ ]) {
	lseek(fd, (long)blk*MAXSIZE, 0);
	read(fd, char_buffer, MAXSIZE);
}
#define BLOC_OFFSET(block) (MAXSIZE + block-1)*MAXSIZE

void get_inode(int fd, int ino, int inode_table,INODE *inode) {
	lseek(fd, BLOC_OFFSET(inode_table) + (ino - 1) * sizeof(INODE), 0);
	read(fd, inode, sizeof(INODE));
}

does_ext2fs_exist() {

	printf(" FILE descriptor: %d", fd);
	printf("char_buffer: %d\n", char_buffer);

	getDiskBlock(fd, 1, char_buffer);  
	sp = (SUPER *)char_buffer;

	printf("VALID EXT2 FS: s_magic = %x\n", sp->s_magic);
	if (sp->s_magic != 0xEF53) {
		printf("NOT an EXT2 FS\n");
		exit(1);
	}

	printf("\n>>>SUPERBLOCK\n - nblocks: %d\n - ninodes:          %d\n - inodes_per_group: %d\n - # free inodes:    %d\n - # free total_blocks:    %d\n",
			sp->s_blocks_count, sp->s_inodes_count, sp->s_inodes_per_group, sp->s_free_inodes_count, sp->s_free_blocks_count);
}

int InodesBeginBlock = 0;

get_group_descriptor_get_inodebegin() {
	getDiskBlock(fd, 2, char_buffer);
	gp = (SUPER *)char_buffer;

	InodesBeginBlock = gp->bg_inode_table;
	printf("\nInodesBeginBlock: %d\n", InodesBeginBlock);
}

parse_inode_from_beginning_block() {
	printf("\nInodesBeginBlock is %d\n", InodesBeginBlock);

	getDiskBlock(fd, InodesBeginBlock, char_buffer);

	ip = (INODE *)char_buffer + 1;        

	printf("\tMode=%4x ", ip->i_mode);
	printf("\tMid=%d  gid=%d\n", ip->i_uid, ip->i_gid);
	printf("\tSize=%d\n", ip->i_size);
	printf("\tTime=%s", ctime(&ip->i_ctime));
	printf("\tLink=%d\n", ip->i_links_count);
	printf("\ti_block[0]=%d\n", ip->i_block[0]);
}

char *name[128];
char *pathname = "/";
int index_counter = 0, n = 0;

tokenize_pathname() {
	printf("\nPathname: %s\n", pathname);

	name[0] = strtok(pathname, "/");
	printf(" - name[0]: %s\n", name[0]);

	while (name[index_counter] != NULL) {
		index_counter++;
		name[index_counter] = strtok(NULL, "/");
		printf(" - name[%d]: %s\n", index_counter, name[index_counter]);
	}

	n = index_counter;
	printf(" - n = %d\n", n);
}

char dbuf[1024];

int search(INODE * inodePtr, char * name) {
	printf("\nSEARCHING FOR: %s", name);
	for (int index_counter = 0; index_counter < 12; index_counter++) {
		if (inodePtr->i_block[index_counter] == 0)
			return 0;
		getDiskBlock(fd, inodePtr->i_block[index_counter], dbuf);  

		DIR *dp = (SUPER *)dbuf;
		char *cp = dbuf;

		while (cp < &dbuf[1024])
		{
			if (strcmp(name, dp->name) == 0)
			{
				printf("\n - Found at INODE: %d\n", dp->inode);
				return dp->inode;
			}
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
		printf(" - Not Found\n");
		return 0;
	}
}




int i_node_number;

showblock() {
	does_ext2fs_exist();

	get_group_descriptor_get_inodebegin();  

	parse_inode_from_beginning_block();

	tokenize_pathname();  

	for (index_counter = 0; index_counter < n; index_counter++)
	{
		i_node_number = search(ip, name[index_counter]);
		if (i_node_number == 0)
		{
			printf("\nCan't find name[%d]: '%s'\n", index_counter, name[index_counter]);
			exit(1);
		} 

		int INODES_PER_BLOCK = MAXSIZE / sizeof(INODE);

		getDiskBlock(fd, (((i_node_number-1)/INODES_PER_BLOCK)+InodesBeginBlock), char_buffer);
		ip = (INODE *)char_buffer + ((i_node_number-1)%INODES_PER_BLOCK);

		printf("\nFound Inode:\n");
		printf("\tinode=%d\n", i_node_number);
		printf("\tInodesPerBlock: %d\n", INODES_PER_BLOCK);
		printf("\tIn Block: %d\n", (((i_node_number-1)/INODES_PER_BLOCK)+InodesBeginBlock));
		printf("\tOffset: %d\n", ((i_node_number - 1) % INODES_PER_BLOCK));
		printf("\tMode: %4x ", ip->i_mode);
		printf("\tuid: %d and gid: %d\n", ip->i_uid, ip->i_gid);
		printf("\tSize: %d\n", ip->i_size);
		printf("\tTime: %s", ctime(&ip->i_ctime));
		printf("\tLink=%d\n", ip->i_links_count);
		printf("\ti_block[0]:%d\n", ip->i_block[0]);

		if(S_ISDIR(ip->i_mode)) 
		{
			continue;
		}
		else 
		{
			if(index_counter == n-1) 
			{        
				continue;
			}
			else 
			{
				printf("\nname[%d] '%s' is not a DIR\n", index_counter, name[index_counter]);
				exit(1);
			}
		}
	}

	int total_blocks[256],disk_block_one[256],disk_block_two[256];
	printf("\n>>> The direct blocks of inode %d is: \n", i_node_number);
	for(int index_counter = 0; index_counter < 14;index_counter++)
	{
		if (ip->i_block[index_counter] == 0)
		{
			continue;
		}
		else
		{
			printf("i_block[%d] = %d \n", index_counter, ip->i_block[index_counter]);
		}


	}

	printf("\n>>> Indirect Blocks\n");
	getDiskBlock(fd, ip->i_block[12], total_blocks);
	printf(" ");

	for (int index_counter = 0; index_counter < (sizeof(total_blocks) / sizeof(int)); index_counter++)
	{
		if (total_blocks[index_counter] != 0)
		{
			printf("%d ", total_blocks[index_counter]);
			if (index_counter % 10 == 0 && index_counter != 0)
			{
				printf("\n ");
			}
		}

		else
		{
			continue;
		}

	}
	printf("\n\n>>>Double Indirect Blocks\n");
	getDiskBlock(fd, ip->i_block[13], disk_block_one);
	printf(" ");
	for (int index_counter = 0; index_counter < (sizeof(disk_block_one) / sizeof(int)); index_counter++)
	{
		if (disk_block_one[index_counter] != 0)
		{
			printf("%d ", disk_block_one[index_counter]);

			printf("\n *************************************\n");
			printf(" ");
			getDiskBlock(fd, disk_block_one[index_counter], disk_block_two);
			for (int itterator = 0; itterator < 256; itterator++)
			{

				if (disk_block_two[itterator] != 0) 
				{
					printf("%d ", disk_block_two[itterator]);
					if (itterator % 10 == 0 && itterator != 0)
					{
						printf("\n ");
					}
				}

				else
				{
					continue;
				}
			}


		}
	}

}




// Vars for mainline
// Name of disk to open
char *disk = "mydisk";

// Mainline handles opening of disk, then calls showblock()
main(int argc, char *argv[ ]) { 
	// If given a diskname, use it instead of mydisk - DEFAULT: "mydisk"
	if (argc > 1) {
		disk = argv[1];
	}
	// If given a pathname, set pathname - DEFAULT: "/"
	if (argc > 2) {
		pathname = argv[2];
	}
	// Open disk for read only
	fd = open(disk, O_RDONLY);
	if (fd < 0) {
		printf("Open failed\n");
		exit(1);
	}
	printf("'%s' is in RDONLY mode\n", disk);

	// Call main function
	showblock();
}

