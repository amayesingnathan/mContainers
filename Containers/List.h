#pragma once

namespace Containers {

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
			Node* prev = nullptr;
			Node* next = nullptr;

			Node() = default;
			Node(Node* _prev, Node* _next)
				: data(), prev(_prev), next(_next) {}
			Node(Node* _prev, Node* _next, const T& _data)
				: data(_data), prev(_prev), next(_next) {}
			template<typename... Args>
			Node(Node* _prev, Node* _next, Args&&... args)
				: data(std::forward<Args>(args)...), prev(_prev), next(_next) {}

			operator T& () { return data; }
			T& operator* () { return data; }
		};

	public:
		using Iterator = ListIterator<List<T>>;
		using ValType = T;
		using NodeType = Node;

	private:
		Node* mHead;
		uint64_t mSize;

	public:
		List() 
			: mHead(new Node), mSize(0)
		{
			mHead->next = mHead;
			mHead->prev = mHead;
		}

		List(size_t count)
			: mHead(new Node)
		{
			mHead->next = mHead;
			mHead->prev = mHead;

			if (count == 0) return;

			Node* currNode = mHead;
			for (mSize = 1; mSize <= count; mSize++)
			{
				Node* nextNode = new Node(currNode, mHead);
				currNode->next = nextNode;
				mHead->prev = nextNode;

				currNode = nextNode;
			}
		}

		List(size_t count, const T& val)
			: mHead(new Node)
		{
			mHead->next = mHead;
			mHead->prev = mHead;

			if (count == 0) return;

			Node* currNode = mHead;
			for (mSize = 1; mSize <= count; mSize++)
			{
				currNode->next = new Node(currNode, mHead, val);

				mHead->prev = currNode->next;
				currNode = currNode->next;
			}
		}

		~List()
		{
			clear();
			delete mHead;
		}

		T& front() { return mHead->next.data; }
		const T& front() const { return mHead->next.data; }
		T& back() { return mHead->prev.data; }
		const T& back() const { return mHead->prev.data; }

		Iterator begin() { return Iterator(mHead->next); }
		const Iterator begin() const { return Iterator(mHead->next); }

		Iterator end() { return Iterator(mHead); }
		const Iterator end() const{	return Iterator(mHead);	}

		bool empty() const { return mSize == 0; }
		size_t size() const { return mSize; };
		size_t max_size() const { return std::numeric_limits<std::size_t>::max(); }

		void clear()
		{
			Iterator it = begin();
			if (it != end())
			{
				while (it != end())
				{
					it++;
					delete it->prev;
				}
			}
		}

		T& insert(Iterator pos, const T& value)
		{
			Node* newNode = new Node(pos->prev, pos, value);
			pos->prev->next = newNode;
			pos->prev = newNode;
			mSize++;

			return newNode->data;
		}

		T& push_back(const T& value)
		{
			Node* newNode = new Node(mHead->prev, mHead, value);
			mHead->prev->next = newNode;
			mHead->prev = newNode;
			mSize++;

			return newNode->data;
		}

		template<typename... Args>
		T& emplace(Iterator pos, Args&&... args)
		{
			Node* newNode = new Node(pos->prev, pos, std::forward<Args>(args)...);
			pos->prev->next = newNode;
			pos->prev = newNode;
			mSize++;

			return newNode->data;
		}

		template<typename... Args>
		T& emplace_back(Args&&... args)
		{
			Node* newNode = new Node(mHead->prev, mHead, std::forward<Args>(args)...);
			mHead->prev->next = newNode;
			mHead->prev = newNode;
			mSize++;

			return newNode->data;
		}


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
	};

}