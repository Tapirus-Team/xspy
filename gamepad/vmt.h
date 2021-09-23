#pragma once


inline void** GetVMTBase(void* Object) { return *static_cast<void***>(Object); };
