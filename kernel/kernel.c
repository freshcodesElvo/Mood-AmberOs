#include  <stdint.h>
#include "screen.h"
#include "interrupts.h"
#include "idt.h"
#include "paging.h"
#include "multiboot.h"
#include "frame_allocator.h"
#include "memory_test.h"
#include "heap.h"
#define HEAP_TEST_ALLOCATION_COUNT 128
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

	print("*************invalid free test**************");
	uint32_t  *test_block = (uint32_t *)kmalloc(4096);
	if(test_block ==0){
		print("error:  allocation failed!\n");

	}else{
		*test_block = 0x12345678;
		print("valid allocation created successfully\n");

		//test, free an intirior pointer
		kfree((void *)((uint32_t)test_block+4));

		if(*test_block==0x12345678){
			print("success: interior pointer free ignored\n");
			
		}else{
			print("Error: interior pointer free corrupted allocation\n");
		}
		//test 1 --> free an address that was never allocated
		kfree((void *)0x03000000);
		print("invallid ponter free atempted\n");
		
		//confirmation that valid allocation still works
		if(*test_block == 0x12345678){
			print("Success: valid allocation is still intact\n");
		
		}
		else{
			print("error: allocation was corrupted\n");
		}
		//test 2--> free the valid allocation
		kfree(test_block);
		print("valid allocation freed\n");

		//test 3 -->> attempt to free it the 2nd time
		kfree(test_block);
		print("double free attempted\n");
	
		uint32_t *reuse_test = (uint32_t *)kmalloc(4096);
		if(reuse_test == 0){print("Error: reallocation failed\n");}
		else{
			*reuse_test = 0xCAFEBABE;
			if(*reuse_test == 0xCAFEBABE){
				print("success: memory reused succesfully\n");
			}
			else{
				print("reused memory corrupted\n");
			}
			kfree(reuse_test);
		}		
	
	}

	print("invalid free test finished__________\n");

	print("\n++++++++++++++++++++ tracking limit test++++++++++++++++++++++++++++++++\n");
	void *tracking_blocks[HEAP_TEST_ALLOCATION_COUNT];
	uint32_t allocated = 0;
	for(uint32_t i = 0; i<HEAP_TEST_ALLOCATION_COUNT; i++){
		tracking_blocks[i] = kmalloc(4096);
		if(tracking_blocks[i] ==0){
			print("allocation stopped at index: ");
			print_hex(i);
			print("\n");
			break;
		}
		allocated++;
	}
	
	print("Succesful allocations: " );
	print_hex(allocated);
	print("\n");
	
	void *extra_block = kmalloc(4096);
	if(extra_block == 0){
		print("success: full tracking table rejected allocation\n");
		
	}else{
		print("error: allocation succeeded beyond tracking limit!!\n");
		kfree(extra_block);
	}
	for(uint32_t i = 0; i< allocated; i++){
		kfree(tracking_blocks[i]);
	}
	print("tracking test allocations freed\n");
	print("???????????????????tracking limit test finished???????????????????\n");

	print("___________different size re use test____________________\n");
	void *fragment_a = kmalloc(4096);
	void *fragment_b = kmalloc(8192);
	void *fragment_c = kmalloc(4096);

	if(fragment_a ==0 || fragment_b == 0 || fragment_c ==0){
		print("Error: initial allocation failed\n");

	}else{
		print("Block A: ");
		print_hex((uint32_t)fragment_a);
		print("\n");

		print("Block B: ");
		print_hex((uint32_t)fragment_b);
		print("\n");
		
		print("Block C: ");
		print_hex((uint32_t)fragment_c);
		print("\n");

		kfree(fragment_b);
		print("Block B freed!! \n");
		void *fragment_reused = kmalloc(8192);
		
		if(fragment_reused == fragment_b){
			print("Success: two-block pge reused succesfully\n");
			
		}else if(fragment_reused ==0){
			print("error: re allocation failed\n");
			
		}else{
			print("warning: a different address was returned");
		}

		if(fragment_reused !=0 ){
			kfree(fragment_reused);
		}
		kfree(fragment_a);
		kfree(fragment_c);
	}
	print("+++++++++++++differnt size reuse test finished++++++++++++++++++++++\n");


	print("\n diffent size reuse test 2+++++++++++++++++++++\n");
	void *large_block = kmalloc(8192);
	if(large_block==0){
		print("Error: large allocation failed\n");

	}
	else{
		print("large block address: ");
		print_hex((uint32_t)large_block);
		print("\n");
		kfree(large_block);
		print("large block freed\n");

		void *small_block =  kmalloc(4096);
		if(small_block == 0){
			print("Error: small allocation failed\n");
			
		}else{
			print("Small block address: ");
			print_hex((uint32_t)small_block);
			print("\n");

			if(small_block == large_block){

				print("small allocation reused large block address \n");
				
			}
			else{
				print("small allocation used a different address!\n");
			}
			kfree(small_block);



		}

	}
	print("========= differnt size reused test 2 finished==================\n");

	print("\n======heap boundary test=================\n");
	void *boundary_test = kmalloc(0x01000000);
	if(boundary_test==0){
		print("success: allocation beyond heap limit rejected!!\n");
		
	}else{
		print("error: heap boundary was exeeded!!\n");
		kfree(boundary_test);
	}
	print("---------------heap boundary test finished---------------\n");

	print("\n@@@@@@@@@@@fragmentation test@@@@@@@@@@@@@@@@@@@@@\n");
	void *frag_a = kmalloc(4*4096);
	void *frag_b = kmalloc(6*4096);
	void *frag_c = kmalloc(8*4096);
	void *frag_d = kmalloc(10*4096);
	void *frag_e =  kmalloc(12*4096);
	
	if(!frag_a || ! frag_b || !frag_c || !frag_d || !frag_e){
		print("Error: fragmentation allocation failed\n");
	}else{
		print("5 allocations successfull!\n");
		kfree(frag_b);
		kfree(frag_d);
	
		print("b and d freed!!!\n");
		void *reuse_b =  kmalloc(6*4096);
		void *reuse_d = kmalloc(10*4096);

		if(reuse_b  == frag_b){
			print("success: block reused!\n");
		}else{
			print("ERROR: block b reuse failed\n");
		}
		if(reuse_d == frag_d){
			print("success: block d reused!!\n");
		}else{
			print("error: block d reuse failed\n");
		}
		kfree(reuse_b);
		kfree(reuse_d);

		kfree(frag_a);
		kfree(frag_c);
		kfree(frag_e);

		print("All fragmentation test allocations freed\n");
	}

	print("+++++++++++fragmentation test finished++++++++++++++\n");

	print("\n++++++++ memory coaelescing test++++++++++++++++++++++++\n");

	void *coal_a = kmalloc(4*4096);
	void *coal_b = kmalloc(6*4096);
	void *coal_c = kmalloc(8*4096);

	if(coal_a == 0 || coal_b == 0|| coal_c ==0){
		print("Error: initial allocation failed\n");
		if(coal_a != 0) kfree(coal_a);
		if(coal_b != 0) kfree(coal_b);
		if(coal_c != 0) kfree(coal_c);
		
	}else{
		print("bloc A: ");
		print_hex((uint32_t)coal_a);
		print("\n");
	
		print("block B: ");
		print_hex((uint32_t)coal_b);
		print("\n");

		print("block c: ");
		print_hex((uint32_t)coal_c);
		print("\n");

		if((uint32_t)coal_c != (uint32_t)coal_b + (6*4096)){
			print("blocks b and c are not adjacent!!\n");
		}else{
			print("block b and c are adjacent!!\n");
			
			kfree(coal_b);
			kfree(coal_c);
		
			print("bocks b and c freed\n");

			void *large_block = kmalloc(14*4096);
			
			if(large_block ==coal_b){ 
				print("success: adascent blocks reused\n");
				
			}else if(large_block ==0){
				print("large allocation failed\n");

			}else{
				print("no coalescing: new address returned!!\n");
			
				print("returned address: ");
				print_hex((uint32_t)large_block);
				print("\n");

				
			}
			if(large_block !=0){
				kfree(large_block);
			}
		}
		kfree(coal_a);
	}
	print("***************memory coalescing test done!!!*************************\n");

	print("\n-------------------three block coalescing test------------------------\n");
	void *three_a = kmalloc(4*4096);
	void *three_b = kmalloc(6*4096);
	void *three_c = kmalloc(8*4096);
	



	
