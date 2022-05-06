#pragma once

#include "mVector.h"

namespace mContainers {

	template<size_t _Rows, size_t _Columns>
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

		mMat(float scalar)
		{
			if (scalar == 0.0f)
			{
				for (size_t i = 0; i < _Rows; i++)
					rows[i].setZero();

				return;
			}

			for (size_t i = 0; i < _Rows; i++)
			{
				for (size_t j = 0; j < _Columns; j++)
				{
					rows[i][j] = scalar;
				}
			}
		}

	public:
		// Access Operators
		Row& operator[] (size_t i)
		{
			TAssert(i < _Rows);

			return rows[i];
		}
		const Row& operator[] (size_t i) const
		{
			TAssert(i < _Rows);

			return rows[i];
		}

		Row operator() (size_t i) const
		{
			TAssert(i < _Rows);

			return rows[i];
		}

		// Math Operators
		constexpr Matrix operator+(const Matrix& other) const
		{
			Matrix result(0.0f);
			for (size_t i = 0; i < _Rows; i++)
				result[i] = (*this)[i] + other[i];

			return result;
		}

		template<size_t oRows, size_t oCols>
		mMat<_Rows, oCols> operator*(const mMat<oRows, oCols>& other)
		{
			TAssert(_Columns == oRows);

			mMat<_Rows, oCols> result(0.0f);
			for (size_t i = 0; i < _Rows; i++)
			{
				for (size_t j = 0; j < oCols; j++)
				{
					for (size_t k = 0; k < _Columns; k++)
					{
						result[i][j] += ((*this)[i][k] * other[k][j]);
					}
				}
			}

			return result;
		}
		mVec<_Rows> operator*(const mVec<_Rows>& other)
		{
			TAssert(_Rows == _Columns);

			mVec<_Rows> result(0.0f);
			for (size_t i = 0; i < _Rows; i++)
			{
				for (size_t j = 0; j < _Columns; j++)
				{
					result[i] += ((*this)[i][j] * other[j]);
				}
			}

			return result;
		}

		Matrix& operator*=(float scalar)
		{
			for (size_t i = 0; i < _Rows; i++)
				this->rows[i] *= scalar;

			return *this;
		}
		Matrix operator*(float scalar)
		{
			Matrix result;
			for (size_t i = 0; i < _Rows; i++)
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
			for (size_t i = 0; i < _Rows; i++)
				this->rows[i] /= scalar;

			return *this;
		}
		Matrix operator/(float scalar)
		{
			Matrix result;
			for (size_t i = 0; i < _Rows; i++)
				result[i] = this->rows[i] / scalar;

			return result;
		}

		// IOStream Operators
		friend std::ostream& operator<<(std::ostream& out, const Matrix& mat)
		{
			for (size_t i = 0; i < _Rows; i++)
				out << mat(i) << "\n";

			return out;
		}
	public:
		constexpr size_t rowCount() const noexcept
		{
			return _Rows;
		}
		constexpr size_t colCount() const noexcept
		{
			return _Columns;
		}

		mVec<_Rows> toVec(size_t col = 0) const
		{
			mVec<_Rows> result;
			for (size_t i = 0; i < _Rows; i++)
				result[i] = rows[i][col];

			return result;
		}

		void set(const std::array<float, _Rows * _Columns>& vals)
		{
			for (size_t i = 0; i < _Rows; i++)
				this->rows[i] = vals[i];
		}
		void set(std::array<float, _Rows* _Columns>&& vals)
		{
			for (size_t i = 0; i < _Rows; i++)
				this->rows[i] = std::move(vals[i]);
		}

		constexpr void setZero()
		{
			for (size_t i = 0; i < _Rows; i++)
				this->rows[i].setZero();
		}
	};

	using mMat22 = mMat<2, 2>;
	using mMat33 = mMat<3, 3>;

}