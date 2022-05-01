#pragma once

namespace mContainers {

	template<typename List>
	class ListIterator
	{
	public:
		using TypeVal = typename List::ValType;
		using TypeRef = typename List::ValType&;
		using TypePtr = typename List::ValType*;

		using NodeVal = typename List::NodeType;
		using NodeRef = typename List::NodeType&;
		using NodePtr = typename List::NodeType*;

	private:
		NodePtr mNode;

	public:
		ListIterator(NodePtr node)
			: mNode(node) {}

		ListIterator& operator++()
		{
			mNode = mNode->next;
			return *this;
		}
		ListIterator operator++(int)
		{
			ListIterator it = *this;
			mNode = mNode->next;
			return it;
		}

		ListIterator& operator+= (size_t offset)
		{
			for (int i = 0; i < offset; i++)
				(*this)++;

			return *this;
		}
		ListIterator operator+ (size_t offset)
		{
			ListIterator it = *this;
			it += offset;
			return it;
		}

		ListIterator& operator--()
		{
			mNode = mNode->prev;
			return *this;
		}
		ListIterator operator--(int)
		{
			ListIterator it = *this;
			mNode = mNode->prev;
			return it;
		}

		ListIterator& operator-= (size_t offset)
		{
			for (int i = 0; i < offset; i++)
				(*this)--;

			return *this;
		}
		ListIterator operator- (int offset)
		{
			ListIterator it = *this;
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

		bool operator== (const ListIterator& other) const
		{
			return mNode == other.mNode;
		}
		bool operator!= (const ListIterator& other) const
		{
			return !(*this == other);
		}

		operator NodePtr() { return mNode; }

	};

	template<typename T>
	class List
	{
	private:
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
		using ListType = List<T>;
		using Iterator = ListIterator<ListType>;
		using ValType = T;
		using NodeType = Node;

	private:
		Node* mHead;
		size_t mSize;

	public:
		List() : mHead(nullptr), mSize(0) {}

		List(size_t count)
			: mHead(nullptr), mSize(0)
		{
			if (count == 0) return;

			for (mSize = 0; mSize < count; mSize++)
				emplace_front();
		}

		List(size_t count, const T& val)
			: mHead(nullptr), mSize(0)
		{
			for (mSize = 0; mSize < count; mSize++)
				push_front(val);
		}

		~List()
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
		size_t size() const { return mSize; };
		size_t max_size() const { return -1; }

		void clear()
		{
			for (size_t i = 0; i < mSize; i++)
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