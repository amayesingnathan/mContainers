#pragma once

#include "../mpch.h"

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

// Hash Table Default Parameters
#define DEFAULT_BUCKETS 7
#define LOAD_SCALE      2
#define MAX_BUCKET_SIZE 5
#define DEFAULT_SEED	64687421

//Client log macros
#define M_TRACE(...)			::mC::mLog::GetLogger()->trace(__VA_ARGS__)
#define M_INFO(...)				::mC::mLog::GetLogger()->info(__VA_ARGS__)
#define M_WARN(...)				::mC::mLog::GetLogger()->warn(__VA_ARGS__)
#define M_ERROR(...)			::mC::mLog::GetLogger()->error(__VA_ARGS__)
#define M_CRITICAL(...)			::mC::mLog::GetLogger()->critical(__VA_ARGS__)

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

namespace mC {

	class mLog
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetLogger() { return sLogger; }

	private:
		static std::shared_ptr<spdlog::logger> sLogger;
	};

	std::shared_ptr<spdlog::logger> mLog::sLogger;

	void mLog::Init()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("mContainers.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		sLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
		spdlog::register_logger(sLogger);
		sLogger->set_level(spdlog::level::trace);
		sLogger->flush_on(spdlog::level::trace);
	}

	class mTimer
	{
	public:
		mTimer()
		{
			reset();
		}

		void reset()
		{
			mStart = std::chrono::high_resolution_clock::now();
		}

		double elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * 0.001 * 0.001 * 0.001;
		}

		double elapsedMillis()
		{
			return elapsed() * 1000.0;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
	};

	/// This function is used to ensure that a floating point number is not a NaN or infinity.
	inline bool mIsValid(float x)
	{
		return std::isfinite(x);
	}

	class Memory
	{
	public:
		template<typename T>
		inline static T* Alloc(uint64_t size)
		{
			return reinterpret_cast<T*>(mAlloc(size * sizeof(T)));
		}
		template<typename T>
		inline static T* AllocDebug(uint64_t size)
		{
			return reinterpret_cast<T*>(mAllocDebug(size * sizeof(T)));
		}

		template<typename T>
		inline static void Free(T* data, uint64_t size)
		{
			mFree(data, size * sizeof(T));
		}
		template<typename T>
		inline static void FreeDebug(T* data, uint64_t size)
		{
			mFreeDebug(data, size * sizeof(T));
		}

		template<typename T, typename... Args>
		inline static T* Emplace(void* mem, Args&&... args)
		{
			return new (mem) T(std::forward<Args>(args)...);
		}

		template<typename T>
		inline static void SetZero(void* data, uint64_t size)
		{
			memset(data, 0, size * sizeof(T));
		}

	private:
		/// Implement this function to use your own memory allocator.
		inline static void* mAlloc(uint64_t size)
		{
			return mAlloc_Default(size);
		}

		// Memory allocators. Modify these to use your own allocator.
		inline static void* mAlloc_Default(uint64_t size)
		{
			return ::operator new(size);
		}
		inline static void* mAllocDebug(uint64_t size)
		{
			M_TRACE("Allocated {0} bytes of memory", size);
			return ::operator new(size);
		}

		/// If you implement mAlloc, you should also implement this function.
		inline static void mFree(void* mem, uint64_t size)
		{
			mFree_Default(mem, size);
		}

		inline static void mFree_Default(void* mem, uint64_t size)
		{
			::operator delete(mem, size);
		}
		inline static void mFreeDebug(void* mem, uint64_t size)
		{
			::operator delete(mem, size);
			M_TRACE("Freed {0} bytes of memory", size);
		}

	};

}