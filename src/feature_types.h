#pragma once

#include <config.h>
#include <array>

namespace Feature {

    enum class Input : uint8 {
        FALSE,
        TRUE,
        EXT_IN1,
        EXT_IN2,
        ANO_SW1,
        ANO_SW2,
        ANO_SW3,
        ANO_SW4,
        ANO_SW5,
        BUTTON1,
        BUTTON2,
        BUTTON3,
        BUTTON4,
        BUTTON5,
        BUTTON6,
        BUTTON7,
        BUTTON8,
        USB_LED1,
        USB_LED2,
        USB_LED3, 
        USB_LED4,
        USB_LED5,
        SOFT1,
        SOFT2,
        SOFT3,
        SOFT4,
        SOFT5,
        SOFT6,
        SOFT7,
        SOFT8,
        HOST_READY,
        _RESERVED1,
        _COUNT
    };
    static_assert(static_cast<uint8>(Input::_COUNT)==32);

    static constexpr uint8  INPUT_FALSE            { static_cast<uint8>(Input::FALSE) };
    static constexpr uint8  INPUT_TRUE             { static_cast<uint8>(Input::TRUE) };
    static constexpr uint8  INPUT_EXT_FIRST        { static_cast<uint8>(Input::EXT_IN1) };
    static constexpr uint8  INPUT_EXT_LAST         { static_cast<uint8>(Input::EXT_IN2) };
    static constexpr uint8  INPUT_EXT_COUNT        { INPUT_EXT_LAST-INPUT_EXT_FIRST+1 };
    static constexpr uint32 INPUT_EXT_MASK         { ((1ul<<INPUT_EXT_COUNT)-1)<<INPUT_EXT_FIRST };
    static constexpr uint8  INPUT_ANO_FIRST        { static_cast<uint8>(Input::ANO_SW1) };
    static constexpr uint8  INPUT_ANO_LAST         { static_cast<uint8>(Input::ANO_SW5) };
    static constexpr uint8  INPUT_ANO_COUNT        { INPUT_ANO_LAST-INPUT_ANO_FIRST+1 };
    static constexpr uint32 INPUT_ANO_MASK         { ((1ul<<INPUT_ANO_COUNT)-1)<<INPUT_ANO_FIRST };
    static constexpr uint8  INPUT_BUTTON_FIRST     { static_cast<uint8>(Input::BUTTON1) };
    static constexpr uint8  INPUT_BUTTON_LAST      { static_cast<uint8>(Input::BUTTON8) };
    static constexpr uint8  INPUT_BUTTON_COUNT     { INPUT_BUTTON_LAST-INPUT_BUTTON_FIRST+1 };
    static constexpr uint32 INPUT_BUTTON_MASK      { ((1ul<<INPUT_BUTTON_COUNT)-1)<<INPUT_BUTTON_FIRST };
    static constexpr uint8  INPUT_USB_LED_FIRST    { static_cast<uint8>(Input::USB_LED1) };
    static constexpr uint8  INPUT_USB_LED_LAST     { static_cast<uint8>(Input::USB_LED5) };
    static constexpr uint8  INPUT_USB_LED_COUNT    { INPUT_USB_LED_LAST-INPUT_USB_LED_FIRST+1 };
    static constexpr uint32 INPUT_USB_LED_MASK     { ((1ul<<INPUT_USB_LED_COUNT)-1)<<INPUT_USB_LED_FIRST };
    static constexpr uint8  INPUT_SOFT_FIRST       { static_cast<uint8>(Input::SOFT1) };
    static constexpr uint8  INPUT_SOFT_LAST        { static_cast<uint8>(Input::SOFT8) };
    static constexpr uint8  INPUT_SOFT_COUNT       { INPUT_SOFT_LAST-INPUT_SOFT_FIRST+1 };
    static constexpr uint32 INPUT_SOFT_MASK        { ((1ul<<INPUT_SOFT_COUNT)-1)<<INPUT_SOFT_FIRST };
    static constexpr uint8  INPUT_HOST_READY_FIRST { static_cast<uint8>(Input::HOST_READY) };
    static constexpr uint32 INPUT_HOST_READY_MASK  { 1ul<<INPUT_HOST_READY_FIRST };
    static_assert(INPUT_EXT_COUNT==::EXT_IN_COUNT);


