/* 
 * File name : SharedMemory
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#include <iostream>
#include "SharedMemory.hpp"
#ifdef WIN32
#include <Windows.h>
HANDLE __mutex_object = NULL;
#endif // WIN32

using namespace SHM;
namespace SHM {

	// staitc
	SharedMemoriesManagent * SharedMemoriesManagent::_that = 0;

	SharedMemoriesManagent & SharedMemoriesManagent::GetInstace()
	{
		if (0 == _that) 
		{
			_that = new SharedMemoriesManagent();
		}
		return *_that;
	}

	void SharedMemoriesManagent::Destory()
	{
		if (0 != _that)
			delete _that;
		// TODO: insert return statement here
	}
	
	// public
	void SharedMemoriesManagent::ProcessAttached(unsigned long id)
	{
// lock
		bool not_finished = true;
		do {
			process_mutex m(_manager->get(), std::string(SHM_CON_DES_MUTEX));
			boost::interprocess::scoped_lock<interprocess_mutex> guard(*m.mutex(), boost::get_system_time() + boost::posix_time::microseconds(10000));
			if (guard.owns()) {
				if (0 == _process)
				{
					Session pid;
					_process = new SharedMemory(SHM_MANAGER_NAME);
					bool finish = true;
					bool found = false;
					do {
						SharedMemoriesManagent::__shared_memory::_SessionList* vector = _process->_shared_mem->GetSessionVector();
						SharedMemoriesManagent::__shared_memory::_SessionList::iterator find;
						found = false;
						for (find = vector->begin(); find != vector->end(); find++) {
							//if (find->key != id) {
							//std::cout << "Processes id : " << find->key << std::endl;
							if (!CheckProcessExist(find->key))
							{
								//std::cout << "Removed" << std::endl;
								_process->Remove(find->key);
								found = true;
								break;
							}
							//}
						}

					} while (found);
					_id = id;
					pid.key = id;
					_process->Add(pid);
					//std::cout << "Processes : " << _process->Count() << std::endl;
					not_finished = false;
				}
			}
		} while (not_finished);

	}
	SharedMemory & SharedMemoriesManagent::GetSharedMemory(std::string name)
	{
		//boost::shared_ptr<SharedMemory> shared(new SharedMemory(name));
		if (0 == _shared) 
		{
			_shared = new SharedMemory(name);
		}
		return *_shared;
		// TODO: insert return statement here
	}
	SharedMemory & SharedMemoriesManagent::GetSharedMutex()
	{
		return *_process;
	}
	process_mutex * SharedMemoriesManagent::GetDynamicLinkerMutex()
	{
		//boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mutex->mutex());
		return _process_mutex;
		// TODO: insert return statement here
	}
	void SharedMemoriesManagent::SetupDynamicLinkerMutex()
	{
		//
		//
		do {
			process_mutex m(_manager->get(), std::string(SHM_CON_DES_MUTEX));
			boost::interprocess::scoped_lock<interprocess_mutex> guard(*m.mutex(), boost::get_system_time() + boost::posix_time::microseconds(10000));
			if (guard.owns())
			{
				if (_shared->Count() == 0)break;
				Session check = _shared->GetHead();
				if (check.key != 0)
				{
					if (!CheckProcessExist(check.key))
					{
						Session chk = _shared->Pop();
						if(chk.key == check.key)
							throw std::runtime_error("lock_error");
					}
				}
				else
					break;
			}else
			{
				if (_shared->Count() == 0) 
				{
					m.mutex()->unlock();
				}
			}

			//std::cout << "_shared : " << _shared->Count() << std::endl;
			boost::this_thread::sleep(boost::posix_time::microseconds(100));
		} while (1);
		
		{
			Session pid;
			pid.key = _id;
			_shared->Add(pid);
		}
		
	}
	void SharedMemoriesManagent::TeardownDynamicLinkerMutex()
	{
		_shared->Pop();
		//boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mutex->mutex());
	}
	unsigned long SharedMemoriesManagent::id()
	{

		return _id;
	}

	// private
	SharedMemoriesManagent::__shared_memories_manager * SharedMemoriesManagent::get_mamager()
	{
		return _manager;
	}

	SharedMemoriesManagent::SharedMemoriesManagent() : _shared(0) , _process(0) , _manager(0) 
	{
		_manager = new SharedMemoriesManagent::__shared_memories_manager(std::string(SHM_MANAGER_NAME));
		_process_mutex = new process_mutex( _manager->get() , std::string(SHM_MANAGER_NAME_MTX));
	}

	SharedMemoriesManagent::~SharedMemoriesManagent()
	{
		//lock
		Session chk = _shared->GetHead();

		bool not_finished = true;
		do {
			process_mutex m(_manager->get(), std::string(SHM_CON_DES_MUTEX));
			boost::interprocess::scoped_lock<interprocess_mutex> guard(*m.mutex(), boost::get_system_time() + boost::posix_time::microseconds(10000));
			if (guard.owns()) {
				bool release = false;
				if (0 != _shared) {
					Session check = _shared->GetHead();
					if (chk.key == check.key &&check.key == _id)
					{
						GetDynamicLinkerMutex()->mutex()->unlock();
						_shared->Pop();
					}
					delete _shared;
				}
				if (0 != _process) {
					_process->Remove(id());
					//std::cout << "Processes : " << _process->Count() << std::endl;
					release = (_process->Count() == 0);
					delete _process;
				}
				delete _process_mutex;
				//if (release)
				//{
				//	boost::interprocess::shared_memory_object::remove(SHM_MANAGER_NAME);

				//	//std::cout << "release" << std::endl;
				//}
				not_finished = false;
			}
			else {

			}
		} while (not_finished);
		delete _manager;

	}

};

SHM::SharedMemory::SharedMemory(std::string name) {
	_shared_mem = new SharedMemoriesManagent::__shared_memory(name);
	_process_mtx = new process_mutex(*_shared_mem->shm_obj , SHM_MUTEX_NAME);
}
SHM::SharedMemory::~SharedMemory()
{
	delete _process_mtx;
	delete _shared_mem;
}
process_mutex * SHM::SharedMemory::get_mutex()
{
	return _process_mtx;
}
bool SHM::SharedMemory::Add(Session & session)
{
	boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mtx->mutex());
	SharedMemoriesManagent::__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	if (vector->size() >= SHM_MAX_SIZE)return false;
	vector->push_back(session);
	////std::cout << vector->size() << std::endl;
	return true;
}

Session SHM::SharedMemory::Pop()
{
	SharedMemoriesManagent::__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	if (vector->empty())
		return Session();
	Session session = vector->front();
	unsigned int id = session.key;
	
	boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mtx->mutex());
	session = vector->front();
	if (id != session.key) 
	{
		return session;
	}
	vector->pop_front();
	////std::cout << vector->size() << std::endl;
	return session;
	
}
Session SHM::SharedMemory::Get(unsigned long key)
{
	SharedMemoriesManagent::__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	for (SharedMemoriesManagent::__shared_memory::_SessionList::iterator find = vector->begin(); find != vector->end(); find++)
		if (find->key == key){return *find;}
	return Session();
}

Session SHM::SharedMemory::GetHead()
{
	boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mtx->mutex());
	SharedMemoriesManagent::__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	if (vector->empty())
		return Session();
	return Session(vector->front());
}

size_t SHM::SharedMemory::Count()
{
	boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mtx->mutex());
	SharedMemoriesManagent::__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	//std::cout << vector->size() << std::endl;
	return size_t(vector->size());
}

bool SHM::SharedMemory::Exists(unsigned long key)
{
	boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mtx->mutex());
	SharedMemoriesManagent::__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	for (SharedMemoriesManagent::__shared_memory::_SessionList::iterator find = vector->begin(); find != vector->end(); find++)
		if (find->key == key)return true;
	//__shared_memory::_SessionList::iterator find = std::find(vector->begin(), vector->end(), session);
	return false;
}

bool SHM::SharedMemory::Remove(unsigned long key)
{
	boost::interprocess::scoped_lock<interprocess_mutex> guard(*_process_mtx->mutex());
	SharedMemoriesManagent::__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	SharedMemoriesManagent::__shared_memory::_SessionList::iterator find;
	for (find = vector->begin(); find != vector->end(); find++)
	{
		if (key == find->key) {
			try {
				//releases->push_back(*find);
				vector->erase(find);
				//std::cout << find->key << std::endl;
				//std::cout << vector->size() << std::endl;
				return true;
			}
			catch (std::runtime_error & e)
			{
				//std::cout << e.what() << std::endl;
				return false;
			}
		}
	}
	return false;
}

bool SHM::SharedMemory::Recovery(unsigned long timetick)
{
	// TODO : verify
	//__shared_memory::_SessionList* vector = _shared_mem->GetSessionVector();
	//__shared_memory::_SessionList releases;
	//for (__shared_memory::_SessionList::iterator find = vector->begin(); find != vector->end(); find++)
	//{
	//	if (find->timetick+find->timeout >= timetick) {
	//		releases.push_back(*find);
	//	}
	//}
	//for (__shared_memory::_SessionList::iterator find = releases.begin(); find != releases.end(); find++)
	//	vector->erase(find);

	//std::cout << vector->size() << std::endl;
	return true;
}
