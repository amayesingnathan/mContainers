#pragma once

#include "mVector.h"

namespace mContainers {

	template<uint64_t _Rows, uint64_t _Columns>
	class mMat
	{
	public:
		using Matrix = mMat<_Rows, _Columns>;
		using Row = mVec<_Columns>;

	private:
		Row rows[_Rows];

	public:
		mMat() = default;
		mMat(const Matrix&) = default;

		mMat(std::initializer_list<std::initializer_list<float>> initVals)
		{
			set(initVals);
		}

		mMat(float scalar)
		{
			set(scalar);
		}

	public:
		// Access Operators
		Row& operator[] (uint64_t i)
		{
			mAssert(i < _Rows);

			return rows[i];
		}
		const Row& operator[] (uint64_t i) const
		{
			mAssert(i < _Rows);

			return rows[i];
		}

		Row operator() (uint64_t i) const
		{
			mAssert(i < _Rows);

			return rows[i];
		}

		// Math Operators
		constexpr Matrix operator+(const Matrix& other) const
		{
			Matrix result(0.0f);
			for (uint64_t i = 0; i < _Rows; i++)
				result[i] = (*this)[i] + other[i];

			return result;
		}

		template<uint64_t oRows, uint64_t oCols>
		mMat<_Rows, oCols> operator*(const mMat<oRows, oCols>& other)
		{
			mAssert(_Columns == oRows);

			mMat<_Rows, oCols> result(0.0f);
			for (uint64_t i = 0; i < _Rows; i++)
			{
				for (uint64_t j = 0; j < oCols; j++)
				{
					for (uint64_t k = 0; k < _Columns; k++)
					{
						result[i][j] += ((*this)[i][k] * other[k][j]);
					}
				}
			}

			return result;
		}
		mVec<_Rows> operator*(const mVec<_Rows>& other)
		{
			mAssert(_Rows == _Columns);

			mVec<_Rows> result(0.0f);
			for (uint64_t i = 0; i < _Rows; i++)
			{
				for (uint64_t j = 0; j < _Columns; j++)
				{
					result[i] += ((*this)[i][j] * other[j]);
				}
			}

			return result;
		}

		Matrix& operator*=(float scalar)
		{
			for (uint64_t i = 0; i < _Rows; i++)
				this->rows[i] *= scalar;

			return *this;
		}
		Matrix operator*(float scalar)
		{
			Matrix result;
			for (uint64_t i = 0; i < _Rows; i++)
				result[i] = this->rows[i] * scalar;

			return result;
		}
		friend Matrix operator*(float lhs, Matrix rhs)
		{
			rhs *= lhs;
			return rhs;
		}

		Matrix& operator/=(float scalar)
		{
			for (uint64_t i = 0; i < _Rows; i++)
				this->rows[i] /= scalar;

			return *this;
		}
		Matrix operator/(float scalar)
		{
			Matrix result;
			for (uint64_t i = 0; i < _Rows; i++)
				result[i] = this->rows[i] / scalar;

			return result;
		}

		// IOStream Operators
		friend std::ostream& operator<<(std::ostream& out, const Matrix& mat)
		{
			for (uint64_t i = 0; i < _Rows; i++)
				out << mat(i) << "\n";

			return out;
		}
	public:
		constexpr uint64_t rowCount() const noexcept
		{
			return _Rows;
		}
		constexpr uint64_t colCount() const noexcept
		{
			return _Columns;
		}

		mVec<_Rows> toVec(uint64_t col = 0) const
		{
			mVec<_Rows> result;
			for (uint64_t i = 0; i < _Rows; i++)
				result[i] = rows[i][col];

			return result;
		}

		void set(std::initializer_list<std::initializer_list<float>> vals)
		{
			uint64_t i = 0;
			for (auto row : vals)
			{
				if (i == _Rows) break;

				rows[i].set(row);
				i++;
			}

			uint64_t initSize = vals.size();
			for (uint64_t k = initSize; k < _Rows; k++)
				rows[k].zero();
		}
		void set(float scalar)
		{
			for (uint64_t i = 0; i < _Rows; i++)
				rows[i].set(scalar);
		}

		constexpr void zero()
		{
			for (uint64_t i = 0; i < _Rows; i++)
				rows[i].setZero();
		}
	};

	using mMat22 = mMat<2, 2>;
	using mMat33 = mMat<3, 3>;

}