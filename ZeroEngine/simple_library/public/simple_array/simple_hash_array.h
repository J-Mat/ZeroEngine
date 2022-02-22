// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include "../simple_core_minimal/simple_c_core/simple_core_minimal.h"

template<class ContainerType,typename ElementType>
class TIndexedContainerIterator
{
public:
	typedef TIndexedContainerIterator<ContainerType, ElementType> TIterator;

	//构造
	TIndexedContainerIterator(ContainerType &InContainer,int InIndex = 0)
		:Container(InContainer)
		,Index(InIndex)
	{}

	//
	TIndexedContainerIterator(const TIterator &InIterator)
		:Container(InIterator.Container)
		, Index(InIterator.Index)
	{}

//	for (; It != AAA.End();) {}
	bool operator!=(const TIterator& InIterator)
	{
		return Container[Index] != InIterator.Container[InIterator.Index];
	}

	TIterator& operator++(int)
	{
		++Index;
		return *this;
	}

	TIterator& operator--(int)
	{
		--Index;
		return *this;
	}

	//auto dd = TArray<int>::i
	TIterator &operator=(const TIterator& InIterator)
	{
		Index = InIterator.Index;
		Container = InIterator.Container;

		return *this;
	}

	ElementType& operator*()
	{
		return *Container[Index];
	}

protected:
	ContainerType& Container;
	int Index;
};

template<typename ElementType>
class TArray
{
public:
	typedef TIndexedContainerIterator<TArray<ElementType>, ElementType> TIterator;

	TArray()
		:Data(nullptr)
		,Size(0)
	{
		//Data = (ElementType**)malloc(sizeof(int) * Allocation);//分配下标
		//memset(Data, 0, sizeof(int) * Allocation);
		//for (int i = 0;i < Allocation;i++)
		//{
		//	Data[i] = (ElementType*)malloc(sizeof(ElementType));
		//	memset(Data[i], 0, sizeof(ElementType));
		//}
	}

	int Num()
	{
		return Size;
	}
	
	int AddUninitialized(int InLength)
	{
		if (Size == 0)
		{
			Data = (ElementType*)malloc(InLength);
		}
		else
		{
			Data = (ElementType*)realloc(Data,InLength + Size);
		}

		memset(&Data[Size], 0, sizeof(ElementType));

		int LastPos = Size;
		Size += InLength;
		return LastPos;
	}

	void Add(ElementType&& InType)
	{
		Add(InType);
	}

	void Add(ElementType &InType)
	{
		if (Size == 0)
		{
			Data = (ElementType*)malloc(sizeof(ElementType));	
		}
		else
		{
			//内存不够了 不够就分配		
			Data = (ElementType*)realloc(Data, sizeof(ElementType) * (Size + 1));
		}
		memset(Data[Size], 0, sizeof(ElementType));
		memcpy(Data[Size], &InType, sizeof(ElementType));
		Size++;
	}

	//void RemoveAt(int Index)
	//{
	//	memset(Data[Size], 0, sizeof(ElementType));
	//
	//	Size--;
	//}

	ElementType &operator[](int Index)
	{
		return Data[Index];
	}

	TIterator Begin()
	{
		return TIterator(*this,0);
	}

	TIterator End()
	{
		return TIterator(*this, Size);
	}

	~TArray()
	{
		free(Data);
	}

	ElementType* GetData()
	{
		return Data;
	}

protected:
	ElementType* Data;//连续内存
	int Size;
};