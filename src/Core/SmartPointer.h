#ifndef __SmartPointer__h__
#define __SmartPointer__h__

#include <cassert>
#include <cstdio>

//------------------------------------------------------------------------------
template <class T>
class TSmartPointer
{
public:
	//------------------------------------------------------------------------------
    inline TSmartPointer();
	
	//------------------------------------------------------------------------------
    inline TSmartPointer(T* p);
	// copy constructor
	//------------------------------------------------------------------------------
    inline TSmartPointer(const TSmartPointer& s);
	//------------------------------------------------------------------------------
    ~TSmartPointer();

	//------------------------------------------------------------------------------
	T * Get() const
	{
		return mPointee;
	}
	//------------------------------------------------------------------------------
	operator bool() const 
	{ 
        return mPointee;
	}
    
	//------------------------------------------------------------------------------
	T & operator *() const
	{
		if( mPointee )
			return *mPointee;
        assert(!"ERROR!!!");
	}	
	
	//------------------------------------------------------------------------------
	T * operator ->() const
	{
		if( mPointee )
			return mPointee;
        assert(!"ERROR!!!");
	}

	//------------------------------------------------------------------------------
	TSmartPointer<T>& operator = (const TSmartPointer& s)
	{
		++ *s.mRefCount;
		if( -- *mRefCount == 0 )
		{
			delete mPointee;
			delete mRefCount;
		}
		mPointee	= s.mPointee;
		mRefCount	= s.mRefCount;	
		return *this;
	}

	//------------------------------------------------------------------------------
	T		* mPointee;
	int		* mRefCount;
	//------------------------------------------------------------------------------
};

template<class T>
TSmartPointer<T>::TSmartPointer()
:	mPointee( NULL )
,	mRefCount( new int(1) )
{
}

template<class T>
TSmartPointer<T>::TSmartPointer(T* p)
:	mPointee( p )
,	mRefCount( new int(1) )
{
    //assert( p );
}

template<class T>
TSmartPointer<T>::~TSmartPointer()
{
    printf("TSmartPointer::~SmartPointer called\n");
    if( -- *mRefCount <= 0 )
    {
        if( mPointee )
        {
            delete mPointee;
            mPointee = NULL;
        }
        if( mRefCount )
        {
            delete mRefCount;
            mRefCount = NULL;
        }
    }
}

template<class T>
TSmartPointer<T>::TSmartPointer(const TSmartPointer & s)
:	mPointee( s.mPointee )
,	mRefCount( s.mRefCount )
{
    ++ * mRefCount;
}


#endif