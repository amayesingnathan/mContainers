#pragma once

#include <cassert>

namespace Labyrinth {

	template<typename Vector>
	class VecIterator
	{
	public:
		using TypeVal = typename Vector::ValType;
		using TypeRef = typename Vector::ValType&;
		using TypePtr = typename Vector::ValType*;

	private:
		TypePtr mPtr;

	public:
		VecIterator(TypePtr ptr)
			: mPtr(ptr) {}

		VecIterator& operator++()
		{
			mPtr++;
			return *this;
		}
		VecIterator operator++(int)
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
		VecIterator operator--(int)
		{
			VecIterator it = *this;
			--(*this);
			return it;
		}

		VecIterator& operator+= (int offset)
		{
			mPtr += offset;
			return *this;
		}
		VecIterator operator+ (int offset)
		{
			VecIterator it = *this;
			it += offset;
			return it;
		}
		VecIterator& operator-= (int offset)
		{
			return *this += -offset;
		}
		VecIterator operator- (int offset)
		{
			VecIterator it = *this;
			it += -offset;
			return it;
		}


		size_t operator- (const VecIterator& rhs)
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

		bool operator== (const VecIterator& other) const
		{
			return mPtr == other.mPtr;
		}
		bool operator!= (const VecIterator& other) const
		{
			return !(*this == other);
		}
	};

	template<typename T>
	class Vector
	{
	public:
		using Iterator = VecIterator<Vector<T>>;
		using ValType = T;

	private:
		T* mData = nullptr;

		size_t mSize;
		size_t mCapacity;

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
			// Destroy object and go to next in vector
			(*it++).~T();

			// Loop from next element after the one to erase to end and overwrite the previous iterator
			for (it; it != end(); it++)
			{
				*(it - 1) = std::move(*it);
			}

			mSize--;
		}

		void erase(Iterator& rangeBegin, Iterator& rangeEnd)
		{
			// Quality of life so if you put end iterator it will use the last element
			if (rangeEnd == end())
				rangeEnd--;	

			size_t span = (rangeEnd - rangeBegin) + 1;
			Iterator start = rangeBegin;
			Iterator next = (rangeEnd + 1);

			for (rangeBegin; rangeBegin != next; rangeBegin++)
				(*rangeBegin).~T();

			for (next; next != end(); next++)
				*start++ = std::move(*next);

			mSize -= span;
		}
		void erase(Iterator&& begin, Iterator&& end)
		{
			erase(begin, end);
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
	};

}