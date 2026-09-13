#include "screen.h"
#include "frame_allocator.h"

void frame_allocator_test(void){
	print("      TESTING FRAME ALLOCATOR          \n");
	uint32_t frame_a = allocate_frame();
	
	print("FRAME A: ");
	print_hex(frame_a);
	print("\n");
	
	uint32_t frame_b = allocate_frame();
	print("FRAME B: ");
	print_hex(frame_b);
	print("\n");
	free_frame(frame_a);
	print("Frame a freed!\n");

	uint32_t  frame_c = allocate_frame();
	print("FRAME C: ");
	print_hex(frame_c);
	print("\n");

	if(frame_c == frame_a){
		print("Frame reclamation test: -> success!\n");
		
	}else{
		print("Frame reclamation test: -> failed!\n");
	}
	//return the test frames to the allocator
	free_frame(frame_b);
	free_frame(frame_c);		
}
