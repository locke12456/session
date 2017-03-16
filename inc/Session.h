/* 
 * File name : Session
 * Author    : Locke Chen
 * E-Mail    : locke12456@gmail.com
 * Language  : C plus plus 
 */
#ifndef __SESSION_H__
#define __SESSION_H__
#include "ISessionObject.h"
namespace SHM {
	class SharedMemory;
	class Session : public ISessionObject
	{
	public:
		Session();
		virtual ~Session();
		unsigned long key;
		unsigned long timetick;
		unsigned long timeout;
		int priority_level;
	};
	class SessionList {
	public:
		SessionList();
		~SessionList();
		bool push(Session* session);
		bool remove(Session *session);
		bool pop();
	private :
		SharedMemory *  _mem;
	};
};
#endif
