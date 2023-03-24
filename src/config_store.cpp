#include "config_store.h"

#include <EEPROM.h>
#include <CRC32.h>

#include <config.h>
#include <debug.h>


using flash_value = ConfigStore::flash_value;
using checksum_type = uint32;

enum StorageId {
    SID_OUTPUT_CONFIGS,
    SID_MODE_CONFIGS,
    SID_BRIGHTNESS_LUT,
    SID_COLOR_LUT
};
static constexpr size_t STORAGE_SIZES[] {
    (Feature::OUTPUT_CONFIGS_SIZE)/sizeof(flash_value),
    (Feature::MODE_CONFIGS_SIZE)/sizeof(flash_value),
    (Feature::BRIGHTNESS_LUT_SIZE)/sizeof(flash_value),
    (Feature::COLOR_LUT_SIZE)/sizeof(flash_value),
};
static_assert((Feature::OUTPUT_CONFIGS_SIZE%sizeof(flash_value))==0);
static_assert((Feature::MODE_CONFIGS_SIZE%sizeof(flash_value))==0);
static_assert((Feature::BRIGHTNESS_LUT_SIZE%sizeof(flash_value))==0);
static_assert((Feature::COLOR_LUT_SIZE%sizeof(flash_value))==0);
static constexpr size_t STORAGE_OFFSETS[] {
    0x10,
    0x10+STORAGE_SIZES[0]+sizeof(checksum_type)/sizeof(flash_value),
    0x10+STORAGE_SIZES[0]+sizeof(checksum_type)/sizeof(flash_value) + STORAGE_SIZES[1]+sizeof(checksum_type)/sizeof(flash_value),
    0x10+STORAGE_SIZES[0]+sizeof(checksum_type)/sizeof(flash_value) + STORAGE_SIZES[1]+sizeof(checksum_type)/sizeof(flash_value) + STORAGE_SIZES[2]+sizeof(checksum_type)/sizeof(flash_value),
};



#if 0
void DisplayConfig(void)
{
	Debug.print  ("EEPROM.PageBase0 : 0x");
	Debug.println(EEPROM.PageBase0, HEX);
	Debug.print  ("EEPROM.PageBase1 : 0x");
	Debug.println(EEPROM.PageBase1, HEX);
	Debug.print  ("EEPROM.PageSize  : 0x");
	Debug.print  (EEPROM.PageSize, HEX);
	Debug.print  (" (");
	Debug.print  (EEPROM.PageSize, DEC);
	Debug.println(")");
}

void DisplayHex(uint16 value)
{
	if (value <= 0xF)
		Debug.print("000");
	else if (value <= 0xFF)
		Debug.print("00");
	else if (value <= 0xFFF)
		Debug.print("0");
	Debug.print(value, HEX);
}

void DisplayPages(uint32 endIndex)
{
	Debug.println("Page 0     Top         Page 1");

	for (uint32 idx = 0; idx < endIndex; idx += 4)
	{
		Debug.print  (EEPROM.PageBase0 + idx, HEX);
		Debug.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		Debug.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		Debug.print  ("    ");
		Debug.print  (EEPROM.PageBase1 + idx, HEX);
		Debug.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		Debug.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		Debug.println();
	}
}

void DisplayPagesEnd(uint32 endIndex)
{
	Debug.println("Page 0     Bottom      Page 1");

	for (uint32 idx = EEPROM.PageSize - endIndex; idx < EEPROM.PageSize; idx += 4)
	{
		Debug.print  (EEPROM.PageBase0 + idx, HEX);
		Debug.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		Debug.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		Debug.print  ("    ");
		Debug.print  (EEPROM.PageBase1 + idx, HEX);
		Debug.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		Debug.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		Debug.println();
	}
}
#endif


ConfigStore::ConfigStore()
{
}


void ConfigStore::begin()
{
	uint16 res;

    res = EEPROM.init();
    if (res!=EEPROM_OK) {
        Debug.println("Erasing storage");
        EEPROM.format();
    }
}


