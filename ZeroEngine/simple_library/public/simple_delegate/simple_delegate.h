// Copyright (C) RenZhai.2022.All Rights Reserved.
#pragma once
#include <map>
#include "../simple_core_minimal/simple_c_guid/simple_guid.h"

template< class TReturn, typename ...ParamTypes>
class FDelegateBase
{
public:
	virtual TReturn Execute(ParamTypes &&...Params)
	{
		return TReturn();
	}
};

//最简单的 支持多参数代理
template<class TObjectType, class TReturn, typename ...ParamTypes>
class FObjectDelegate :public FDelegateBase<TReturn, ParamTypes...>
{
public:
	FObjectDelegate(TObjectType* InObject, TReturn(TObjectType::* InFuncation)(ParamTypes ...))
		:Object(InObject)
		, Funcation(InFuncation)
	{}

	virtual TReturn Execute(ParamTypes &&...Params)
	{
		return (Object->*Funcation)(std::forward<ParamTypes>(Params)...);
	}

private:
	TObjectType* Object;
	TReturn(TObjectType::* Funcation)(ParamTypes ...);
};

template<class TReturn, typename ...ParamTypes>
class FFunctionDelegate :public FDelegateBase<TReturn, ParamTypes...>
{
public:
	FFunctionDelegate(TReturn(*InFuncation)(ParamTypes ...))
		:Funcation(InFuncation)
	{}

	virtual TReturn Execute(ParamTypes &&...Params)
	{
		return (*Funcation)(std::forward<ParamTypes>(Params)...);
	}

private:
	TReturn(* Funcation)(ParamTypes ...);
};

template<class TReturn, typename ...ParamTypes>
class FDelegate
{
public:
	FDelegate()
		:CurrentDelegatePtr(nullptr)
	{}

	~FDelegate()
	{
		//CheckDelegate();
	}

	void ReleaseDelegate()
	{
		if (CurrentDelegatePtr)
		{
			delete CurrentDelegatePtr;
			CurrentDelegatePtr = nullptr;
		}
	}

public:
	template<class TObjectType>
	static FDelegate<TReturn, ParamTypes...> Create(TObjectType* InObject, TReturn(TObjectType::* InFuncation)(ParamTypes ...))
	{
		FDelegate<TReturn, ParamTypes...>  DelegateInstance;
		DelegateInstance.Bind(InObject, InFuncation);
		return DelegateInstance;
	}

	static FDelegate<TReturn, ParamTypes...> Create(TReturn(*InFuncation)(ParamTypes...))
	{
		FDelegate<TReturn, ParamTypes...>  DelegateInstance;
		DelegateInstance.Bind(InFuncation);
		return DelegateInstance;
	}

public:
	template<class TObjectType>
	void Bind(TObjectType *InObject, TReturn(TObjectType::* InFuncation)(ParamTypes ...))
	{
		ReleaseDelegate();

		CurrentDelegatePtr = new FObjectDelegate<TObjectType,TReturn, ParamTypes...>(InObject, InFuncation);
	}

	void Bind(TReturn(* InFuncation)(ParamTypes...))
	{
		ReleaseDelegate();

		CurrentDelegatePtr = new FFunctionDelegate<TReturn,ParamTypes...>(InFuncation);
	}

	bool IsBound()
	{
		return CurrentDelegatePtr != nullptr;
	}

	virtual TReturn Execute(ParamTypes &&...Params)
	{
		return CurrentDelegatePtr->Execute(std::forward<ParamTypes>(Params)...);
	}

	FDelegate<TReturn, ParamTypes...> &operator=(const FDelegate<TReturn, ParamTypes...> &InDelegate)
	{
		CurrentDelegatePtr = InDelegate.CurrentDelegatePtr;
		return *this;
	}
private:
	FDelegateBase<TReturn, ParamTypes...>* CurrentDelegatePtr;
};

template<class TReturn, typename ...ParamTypes>
class FSingleDelegate :public FDelegate<TReturn, ParamTypes...>
{
public:
	FSingleDelegate()
		:FDelegate<TReturn, ParamTypes...>()
	{}
};

struct FDelegateHandle
{
	FDelegateHandle()
	{
		create_guid(&Guid);
	}

	friend bool operator<(const FDelegateHandle &K1,const FDelegateHandle &K2)
	{
		return K1.Guid.a < K2.Guid.a;
	}

	simple_c_guid Guid;
};

template<class TReturn, typename ...ParamTypes>
class FMulticastDelegate :public std::map<FDelegateHandle,FDelegate<TReturn, ParamTypes...>>
{
	typedef FDelegate<TReturn, ParamTypes...> TDelegate;
public:
	FMulticastDelegate()
	{}

	void RemoveDelegate(const FDelegateHandle &InGuid)
	{
		this->erase(InGuid);
	}

	template<class TObjectType>
	const FDelegateHandle AddFunction(TObjectType* InObject, TReturn(TObjectType::* InFuncation)(ParamTypes ...))
	{
		//生成GUID
		FDelegateHandle Handle;

		this->insert(std::make_pair(Handle,TDelegate()));//添加

		TDelegate &InDelegate = this->at(Handle);
		InDelegate.Bind(InObject, InFuncation);

		return Handle;
	}

	const FDelegateHandle &AddFunction(TReturn(*InFuncation)(ParamTypes...))
	{
		FDelegateHandle Handle;
		this->insert(std::make_pair(Handle, TDelegate()));//添加

		TDelegate& InDelegate = this->at(Handle);
		InDelegate.Bind(InFuncation);

		return Handle;
	}

	void Broadcast(ParamTypes &&...Params)
	{
		for (auto &Tmp :*this)
		{
			Tmp.second.Execute(std::forward<ParamTypes>(Params)...);
		}		
	}

	void ReleaseDelegates()
	{
		for (auto& Tmp : *this)
		{
			Tmp.ReleaseDelegate();
		}
	}
};

#define SIMPLE_SINGLE_DELEGATE(Name,Return,...) FSingleDelegate<Return,__VA_ARGS__> Name
#define MULTICAST_SINGLE_DELEGATE(Name,Return,...) FMulticastDelegate<Return,__VA_ARGS__> Name
#define DEFINITION_SIMPLE_SINGLE_DELEGATE(DefinitionName,Return,...) class DefinitionName :public FSingleDelegate<Return,__VA_ARGS__> {};
#define DEFINITION_MULTICAST_SINGLE_DELEGATE(DefinitionName,Return,...) class DefinitionName :public FMulticastDelegate<Return,__VA_ARGS__> {};
