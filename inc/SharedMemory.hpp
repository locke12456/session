/* 
 * File name : Session
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#ifndef __SHAREDMEMORY_HPP__
#define __SHAREDMEMORY_HPP__
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp> 
#include <boost/interprocess/sync/interprocess_condition.hpp> 
#include <boost/interprocess/sync/scoped_lock.hpp> 
#include <iostream>
#include "SharedMemory.h"
using namespace boost::interprocess;
#ifdef WIN32
//extern HANDLE __mutex_object;

//#define SHM_MANAGER_NAME "__shm_manager_x86"
#else
#ifdef _WIN64
//#define SHM_MANAGER_NAME "__shm_manager_x64"
#endif
#endif
#define SHM_MANAGER_NAME_MTX "__shm_manager_mutex"
#define SHM_MUTEX_NAME	"shm_seesion_mutex"
#define SHM_CON_DES_MUTEX "__local_lock_create_release"
#define SHM_MAX_SIZE 1024

#define SHM_GUARD_DEFINE(mutex)	SHM::ProcessMutex mut;\
    mut.try_lock();

#define SHM_GUARD SHM_GUARD_DEFINE(true)
bool CheckProcessExist(unsigned long id);
namespace SHM {
	class process_mutex
	{

	public:
		friend class ProcessMutex;
		typedef boost::function<void()> CallBack;

		process_mutex(managed_shared_memory & _managed_shm, std::string _name) :managed_shm(_managed_shm), name(_name) {
			boost::interprocess::interprocess_mutex *mtx = managed_shm.find_or_construct<boost::interprocess::interprocess_mutex>(name.c_str())();
			boost::interprocess::interprocess_condition *cnd = managed_shm.find_or_construct<boost::interprocess::interprocess_condition>(name.c_str())();
		}
		~process_mutex() {
		}
		boost::interprocess::interprocess_mutex* mutex()
		{
			return managed_shm.find_or_construct<boost::interprocess::interprocess_mutex>(name.c_str())();
		}
		bool try_release() 
		{
			return true;
		}
	private:
		std::string name;
		managed_shared_memory & managed_shm;
		CallBack callbak;
		boost::interprocess::interprocess_condition *cnd;
		boost::interprocess::interprocess_mutex *mtx;
	};

	class ProcessMutex : public boost::interprocess::scoped_lock<interprocess_mutex> 
	{
	public:

		typedef boost::function<void()> CallBack;
		ProcessMutex()
		{
		}

		~ProcessMutex() 
		{
			try_release();
		}

		bool try_lock() 
		{
			//LOG_DEBUG("Mutex") << " try lock ";
			SharedMemoriesManagent &manager = SharedMemoriesManagent::GetInstace();
			_setup = boost::bind(&SharedMemoriesManagent::SetupDynamicLinkerMutex, &manager);
			_teardown = boost::bind(&SharedMemoriesManagent::TeardownDynamicLinkerMutex, &manager);
			try {
				_setup();
			}
			catch (std::runtime_error &e)
			{
				//std::cout << e.what() << std::endl;
				manager.GetDynamicLinkerMutex()->mutex()->unlock();
			}
			manager.GetDynamicLinkerMutex()->mutex()->lock();
			return true;
		}

		bool try_release() 
		{
			//LOG_DEBUG("Mutex") << " try unlock ";
			SharedMemoriesManagent &manager = SharedMemoriesManagent::GetInstace();
			_teardown();
			manager.GetDynamicLinkerMutex()->mutex()->unlock();
			return true;
		}
	private:
		typedef boost::interprocess::scoped_lock<interprocess_mutex> _MyBase;
		CallBack _setup , _teardown;
		void initialize() 
		{			

		}
	};
	class SharedMemoriesManagent::__shared_memories_manager
	{
	public:
		friend class SharedMemoriesManagent;
		managed_shared_memory & get() const
		{
			return *shm_obj;
		}
	protected:
		__shared_memories_manager(std::string _name) : name(_name) {

			//boost::interprocess::shared_memory_object::remove(name.c_str());
			__init();
		};
		~__shared_memories_manager()
		{
			//boost::interprocess::shared_memory_object::remove(name.c_str());
		};


	private:
		std::string name;
		boost::shared_ptr<managed_shared_memory> shm_obj;
		void __init()
		{
			shm_obj = boost::shared_ptr<managed_shared_memory>(new managed_shared_memory
				(open_or_create               //open or create
					, name.c_str()              //name
					, 65535
					));
		}
	};
	class SharedMemoriesManagent::__shared_memory
	{
	public:
		friend class SharedMemory;
		//Define an STL compatible allocator of ints that allocates from the managed_shared_memory.
		//This allocator will allow placing containers in the segment
		typedef boost::interprocess::allocator< Session, boost::interprocess::managed_shared_memory::segment_manager >  ShmemAllocator;

		//Alias a vector that uses the previous STL-like allocator so that allocates
		//its values from the segment
		typedef boost::interprocess::list< Session, ShmemAllocator> _SessionList;

		__shared_memory(std::string _name) : name(_name)
		{

			//
			//
			//int size = sizeof(Session);

			try {
				__init();
			}
			catch (interprocess_exception & e)
			{
				__init();
			}

			//Initialize shared memory STL-compatible allocator
			const ShmemAllocator allocator(shm_obj->get_segment_manager());
			vector = shm_obj->find_or_construct<_SessionList>(name.c_str())(allocator);
			//Construct a vector named "MyVector" in shared memory with argument alloc_inst
		}
		~__shared_memory() {
			//if (vector->empty()) {
			//	//shm_obj->destroy<_SessionList>(name.c_str());
			//	//shm_obj;
			//}
		};
		_SessionList* GetSessionVector() const
		{
			return vector;
		}

	protected:
		//ShmemAllocator allocator;
		std::string name;
		_SessionList* vector;
		managed_shared_memory* shm_obj;
		void __init()
		{
			SharedMemoriesManagent & sh = SharedMemoriesManagent::GetInstace();
			shm_obj = &sh.get_mamager()->get();
		}
	};
};
#endif
