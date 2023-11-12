/// Gets amount of free SRAM. Does not detect heap fragmentation - only use for debugging.

#ifndef FreeRAM_H
#define FreeRAM_H

#ifdef __cplusplus
extern "C" {
#endif

int getFreeRam();

#ifdef __cplusplus
}
#endif

#endif
