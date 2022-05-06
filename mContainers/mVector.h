#pragma once

#include "mCore.h"

namespace mContainers {

	template<typename mVec>
	class mVecIterator
	{
	public:
		using fVal = float;
		using fRef = float&;
		using fPtr = float*;

	private:
		fPtr mPtr;

	public:
		mVecIterator(fPtr ptr)
			: mPtr(ptr) {}

		mVecIterator& operator++()
		{
			mPtr++;
			return *this;
		}
		mVecIterator operator++(int)
		{
			mVecIterator it = *this;
			++(*this);
			return it;
		}

		mVecIterator& operator--()
		{
			mPtr--;
			return *this;
		}
		mVecIterator operator--(int)
		{
			mVecIterator it = *this;
			--(*this);
			return it;
		}

		mVecIterator& operator+= (int offset)
		{
			mPtr += offset;
			return *this;
		}
		mVecIterator operator+ (int offset)
		{
			mVecIterator it = *this;
			it += offset;
			return it;
		}
		mVecIterator& operator-= (int offset)
		{
			return *this += -offset;
		}
		mVecIterator operator- (int offset)
		{
			mVecIterator it = *this;
			it += -offset;
			return it;
		}


		size_t operator- (const mVecIterator& rhs)
		{
			return mPtr - rhs.mPtr;
		}

		fRef operator[](int index)
		{
			return *(mPtr + index);
		}

		fPtr operator->()
		{
			return mPtr;
		}

		fRef operator*()
		{
			return *mPtr;
		}

		bool operator== (const mVecIterator& other) const
		{
			return mPtr == other.mPtr;
		}
		bool operator!= (const mVecIterator& other) const
		{
			return !(*this == other);
		}

		operator fPtr() { return mPtr; }
	};

	template<size_t _Size>
	class mVec
	{
	private:
		float elements[_Size];

	public:
		using Self = mVec<_Size>;
		using Iterator = mVecIterator<Self>;

		mVec() { zero(); }
		mVec(const Self&) = default;

		mVec(std::initializer_list<float> initVals)
		{
			set(initVals);
		}

		mVec(float scalar)
		{
			set(scalar);
		}

	public:
		Iterator begin()
		{
			return Iterator(elements);
		}
		const Iterator begin() const
		{
			return Iterator(elements);
		}

		Iterator end()
		{
			return Iterator(elements + _Size);
		}
		const Iterator end() const
		{
			return Iterator(elements + _Size);
		}

	public:
		//Math Operators

		constexpr Self& operator+=(float scalar)
		{
			for (size_t i = 0; i < _Size; i++)
				this->elements[i] += scalar;

			return *this;
		}
		constexpr Self& operator+=(const Self& other)
		{
			for (size_t i = 0; i < _Size; i++)
				this->elements[i] += other.elements[i];

			return *this;
		}
		constexpr Self operator+(const Self& other) const
		{
			Self result;
			for (size_t i = 0; i < _Size; i++)
				result.elements[i] = this->elements[i] + other.elements[i];

			return result;
		}
		constexpr Self operator+(float scalar) const
		{
			Self result;
			for (size_t i = 0; i < _Size; i++)
				result.elements[i] = this->elements[i] + scalar;

			return result;
		}

		constexpr Self& operator-=(float scalar)
		{
			for (size_t i = 0; i < _Size; i++)
				this->elements[i] -= scalar;

			return *this;
		}
		constexpr Self& operator-=(const Self& other)
		{
			for (size_t i = 0; i < _Size; i++)
				this->elements[i] -= other.elements[i];

			return *this;
		}
		constexpr Self operator-(const Self& other) const
		{
			Self result;
			for (size_t i = 0; i < _Size; i++)
				result.elements[i] = this->elements[i] - other.elements[i];

			return result;
		}
		constexpr Self operator-(float scalar) const
		{
			Self result;
			for (size_t i = 0; i < _Size; i++)
				result.elements[i] = this->elements[i] - scalar;

			return result;
		}

		Self operator -() const
		{
			Self result;
			for (size_t i = 0; i < _Size; i++)
				result.elements[i] = -this->elements[i];
			return result;
		}

		constexpr Self& operator*=(float scalar)
		{
			for (size_t i = 0; i < _Size; i++)
				this->elements[i] *= scalar;

			return *this;
		}
		friend constexpr Self operator*(float lhs, Self rhs)
		{
			rhs *= lhs;
			return rhs;
		}
		constexpr Self operator*(float scalar)
		{
			Self result;
			for (size_t i = 0; i < _Size; i++)
				result.elements[i] = this->elements[i] * scalar;

			return result;
		}