print("Block A: ");
print_hex((uint32_t)three_a);
print("\n");

print("Block B: ");
print_hex((uint32_t)three_b);
print("\n");

print("Block C: ");
print_hex((uint32_t)three_c);
print("\n");











	
	if(!three_a || !three_b || !three_c ){
		print("Error: initial allocations failed! \n");
		if(three_a)kfree(three_a);
		if(three_b)kfree(three_b);
		if(three_c)kfree(three_c);
		
	}else{
		print("Three allocations success!!\n");
		kfree(three_b);
		print("block_b freed\n");
		
		kfree(three_c);
		print("bolck c freed\n");
		
		kfree(three_a);
		print("block a freed!\n");
		
		void *merged_block=kmalloc(18*4096);
		if(merged_block==three_a){
			print("SUCCESS!: three blocks mrged!!\n");
		}else if(merged_block ==0){
			print("error: meged allocations failed!\n");		
		}else{
			print("warning: differnt address returned\n");
			print_hex((uint32_t)merged_block);
			print("\n");
		}
		if(merged_block){
			kfree(merged_block);
			
		}
		
	}
	print("---------------------three block coalescing test finished--------------------------\n");

	print("----block splitting test---------------");
	void *large = kmalloc(22*4096);
	if(large == 0){
		print("Error: large allocation failed!\n");
		
	}else{
		kfree(large);
		print("22 page block frred!\n");

		void *first = kmalloc(18*4096);
		if(first==large){
		print("success: 18 page allocation reused original address !\n");	
		}else{
			print("warning: different address returned\n");
			
		}if(first !=0){
			//requesting the remaining 4 pages
			void *remainder = kmalloc(4*4096);
			if((uint32_t)remainder == (uint32_t)large + (18*4096)){
				print("success: remaining 4 pages reused!\n");
			}else{
				print("error: remaining block not reused \n");
			}
			if(remainder!=0){
				kfree(remainder);
			}
			kfree(first);
		}
	}
	print("--------------- block splitting test done -----------------\n");

	keyboard_irq_enable();
	asm volatile ("sti");

    while (1)
    {
	asm volatile("hlt");
    }
}

