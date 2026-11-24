#pragma once

#include <cstddef>
#include <cstdint>

// Stub implementation of HookThings used by the gameserver.
// In a full implementation this would set up a MinHook hook.
inline void HookThings(uintptr_t address, void* fn, void** original)
{
    // Simple stub: if original pointer provided, store the original function pointer.
    if (original)
    {
        *original = fn; // not a real hook, just assign for compilation.
    }
    // No actual hooking performed.
}
