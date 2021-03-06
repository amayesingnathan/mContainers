#pragma once

#include "mCore.h"
#include "mBlock.h"

namespace mContainers {

	template<typename mDynArray>
	class mDynIterator
	{
	public:
		using TypeVal = typename mDynArray::ValType;
		using TypeRef = typename mDynArray::ValType&;
		using TypePtr = typename mDynArray::ValType*;

	private:
		TypePtr mPtr;

	public:
		mDynIterator(TypePtr ptr)
			: mPtr(ptr) {}

		mDynIterator& operator++()
		{
			mPtr++;
			return *this;
		}
		mDynIterator operator++(int)
		{
			mDynIterator temp = *this;
			temp++;
			return temp;
		}

		mDynIterator& operator--()
		{
			mPtr--;
			return *this;
		}
		mDynIterator operator--(int)
		{
			mDynIterator temp = *this;
			temp--;
			return temp;
		}

		mDynIterator& operator+= (int offset)
		{
			mPtr += offset;
			return *this;
		}
		mDynIterator operator+ (int offset) const
		{
			mDynIterator it = *this;
			it += offset;
			return it;
		}
		mDynIterator& operator-= (int offset)
		{
			return *this += -offset;
		}
		mDynIterator operator- (int offset) const
		{
			mDynIterator it = *this;
			it += -offset;
			return it;
		}

		uint64_t operator- (const mDynIterator& rhs) const
		{
			return mPtr - rhs.mPtr;
		}


		TypeRef operator[](int index)
		{
			return *(mPtr + index);
		}

		TypePtr operator->()
		{
			return mPtr;
		}

		TypeRef operator*()
		{
			return *mPtr;
		}

		bool operator== (const mDynIterator& other) const
		{
			return mPtr == other.mPtr;
		}
		bool operator!= (const mDynIterator& other) const
		{
			return !(*this == other);
		}

		operator TypePtr() { return mPtr; }
	};

	template<typename T>
	class mDynArray
	{
	public:
		using VecType = mDynArray<T>;
		using Iterator = mDynIterator<mDynArray<T>>;
		using ValType = T;

	protected:
		T* mData;

		uint64_t mSize;
		uint64_t mCapacity;

	public:
		mDynArray()
			: mData(nullptr), mSize(0), mCapacity(0)
		{ 
			ReAlloc(5);
		}

		mDynArray(uint64_t count)
			: mData(nullptr), mSize(0), mCapacity(0)
		{
			ReAllocConstruct(count);
		}

		mDynArray(uint64_t count, const T& val)
			: mData(nullptr), mSize(0), mCapacity(0)
		{
			ReAllocConstruct(count, val);
		}

	//private: //Only for use by dictionary - must be fully initialised or unitialised, no mix state
		mDynArray(T* dataBlock, uint64_t length, bool initialised = false)
		{
			mCapacity = length;
			if (initialised) mSize = length;
			mData = dataBlock;
		}

	public:
		~mDynArray()
		{
			clear();
			::operator delete(mData, mCapacity * sizeof(T));
		}

		void push_back(const T& value)
		{
			if (mSize >= mCapacity)
				ReAlloc(2 * mCapacity);

			mData[mSize++] = value;
		}
		void push_back(T&& value)
		{
			if (mSize >= mCapacity)
				ReAlloc(2 * mCapacity);

			mData[mSize++] = std::move(value);
		}

		template<typename... Args>
		T& emplace_back(Args&&... args)
		{
			if (mSize >= mCapacity)
				ReAlloc(2 * mCapacity);

			Memory::Emplace<T>(&mData[mSize], std::forward<Args>(args)...);
			return mData[mSize++];
		}

		void pop_back()
		{
			assert(mSize > 0);

			mData[mSize--].~T();
		}

		void clear()
		{
			for (uint64_t i = 0; i < mSize; i++)
				mData[i].~T();

			mSize = 0;
		}
		
	protected:
		T* data() { return mData; }

	public:
		T& operator[](uint64_t index)
		{
			mAssert(index < mSize, "Index out of range!");

			return mData[index];
		}
		const T& operator[](uint64_t index) const
		{
			mAssert(index < mSize, "Index out of range!");

			return mData[index];
		}

