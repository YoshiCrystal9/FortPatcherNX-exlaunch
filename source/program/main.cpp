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

namespace nn::diag::detail {
    void LogImpl();
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