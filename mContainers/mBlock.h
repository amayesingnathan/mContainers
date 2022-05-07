#pragma once
#pragma once

#include "mCore.h"

namespace mContainers {

	template<typename mBlock>
	class mBlockIterator
	{
	public:
		using TypeVal = typename mBlock::ValType;
		using TypeRef = typename mBlock::ValType&;
		using TypePtr = typename mBlock::ValType*;

	private:
		TypePtr mPtr;

	public:
		mBlockIterator(TypePtr ptr)
			: mPtr(ptr) {}

		mBlockIterator& operator++()
		{
			mPtr++;
			return *this;
		}
		mBlockIterator operator++(int)
		{
			mBlockIterator temp = *this;
			temp++;
			return temp;
		}

		mBlockIterator& operator--()
		{
			mPtr--;
			return *this;
		}
		mBlockIterator operator--(int)
		{
			mBlockIterator temp = *this;
			temp--;
			return temp;
		}

		mBlockIterator& operator+= (int offset)
		{
			mPtr += offset;
			return *this;
		}
		mBlockIterator operator+ (int offset) const
		{
			mBlockIterator it = *this;
			it += offset;
			return it;
		}
		mBlockIterator& operator-= (int offset)
		{
			return *this += -offset;
		}
		mBlockIterator operator- (int offset) const
		{
			mBlockIterator it = *this;
			it += -offset;
			return it;
		}

		size_t operator- (const mBlockIterator& rhs) const
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

		bool operator== (const mBlockIterator& other) const
		{
			return mPtr == other.mPtr;
		}
		bool operator!= (const mBlockIterator& other) const
		{
			return !(*this == other);
		}

		operator TypePtr() { return mPtr; }
	};

	template<typename T>
	class mBlock
	{
	public:
		using Iterator = mBlockIterator<mBlock<T>>;
		using ValType = T;

	protected:
		T* mData;

		size_t mSize;
		size_t mCapacity;

	public:
		mBlock() : mData(nullptr), mSize(0), mCapacity(0) {}

		mBlock(T* dataBlock, size_t length)
		{
			mCapacity = length;
			mSize = 0;
			mData = dataBlock;
		}
		template<typename... Args>
		mBlock(T* dataBlock, size_t length, Args&&... args)
		{
			mCapacity = length;
			mSize = length;
			mData = dataBlock;
			for (size_t i = 0; i < length; i++)
				new(&mData[i]) T(std::forward<Args>(args)...);
		}

	public:
		~mBlock()
		{
			clear();
		}

		void clear()
		{
			for (size_t i = 0; i < mSize; i++)
				mData[i].~T();

			mSize = 0;
		}

	protected:
		T* data() { return mData; }

	public:
		T& operator[](size_t index)
		{
			assert(index < mSize);

			return mData[index];
		}
		const T& operator[](size_t index) const
		{
			assert(index < mSize);

			return mData[index];
		}

		template<typename... Args>
		T& emplace_back(Args&&... args)
		{
			mAssert(mSize < mCapacity, "Block is full")

			new(&mData[mSize]) T(std::forward<Args>(args)...);
			return mData[mSize++];
		}

		size_t size() const { return mSize; }
		size_t capacity() const { return mCapacity; }

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

	};

}