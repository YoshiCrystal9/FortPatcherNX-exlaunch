#pragma once

#include "fs_types.hpp"

namespace nn::fs {

    /*
        Mount SD card. Must have explicit permission.
        mount: drive to mount to.
    */
    Result MountSdCardForDebug(char const* mount);

    /*
    Unmounts the given drive.
    mount: drive to unmount.
*/
    void Unmount(const char* mount);

    /*
    Mount the romfs.
    mount: drive to mount to.
    cache: memory to offer as a cache
    cacheSize: size of cache memory offered
*/
    Result MountRom(char const* mount, void* cache, ulong cacheSize);
};