#include <cstdio>
#include <utility>
#include <nn.hpp>

static constinit char s_SdCardMount[] = "sd";
static constinit char s_LogFilePath[] = "sd:/log.txt";

static long s_LogFilePosition = 0;
static nn::fs::FileHandle s_LogFileHandle;

static constexpr bool s_Enabled = true;

#define CHECK_ENABLED()         \
        if constexpr (!s_Enabled) { \
            return;                 \
        }


namespace impl {
    nn::os::MutexType s_LogMutex;
    char s_FormatBuffer[500];

    void Log(const char* text, size_t length) {
        CHECK_ENABLED();
        /* Write text to file. */
        R_ABORT_UNLESS(nn::fs::WriteFile(s_LogFileHandle, s_LogFilePosition, text, length, nn::fs::WriteOption::Create(nn::fs::WriteOptionFlag_Flush)));
        /* Move forward position. */
        s_LogFilePosition += length;
    }
}

void Initialize() {
    CHECK_ENABLED();
    nn::os::InitializeMutex(&impl::s_LogMutex, false, 0);

    /* Mount SD card. */
    R_ABORT_UNLESS(nn::fs::MountSdCardForDebug(s_SdCardMount));

    /* Try create log file, ignore if it already exists. */
    auto r = nn::fs::CreateFile(s_LogFilePath, 0);
    if (r != 0x402) /* ResultAlreadyExists. */
        R_ABORT_UNLESS(r);

    /* Open log file. */
    R_ABORT_UNLESS(nn::fs::OpenFile(std::addressof(s_LogFileHandle), s_LogFilePath, nn::fs::OpenMode_All));

    /* Get size of log file, so we can set our position to the end of it. */
    R_ABORT_UNLESS(nn::fs::GetFileSize(std::addressof(s_LogFilePosition), s_LogFileHandle));
}

void Finalize() {
    CHECK_ENABLED();
    /* Flush, then close the log file. */
    R_ABORT_UNLESS(nn::fs::FlushFile(s_LogFileHandle));
    nn::fs::CloseFile(s_LogFileHandle);

    /* Unmount SD card. */
    nn::fs::Unmount(s_SdCardMount);

    nn::os::FinalizeMutex(&impl::s_LogMutex);
}