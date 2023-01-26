#include <Arduino.h>
#undef abs
#include <libmaple/dma.h>
#include <algorithm>

#include <USBComposite.h>
#include <UniqueID.h>

#include <config.h>
#include <debug.h>
#include "config_store.h"
#include "leds.h"
#include "neopixels.h"
#include "buttons.h"
#include "analog_input.h"
#include "ano_dial.h"
#include "external_io.h"
#include "hid_device.h"
#include "feature_engine.h"
#include "watchdog.h"



static constexpr uint32 MIN_HID_REPORT_INTERVAL { 25 };
static constexpr uint32 MIN_HID_FEATURE_INTERVAL { 50 };
static USBHID HID;
static HIDDevice g_hid_device(HID);

static ConfigStore      g_config_store;
static Leds             g_leds;
static Neopixels        g_neopixels;
static Buttons          g_buttons;
static AnoDial          g_ano_dial;
static ExternalIO       g_external_io;
static AnalogInput      g_analog_input;
static Feature::Engine  g_feature_engine { g_leds, g_neopixels, g_external_io };


static constexpr uint8 ANO_BUTTON_START { 0 };
static constexpr uint8 BUTTON_START { 1 };


/* ------------------------------------------------------------------
 * Setup
 * ------------------------------------------------------------------ */


static void banner()
{
    Console.println("----------------------------");
    Console.println("Robot Controller");
    Console.print  ("  Serial: "); Debug.println(get_unique_id_string());
    Console.println("----------------------------");
    Console.flush();
}


void setup()
{
    afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);
    afio_remap(AFIO_REMAP_TIM2_FULL);       // TIMER2 -> PA15
    //afio_remap(AFIO_REMAP_TIM3_PARTIAL);    // TIMER3 -> PB4, PB5
    afio_remap(AFIO_REMAP_I2C1);            // I2C1 -> PB8, PB9
    dma_init(DMA1);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    //Debug.begin(115200);
    Console.begin(CONSOLE_BAUD);
    banner();

    Console.println();

    Console.print("Config init:        ");
    g_config_store.begin();
    Console.println("[done]");

    Console.print("Led init:           ");
    g_leds.begin();
    Console.println("[done]");

    Console.print("Neopixel init       ");
    g_neopixels.begin();
    Console.println("[done]");

    Console.print("ANO init            ");
    g_ano_dial.begin();
    Console.println("[done]");

    Console.print("Button init         ");
    g_buttons.begin();
    Console.println("[done]");

    Console.print("External IO init    ");
    g_external_io.begin();
    Console.println("[done]");

    Console.print("Analog input init   ");
    g_analog_input.begin();
    Console.println("[done]");

    Console.print("Feature engine init ");
    g_feature_engine.begin(g_config_store);
    Console.println("[done]");

    Console.print("USB init            ");
    USBComposite.setManufacturerString(USB_MANUFACTURER_STRING);
    USBComposite.setProductString(USB_PRODUCT_STRING);
    USBComposite.setVendorId(USB_VID);
    USBComposite.setProductId(USB_PID);
    USBComposite.setSerialString(get_unique_id_string());
    HID.begin(HIDDevice::descriptor(), HIDDevice::descriptor_size());
    g_hid_device.begin();
    g_hid_device.set_feature(HIDDevice::FEATURE_COMMAND,        Feature::COMMAND_NOOP);
    g_hid_device.set_feature(HIDDevice::FEATURE_STATE,          g_feature_engine.state());
    Console.println("[done]");

    Console.println();

    Console.println("Loading configs");
    if (!g_feature_engine.load_output_configs(g_config_store)) {
        Console.println("  Using default output configs");
    }
    g_hid_device.set_feature(HIDDevice::FEATURE_OUTPUT_CONFIGS, g_feature_engine.output_configs());
    if (!g_feature_engine.load_mode_configs(g_config_store)) {
        Console.println("  Using default mode configs");
    }
    g_hid_device.set_feature(HIDDevice::FEATURE_MODE_CONFIGS,   g_feature_engine.mode_configs());
    if (!g_feature_engine.load_color_lut(g_config_store)) {
        Console.println("  Using default color LUT");
    }
    g_hid_device.set_feature(HIDDevice::FEATURE_COLOR_LUT,      g_feature_engine.color_lut());
    if (!g_feature_engine.load_brightness_lut(g_config_store)) {
        Console.println("  Using default brightness LUT");
    }
    g_hid_device.set_feature(HIDDevice::FEATURE_BRIGHTNESS_LUT, g_feature_engine.brightness_lut());

    Console.println();

    #if ENABLE_WATCHDOG
    Console.println("Enabling watchdog");
    watchdog_init();
    #endif

    Console.println("Setup complete, starting main loop");
}