void ConfigStore::erase()
{
    EEPROM.format();
}



bool ConfigStore::load(Feature::OutputConfigs &feature)
{
    constexpr StorageId id = SID_OUTPUT_CONFIGS;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _load(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}

bool ConfigStore::store(Feature::OutputConfigs &feature)
{
    constexpr StorageId id = SID_OUTPUT_CONFIGS;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _store(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}


bool ConfigStore::load(Feature::ModeConfigs &feature)
{
    constexpr StorageId id = SID_MODE_CONFIGS;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _load(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}

bool ConfigStore::store(Feature::ModeConfigs &feature)
{
    constexpr StorageId id = SID_MODE_CONFIGS;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _store(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}


bool ConfigStore::load(Feature::ColorLUT &feature)
{
    constexpr StorageId id = SID_COLOR_LUT;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _load(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}

bool ConfigStore::store(Feature::ColorLUT &feature)
{
    constexpr StorageId id = SID_COLOR_LUT;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _store(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}


bool ConfigStore::load(Feature::BrightnessLUT &feature)
{
    constexpr StorageId id = SID_BRIGHTNESS_LUT;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _load(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}

bool ConfigStore::store(Feature::BrightnessLUT &feature)
{
    constexpr StorageId id = SID_BRIGHTNESS_LUT;
    static_assert(sizeof(feature)==STORAGE_SIZES[id]*sizeof(flash_value));
    return _store(STORAGE_OFFSETS[id], (flash_value*)&feature, STORAGE_SIZES[id]);
}


bool ConfigStore::_load(uint16 off, flash_value *data, uint16 size)
{
    flash_value value;
    uint16 res;
    CRC32 crc;

    #if 0
    Debug.print("Load: ");
    Debug.print(off);
    Debug.print(" ");
    Debug.println(size);
    #endif

    for (uint16 i=0; i<size; i++) {
        res = EEPROM.read(off+i, &value);
        if (res == EEPROM_BAD_ADDRESS) {
            #if 0
            Debug.print("Addr ");
            Debug.print(off+i);
            Debug.print(" ");
            Debug.print(res);
            Debug.println(" bad-add");
            #endif
            data[i] = 0xFFFF;
        }
        else if (res != EEPROM_OK) {
            #if 0
            Debug.print("Addr ");
            Debug.print(off+i);
            Debug.print(" ");
            Debug.print(res);
            Debug.println(" fail");
            #endif
            return false;
        }
        else {
            #if 0
            Debug.print("Addr ");
            Debug.print(off+i);
            Debug.print(" ");
            Debug.print(res);
            Debug.println(" ok");
            #endif
            data[i] = value;
        }
        crc.update(value);
    }
    uint32 checksum = crc.finalize();
    uint32 expect = static_cast<uint32>(EEPROM.read(off+size))<<16 | EEPROM.read(off+size+1);
    #if 0
    Debug.print("CRC ");
    Debug.print(checksum, HEX);
    Debug.print(" ");
    Debug.print(expect, HEX);
    Debug.println();
    #endif
    return checksum == expect;
}

bool ConfigStore::_store(uint16 off, flash_value *data, uint16 size)
{
    uint16 res;
    for (uint16 i=0; i<size; i++) {
        res = EEPROM.update(off+i, data[i]);
        if (res!=EEPROM_SAME_VALUE && res!=EEPROM_OK) {
            #if 0
            Debug.print("Addr ");
            Debug.print(off+i);
            Debug.print(" ");
            Debug.print(res);
            Debug.println(" fail");
            #endif
            return false;
        }
        else {
            #if 0
            Debug.print("Addr ");
            Debug.print(off+i);
            Debug.print(" ");
            Debug.print(res);
            Debug.println(" ok");
            #endif
        }
    }
    CRC32 crc;
    crc.update(data, size);
    uint32 checksum = crc.finalize();
    EEPROM.update(off+size, (checksum>>16) & 0xFFFF);
    EEPROM.update(off+size+1, (checksum) & 0xFFFF);

    return true;
}
