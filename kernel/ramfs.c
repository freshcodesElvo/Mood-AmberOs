	#include  "ramfs.h"

//fixed memory pool for file syst nodes
static ramfs_node_t node_pool[RAMFS_MAX_NODES];

//keep track of allocated nodes
static uint32_t nodes_used = 0;

//root dir
static ramfs_node_t *root = 0;

//allocate a new file syst node
static ramfs_node_t *ramfs_allocate_node(void){
	if(nodes_used >= RAMFS_MAX_NODES){return 0;}
	ramfs_node_t *node = &node_pool[nodes_used++];
	
	//initialize node memory
	node ->name[0] =  '\0';
	node ->type = RAMFS_FILE;
	node ->parent =0;
	node ->children = 0;
	node ->next_sibling = 0;
	node ->size = 0;

	for(uint32_t i =0; i<RAMFS_DATA_MAX; i++){
		node->data[i] = '\0';
	}
	return node;


}
//initialize file syst
void ramfs_init(void){
	nodes_used =0;
	root = ramfs_allocate_node();
	if(root==0){
		return;
	}
	root->name[0]='/';
	root->name[1]='\0';
	
	root->type = RAMFS_DIRECTORY;

}

//return root directory
ramfs_node_t *ramfs_root(void){
	return root;
}
ramfs_node_t *ramfs_create_directory(ramfs_node_t *parent, const char *name){
	//1 validate the parent directory and name
	if(parent==0 || parent->type != RAMFS_DIRECTORY || name==0 || name[0]=='\0'){
		return 0;
	}
	//2 check the  name length
	uint32_t length =0;
	while(name[length] != '\0'){
		if(length>=RAMFS_NAME_MAX-1){
			return 0;
		}
		length++;
	}
	//3 check whether a child with this name already exists
	ramfs_node_t *current = parent->children;
	while(current !=0){
		uint32_t i =0;
		while(current->name[i]==name[i] && name[i] !='\0'){
			i++;
		}
		if(current->name[i]=='\0' && name[i]=='\0'){
			return 0; //duplicate name
		}
		current = current->next_sibling;
	
	}
	//4 allocate  a new node
	ramfs_node_t *directory = ramfs_allocate_node();
	if(directory==0){
		return 0;
	}
	//5 copy the dir name
	for(uint32_t i=0; i<=length;i++){
		directory->name[i]=name[i];
		
	}
	//6set its properties
	directory->type = RAMFS_DIRECTORY;
	directory->parent = parent;
	//7  attach it to the parent's children
	directory->next_sibling=parent->children;
	parent->children= directory;
	return directory;
}


ramfs_node_t *ramfs_find(ramfs_node_t *parent, const char *name)
{
    // Validate inputs
    if (parent == 0 || parent->type != RAMFS_DIRECTORY || name == 0) {
        return 0;
    }

    // Start with the first child
    ramfs_node_t *current = parent->children;

    // Search through the linked list
    while (current != 0) {
        uint32_t i = 0;

        // Compare the names character by character
        while (current->name[i] == name[i] && name[i] != '\0') {
            i++;
        }

        // Both strings ended at the same position
        if (current->name[i] == '\0' && name[i] == '\0') {
            return current;
        }

        current = current->next_sibling;
    }

    // No matching node found
    return 0;
}
ramfs_node_t *ramfs_first_child(ramfs_node_t *directory){
	//validate taht the node exists and is a dir
	if(directory==0 || directory->type !=RAMFS_DIRECTORY){
		return 0;
	}
	//return the 1st child
	return directory->children;
}

ramfs_node_t *ramfs_create_file(ramfs_node_t *parent, const char *name){
	//1 validate inputs
	if(parent==0 ||parent->type !=RAMFS_DIRECTORY || name==0 ||name[0] == '\0'){
	return 0;	
	}
	
	//2 check name length
	uint32_t length =0;
	while(name[length] != '\0'){
		if(length>=RAMFS_NAME_MAX-1){
			return 0;
		}		
		length++;
	}
	//3 prevent duplicate names
	if(ramfs_find(parent,name)!=0){return 0;}
	//allocate a node
	ramfs_node_t *file = ramfs_allocate_node();
	if(file==0){return 0;}
	//copy the file name
	for(uint32_t i =0 ; i<=length; i++){
		file->name[i]=name[i];
		
	}
	//6. set file properties
	file->type = RAMFS_FILE;
	file->parent = parent;
	file->size =0;
	//attach file to parent dir
	file->next_sibling=parent->children;
	parent->children = file;
	return file;

}

int ramfs_write_file(ramfs_node_t *file, const char *data){
	// validate inputs
	if(file ==0 || file->type != RAMFS_FILE || data ==0){
		return -1;
	}
	//2. calculate the length of the incoming data
	uint32_t length = 0;
	while(data[length] !='\0'){
		length++;

		//prevent writing beyond the file capacity
		if(length> RAMFS_DATA_MAX){
			return -1;
		}
	}
	//3. copy data into the file
	for(uint32_t i= 0; i< length; i++){
		file->data[i]=data[i];
		
	}
	//4.add a null terminator if there is  a space
	if(length<RAMFS_DATA_MAX){
		file->data[length]='\0';
		
	}
	//5.	 record the file size
	file->size = length;
	return (int)length;
}


//append content into a file
int ramfs_append_file(ramfs_node_t *file, const char *data){
	//validate the file
	if(file==0||data==0){
		return -1;
	
	}
	if(file->type != RAMFS_FILE){
		return -1;
	}
	//find the length of the incoming data
	uint32_t length =0;
	while(data[length] != '\0'){
		length++;
	}
	//check whether the new content fits
	if(file->size+length>=RAMFS_DATA_MAX){
		return -1;
	}
	//copy new content to the end of existing content
	for(uint32_t i = 0; i<length;i++){
		file->data[file->size+i]=data[i];
		
	}
	//update file size and terminate the string 
	file->size +=length;
	file->data[file->size]='\0';
	return (int)length;
}
int ramfs_read_file(ramfs_node_t *file, char *buffer, uint32_t buffer_size){
	//1 validate iputs
	if(file==0 || file->type != RAMFS_FILE || buffer==0 || buffer_size ==0){return -1;}
	//2 ensure the destination can hold the data and null terminator
	if(buffer_size<=file->size){
		return -1;
	}
	//copy file contents int o the destonation buffer
	for(uint32_t i =0; i<file->size; i++){
		buffer[i]=file->data[i];
		
	}
	//4 terminate the string
	buffer[file->size]='\0';
	//5 return the num of bytes read
	return (int)file->size;
}
