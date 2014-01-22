#ifndef _SINGLETON_
#define _SINGLETON_

template <class T> 
class cSingleton
{
	static T* ms_singleton;
public:
	cSingleton()
	{
		int offset = (int)(T*)1 - (int)(cSingleton <T>*)(T*)1;
		ms_singleton = (T*)((int)this + offset);
	}

	~cSingleton()
	{
		ms_singleton = 0;
	}

	static inline T& GetSingleton()
	{
		return *ms_singleton;
	}

	static inline T* GetSingletonPtr()
	{
		return ms_singleton;
	}
};

template <class T> T* cSingleton <T>::ms_singleton = 0;

#endif