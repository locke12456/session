#include <stdio.h>
#include "SharedMemory.h"
#include <Windows.h>
int main()
{
	SHM::SharedMemoriesManagent & that = SHM::SharedMemoriesManagent::GetInstace();

	for (int i = 0; i < 10000; i++) {
		//test_call();
		//SHM::Session * stacks[1000] = {0};
		//SHM::SessionList* list = new SHM::SessionList();
		//	stacks[i] = new SHM::Session();
		//	stacks[i]->key = i;
		//	list->push(stacks[i]);
		//	Sleep(10);
		//}
		//for (int i = 0; i < 500; i++) {
		//	// = i;
		//	list->remove(stacks[i]);
		//	//Sleep(1000);
		//	//list->pop();
		//}
		//delete list;
	}
	getchar();
	return 0;
}