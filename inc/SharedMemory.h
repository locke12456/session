/* 
 * File name : SharedMemory
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#ifndef __SHAREDMEMORY_H__
#define __SHAREDMEMORY_H__
#include "ISharedMemoryObject.h"
#include "Session.h"
#include <string>

namespace SHM {
	class  ProcessMutex;
	class  process_mutex;

	class  SharedMemoriesManagent
	{
	protected:
		class __shared_memory;
		class __shared_memories_manager;
		__shared_memories_manager* get_mamager();
	public:
		
		friend class SharedMemory;
		static SharedMemoriesManagent & GetInstace();
		static void Destory();
		void ProcessAttached(unsigned long id);
		SharedMemory& GetSharedMemory(std::string name);
		SharedMemory& GetSharedMutex();
		process_mutex* GetDynamicLinkerMutex();
		void SetupDynamicLinkerMutex();
		void TeardownDynamicLinkerMutex();
		unsigned long id();
	protected:

	private:
		SharedMemoriesManagent();
		~SharedMemoriesManagent();
		static SharedMemoriesManagent *_that; 
		unsigned long _id;
		__shared_memories_manager * _manager;
		SharedMemory * _shared;
		SharedMemory * _process;
		process_mutex * _process_mutex;
	};

	class  SharedMemory : public ISharedMemoryObject
	{
	public:
		friend class SharedMemoriesManagent;
		SharedMemory(std::string name);
		virtual ~SharedMemory();
		process_mutex * get_mutex();
		bool Add(Session & session);

		Session Pop();
		Session Get(unsigned long key);
		Session GetHead();
		size_t Count();
		bool Exists(unsigned long key);
		bool Remove(unsigned long key);
		bool Recovery(unsigned long timetick);
	protected:
		SharedMemoriesManagent::__shared_memory * _shared_mem;
		process_mutex * _process_mtx;
	};
	//class SharedMemoriesManagent::

};
#endif