    enum class CommandId : uint8 {
        NOOP,
        CONFIG_LOAD,
        CONFIG_STORE,
        CONFIG_ERASE,
    };
    static constexpr uint32 CONFIG_LOAD_STORE_ARG_OUTPUT_CONFIGS { 1<<0 };
    static constexpr uint32 CONFIG_LOAD_STORE_ARG_MODE_CONFIGS   { 1<<1 };
    static constexpr uint32 CONFIG_LOAD_STORE_ARG_COLOR_LUT      { 1<<2 };
    static constexpr uint32 CONFIG_LOAD_STORE_ARG_BRIGHTNESS_LUT { 1<<3 };
    static constexpr uint32 CONFIG_LOAD_STORE_ALL { CONFIG_LOAD_STORE_ARG_OUTPUT_CONFIGS | CONFIG_LOAD_STORE_ARG_MODE_CONFIGS | CONFIG_LOAD_STORE_ARG_COLOR_LUT | CONFIG_LOAD_STORE_ARG_BRIGHTNESS_LUT };

    using command_arg = uint32;
    struct __packed Command {
        CommandId id;
        command_arg arg;  
        constexpr Command() : id { CommandId::NOOP }, arg { 0 } {}
    };
    static constexpr size_t COMMAND_SIZE { sizeof(Command) };
    static constexpr Command COMMAND_NOOP {};


    using soft_input_type = uint8;
    static_assert(sizeof(soft_input_type)*8>=INPUT_SOFT_COUNT);
    struct __packed SoftInput {
        soft_input_type input : INPUT_SOFT_COUNT;
        //soft_input_type _pack1 : (8*sizeof(soft_input_type)-INPUT_SOFT_COUNT);
        SoftInput() :
            input { 0x00 }
        {}
    };
    static constexpr size_t SOFT_INPUT_SIZE { sizeof(SoftInput) };

    enum class Output : uint8 {
        EXT_OUT1,
        EXT_OUT2,
        LED1,
        LED2,
        LED3,
        LED4,
        NEOPIXEL1,
        NEOPIXEL2,
        NEOPIXEL3,
        NEOPIXEL4,
        _COUNT
    };
    static constexpr size_t OUTPUT_COUNT           { static_cast<size_t>(Output::_COUNT) };
    static constexpr uint8 OUTPUT_EXT_FIRST        { static_cast<uint8>(Output::EXT_OUT1) };
    static constexpr uint8 OUTPUT_EXT_LAST         { static_cast<uint8>(Output::EXT_OUT2) };
    static constexpr uint8 OUTPUT_EXT_COUNT        { OUTPUT_EXT_LAST-OUTPUT_EXT_FIRST+1 };
    static constexpr uint8 OUTPUT_LED_FIRST        { static_cast<uint8>(Output::LED1) };
    static constexpr uint8 OUTPUT_LED_LAST         { static_cast<uint8>(Output::LED4) };
    static constexpr uint8 OUTPUT_LED_COUNT        { OUTPUT_LED_LAST-OUTPUT_LED_FIRST+1 };
    static constexpr uint8 OUTPUT_NEOPIXEL_FIRST   { static_cast<uint8>(Output::NEOPIXEL1) };
    static constexpr uint8 OUTPUT_NEOPIXEL_LAST    { static_cast<uint8>(Output::NEOPIXEL4) };
    static constexpr uint8 OUTPUT_NEOPIXEL_COUNT   { OUTPUT_NEOPIXEL_LAST-OUTPUT_NEOPIXEL_FIRST+1 };
    static_assert(OUTPUT_EXT_COUNT==::EXT_OUT_COUNT);
    static_assert(OUTPUT_LED_COUNT==::LED_COUNT);
    static_assert(OUTPUT_NEOPIXEL_COUNT==::NEOPIXEL_COUNT);



    struct __packed OutputConfigEntry {
        uint8 _pack1: 2;
        bool  enable_inv: 1;
        uint8 enable: 5;

