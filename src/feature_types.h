#pragma once

#include <config.h>
#include <array>

namespace Feature {

    enum class Input : uint8 {
        TRUE,
        ANO_SW1,
        ANO_SW2,
        ANO_SW3,
        ANO_SW4,
        ANO_SW5,
        EXT_IN1,
        EXT_IN2,
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
        USB_LED6,
        USB_LED7,
        USB_LED8,
        SOFTWARE1,
        SOFTWARE2,
        SOFTWARE3,
        SOFTWARE4,
        SOFTWARE5,
        SOFTWARE6,
        SOFTWARE7,
        SOFTWARE8,
        _COUNT
    };
    static_assert(static_cast<uint8>(Input::_COUNT)==32);

    static constexpr uint8  INPUT_TRUE             { static_cast<uint8>(Input::TRUE) };
    static constexpr uint8  INPUT_ANO_FIRST        { static_cast<uint8>(Input::ANO_SW1) };
    static constexpr uint8  INPUT_ANO_LAST         { static_cast<uint8>(Input::ANO_SW5) };
    static constexpr uint8  INPUT_ANO_COUNT        { INPUT_ANO_LAST-INPUT_ANO_FIRST+1 };
    static constexpr uint32 INPUT_ANO_MASK         { ((1ul<<INPUT_ANO_COUNT)-1)<<INPUT_ANO_FIRST };
    static constexpr uint8  INPUT_EXT_FIRST        { static_cast<uint8>(Input::EXT_IN1) };
    static constexpr uint8  INPUT_EXT_LAST         { static_cast<uint8>(Input::EXT_IN2) };
    static constexpr uint8  INPUT_EXT_COUNT        { INPUT_EXT_LAST-INPUT_EXT_FIRST+1 };
    static constexpr uint32 INPUT_EXT_MASK         { ((1ul<<INPUT_EXT_COUNT)-1)<<INPUT_EXT_FIRST };
    static constexpr uint8  INPUT_BUTTON_FIRST     { static_cast<uint8>(Input::BUTTON1) };
    static constexpr uint8  INPUT_BUTTON_LAST      { static_cast<uint8>(Input::BUTTON8) };
    static constexpr uint8  INPUT_BUTTON_COUNT     { INPUT_BUTTON_LAST-INPUT_BUTTON_FIRST+1 };
    static constexpr uint32 INPUT_BUTTON_MASK      { ((1ul<<INPUT_BUTTON_COUNT)-1)<<INPUT_BUTTON_FIRST };
    static constexpr uint8  INPUT_USB_LED_FIRST    { static_cast<uint8>(Input::USB_LED1) };
    static constexpr uint8  INPUT_USB_LED_LAST     { static_cast<uint8>(Input::USB_LED8) };
    static constexpr uint8  INPUT_USB_LED_COUNT    { INPUT_USB_LED_LAST-INPUT_USB_LED_FIRST+1 };
    static constexpr uint32 INPUT_USB_LED_MASK     { ((1ul<<INPUT_USB_LED_COUNT)-1)<<INPUT_USB_LED_FIRST };
    static constexpr uint8  INPUT_SOFTWARE_FIRST   { static_cast<uint8>(Input::SOFTWARE1) };
    static constexpr uint8  INPUT_SOFTWARE_LAST    { static_cast<uint8>(Input::SOFTWARE8) };
    static constexpr uint8  INPUT_SOFTWARE_COUNT   { INPUT_SOFTWARE_LAST-INPUT_SOFTWARE_FIRST+1 };
    static constexpr uint32 INPUT_SOFTWARE_MASK    { ((1ul<<INPUT_SOFTWARE_COUNT)-1)<<INPUT_SOFTWARE_FIRST };

    typedef struct {
        uint8 soft_input;
    } __packed State;
    static constexpr size_t STATE_SIZE { sizeof(State) };
    static_assert(sizeof(State::soft_input)*8==INPUT_SOFTWARE_COUNT);

