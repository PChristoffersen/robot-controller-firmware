from typing import Tuple,List,Optional, ClassVar
from enum import Enum
import struct
import ctypes
import hidutil


class State(ctypes.LittleEndianStructure):
    _fields_ = [
        ('soft_input', ctypes.c_uint8, 8)
    ]

class ModeConfig(ctypes.LittleEndianStructure):
    _fields_ = [
        ("active_time",  ctypes.c_uint16, 16),
        ("passive_time", ctypes.c_uint16, 16),
        ("rise_time",    ctypes.c_uint16, 16),
        ("fall_time",    ctypes.c_uint16, 16),
    ]

class OutputConfigEntry(ctypes.LittleEndianStructure):
    _fields_ = [
        ("enable_inv"   , ctypes.c_bool,  1),
        ("_pack1"       , ctypes.c_uint8, 2),
        ("enable"       , ctypes.c_uint8, 5),
        ("active_inv"   , ctypes.c_bool,  1),
        ("_pack2"       , ctypes.c_uint8, 2),
        ("active"       , ctypes.c_uint8, 5),
        ("active_mode"  , ctypes.c_uint8, 4),
        ("passive_mode" , ctypes.c_uint8, 4),
        ("primary_lut"   , ctypes.c_uint8, 4),
        ("secondary_lut"  , ctypes.c_uint8, 4),
    ]
    ENTRIES_PER_OUTPUT = 4

OutputConfig = OutputConfigEntry*OutputConfigEntry.ENTRIES_PER_OUTPUT


class ColorLUTEntry(ctypes.LittleEndianStructure):
    _fields_ = [
        ("red",   ctypes.c_uint8, 8),
        ("green", ctypes.c_uint8, 8),
        ("blue",  ctypes.c_uint8, 8),
    ]

class BrightnessLUTEntry(ctypes.LittleEndianStructure):
    _fields_ = [
        ("brightness", ctypes.c_uint16, 16)
    ]


class Input(Enum):
    TRUE        = 0
    ANO_SW1     = 1
    ANO_SW2     = 2
    ANO_SW3     = 3
    ANO_SW4     = 4
    ANO_SW5     = 5
    EXT_IN1     = 6
    EXT_IN2     = 7
    BUTTON1     = 8
    BUTTON2     = 9
    BUTTON3     = 10
    BUTTON4     = 11
    BUTTON5     = 12
    BUTTON6     = 13
    BUTTON7     = 14
    BUTTON8     = 15
    USB_LED1    = 16
    USB_LED2    = 17
    USB_LED3    = 18
    USB_LED4    = 19
    USB_LED5    = 20
    USB_LED6    = 21
    USB_LED7    = 22
    USB_LED8    = 23
    SOFTWARE1   = 24
    SOFTWARE2   = 25
    SOFTWARE3   = 26
    SOFTWARE4   = 27
    SOFTWARE5   = 28
    SOFTWARE6   = 29
    SOFTWARE7   = 30
    SOFTWARE8   = 31


class Output(Enum):
    LED1        = 0
    LED2        = 1
    LED3        = 2
    LED4        = 3
    NEOPIXEL1   = 4
    NEOPIXEL2   = 5
    NEOPIXEL3   = 6
    NEOPIXEL4   = 7
    EXT_OUT1    = 8
    EXT_OUT2    = 9



class FeatureId(Enum):
    DEFAULT = 1
    OUTPUT_CONFIGS = 2
    MODE_CONFIG = 3
    COLOR_LUT = 4
    BRIGHTNESS_LUT = 5


class Controller:
    COLOR_FORMAT = '<BBB'
    BRIGHTNESS_FORMAT = '<H'

    def __init__(self, dev: hidutil.Device):
        self._device = dev
        self._color_lut = None
        self._color_lut_buffer = None
        self._brightness_lut = None
        self._brightness_lut_buffer = None
        self._output_configs = None
        self._output_configs_buffer = None
        self._state = None
        self._state_buffer = None
        self._mode_configs = None
        self._mode_configs_buffer = None

    @property
    def state(self) -> State:
        if not self._state_buffer:
            self._state_buffer = self._device.get_feature_report(FeatureId.DEFAULT.value)
            self._state = State.from_buffer(self._state)
        return self._state

    def update_state(self):
        if self._state_buffer:
            self._device.set_feature_report(FeatureId.DEFAULT.value, self._state_buffer)


    @property
    def mode_configs(self) -> List[ModeConfig]:
        if not self._mode_configs_buffer:
            sz = ctypes.sizeof(ModeConfig)
            self._mode_configs_buffer = self._device.get_feature_report(FeatureId.MODE_CONFIG.value)
            self._mode_configs = [ModeConfig.from_buffer(self._mode_configs_buffer, offset) for offset in range(0, len(self._mode_configs_buffer), sz)]
        return self._mode_configs

    def update_mode_configs(self):
        if self._mode_configs_buffer:
            self._device.set_feature_report(FeatureId.MODE_CONFIG.value, self._mode_configs_buffer)


    @property
    def output_configs(self) -> List[OutputConfig]:
        if not self._output_configs_buffer:
            sz = ctypes.sizeof(OutputConfig)
            self._output_configs_buffer = self._device.get_feature_report(FeatureId.OUTPUT_CONFIGS.value)
            self._output_configs = [OutputConfig.from_buffer(self._output_configs_buffer, offset) for offset in range(0, len(self._output_configs_buffer), sz)]
            print([offset for offset in range(0, len(self._output_configs_buffer), sz)])
        return self._output_configs
    
    def update_output_configs(self):
        if self._output_configs_buffer:
            self._device.set_feature_report(FeatureId.OUTPUT_CONFIGS.value, self._output_configs_buffer)


    @property
    def color_lut(self) -> List[ColorLUTEntry]:
        if not self._color_lut_buffer:
            sz = ctypes.sizeof(ColorLUTEntry)
            self._color_lut_buffer = self._device.get_feature_report(FeatureId.COLOR_LUT.value)
            self._color_lut = [ColorLUTEntry.from_buffer(self._color_lut_buffer, offset) for offset in range(0, len(self._color_lut_buffer), sz)]
        return self._color_lut

    def update_color_lut(self):
        if self._color_lut_buffer:
            self._device.set_feature_report(FeatureId.COLOR_LUT.value, self._color_lut_buffer)


    @property
    def brightness_lut(self) -> List[BrightnessLUTEntry]:
        if not self._brightness_lut_buffer:
            sz = ctypes.sizeof(BrightnessLUTEntry)
            self._brightness_lut_buffer = self._device.get_feature_report(FeatureId.BRIGHTNESS_LUT.value)
            self._brightness_lut = [BrightnessLUTEntry.from_buffer(self._brightness_lut_buffer, offset) for offset in range(0, len(self._brightness_lut_buffer), sz)]
        return self._brightness_lut

    def update_brightness_lut(self):
        if self._brightness_lut_buffer:
            self._device.set_feature_report(FeatureId.BRIGHTNESS_LUT.value, self._brightness_lut_buffer)