/* ------------------------------------------------------------------
 * Command processing
 * ------------------------------------------------------------------ */

static void command_config_load(Feature::command_arg arg)
{
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_OUTPUT_CONFIGS) {
        Console.println("Loading output configs");
        g_feature_engine.load_output_configs(g_config_store);
        g_hid_device.set_feature(HIDDevice::FEATURE_OUTPUT_CONFIGS, g_feature_engine.output_configs());
    }
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_MODE_CONFIGS) {
        Console.println("Loading mode configs");
        g_feature_engine.load_mode_configs(g_config_store);
        g_hid_device.set_feature(HIDDevice::FEATURE_MODE_CONFIGS,   g_feature_engine.mode_configs());
    }
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_COLOR_LUT) {
        Console.println("Loading color LUT");
        g_feature_engine.load_color_lut(g_config_store);
        g_hid_device.set_feature(HIDDevice::FEATURE_COLOR_LUT,      g_feature_engine.color_lut());
    }
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_BRIGHTNESS_LUT) {
        Console.println("Loading brightness LUT");
        g_feature_engine.load_brightness_lut(g_config_store);
        g_hid_device.set_feature(HIDDevice::FEATURE_BRIGHTNESS_LUT, g_feature_engine.brightness_lut());
    }
}

static void command_config_store(Feature::command_arg arg)
{
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_OUTPUT_CONFIGS) {
        Console.println("Saving output configs");
        g_config_store.store(g_feature_engine.output_configs());
    }
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_MODE_CONFIGS) {
        Console.println("Saving mode configs");
        g_config_store.store(g_feature_engine.mode_configs());
    }
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_COLOR_LUT) {
        Console.println("Saving color LUT");
        g_config_store.store(g_feature_engine.color_lut());
    }
    if (arg & Feature::CONFIG_LOAD_STORE_ARG_BRIGHTNESS_LUT) {
        Console.println("Saving brightness LUT");
        g_config_store.store(g_feature_engine.brightness_lut());
    }
}

static void command_config_erase(Feature::command_arg arg)
{
    Console.println("Erasing config store");
    g_config_store.erase();
    g_feature_engine.default_output_configs();
    g_feature_engine.default_mode_configs();
    g_feature_engine.default_color_lut();
    g_feature_engine.default_brightness_lut();
    g_hid_device.set_feature(HIDDevice::FEATURE_OUTPUT_CONFIGS, g_feature_engine.output_configs());
    g_hid_device.set_feature(HIDDevice::FEATURE_MODE_CONFIGS,   g_feature_engine.mode_configs());
    g_hid_device.set_feature(HIDDevice::FEATURE_COLOR_LUT,      g_feature_engine.color_lut());
    g_hid_device.set_feature(HIDDevice::FEATURE_BRIGHTNESS_LUT, g_feature_engine.brightness_lut());
}


static void process_command(Feature::Command &command) 
{
    switch (command.id) {
        case Feature::CommandId::NOOP:
            break;
        case Feature::CommandId::CONFIG_LOAD:
            command_config_load(command.arg);
            break;
        case Feature::CommandId::CONFIG_STORE:
            command_config_store(command.arg);
            break;
        case Feature::CommandId::CONFIG_ERASE:
            command_config_erase(command.arg);
            break;
    }
}




/* ------------------------------------------------------------------
 * Event processing
 * ------------------------------------------------------------------ */

static void update_neopixel()
{
    static constexpr uint32 MIN_SHOW_INTERVAL { 20 };
    static unsigned long last = 0;
    auto now = millis();

    if ((now-last)<MIN_SHOW_INTERVAL)
        return;

    if (g_neopixels.is_dirty()) {
        g_neopixels.show();
        last = now;
    }
}




