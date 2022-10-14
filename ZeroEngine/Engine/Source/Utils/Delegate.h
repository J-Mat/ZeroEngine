#pragma once

#include <map> 
#include <functional>
#include "Utils.h"

namespace Zero
{
	template< class TReturn, typename ...ParamTypes>
	class FDelegateBase
	{
	public:
		virtual TReturn Execute(ParamTypes ...Params)
		{
			return TReturn();
		}
	};


	// ¶à²¥´úÀí
	template<class TObjectType, class TReturn, typename ...ParamTypes>
	class FObjectDelegate : public FDelegateBase<TReturn, ParamTypes...>
	{
	public:
		FObjectDelegate(TObjectType* Object, TReturn(TObjectType::* Funcation)(ParamTypes ...))
			: m_Object(Object)
			, m_Function(Funcation)
		{}
		virtual TReturn Execute(ParamTypes ...Params)
		{
			return (m_Object->*m_Funciton)(Params...);
		}
	private:
		TObjectType* m_Object;
		TReturn(TObjectType::* m_Function)(ParamTypes...);
	};


	template<class TReturn, typename ...ParamTypes>
	class FFunctionDelegate :public FDelegateBase<TReturn, ParamTypes...>
	{
	public:
		FFunctionDelegate(TReturn(*Funcation)(ParamTypes ...))
			: m_Function(Funcation)
		{}

		virtual TReturn Execute(ParamTypes ...Params)
		{
			return (*m_Function)(Params...);
		}
	private:
		TReturn(*m_Funcation)(ParamTypes ...);
	};

	template<class TReturn, typename ...ParamTypes>
	class FLambdaDelegate :public FDelegateBase<TReturn, ParamTypes...>
	{
	public:
		FLambdaDelegate(std::function<TReturn(ParamTypes...)> Lambda)
			:m_LambdaFunction(Lambda)
		{
		}
		virtual TReturn Execute(ParamTypes ...Params)
		{
			return m_LambdaFunction(Params...);
		}

	private:
		std::function<TReturn(ParamTypes...)> m_LambdaFunction;
	};

	template<class TReturn, typename ...ParamTypes>
	class FDelegate
	{
	public:
		FDelegate() = default;
		~FDelegate()
		{
			ReleaseDelegate();
		}

		void ReleaseDelegate()
		{
			if (m_CurDelegatePtr)
			{
				delete m_CurDelegatePtr;
				m_CurDelegatePtr = nullptr;
			}
		}

	public:
		template<class TObjectType>
		static FDelegate<TReturn, ParamTypes...> Create(TObjectType* Object, TReturn(TObjectType::* Funcation)(ParamTypes ...))
		{
			FDelegate<TReturn, ParamTypes...> DelegateInstance;
		}

	public:
		template<class TObjectType>
		void Bind(TObjectType* Object, TReturn(TObjectType::* Function)(ParamTypes ...))
		{
			ReleaseDelegate();
			m_CurDelegatePtr = new FObjectDelegate<TObjectType, TReturn, ParamTypes...>(Object, Function);
		}

		void BindLambda(std::function<TReturn(ParamTypes...)> Lambda)
		{
			ReleaseDelegate();
			m_CurDelegatePtr = new FLambdaDelegate<TReturn, ParamTypes...>(Lambda);
		}

		void Bind(TReturn(*Function)(ParamTypes...))
		{
			ReleaseDelegate();
			m_CurDelegatePtr = new FFunctionDelegate<TReturn, ParamTypes...>(Function);
		}

		bool IsBound()
		{
			return m_CurDelegatePtr != nullptr;
		}

		virtual TReturn Execute(ParamTypes ...Params)
		{
			return m_CurDelegatePtr->Execute(Params...);
		}

		FDelegate<TReturn, ParamTypes...>& operator=(const FDelegate<TReturn, ParamTypes...>& Delegate)
		{
			m_CurDelegatePtr = Delegate.m_CurDelegatePtr;
			return *this;
		}
	private:
		FDelegateBase<TReturn, ParamTypes...>* m_CurDelegatePtr = nullptr;
	};

	template<class TReturn, typename ...ParamTypes>
	class FSingleDelegate :public FDelegate<TReturn, ParamTypes...>
	{
	public:
		FSingleDelegate() = default;
	};


	struct FDelegateHandle
	{
		FDelegateHandle()
		{
			Guid = Utils::newGuid();
		}

		friend bool operator<(const FDelegateHandle& L, const FDelegateHandle& R)
		{
			return L.Guid < R.Guid;
		}

		Utils::Guid Guid;
	};

	template<class TReturn, typename ...ParamTypes>
	class FMulticastDelegate :public std::map<FDelegateHandle, FDelegate<TReturn, ParamTypes...>>
	{
	public:
		using TDelegate = FDelegate<TReturn, ParamTypes...>;
		FMulticastDelegate() = default;
		
		void RemoveDelegate(const FDelegateHandle& Handle)
		{
			this->erase(Handle);
		}

		template<class TObjectType>
		const FDelegateHandle AddFuncion(TObjectType* Object, TReturn(TObjectType::* Funcation)(ParamTypes ...))
		{
			FDelegateHandle Handle;
			TDelegate Delegate;
			Delegate.Bind(Object, Funcation);
			this->insert({Handle, Delegate});
		
			return Handle;
		}

		const FDelegateHandle& AddFunction(TReturn(*InFuncation)(ParamTypes...))
		{
			FDelegateHandle Handle;
			TDelegate Delegate;
			Delegate.Bind(Funcation);
			this->insert({Handle, Delegate});

			return Handle;
		}

		void Broadcast(ParamTypes ...Params)
		{
			for (auto& Tmp : *this)
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
#define DEFINITION_SIMPLE_SINGLE_DELEGATE(DefinitionName,Return,...) class DefinitionName :public FSingleDelegate<Return, __VA_ARGS__> {};
#define DEFINITION_MULTICAST_SINGLE_DELEGATE(DefinitionName,Return,...) class DefinitionName :public FMulticastDelegate<Return, __VA_ARGS__> {};
}