        uint8 _pack2: 2;
        bool  active_inv: 1;
        uint8 active: 5;

        uint8 active_mode: 4;
        uint8 passive_mode: 4;
        uint8 primary_lut: 4;
        uint8 secondary_lut: 4;
        
        OutputConfigEntry() : 
            enable_inv      { false },
            enable          { INPUT_FALSE },
            active_inv      { false },
            active          { INPUT_FALSE },
            active_mode     { 0 },
            passive_mode    { 0 },
            primary_lut     { 0 },
            secondary_lut   { 0 }
        {
        }
    };

    static constexpr size_t OUTPUT_CONFIG_ENTRIES_PER_OUTPUT { 4 };
    using OutputConfig = OutputConfigEntry[OUTPUT_CONFIG_ENTRIES_PER_OUTPUT];
    static constexpr size_t OUTPUT_CONFIG_SIZE { sizeof(OutputConfig) };
    static constexpr size_t OUTPUT_CONFIG_COUNT { static_cast<size_t>(Output::_COUNT) };
    using OutputConfigs = OutputConfig[OUTPUT_CONFIG_COUNT];
    static constexpr size_t OUTPUT_CONFIGS_SIZE { sizeof(OutputConfigs) };
    static_assert(OUTPUT_CONFIGS_SIZE==(OUTPUT_CONFIG_SIZE*OUTPUT_CONFIG_COUNT));


    struct __packed ModeConfig {
        uint16 active_time;
        uint16 passive_time;
        uint16 rise_time;
        uint16 fall_time;

        ModeConfig() :
            active_time     { 1000 },
            passive_time    { 0 },
            rise_time       { 0 },
            fall_time       { 0 }
        {
        }

        bool is_animated() const
        { 
            return rise_time!=0 || fall_time!=0 || (active_time!=0 && passive_time!=0);
        }
        uint32 cycle_time() const 
        {
            return rise_time+active_time+fall_time+passive_time;
        }
    };

    static constexpr size_t MODE_CONFIG_SIZE { sizeof(ModeConfig) }; 
    static constexpr size_t MODE_CONFIG_COUNT { 16 }; 
    using ModeConfigs = ModeConfig[MODE_CONFIG_COUNT];
    static constexpr size_t MODE_CONFIGS_SIZE { sizeof(ModeConfigs) };
    static_assert(MODE_CONFIGS_SIZE==(MODE_CONFIG_SIZE*MODE_CONFIG_COUNT));



    static constexpr size_t LUT_COUNT { 16 };

    struct __packed ColorLUTEntry {
        uint8 red;
        uint8 green;
        uint8 blue;
        ColorLUTEntry() :
            red     { 0x00 },
            green   { 0x00 },
            blue    { 0x00 }
        {}
        ColorLUTEntry(uint8 r, uint8 g, uint8 b) :
            red     { r },
            green   { g },
            blue    { b }
        {}
    };
    static constexpr size_t COLOR_LUT_ENTRY_SIZE { sizeof(ColorLUTEntry) };
    static constexpr size_t COLOR_LUT_ENTRY_COUNT { LUT_COUNT };
    using ColorLUT = ColorLUTEntry[COLOR_LUT_ENTRY_COUNT];
    static constexpr size_t COLOR_LUT_SIZE { sizeof(ColorLUT) };

    struct __packed BrightnessLUTEntry {
        uint16 brightness;
        BrightnessLUTEntry() : brightness { 0x0000 } {}
        BrightnessLUTEntry(uint16 b) : brightness { b } {}
        operator uint16() const { return brightness; }
    };
    static constexpr size_t BRIGHTNESS_LUT_ENTRY_SIZE { sizeof(BrightnessLUTEntry) };
    static constexpr size_t BRIGHTNESS_LUT_ENTRY_COUNT { LUT_COUNT };
    using BrightnessLUT = BrightnessLUTEntry[BRIGHTNESS_LUT_ENTRY_COUNT];
    static constexpr size_t BRIGHTNESS_LUT_SIZE { sizeof(BrightnessLUT) };

};
