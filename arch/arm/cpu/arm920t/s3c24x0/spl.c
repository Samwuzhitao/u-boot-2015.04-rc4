#include <common.h>
#include <spl.h>

u32 spl_boot_device(void)
{
    return BOOT_DEVICE_NAND;
}
