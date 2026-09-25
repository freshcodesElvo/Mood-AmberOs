#include  <stdint.h>
#include "screen.h"
#include "interrupts.h"
#include "idt.h"
#include "paging.h"
#include "multiboot.h"
#include "frame_allocator.h"
#include "memory_test.h"
#include "heap.h"
//#include "frame_allocator_test.h"

extern uint32_t kernel_start;
extern uint32_t kernel_end;

void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info_addr)
{
    clear_screen();

    print("---------------------------------\n");
    print("MOOD-AMBER OS\n");
    print("---------------------------------\n\n");
	
	print("Multiboot magic >>>> ");
	print_hex(multiboot_magic);
	print("\n");

	



	

	/* FRAME ALLOCATOR */

	print("\nStarting frame allocator...\n");

	frame_allocator_init(multiboot_info_addr);
	//frame_allocator_test();
	print("Frame allocator initialized!\n");



		
    print("Kernel Version 0.0.1\n");

    interrupts_init();
    idt_init();

	paging_init();
	
	print("********TESTING KERNEL HEAP***********\n");
	uint32_t *heap_test = (uint32_t *)kmalloc(sizeof(uint32_t));
	if(heap_test ==0){
		print("Heap allocation failed: \n");
	}else{
		*heap_test = 0xCAFEBABE;
		print("Heap allocation successful!\n");
		print("Heap value: ");
		print_hex(*heap_test);
		print("\n");
	}
	print("**********KERNEL HEAP TEST DONE!***************");

	print("\n\n******** RUNNING NEW MEMORY TEST ********\n");
	print("TESTING HEAP DEALLOCATION...\n");
	kfree(heap_test);
	print("Heap memory freed succesfully\n");
	print("KERNEL HEAP TES DONE\n");
       //memory_test();
	//print("******** NEW MEMORY TEST FINISHED ********\n");	
	print("??????????????????????????????????????????????\n");
	print("Testing heap re use\n");
	uint32_t *reused_heap  = (uint32_t *)kmalloc(sizeof(uint32_t));
	if(reused_heap ==0){
		print("Heap re use failed\n");
	}else{
		print("Heap re use succesful\n");
		print("Original address: ");
		print_hex((uint32_t)heap_test);
		print("\n");
		
		print("Reused address: ");
		print_hex((uint32_t)reused_heap);
		print("\n");
		
		*reused_heap = 0x12345678;
		print_hex(*reused_heap);
		print("\n");
		
		kfree(reused_heap);

		print("???????????????????????????????????????????????\n");
		print("Testing multi page allocations\n");
		uint32_t *multi_page_heap = (uint32_t *)kmalloc(8192);
		if(multi_page_heap==0){
			print("multipage allocation failed terriblyyyyyyyy  buuuuuH!! haahahahhhah!!!\n");
			
		}else{
			print("Multi page allocation siuccessful!1 yeeeyy!!\n");
			//write to the first pge
			multi_page_heap[0] = 0x11111111;
			//write to the 2nd pge
			multi_page_heap[1024] = 0x22222222;
			
			print("First page value: ");
			print_hex(multi_page_heap[0]);
			print("\n");
			
			print("Second page value: ");
			print_hex(multi_page_heap[1024]);
			print("\n");
			
			kfree(multi_page_heap);
			print("Multi page memory freed succesful!\n");
		}
	}
	
	print("---------------Heap stress test-----------------------\n");
	//allocate  3 different block
	uint32_t *stress_a = (uint32_t *)kmalloc(4096);
	uint32_t *stress_b = (uint32_t *)kmalloc(4096);
	uint32_t *stress_c = (uint32_t *)kmalloc(4096);

	if(stress_a==0 || stress_b==0 || stress_c==0){
		print("Stress allocation failled !!\n");
		
	}else{
		//write diff vals into each block
		*stress_a = 0xAAAAAAAA;
		*stress_b = 0xBBBBBBBB;
		*stress_c = 0xCCCCCCCC;
		print("Three allocations successful\n");

		print("Block A:\n");
		print_hex((uint32_t)stress_a);
		print("\n");

		print("Block B \n");
		print_hex((uint32_t)stress_b);
		print("\n");

		print("Block C: \n");
		print_hex((uint32_t)stress_c);
		print("\n");

		//free the middle block first
		kfree(stress_b);
		print("Block B freed!\n");
		//allocate another bloack of the same size
		uint32_t *stress_reused=(uint32_t *)kmalloc(4096);
		if(stress_reused == 0){
			print("Reused allocation failed!!!!! buuuuhhh !!!!!\n");
		}else{
			print("Re used block Addr: ");
			print_hex((uint32_t)stress_reused);
			print("\n");

			if(stress_reused == stress_b){
				print("success!! block re used!!\n");
				
			}else{
				print("Warning: different addr returned\n");
			}
			*stress_reused = 0xDDDDDDDD;
			print("Reused block value:");
			print_hex(*stress_reused);
			print("\n");
			kfree(stress_reused);
			
		}
		//free the remaining original blocks
		kfree(stress_a);
		kfree(stress_c);
		print("Remaining blocks freedd!! hurrayyy!!!\n");
		print("----------Heap  stress test finished---------------------");

		
	}
	print("------------ Allocation sizes test----------------\n");
	uint32_t sizes[] = {1,100,4096,4097,8192,8193};
	void *blocks[6] ={0};
	for(int i =0; i<6; i++){
		blocks[i] = kmalloc(sizes[i]);
		if(blocks[i] == 0){
			print("Allocations failed for size: ");
			print_hex(sizes[i]);
			print("\n");
			continue;
		}
		uint8_t *memory = (uint8_t *)blocks[i];
		//write to the 1st and last requested bytes
		memory[0] = 0xAA;
		memory[sizes[i]-1]=0xBB;
	
		print("Requested bytes: ");
		print_hex(sizes[i]);
		print(" | Address: ");
		print_hex((uint32_t)blocks[i]);
		print(" | SUCCESS\n");
		
	}
	print("Freeing allocations...\n");
	for(int i=0; i<6; i++){
		if(blocks[i] !=0){
			kfree(blocks[i]);
		}
		
	}
	print("all allocations freed\n");
	print("-----------allocation size test finished----");

	print("-----=-=-=-=-=zero size test=-=-=-=-=-=-=-=-=\n");
	void *zero_test = kmalloc(0);
	if(zero_test == 0){
		print("Success: zero size allocation rejected!!\n");
		
	}else{
		print("Error: zero size allocation returned an address!!!\n");
		kfree(zero_test);
	
	
	}
	print("=-=-=-=-=-=-=-=-=-=- overflow test=-=-=-=-=-=-=-=-=-=-=-\n");
	void *overflow_test = kmalloc(0xFFFFFFFFu);
	if(overflow_test == 0){
		print("success: oversized allocation rejected\n");
	}else{
		print("Error: oversized alloccation returned an address\n");
		kfree(overflow_test);
	}
	print("==========overflow test finished============");
    while (1)
    {
    }
}

