#include "SharedMemory.hpp"
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 
#include "test_call.h"
void test_call()
{
	SHM_GUARD;
	//SHM::ProcessMutex mut;
	int counter = 30;
	do 
	{
		std::cout << counter << std::endl;
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	} while (counter-->0);

}