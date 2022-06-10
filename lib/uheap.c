
#include <inc/lib.h>

// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

uint32 following = USER_HEAP_START;
int memArr[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE] = {0};
struct mem
  {
	int nOfPages;
  	uint32 StartAdress;
  	uint32 FINITOAdress;

  }mem_Arr[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE];

void* malloc(uint32 size)
{
	//TODO: [PROJECT 2022 - [9] User Heap malloc()] [User Side]
	// Write your code here, remove the panic and write your code
	// Steps:
	//	1) Implement NEXT FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//
	size = ROUNDUP(size , PAGE_SIZE);
	int numOfPages = size / PAGE_SIZE;
	//cprintf("__size__%d\n", size);
	//cprintf("__pages__%d\n", numOfPages);
	uint32 used;
	int requiredPages = 0;
	uint32 tempBegining = 0;
	uint32 tempFini;
	int flag = 0;
	if (sys_isUHeapPlacementStrategyNEXTFIT())
	{
	for(uint32 i = following; i<= USER_HEAP_MAX; i+=PAGE_SIZE)
	{
		if (i == USER_HEAP_MAX)
		{
			i = USER_HEAP_START;
			flag = 1;
		}
		if (i == following && flag == 1)
		{
			if (numOfPages > requiredPages)
				return NULL;
		}
		int is_used = memArr[(i - USER_HEAP_START)/PAGE_SIZE];
		if (is_used == 0)
		{
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
	}
	used = tempBegining;

	//cprintf("__returned address__%x\n", used);
	//cprintf("__start address__%x\n", tempBegining);

	if (requiredPages == numOfPages)
	{
		sys_allocateMem((uint32)tempBegining, size);
		int x = 0;
		for(uint32 i = tempBegining; i <= tempFini ; i+= PAGE_SIZE){
			memArr[(i - USER_HEAP_START)/ PAGE_SIZE] = 1;
		}
		mem_Arr[(used - USER_HEAP_START)/ PAGE_SIZE].StartAdress = used;
		mem_Arr[(used - USER_HEAP_START)/ PAGE_SIZE].nOfPages = numOfPages;
		mem_Arr[(used - USER_HEAP_START)/ PAGE_SIZE].FINITOAdress = tempFini;
		following = tempBegining + PAGE_SIZE*numOfPages;
		//cprintf("__end address__%x\n", tempBegining);
		return (void *) used;
	}


	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyNEXTFIT() and
	//sys_isUHeapPlacementStrategyBESTFIT() for the bonus
	//to check the current strategy

	return NULL;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//you shold get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details


	//TODO: [PROJECT 2022 - [11] User Heap free()] [User Side]
	// Write your code here, remove the panic and write your code
	//int index =  ((uint32 ) virtual_address - USER_HEAP_START)/ PAGE_SIZE ;
	//uint32 start = mem_Arr[index].StartAdress;

	int numereDeFeuille;
	int index = ((uint32 ) virtual_address - USER_HEAP_START) / PAGE_SIZE;
	uint32 address = mem_Arr[index].StartAdress;
	if (address == (uint32)virtual_address)
	{
		numereDeFeuille = mem_Arr[index].nOfPages;
		int size = numereDeFeuille * PAGE_SIZE;
		sys_freeMem((uint32) address, size);

		mem_Arr[index].nOfPages = -1;
		mem_Arr[index].StartAdress = -1;
		mem_Arr[index].FINITOAdress = -1;
	}

	int x = 0;
	uint32 pp = (uint32)virtual_address;
	while(x<numereDeFeuille){
		memArr[( pp- USER_HEAP_START)/ PAGE_SIZE] = 0;
		pp+=PAGE_SIZE;
		x++;
	}


}


void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
