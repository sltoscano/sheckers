// Copyright: Steven Toscano

#pragma once

// ---------------------------------------------------------------------------

/* Definitions to allow header to be compiled without windows.h. */

#ifndef HRESULT
	typedef long HRESULT;
#endif

#ifndef _WINBASE_

	__declspec(dllimport)
	long
	__stdcall
	InterlockedIncrement (
		long volatile *lpAddend
		);

	__declspec(dllimport)
	long
	__stdcall
	InterlockedDecrement (
		long volatile *lpAddend
		);

	#define InterlockedIncrement _InterlockedIncrement
	#define InterlockedDecrement _InterlockedDecrement

	#pragma intrinsic(_InterlockedIncrement)
	#pragma intrinsic(_InterlockedDecrement)

#endif // _WINBASE_

#ifndef ASSERT
	#error This header requires the use of asserts
#endif

// ---------------------------------------------------------------------------

template <class T>
class CDefaultSharedPtrTraits
{
public:
	static void AddRef(T* t)
	{
		t->AddRef();
	}

	static void Release(T* t)
	{
		t->Release();
	}
};

// ---------------------------------------------------------------------------

/* This is a shared-pointer object for any object that has an AddRef/Release
	pair. If you need to avoid ambiguity, declare TTraits as the un-ambiguous
	class to which the object will be cast to before calling AddRef(). */

template <class T, class TTraits = CDefaultSharedPtrTraits<T>>
class CSharedPtr
{
private:
	T *Assign(T **pp, T *pObj)
	{
		if (pObj != NULL)
			TTraits::AddRef(pObj);
		if (*pp != NULL)
			TTraits::Release(*pp);
		*pp = pObj;
		return pObj;
	}

public:
	T *p;

	CSharedPtr() : p(NULL)
	{
	}

	CSharedPtr(T *pObj) : p(pObj)
	{
		if (p != NULL)
			TTraits::AddRef(p);
	}

	CSharedPtr(const CSharedPtr<T> &sp) : p(sp.p)
	{
		if (p != NULL)
			TTraits::AddRef(p);
	}

	~CSharedPtr()
	{
		Release();
	}

	void Release()
	{
		if (p != NULL)
		{
			TTraits::Release(p);
			p = NULL;
		}
	}

	template<class Q>
	HRESULT QueryInterface(Q** ppDestination)
	{
		ASSERT(p != NULL);
		return p->QueryInterface(__uuidof(Q), (void**)ppDestination);
	}

	operator T * ()
	{
		return p;
	}

	operator const T * () const
	{
		return p;
	}

	T ** operator & ()
	{
		ASSERT(p == NULL);
		return &p;
	}

	T * operator -> ()
	{
		ASSERT(p != NULL);
		return p;
	}

	const T * operator -> () const
	{
		ASSERT(p != NULL);
		return p;
	}

	T * operator = (T *pObj)
	{
		return Assign(&p, pObj);
	}

	T * operator = (const CSharedPtr<T>& sp)
	{
		return Assign(&p, sp.p);
	}

	bool operator ! ()
	{
		return p == NULL;
	}

	bool operator != (const T * _p) const
	{
		return _p != p;
	}

	bool operator == (const T * _p) const
	{
		return _p == p;
	}

	bool operator != (const CSharedPtr<T>& sp) const
	{
		return sp.p != p;
	}

	bool operator == (const CSharedPtr<T>& sp) const
	{
		return sp.p == p;
	}

	T *Detach()
	{
		T *pTmp = p;
		p = NULL;
		return pTmp;
	}
};

// ---------------------------------------------------------------------------

/* Reference counting interface */
class IReferenceCounted
{
public:
	virtual long AddRef() = 0;
	virtual long Release() = 0;
};

// ---------------------------------------------------------------------------

/* Reference counting implementation */
class CAutoRef
{
public:
	CAutoRef() : m_iRef(0)
	{
	};

	virtual ~CAutoRef()
	{
	}

	long AddRef()
	{
		return InterlockedIncrement(&m_iRef);
	}

	long Release()
	{
		long l = InterlockedDecrement(&m_iRef);
		if (l == 0)
			delete this;
		return l;
	}

#ifdef DEBUG
	/* You can access this field in debug for ASSERTs.	However, you should
		not be writing any real code that depends on querying this value. */
	long GetReferenceCount()
	{
		return m_iRef;
	}
#endif // DEBUG

private:
	mutable volatile long m_iRef;	// Ref count
};

// ---------------------------------------------------------------------------
