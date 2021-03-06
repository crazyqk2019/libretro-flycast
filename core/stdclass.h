#pragma once
#include "types.h"
#include <stdlib.h>
#include <vector>
#include <string.h>

#include <rthreads/rthreads.h>

#ifdef _ANDROID
#include <sys/mman.h>
#else
#define PAGE_SIZE 4096
#endif

#undef PAGE_MASK
#define PAGE_MASK (PAGE_SIZE-1)

//Threads
#if !defined(HOST_NO_THREADS)
typedef  void* ThreadEntryFP(void* param);

typedef void* THREADHANDLE;

class cThread
{
private:
	ThreadEntryFP* Entry;
	void* param;
public :
	sthread_t *hThread;
	cThread(ThreadEntryFP* function,void* param);
	~cThread() { WaitToEnd(); }
	
	void Start();
	void WaitToEnd();
};
#endif

//Wait Events
class cResetEvent
{
#ifndef TARGET_NO_THREADS
   slock_t *mutx;
   scond_t *cond;
#endif

public :
   bool state;
	cResetEvent();
	~cResetEvent();
	void Set();		//Set state to signaled
	void Reset();	//Set state to non signaled
	bool Wait(u32 msec);//Wait for signal , then reset[if auto]. Returns false if timeout expired, true otherwise
	void Wait();	//Wait for signal , then reset[if auto]
};

class cMutex
{
private:
#ifndef TARGET_NO_THREADS
   slock_t *mutx;
#endif

public :
	cMutex()
	{
#ifndef TARGET_NO_THREADS
      mutx = slock_new();
#endif
	}
	~cMutex()
	{
#ifndef TARGET_NO_THREADS
      slock_free(mutx);
#endif
	}
	void Lock()
	{
#ifndef TARGET_NO_THREADS
      slock_lock(mutx);
#endif
	}
	bool TryLock()
	{
#ifndef TARGET_NO_THREADS
      return slock_try_lock(mutx);
#else
      return false ;
#endif
	}
	void Unlock()
	{
#ifndef TARGET_NO_THREADS
      slock_unlock(mutx);
#endif
	}
};

//Set the path !
void set_user_config_dir(const string& dir);
void set_user_data_dir(const string& dir);
void add_system_config_dir(const string& dir);
void add_system_data_dir(const string& dir);

//subpath format: /data/fsca-table.bit
string get_writable_data_path(const string& filename);
string get_writable_vmu_path(const char *logical_port);

bool mem_region_lock(void *start, size_t len);
bool mem_region_unlock(void *start, size_t len);
bool mem_region_set_exec(void *start, size_t len);
void *mem_region_reserve(void *start, size_t len);
bool mem_region_release(void *start, size_t len);
void *mem_region_map_file(void *file_handle, void *dest, size_t len, size_t offset, bool readwrite);
bool mem_region_unmap_file(void *start, size_t len);

class VArray2
{
public:

	u8* data;
	u32 size;
	//void Init(void* data,u32 sz);
	//void Term();
#ifdef TARGET_NO_EXCEPTIONS
	void LockRegion(u32 offset,u32 size) {}
	void UnLockRegion(u32 offset,u32 size) {}
#else
	void LockRegion(u32 offset,u32 size);
	void UnLockRegion(u32 offset,u32 size);
#endif

	void Zero()
	{
		UnLockRegion(0,size);
		memset(data,0,size);
	}

	INLINE u8& operator [](const u32 i)
    {
#ifdef MEM_BOUND_CHECK
        if (i>=size)
		{
			printf("Error: VArray2 , index out of range (%d>%d)\n",i,size-1);
			MEM_DO_BREAK;
		}
#endif
		return data[i];
    }
};

int ExeptionHandler(u32 dwCode, void* pExceptionPointers);
int msgboxf(const char* text,unsigned int type,...);

#define MBX_OK                       0x00000000L
#define MBX_OKCANCEL                 0x00000001L
#define MBX_ABORTRETRYIGNORE         0x00000002L
#define MBX_YESNOCANCEL              0x00000003L
#define MBX_YESNO                    0x00000004L
#define MBX_RETRYCANCEL              0x00000005L


#define MBX_ICONHAND                 0x00000010L
#define MBX_ICONQUESTION             0x00000020L
#define MBX_ICONEXCLAMATION          0x00000030L
#define MBX_ICONASTERISK             0x00000040L


#define MBX_USERICON                 0x00000080L
#define MBX_ICONWARNING              MBX_ICONEXCLAMATION
#define MBX_ICONERROR                MBX_ICONHAND


#define MBX_ICONINFORMATION          MBX_ICONASTERISK
#define MBX_ICONSTOP                 MBX_ICONHAND

#define MBX_DEFBUTTON1               0x00000000L
#define MBX_DEFBUTTON2               0x00000100L
#define MBX_DEFBUTTON3               0x00000200L

#define MBX_DEFBUTTON4               0x00000300L


#define MBX_APPLMODAL                0x00000000L
#define MBX_SYSTEMMODAL              0x00001000L
#define MBX_TASKMODAL                0x00002000L

#define MBX_HELP                     0x00004000L // Help Button


#define MBX_NOFOCUS                  0x00008000L
#define MBX_SETFOREGROUND            0x00010000L
#define MBX_DEFAULT_DESKTOP_ONLY     0x00020000L

#define MBX_TOPMOST                  0x00040000L
#define MBX_RIGHT                    0x00080000L
#define MBX_RTLREADING               0x00100000L

#define MBX_RV_OK                1
#define MBX_RV_CANCEL            2
#define MBX_RV_ABORT             3
#define MBX_RV_RETRY             4
#define MBX_RV_IGNORE            5
#define MBX_RV_YES               6
#define MBX_RV_NO                7