		constexpr Self& operator/=(float scalar)
		{
			float inverse = 1.f / scalar;
			for (size_t i = 0; i < _Size; i++)
				this->elements[i] /= inverse;
		}
		constexpr Self operator/(float scalar)
		{
			Self result;
			float inverse = 1.f / scalar;
			for (size_t i = 0; i < _Size; i++)
				result.elements[i] = this->elements[i] / inverse;

			return result;
		}

		bool operator<(const Self& other)
		{
			for (size_t i = 0; i < _Size; i++)
			{
				if (this->elements[i] > other.elements[i]) return false;
			}

			return true;
		}

		// Boolean Operators

		bool operator== (const Self& other)
		{
			for (size_t i = 0; i < _Size; i++)
			{
				if (this->elements[i] != other.elements[i]) return false;
			}

			return true;
		}
		bool operator!= (const Self& other)
		{
			return !(*this == other);
		}

		// IOStream Operators

		friend std::ostream& operator<<(std::ostream& out, const Self& vec)
		{
			for (size_t i = 0; i < _Size - 1; i++)
				out << vec(i) << ", ";
			out << vec(_Size - 1);

			return out;
		}


		// Access Operators

		float& operator[] (size_t i)
		{
			mAssert(i < _Size);

			return this->elements[i];
		}
		const float& operator[] (size_t i) const
		{
			mAssert(i < _Size);

			return this->elements[i];
		}

		float operator() (size_t i) const
		{
			mAssert(i < _Size);

			return this->elements[i];
		}

		// Access Functions for mVec2 and/or mVec3

		float& x()
		{
			mStaticAssert(_Size > 0);
			return this->elements[0];
		}

		float& y()
		{
			mStaticAssert(_Size > 1);
			return this->elements[1];
		}

		float& z()
		{
			mStaticAssert(_Size > 2);
			return this->elements[2];
		}

		float x() const
		{
			mStaticAssert(_Size > 0);
			return this->elements[0];
		}

		float y() const
		{
			mStaticAssert(_Size > 1);
			return this->elements[1];
		}

		float z() const
		{
			mStaticAssert(_Size > 2);
			return this->elements[2];
		}

	public:
		constexpr size_t size() const noexcept
		{
			return _Size;
		}

		constexpr void zero() { memset(elements, 0, _Size * sizeof(float)); }

		float sum() const
		{
			float result = 0.0f;
			for (size_t i = 0; i < _Size; i++)
				result += this->elements[i];

			return result;
		}

		constexpr float abs() const
		{
			Self resVec(0.0f);
			for (size_t i = 0; i < _Size; i++)
				resVec.elements[i] = this->elements[i] * this->elements[i];

			return mSqrt(resVec.sum());
		}

		void set(std::initializer_list<float> vals)
		{
			size_t i = 0;
			for (float val : vals)
			{
				if (i == _Size) break;

				elements[i] = val;
				i++;
			}

			size_t size = vals.size();
			for (size_t k = size; k < _Size; k++)
				elements[k] = 0;
		}
		void set(float scalar) { memset(elements, scalar, _Size * sizeof(float)); }

		bool isValid() const
		{
			bool result;
			for (size_t i = 0; i < _Size; i++)
			{
				result = mIsValid(elements[i]);
				if (!result) return result;
			}

			return true;
		}

		/// Get the length of this vector (the norm).
		float length() const
		{
			float square = 0.0f;
			for (size_t i = 0; i < _Size; i++)
				square += (this->elements[i] * this->elements[i]);

			return mSqrt(square);
		}

		/// Get the length squared. For performance, use this instead of
		/// mVec2::length (if possible).
		float lengthSquared() const
		{
			float square = 0.0f;
			for (size_t i = 0; i < _Size; i++)
				square += (this->elements[i] * this->elements[i]);

			return square;
		}

		/// Convert this vector into a unit vector. Returns the length.
		float normalise()
		{
			float l = length();
			if (l < mEpsilon)
			{
				return 0.0f;
			}
			float invLength = 1.0f / l;

			*this *= invLength;

			return l;
		}
	};

	using mVec2 = mVec<2>;
	using mVec3 = mVec<3>;
}
