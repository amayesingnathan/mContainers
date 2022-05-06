#pragma once

#include <cstddef>
#include <cmath>
#include <cassert>
#include <cfloat>
#include <initializer_list>
#include <array>
#include <iostream>

#if !defined(NDEBUG)
#define M_DEBUG
#endif

#define M_NOT_USED(x) ((void)(x))
#define MAssert(A) assert(A)

#define	mMaxFloat		FLT_MAX
#define mEpsilon		FLT_EPSILON
#define mPi				3.14159265359f

#define	mSqrt(x)		sqrtf(x)
#define	mAtan2(y, x)	atan2f(y, x)
		
#define mEnableIf(...)	template<typename = std::enable_if<__VA_ARGS__>::type>

/// This function is used to ensure that a floating point number is not a NaN or infinity.
inline bool mIsValid(float x)
{
	return isfinite(x);
}