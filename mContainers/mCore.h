#pragma once

#include "mpch.h"

#if !defined(NDEBUG)
#define M_DEBUG
#endif

#if defined(_WIN32)
	#define M_PLATFORM_WINDOWS
#elif defined(__linux__)
	#define M_PLATFORM_LINUX
#endif

#ifdef M_DEBUG
	#if defined(M_PLATFORM_WINDOWS)
		#define M_DEBUGBREAK() __debugbreak()
	#elif defined(M_PLATFORM_LINUX)
		#include <signal.h>
	#define M_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif

	#define M_ENABLE_ASSERTS
#else
	#define M_DEBUGBREAK()
#endif

#define M_EXPAND_MACRO(x) x
#define M_STRINGIFY_MACRO(x) #x
#define M_NOT_USED(x) ((void)(x))

#define	mMaxFloat		FLT_MAX
#define mEpsilon		FLT_EPSILON
#define mPi				3.14159265359f

#define	mSqrt(x)		sqrtf(x)
#define	mAtan2(y, x)	atan2f(y, x)

//Client log macros
#define M_TRACE(...)			::mContainers::mLog::GetLogger()->trace(__VA_ARGS__)
#define M_INFO(...)				::mContainers::mLog::GetLogger()->info(__VA_ARGS__)
#define M_WARN(...)				::mContainers::mLog::GetLogger()->warn(__VA_ARGS__)
#define M_ERROR(...)			::mContainers::mLog::GetLogger()->error(__VA_ARGS__)
#define M_CRITICAL(...)			::mContainers::mLog::GetLogger()->critical(__VA_ARGS__)


#ifdef M_ENABLE_ASSERTS
	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define M_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { M_ERROR(msg, __VA_ARGS__); M_DEBUGBREAK(); } }
	#define M_INTERNAL_ASSERT_WITH_MSG(type, check, ...) M_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define M_INTERNAL_ASSERT_NO_MSG(type, check) M_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", M_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define M_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define M_INTERNAL_ASSERT_GET_MACRO(...) M_EXPAND_MACRO( M_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, M_INTERNAL_ASSERT_WITH_MSG, M_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define mAssert(...) M_EXPAND_MACRO( M_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define mStaticAssert(...) static_assert(__VA_ARGS__);
#else
	#define mAssert(...)
	#define mStaticAssert(...)
#endif

namespace mContainers {

	class mLog
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetLogger() { return sLogger; }

	private:
		static std::shared_ptr<spdlog::logger> sLogger;
	};

	/// This function is used to ensure that a floating point number is not a NaN or infinity.
	inline bool mIsValid(float x)
	{
		return std::isfinite(x);
	}
}