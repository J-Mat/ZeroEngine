#pragma once

#include "Core.h"

template<typename T>
class TThreadSafeQueue
{
public:
	TThreadSafeQueue() {}
	TThreadSafeQueue(const TThreadSafeQueue<T>& Copy)
	{
		std::lock_guard<std::mutex> Lock(Copy.Mutex);
		Queue = Copy.Queue;
	}
	void Push(T Value)
	{
		std::lock_guard<std::mutex> Lock(Mutex);
		Queue.push(std::move(Value));
	}
	
	bool TryPop(T& Value)
	{
		std::lock_guard<std::mutex> Lock(Mutex);
		if (Queue.empty())
		{
			return false;
		}
		Value = Queue.front();
		Queue.pop();
		
		return true;
	}
	
	bool IsEmpty() const
	{
		std::lock_guard<std::mutex> Lock(Mutex);
		return Queue.empty();
	}

	size_t Num() const 
	{
		std::lock_guard<std::mutex> Lock(Mutex);
		return Queue.size();
	}

private:
	std::queue<T> Queue;
	mutable std::mutex Mutex;
};