#include "UniqueID.h"
#include <Arduino.h>
#include <itoa.h>

#define UID_BASE              0x1FFFF7E8UL    /*!< Unique device ID register base address */

#define READ_REG(REG)         ((REG))


/**
  * @brief  Returns first word of the unique device identifier (UID based on 96 bits)
  * @retval Device identifier
  */
static uint32_t GetUIDw0(void)
{
   return(READ_REG(*((uint32_t *)UID_BASE)));
}

/**
  * @brief  Returns second word of the unique device identifier (UID based on 96 bits)
  * @retval Device identifier
  */
static uint32_t GetUIDw1(void)
{
   return(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
}

/**
  * @brief  Returns third word of the unique device identifier (UID based on 96 bits)
  * @retval Device identifier
  */
static uint32_t GetUIDw2(void)
{
   return(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
}


const char *get_unique_id_string()
{
    static char _uid_str[32] = { 0x00 };

    if (!_uid_str[0]) {
        char buf[9];
        _uid_str[0] = '\0';
        ultoa(GetUIDw0(), buf, HEX);
        strcat(_uid_str, buf);
        strcat(_uid_str, "-");
        ultoa(GetUIDw1(), buf, HEX);
        strcat(_uid_str, buf);
        strcat(_uid_str, "-");
        ultoa(GetUIDw2(), buf, HEX);
        strcat(_uid_str, buf);
    }

    return _uid_str;
}
