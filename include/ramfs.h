#ifndef RAMFS_H
#define RAMFS_H

# include <stdint.h>

#define RAMFS_MAX_NODES 64
#define RAMFS_NAME_MAX 12
#define RAMFS_DATA_MAX 512

//node types
typedef enum{
	RAMFS_DIRECTORY,
	RAMFS_FILE
} ramfs_type_t;

//file system node
typedef struct ramfs_node{
	char name[RAMFS_NAME_MAX];
	
	ramfs_type_t type;
	
	struct ramfs_node *parent;
	struct ramfs_node *children;
	struct ramfs_node *next_sibling;

	char data[RAMFS_DATA_MAX];
	uint32_t size;

	
}ramfs_node_t;
//file system initiliazation
void ramfs_init(void);
//root dir
ramfs_node_t *ramfs_root(void);
ramfs_node_t *ramfs_create_directory(ramfs_node_t *parent, const char *name);
ramfs_node_t *ramfs_find(ramfs_node_t *parent, const char *name);
ramfs_node_t *ramfs_create_file(ramfs_node_t *parent, const char *name);
int ramfs_write_file(ramfs_node_t *file, const char *data);
int ramfs_read_file(ramfs_node_t *file, char *buffer, uint32_t buffer_size);
ramfs_node_t *ramfs_first_child(ramfs_node_t *directory);
#endif
