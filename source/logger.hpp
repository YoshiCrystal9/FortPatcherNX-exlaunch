#include <mutex>
#include <source_location>
#include <cstdarg>

namespace logger {

    namespace impl {
        extern nn::os::MutexType s_LogMutex;
        extern char s_FormatBuffer[500];

        void Log(const char* text, size_t length);
    }

    void Initialize();
    void Finalize();

    template<typename... Args>
    void LogUnsafe(const char* fmt, Args &&... args) {
        /* Format string, passing the templated arguments. */
        auto length = std::snprintf(impl::s_FormatBuffer, sizeof(impl::s_FormatBuffer), fmt, std::forward<Args>(args)...);
        /* Call underlying implementation. */
        impl::Log(impl::s_FormatBuffer, length);
    }

    inline void VLogUnsafe(const char* fmt, va_list args) {
        /* Format string, passing va list. */
        auto length = std::vsnprintf(impl::s_FormatBuffer, sizeof(impl::s_FormatBuffer), fmt, args);
        /* Call underlying implementation. */
        impl::Log(impl::s_FormatBuffer, length);
    }

    template<typename... Args>
    void Log(const char* fmt, Args &&... args) {
        /* Acquire lock to ensure thread safety. */
        nn::os::LockMutex(&impl::s_LogMutex);
        LogUnsafe(fmt, std::forward<Args>(args)...);
        nn::os::UnlockMutex(&impl::s_LogMutex);
    }

    inline void VLog(const char* fmt, va_list args) {
        nn::os::LockMutex(&impl::s_LogMutex);
        VLogUnsafe(fmt, args);
        nn::os::UnlockMutex(&impl::s_LogMutex);
    }
}



