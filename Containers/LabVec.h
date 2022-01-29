#pragma once

#include <cassert>

namespace Labyrinth {

	template<typename Vector>
	class VecIterator
	{
	public:
		using Val = typename Vector::ValType;
		using Ref = typename Vector::ValType&;
		using Ptr = typename Vector::ValType*;

	public:
		VecIterator(Ptr ptr)
			: mPtr(ptr) {}

		VecIterator& operator++()
		{
			mPtr++;
			return *this;
		}
		VecIterator& operator++(int)
		{
			VecIterator it = *this;
			++(*this);
			return it;
		}

		VecIterator& operator--()
		{
			mPtr--;
			return *this;
		}
		VecIterator& operator--(int)
		{
			VecIterator it = *this;
			--(*this);
			return it;
		}

		Ref operator[](int index)
		{
			return *(mPtr + index);
		}

		Ptr operator->()
		{
			return mPtr;
		}

		Ref operator*()
		{
			return *mPtr;
		}

		bool operator== (const VecIterator& other) const
		{
			return mPtr == other.mPtr;
		}
		bool operator!= (const VecIterator& other) const
		{
			return !(*this == other);
		}

	private:
		Ptr mPtr;
	};

	template<typename T>
	class Vector
	{
	public:
		using Iterator = VecIterator<Vector<T>>;
		using ValType = T;

	public:
		Vector()
		{
			mSize = 0;
			Realloc(4);
		}

		~Vector()
		{
			delete[] mData;
		}

		void push_back(const T& value)
		{
			if (mSize >= mCapacity)
				Realloc(mCapacity * 2);

			mData[mSize++] = value;
		}
		void push_back(T&& value)
		{
			if (mSize >= mCapacity)
				Realloc(mCapacity * 2);

			mData[mSize++] = std::move(value);
		}

		template<typename... Args>
		T& emplace_back(Args&&... args)
		{
			if (mSize >= mCapacity)
				Realloc(mCapacity * 2);

			new(&mData[mSize]) T(std::forward<Args>(args)...);
			return mData[mSize++];
		}

		void pop_back()
		{
			assert(mSize > 0);

			mData[mSize--].~T();
		}

		void clear()
		{
			for (size_t i = 0; i < mSize; i++)
				mData[i].~T();

			mSize = 0;
		}

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

		void resize(size_t newSize)
		{
			if (newSize > mCapacity)
				Realloc(newSize, true);

			mSize = newSize;
		}
		void reserve(size_t newCapacity)
		{
			if (newCapacity > mCapacity)
				Realloc(newCapacity);

			mCapacity = newCapacity;
		}

		size_t size() const { return mSize; }
		size_t capacity() const { return mCapacity; }

		void erase(const Iterator& toRemove)
		{

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

	private:
		void Realloc(size_t newCapacity)
		{
			T* newBlock = new T[newCapacity];

			if (mSize > newCapacity)
				mSize = newCapacity;

			for (size_t i = 0; i < mSize; i++)
				newBlock[i] = std::move(mData[i]);

			delete[] mData;
			mData = newBlock;
			mCapacity = newCapacity;
		}
		void Realloc(size_t newCapacity, bool construct)
		{
			T* newBlock = new T[newCapacity];

			if (mSize > newCapacity)
				mSize = newCapacity;

			for (size_t i = 0; i < mSize; i++)
				newBlock[i] = std::move(mData[i]);

			delete[] mData;
			mData = newBlock;
			mCapacity = newCapacity;
		}

	private:
		T* mData = nullptr;

		size_t mSize;
		size_t mCapacity;

	};

}