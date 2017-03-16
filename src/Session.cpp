/* 
 * File name : Session
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#include "Session.h"
#include "SharedMemory.h"
#include <boost/thread/thread.hpp>
#include <boost/lockfree/stack.hpp>
#include <iostream>


using namespace SHM;
namespace SHM 
{

};
Session::Session() : key(0) , timeout(0) , timetick(0) , priority_level(3)
{
}
Session::~Session()
{
}

SHM::SessionList::SessionList()
{
	SharedMemoriesManagent& that = SharedMemoriesManagent::GetInstace();
	_mem = &that.GetSharedMemory("shared_session");
}

SHM::SessionList::~SessionList()
{
	SharedMemoriesManagent::Destory();
	//delete _mem;
}

bool SHM::SessionList::push(Session * session)
{
	return _mem->Add(*session);
}

bool SHM::SessionList::remove(Session * session)
{
	return _mem->Remove(session->key);
}

bool SHM::SessionList::pop()
{
	_mem->Pop();
	return false;
}
