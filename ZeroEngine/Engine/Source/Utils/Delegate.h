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
		FObjectDelegate(TObjectType* Object, Return(TObjectType::* Funcation)(ParamTypes ...))
			: m_Object(Object)
			, m_Function(Funcation)
		{}
		virtual TReturn Execute(ParamTypes ...Params)
		{
			return (m_Object->*m_Funciton)(Params...);
		}
	private:
		TObjectType* m_Object;
		TReturn(TObjectType::* m_Function)(Params...);
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
}