static void update_ano() 
{
    static int last_pos = 0;
    static bool last_select = false;
    static auto last_hat = HIDDevice::HAT_NONE;

    auto pos = g_ano_dial.get_pos();
    if (pos!=last_pos) {
        g_hid_device.set_dial(pos-last_pos);
        last_pos = pos;
    }
    else {
        g_hid_device.set_dial(0);
    }

    bool select = g_ano_dial.get_switch(AnoDial::SW_CENTER);
    if (select != last_select) {
        constexpr uint16 mask { 1<<ANO_BUTTON_START };
        #if 0
        Debug.print("Select: ");
        Debug.println(select?"1":"_");
        #endif
        g_hid_device.set_buttons(select?mask:0, mask);
        last_select = select;
    }

    uint8 st = (g_ano_dial.get_switch(AnoDial::SW_UP)?1:0)<<0;
    st |= (g_ano_dial.get_switch(AnoDial::SW_DOWN)?1:0)<<1;
    st |= (g_ano_dial.get_switch(AnoDial::SW_LEFT)?1:0)<<2;
    st |= (g_ano_dial.get_switch(AnoDial::SW_RIGHT)?1:0)<<3;
    HIDDevice::hat_position hat = HIDDevice::HAT_NONE;
    switch (st) {
        case 0b0101:
            hat = HIDDevice::HAT_UP_LEFT;
            break;
        case 0b0001:
            hat = HIDDevice::HAT_UP;
            break;
        case 0b1001:
            hat = HIDDevice::HAT_UP_RIGHT;
            break;
        case 0b0100:
            hat = HIDDevice::HAT_LEFT;
            break;
        case 0b1000:
            hat = HIDDevice::HAT_RIGHT;
            break;
        case 0b0110:
            hat = HIDDevice::HAT_DOWN_LEFT;
            break;
        case 0b0010:
            hat = HIDDevice::HAT_DOWN;
            break;
        case 0b1010:
            hat = HIDDevice::HAT_DOWN_RIGHT;
            break;
        default:
            hat = HIDDevice::HAT_NONE;
            break;
    }
    if (hat!=last_hat) {
        #if 0
        DebugPrint("Hat: ");
        DebugPrint(st, BIN);
        DebugPrintLn("  ");
        DebugPrint((int)hat);
        DebugPrintLn();
        #endif
        g_hid_device.set_hat(hat);
        last_hat = hat;
    }
}


static void update_buttons()
{
    static constexpr uint32 MIN_UPDATE_INTERVAL { 10 };
    constexpr uint16 MASK { 0b11111111<<BUTTON_START };
    static uint32 last = 0;
    static uint16 last_state;
    uint32 now = millis();    

    if ((now-last)<MIN_UPDATE_INTERVAL) 
        return;
    last = now;

    uint16 state = 0x00;
    for (size_t sw=0; sw<g_buttons.count(); sw++) {
        bool pressed = g_buttons.get(static_cast<Buttons::Switch>(sw));
        if (pressed) {
            state |= 1<<sw;
        }
    }
    if (state!=last_state) {
        g_feature_engine.set_buttons(state);
        g_hid_device.set_buttons(state<<BUTTON_START, MASK);
        last_state = state;
    }
}


static void update_external()
{
    static constexpr uint32 MIN_UPDATE_INTERVAL { 10 };
    static uint32 last = 0;
    static bool in1_state = false;
    static bool in2_state = false;
    uint32 now = millis();    


    if ((now-last)<MIN_UPDATE_INTERVAL) 
        return;
    last = now;

    auto in1 = g_external_io.get(ExternalIO::IN_1);
    auto in2 = g_external_io.get(ExternalIO::IN_2);

    #if 0
    static bool state = false;
    g_external_io.put(ExternalIO::OUT_1, state);
    g_external_io.put(ExternalIO::OUT_2, !state);
    state = !state;
    #endif

    #if 0
    static uint32 last2 = 0;
    static uint8 state = 0;
    if ((now-last2)>1) {
        switch (state) {
            case 0:
                g_external_io.put(ExternalIO::OUT_1, true);
                break;
            case 1:
                g_external_io.put(ExternalIO::OUT_2, true);
                break;
            case 2:
                g_external_io.put(ExternalIO::OUT_1, false);
                break;
            case 3:
                g_external_io.put(ExternalIO::OUT_2, false);
                break;

        }
        last2 = now;
        state++;
        if (state>3) state = 0;

    }
    #endif


    if (in1!=in1_state || in2!=in2_state) {
        Debug.print(now);
        Debug.print(" EXTIN: ");
        Debug.print(in1?"1":"_");
        Debug.print(in2?"1":"_");
        Debug.println();

        g_leds.set(Leds::LED_1, in1?0xFFFF:0x0000);
        g_leds.set(Leds::LED_2, in2?0xFFFF:0x0000);

        in1_state = in1;
        in2_state = in2;
    }


}