		VecType& operator=(const mBlock<T>& other)
		{
			clear();
			::operator delete(mData, mCapacity * sizeof(T));

			mData = other.mData;
			mSize = other.mSize;
			mCapacity = other.mCapacity;

			return *this;
		}

		virtual void resize(uint64_t newSize)
		{
			ReAllocConstruct(newSize);
		}
		void resize(uint64_t newSize, const T& value)
		{
			ReAllocConstruct(newSize, value);
		}
		void reserve(uint64_t newCapacity)
		{
			if (newCapacity <= mCapacity) return;

			ReAlloc(newCapacity);
		}

		uint64_t size() const { return mSize; }
		uint64_t capacity() const { return mCapacity; }

		// O(n) time linear search (are other searches possible with iterators?)
		Iterator find(const Iterator& begin, const Iterator& end, const T& value)
		{
			Iterator it = begin;
			for (it; it != end; it++)
				if (*it == value) return it;

			return it;
		}

		Iterator find(const T& value)
		{
			return find(begin(), end(), value);
		}

		void erase(Iterator& it)
		{
			// Destroy object and go to next in mDynArray
			(*it++).~T();

			// Loop from next element after the one to erase to end and overwrite the previous iterator
			for (it; it != end(); it++)
				*(it - 1) = std::move(*it);

			mSize--;
		}

		void erase(const Iterator& rangeBegin, const Iterator& rangeEnd)
		{
			// Quality of life addition so using end iterator will use the last element
			// (end() points to first pointer after the last element)
			Iterator endCheck = rangeEnd;
			if (rangeEnd == end())
				endCheck--;

			Iterator to = rangeBegin;
			for (Iterator from = endCheck + 1; from != end(); from++, to++)
				*to = std::move(*from);

			for (; to != end(); to++)
				(*to).~T();

			mSize -= (rangeEnd - rangeBegin);
		}

		Iterator begin()
		{
			return Iterator(mData);
		}
		const Iterator begin() const
		{
			return Iterator(mData);
		}

		Iterator end()
		{
			return Iterator(mData + mSize);
		}
		const Iterator end() const
		{
			return Iterator(mData + mSize);
		}

	protected:
		void ReAlloc(uint64_t newCapacity)
		{
			T* newBlock = Memory::Alloc<T>(newCapacity);

			uint64_t newSize = mSize;
			if (newCapacity < mSize) newSize = newCapacity;
			for (uint64_t i = 0; i < newSize; i++)
				Memory::Emplace<T>(&newBlock[i], std::move(mData[i])); // Move construct new data from current data

			if (mData) Memory::Free<T>(mData, mCapacity);
			mData = newBlock;
			mCapacity = newCapacity;
		}

		void ReAllocConstruct(uint64_t newCapacity)
		{
			T* newBlock = Memory::Alloc<T>(newCapacity);

			uint64_t oldSize = mSize;
			if (newCapacity < mSize) mSize = newCapacity;
			for (uint64_t i = 0; i < mSize; i++)
				Memory::Emplace<T>(&newBlock[i], std::move(mData[i])); // Move construct new data from current data

			for (uint64_t i = mSize; i < newCapacity; i++)
				Memory::Emplace<T>(&newBlock[i]); // Initialise new data if growing

			for (uint64_t i = 0; i < oldSize; i++)
				mData[i].~T(); // Call destructor for moved data

			if (mData) 
				Memory::Free<T>(mData, mCapacity);
			mData = newBlock;
			mCapacity = newCapacity;
			mSize = newCapacity;
		}

		void ReAllocConstruct(uint64_t newCapacity, const T& val)
		{
			T* newBlock = Memory::Alloc<T>(newCapacity);

			uint64_t newSize = mSize;
			if (newCapacity < mSize) newSize = newCapacity;
			for (uint64_t i = 0; i < newSize; i++)
				Memory::Emplace<T>(&newBlock[i], std::move(mData[i])); // Move construct new data from current data

			for (uint64_t i = mSize; i < newSize; i++)
				Memory::Emplace<T>(&newBlock[i]); // Initialise new data if growing

			for (uint64_t i = 0; i < mSize; i++)
				mData[i].~T(); // Call destructor for moved data

			if (mData)
				Memory::Free<T>(mData, mCapacity);
			mData = newBlock;
			mCapacity = newCapacity;
			mSize = newCapacity;
		}
	};

}