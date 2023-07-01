#include "lib.hpp"
#include "logger.cpp"

void main() {
    Logger::Initialize();
}

/* Define hook StubCopyright. Trampoline indicates the original function should be kept. */
/* HOOK_DEFINE_REPLACE can be used if the original function does not need to be kept. */
HOOK_DEFINE_TRAMPOLINE(LoggingThing) {

    /* Define the callback for when the function is called. Don't forget to make it static and name it Callback. */
    static void Callback(bool enabled) {

        /* Call the original function, with the argument always being false. */
        Orig(false);
    }

};

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
        VLog(fmt, args)
            nn::diag::detail::VLogImpl(meta, fmt, args);
        va_end(args);
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    exl::hook::Initialize();

    /* Install the hook at the provided function pointer. Function type is checked against the callback function. */
    LoggingThing::InstallAtFuncPtr(nn::diag::detail::LogImpl);

}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}