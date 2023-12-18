#include "lib.hpp"
#include "logger.cpp"

/*
 * const void* memmem(const void* haystack, size_t haystack_len, const void* needle, size_t needle_len) {
    if (needle_len == 0) {
        return haystack;
    }

    const char* hay = static_cast<const char*>(haystack);
    const char* ndl = static_cast<const char*>(needle);

    for (size_t i = 0; i <= haystack_len - needle_len; ++i) {
        if (std::memcmp(hay + i, ndl, needle_len) == 0) {
            return hay + i;
        }
    }

    return nullptr;
}

static constexpr char stringToFind[] = "NativePlatformService";
static constexpr char stringToWrite[] = "farts";

static void PatchWhateverString() {
    const auto& rodata = exl::util::GetMainModuleInfo().m_Rodata;
    auto foundNativePlatform = memmem(
        reinterpret_cast<void*>(rodata.m_Start),    // haystack
        rodata.m_Size,                              // haystack size
        stringToFind,                               // needle
        sizeof(stringToFind)                        // needle size
);
    exl::patch::RandomAccessPatcher p{};
    auto offset = p.AddrFromRoPointer(foundNativePlatform);
    auto dest = reinterpret_cast<void*>(p.RwFromAddr(offset));
    memcpy(dest, stringToWrite, sizeof(stringToWrite));
}
*/

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
        //logger::Log("%s:%s:%s:%d\t", meta.mModuleName, meta.mFileName, meta.mFunctionName, meta.mLineNumber); //ONLY ENABLE THIS IF YOU WANT THE FILE NAME AND PATH LOGGED AS WELL
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
    //PatchWhateverString();
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}