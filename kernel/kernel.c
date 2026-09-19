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
	
    while (1)
    {
    }
}