    enum class Output : uint8 {
        LED1,
        LED2,
        LED3,
        LED4,
        NEOPIXEL1,
        NEOPIXEL2,
        NEOPIXEL3,
        NEOPIXEL4,
        EXT_OUT1,
        EXT_OUT2,
        _COUNT
    };
    static constexpr size_t OUTPUT_COUNT           { static_cast<size_t>(Output::_COUNT) };
    static constexpr uint8 OUTPUT_LED_FIRST        { static_cast<uint8>(Output::LED1) };
    static constexpr uint8 OUTPUT_LED_LAST         { static_cast<uint8>(Output::LED4) };
    static constexpr uint8 OUTPUT_LED_COUNT        { OUTPUT_LED_LAST-OUTPUT_LED_FIRST+1 };
    static constexpr uint8 OUTPUT_NEOPIXEL_FIRST   { static_cast<uint8>(Output::NEOPIXEL1) };
    static constexpr uint8 OUTPUT_NEOPIXEL_LAST    { static_cast<uint8>(Output::NEOPIXEL4) };
    static constexpr uint8 OUTPUT_NEOPIXEL_COUNT   { OUTPUT_NEOPIXEL_LAST-OUTPUT_NEOPIXEL_FIRST+1 };
    static constexpr uint8 OUTPUT_EXT_FIRST        { static_cast<uint8>(Output::NEOPIXEL1) };
    static constexpr uint8 OUTPUT_EXT_LAST         { static_cast<uint8>(Output::NEOPIXEL4) };
    static constexpr uint8 OUTPUT_EXT_COUNT        { OUTPUT_EXT_LAST-OUTPUT_EXT_FIRST+1 };
    static_assert(OUTPUT_LED_COUNT==::LED_COUNT);
    static_assert(OUTPUT_NEOPIXEL_COUNT==::NEOPIXEL_COUNT);



    typedef struct {
        bool  enable_inv: 1;
        uint8 _pack1: 2;
        uint8 enable: 5;

        bool  active_inv: 1;
        bool  fallthrough: 1;
        uint8 _pack2: 1;
        uint8 active: 5;

        uint8 active_mode: 4;
        uint8 passive_mode: 4;
        uint8 active_lut: 4;
        uint8 passive_lut: 4;
    } __packed OutputConfigEntry;

    static constexpr size_t OUTPUT_CONFIG_ENTRIES_PER_OUTPUT { 4 };
    using OutputConfig = OutputConfigEntry[OUTPUT_CONFIG_ENTRIES_PER_OUTPUT];
    static constexpr size_t OUTPUT_CONFIG_SIZE { sizeof(OutputConfig) };
    static constexpr size_t OUTPUT_CONFIG_COUNT { static_cast<size_t>(Output::_COUNT) };
    using OutputConfigs = OutputConfig[OUTPUT_CONFIG_COUNT];
    static constexpr size_t OUTPUT_CONFIGS_SIZE { sizeof(OutputConfigs) };
    static_assert(OUTPUT_CONFIGS_SIZE==(OUTPUT_CONFIG_SIZE*OUTPUT_CONFIG_COUNT));


    typedef struct {
        uint16 active_time;
        uint16 passive_time;
        uint16 rise_time;
        uint16 fall_time;
    } __packed ModeConfig;

    static constexpr size_t MODE_CONFIG_SIZE { sizeof(ModeConfig) }; 
    static constexpr size_t MODE_CONFIG_COUNT { 16 }; 
    using ModeConfigs = ModeConfig[MODE_CONFIG_COUNT];
    static constexpr size_t MODE_CONFIGS_SIZE { sizeof(ModeConfigs) };
    static_assert(MODE_CONFIGS_SIZE==(MODE_CONFIG_SIZE*MODE_CONFIG_COUNT));



    static constexpr size_t LUT_COUNT { 16 };

    using ColorLUTEntry = uint32;
    static constexpr size_t COLOR_LUT_ENTRY_SIZE { sizeof(ColorLUTEntry) };
    static constexpr size_t COLOR_LUT_ENTRY_COUNT { LUT_COUNT };
    using ColorLUT = ColorLUTEntry[COLOR_LUT_ENTRY_COUNT];
    static constexpr size_t COLOR_LUT_SIZE { sizeof(ColorLUT) };

    using BrightnessLUTEntry = uint16;
    static constexpr size_t BRIGHTNESS_LUT_ENTRY_SIZE { sizeof(BrightnessLUTEntry) };
    static constexpr size_t BRIGHTNESS_LUT_ENTRY_COUNT { LUT_COUNT };
    using BrightnessLUT = BrightnessLUTEntry[BRIGHTNESS_LUT_ENTRY_COUNT];
    static constexpr size_t BRIGHTNESS_LUT_SIZE { sizeof(BrightnessLUT) };

};