static void update_analog_input()
{
    static constexpr AnalogInput::axis_value AXIS_CHANGE_THRESHOLD { 8 };
    static uint32 last_count = 0;
    static AnalogInput::axis_value last_values[g_analog_input.count()];
    bool changed = false;

    uint32 count = g_analog_input.data_count();
    if (count!=last_count) {
        for (size_t i=0; i<g_analog_input.count(); i++) {
            auto val = g_analog_input.get(i);
            if (val!=last_values[i]) {
                changed |= std::abs(last_values[i]-val)>AXIS_CHANGE_THRESHOLD;
                g_hid_device.setAxis(static_cast<HIDDevice::axis_type>(i), val);
                last_values[i] = val;
            }
        }

        last_count = count;
    }
}


static void update_usb_features()
{
    static unsigned long last = 0;
    auto now = millis();

    if ((now-last)>MIN_HID_FEATURE_INTERVAL) {
        uint16 res;
        HIDDevice::OutputReport output;
        if ( (res=g_hid_device.read_output(output)) ) {
            DebugPrint("Output: ");
            DebugPrint(res);
            DebugPrint(" |");
            for (size_t i=0; i<sizeof(output.leds); i++) {
                DebugPrint((output.leds&(1<<i))?"1":"_");
            }
            DebugPrint("|");
            DebugPrintLn();
            g_feature_engine.set_usb_leds(output.leds);
        }

        Feature::Command command;
        if ( (res=g_hid_device.read_feature(HIDDevice::FEATURE_COMMAND, command)) ) {
            DebugPrint("Command ");
            DebugPrintLn(res);
            process_command(command);
            g_hid_device.set_feature(HIDDevice::FEATURE_COMMAND, Feature::COMMAND_NOOP);
        }
        if ( (res=g_hid_device.read_feature(HIDDevice::FEATURE_STATE, g_feature_engine.output_configs())) ) {
            DebugPrint("OutputConfigs ");
            DebugPrintLn(res);
            g_feature_engine.dirty_output_configs();
        }
        if ( (res=g_hid_device.read_feature(HIDDevice::FEATURE_OUTPUT_CONFIGS, g_feature_engine.output_configs())) ) {
            DebugPrint("OutputConfigs ");
            DebugPrintLn(res);
            g_feature_engine.dirty_output_configs();
        }
        if ( (res=g_hid_device.read_feature(HIDDevice::FEATURE_MODE_CONFIGS, g_feature_engine.mode_configs())) ) {
            DebugPrint("ModeConfigs ");
            DebugPrintLn(res);
            g_feature_engine.dirty_mode_configs();
        }
        if ( (res=g_hid_device.read_feature(HIDDevice::FEATURE_COLOR_LUT, g_feature_engine.color_lut())) ) {
            DebugPrint("ColorLUT ");
            DebugPrintLn(res);
            g_feature_engine.dirty_color_lut();
        }
        if ( (res=g_hid_device.read_feature(HIDDevice::FEATURE_BRIGHTNESS_LUT, g_feature_engine.brightness_lut())) ) {
            DebugPrint("BrightnessLUT ");
            DebugPrintLn(res);
            g_feature_engine.dirty_brightness_lut();
        }


        last = now;
    }
}



static void print_report()
{
    // Debug
    static HIDDevice::Report last_report;
    const HIDDevice::Report &report = g_hid_device.report();

    if (report.buttons!=last_report.buttons || report.dial!=last_report.dial || report.hat!=last_report.hat) {
        Debug.print("|");
        for (uint8 i=0; i<sizeof(report.buttons)*8; i++) {
            Debug.print((report.buttons&(1<<i))?"1":"_");
        }
        Debug.print("|");
        Debug.print(report.hat);
        Debug.print("|");
        Debug.print(report.dial);
        Debug.print("|");
        Debug.println();

        last_report = report;
    }
}

static void update_usb()
{
    static unsigned long last = 0;
    auto now = millis();

    if ((now-last)<MIN_HID_REPORT_INTERVAL)
        return;

    if (g_hid_device.is_report_pending()) {
        print_report();
        g_hid_device.send_report();
        last = now;
    }

}



static void update_features()
{
    g_feature_engine.update();
}


static void update_builtin_led()
{
    static uint32 last = 0;
    static bool state = true;
    auto now = millis();

    if ((now-last)>500) {
        state = !state;
        digitalWrite(LED_BUILTIN, state?HIGH:LOW);
        last = now;
    }
}




void loop() 
{
    update_ano();
    update_buttons();
    update_external();
    update_analog_input();
    update_usb();
    update_usb_features();
    update_features();

    update_neopixel();
    update_builtin_led();
    watchdog_tick();
}

