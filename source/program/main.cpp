#include "lib.hpp"
#include "logger.cpp"

namespace nn::diag {

    struct LogMetaData {
        int mLineNumber;
        const char* mFileName;
        const char* mFunctionName;
        const char* mModuleName;
        int field_20;
        int field_24;
        char field_28;
        int field_2C;
    };

    namespace detail {
        void LogImpl(LogMetaData const&, char const*, ...);
        void VLogImpl(LogMetaData const&, char const*, va_list);
    }
}

HOOK_DEFINE_REPLACE(LoggingHook) {
    static void Callback(nn::diag::LogMetaData const& meta, char const* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        //logger::Log("%s:%s:%s:%d\t", meta.mModuleName, meta.mFileName, meta.mFunctionName, meta.mLineNumber);
        logger::VLog(fmt, args);
        nn::diag::detail::VLogImpl(meta, fmt, args);
        va_end(args);
    }
};

HOOK_DEFINE_TRAMPOLINE(MountRom) {
    static Result Callback(const char* mount, void* cache, ulong cacheSize) {
        logger::Initialize();
        LoggingHook::InstallAtPtr(reinterpret_cast<uintptr_t>(&nn::diag::detail::LogImpl));
        return Orig(mount, cache, cacheSize);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    exl::hook::Initialize();
    MountRom::InstallAtFuncPtr(nn::fs::MountRom); 
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}