// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "simple_cpp_shared_ptr_type.h"

template<class T>
class FWeakPtr;

template<class T>
class FSharedPtr
{
	friend class FWeakPtr<T>;
public:
	//FSharedPtr<FTest> Instance(new FTest);
	FSharedPtr(T *InInstance = NULL)
		:Ptr(InInstance)
	{
		Count = new FRefCounter();
		if (InInstance)
		{
			Count->SharedCount = 1;
		}
	}

	//FSharedPtr<FTest> Instance1(new FTest);
	//FSharedPtr<FTest> Instance(Instance1);
	FSharedPtr(FSharedPtr<T> &InInstance)
	{
		this->Ptr = InInstance.Ptr;

		InInstance.Count->SharedCount++;
		this->Count = InInstance.Count;
	}

	FSharedPtr(FWeakPtr<T>& InInstance)
	{
		this->Count = InInstance.Count;
		this->Count->SharedCount++;
		this->Ptr = InInstance.Ptr;
	}

	//FSharedPtr<FTest> Instance1(new FTest);
	//FSharedPtr<FTest> Instance = Instance1;
	FSharedPtr<T>& operator=(FSharedPtr<T> &InInstance)
	{
		if (this != &InInstance)
		{
			Release();
			InInstance.Count->SharedCount++;
			this->Count = InInstance.Count;
			this->Ptr = InInstance.Ptr;
		}

		return *this;
	}

	FSharedPtr<T>& operator=(FWeakPtr<T>& InInstance)
	{
		if (this->Ptr != InInstance.Ptr)
		{
			Release();
			this->Count = InInstance.Count;
			this->Count->SharedCount++;
			this->Ptr = InInstance.Ptr;
		}
	}

	T *operator*()
	{
		return *Ptr;
	}

	T * operator->()
	{
		return Ptr;
	}

	void Release()
	{
		Count->SharedCount--;

		if (Count->SharedCount >= 0)
		{
			if (Count < 1)
			{
				if (Count->WeakCount < 1)
				{
					delete Count; //弱指针也是需要判断 告诉弱指针 我们智能指针的情况
					Count = nullptr;
				}

				delete Ptr;
				Ptr = nullptr;
			}
		}
	}

	~FSharedPtr()
	{
		Release();
	}

private:
	T* Ptr;
	FRefCounter *Count;
};

template<class T>
class FWeakPtr
{

public:
	FWeakPtr()
		:Ptr(nullptr)
		, Count(nullptr)
	{

	}

	FWeakPtr(FSharedPtr<T>& InInstance)
		:Ptr(InInstance.Ptr)
		, Count(InInstance.Count)
	{
		Count->WeakCount++;
	}

	FWeakPtr<T>& operator=(FWeakPtr<T>& Wt)
	{
		if (Wt != this)
		{
			Release();
			Count = Wt.Count;
			Ptr = Wt.Ptr;

			Count->WeakCount++;
		}

		return *this;
	}

	FWeakPtr<T>& operator=(FSharedPtr<T>& St)
	{
		Release();
		Count = St.Count;
		Ptr = St.Ptr;

		Count->WeakCount++;

		return *this;
	}

	FSharedPtr<T> Pin()
	{
		FSharedPtr<T> SP(*this);
		return SP;//弱指针
	}

	bool /*expired*/ IsVaild()
	{
		if (Count)
		{
			if (Count > 0)
			{
				return true;
			}

			Ptr = NULL;
		}

		return false;
	}

	~FWeakPtr()
	{
		Release();
	}

	void Release()
	{
		if (Count)
		{
			Count->WeakCount--;
			if (Count->WeakCount < 1 && Count->SharedCount < 1)
			{
				delete Count;
				Count = nullptr;
			}
		}
	}

	friend class FSharedPtr<T>;
private:
	T* Ptr;
	FRefCounter* Count;
};

template<class T>
class FSharedFromThis
{
public:
	FSharedFromThis()
	{
		WeakPtr = this;
	}

	FSharedPtr<T> Pin()
	{
		return FSharedPtr<T>(WeakPtr);
	}

	FSharedPtr<const T> Pin() const
	{
		return FSharedPtr<const T>(WeakPtr);
	}

	FWeakPtr<T> Weak()noexcept
	{
		return WeakPtr;
	}

	FWeakPtr<const T> Weak() const noexcept
	{
		return WeakPtr;
	}

protected:
	~FSharedFromThis() = default;

	FSharedFromThis& operator=(const FSharedFromThis&) noexcept
	{
		return *this;
	}

private:
	mutable FWeakPtr<T> WeakPtr;
};

template <class T>
FSharedPtr<T> MakeSharedEnable()
{
	return FSharedPtr<T>(new T());
}