#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)




  uint32 following = KERNEL_HEAP_START;
  struct mem
  {
	int nOfPages;
  	uint32 StartAdress;

  }memArr[((KERNEL_HEAP_MAX-KERNEL_HEAP_START))/PAGE_SIZE];

void* kmalloc(unsigned int size)
{
	size = ROUNDUP(size , PAGE_SIZE);
	int numOfPages = size / PAGE_SIZE;
	//cprintf("__size__%d\n", size);
	//cprintf("__pages__%d\n", numOfPages);
	uint32 used;
	int requiredPages = 0;
	uint32 tempBegining = 0;
	uint32 tempFini;
	int flag = 0;
		for(uint32 i = following; i<= KERNEL_HEAP_MAX; i+=PAGE_SIZE)
		{
			struct Frame_Info *ptr_frame_info ;
			uint32 *ptr_table;
			if (i == KERNEL_HEAP_MAX)
			{
				i = KERNEL_HEAP_START;
				flag = 1;
			}
			if (i == following && flag == 1){
				if (numOfPages > requiredPages)
					return NULL;
			}
			ptr_frame_info = get_frame_info(ptr_page_directory, (void *)i, &ptr_table);
			if (ptr_frame_info == NULL){
				if (requiredPages == 0)
					 tempBegining =  i;
				requiredPages+=1;
			}
			else{
				requiredPages = 0;
			}
			if (requiredPages==numOfPages)
			{
				tempFini =  i;
				break;
			}
		}
		used = tempBegining;
		//cprintf("__returned address__%x\n", used);
		//cprintf("__start address__%x\n", tempBegining);

		if (requiredPages == numOfPages)
		{
			for(uint32 i = tempBegining; i <= tempFini ; i+= PAGE_SIZE)
			{
				struct Frame_Info *ptr_FI = NULL;
				allocate_frame(&ptr_FI);
				map_frame(ptr_page_directory,ptr_FI,(void*)i,PERM_WRITEABLE);
				//tempBegining+= PAGE_SIZE;
			}
			following = tempFini;
			//cprintf("__end address__%x\n", tempBegining);

			memArr[(used - KERNEL_HEAP_START)/ PAGE_SIZE].StartAdress= tempBegining;
			memArr[(used - KERNEL_HEAP_START)/ PAGE_SIZE].nOfPages = numOfPages;
			//cprintf("__returned address__%x\n", used);
			return (void *) used;
		}

	return NULL;
}

void kfree(void* virtual_address)
{
	int index =  ((int32 ) virtual_address - KERNEL_HEAP_START)/ PAGE_SIZE ;
	int x =27;
	uint32 start = memArr[index].StartAdress;
	int conniasse =56;
	for (int i = 0; i<memArr[index].nOfPages; i++){
		unmap_frame(ptr_page_directory, (void*) start);
		start+=PAGE_SIZE;
	}

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT 2022 - [3] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	for(uint32 i = KERNEL_HEAP_START ; i< KERNEL_HEAP_MAX ; i+=PAGE_SIZE)
			{
				struct Frame_Info* frame_info = NULL;
				uint32* ptr_page_table;
				int marker = 0;
				frame_info = get_frame_info(ptr_page_directory, (void*)i, &ptr_page_table);
				if(frame_info != NULL)
				{
					marker = 1;
				}
				if (marker == 1){
					uint32 Frame_physical_add = to_physical_address(frame_info);
										if(Frame_physical_add == physical_address)
											return i;

				}
			}
		return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
		uint32* ptr_page_table = NULL;
		get_page_table(ptr_page_directory, (void*)virtual_address, &ptr_page_table);
		return (ptr_page_table[PTX(virtual_address)]>>12)*PAGE_SIZE;
}

