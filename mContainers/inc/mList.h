#pragma once

#include "mCore.h"

namespace mContainers {

	template<typename mList>
	class mListIterator
	{
	public:
		using TypeVal = typename mList::ValType;
		using TypeRef = typename mList::ValType&;
		using TypePtr = typename mList::ValType*;

		using NodeVal = typename mList::NodeType;
		using NodeRef = typename mList::NodeType&;
		using NodePtr = typename mList::NodeType*;

	private:
		NodePtr mNode;

	public:
		mListIterator(NodePtr node)
			: mNode(node) {}

		mListIterator& operator++()
		{
			mNode = mNode->next;
			return *this;
		}
		mListIterator operator++(int)
		{
			mListIterator it = *this;
			mNode = mNode->next;
			return it;
		}

		mListIterator& operator+= (uint64_t offset)
		{
			for (int i = 0; i < offset; i++)
				(*this)++;

			return *this;
		}
		mListIterator operator+ (uint64_t offset)
		{
			mListIterator it = *this;
			it += offset;
			return it;
		}

		mListIterator& operator--()
		{
			mNode = mNode->prev;
			return *this;
		}
		mListIterator operator--(int)
		{
			mListIterator it = *this;
			mNode = mNode->prev;
			return it;
		}

		mListIterator& operator-= (uint64_t offset)
		{
			for (int i = 0; i < offset; i++)
				(*this)--;

			return *this;
		}
		mListIterator operator- (int offset)
		{
			mListIterator it = *this;
			it -= offset;
			return it;
		}

		NodePtr operator->()
		{
			return mNode;
		}

		TypeRef operator*()
		{
			return mNode->data;
		}

		bool operator== (const mListIterator& other) const
		{
			return mNode == other.mNode;
		}
		bool operator!= (const mListIterator& other) const
		{
			return !(*this == other);
		}

		operator NodePtr() { return mNode; }

	};

	template<typename T>
	class mList
	{
	public:
		struct Node
		{
			T data;
			Node* next;

			Node()
				: data(), next(nullptr) {}
			Node(Node* _next)
				: data(), next(_next) {}
			Node(Node* _next, const T& _data)
				: data(_data), next(_next) {}
			template<typename... Args>
			Node(Node* _next, Args&&... args)
				: data(std::forward<Args>(args)...), next(_next) {}

			operator T& () { return data; }
			T& operator* () { return data; }
		};

	public:
		using mListType = mList<T>;
		using Iterator = mListIterator<mListType>;
		using ValType = T;
		using NodeType = Node;

	private:
		Node* mHead;
		uint64_t mSize;

	public:
		mList() : mHead(nullptr), mSize(0) {}

		mList(uint64_t count)
			: mHead(nullptr), mSize(0)
		{
			if (count == 0) return;

			for (mSize = 0; mSize < count; mSize++)
				emplace_front();
		}

		mList(uint64_t count, const T& val)
			: mHead(nullptr), mSize(0)
		{
			for (mSize = 0; mSize < count; mSize++)
				push_front(val);
		}

		~mList()
		{
			clear();
		}

		T& front() { return mHead->data; }
		const T& front() const { return mHead->data; }

		Iterator begin() { return Iterator(mHead); }
		const Iterator begin() const { return Iterator(mHead); }

		Iterator end() { return Iterator(nullptr); }
		const Iterator end() const{	return Iterator(nullptr);	}

		bool empty() const { return mSize == 0; }
		uint64_t size() const { return mSize; };
		uint64_t max_size() const { return -1; }

		void clear()
		{
			for (uint64_t i = 0; i < mSize; i++)
				pop_front();
			mSize = 0;
		}

		T& insert_after(Iterator pos, const T& value)
		{
			Node* newNode = new Node(pos->next, value);
			pos->next = newNode;
			mSize++;

			return newNode->data;
		}

		T& push_front(const T& value)
		{
			Node* newNode = new Node(mHead, value);
			mHead = newNode;
			mSize++;

			return newNode->data;
		}

		T& push_front(T&& value)
		{
			Node* newNode = new Node(mHead, std::move(value));
			mHead = newNode;
			mSize++;

			return newNode->data;
		}

		void pop_front()
		{
			if (!mHead) return;

			Node* temp = mHead;
			mHead = mHead->next;
			delete temp;
		}

		template<typename... Args>
		T& emplace_after(Iterator pos, Args&&... args)
		{
			Node* newNode = new Node(pos->next, std::forward<Args>(args)...);
			pos->next = newNode;
			mSize++;

			return newNode->data;
		}

		template<typename... Args>
		T& emplace_front(Args&&... args)
		{
			Node* newNode = new Node(mHead, std::forward<Args>(args)...);
			mHead = newNode;
			mSize++;

			return newNode->data;
		}

		Iterator find(const Iterator& begin, const Iterator& end, const T& value)
		{
			for (Iterator it = begin; it != end; it++)
				if (*it == value) return it;

			return end;
		}

		Iterator find(const T& value)
		{
			return find(begin(), end(), value);
		}
	};

}