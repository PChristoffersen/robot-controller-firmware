from typing import Tuple,List,Optional, ClassVar
from enum import Enum
import struct
import ctypes
import hidutil


class State(ctypes.LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ('virtual_input', ctypes.c_uint16, 10),
        ('_pack1',        ctypes.c_uint16, 6),
    ]

class ModeConfig(ctypes.LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("active_time",  ctypes.c_uint16, 16),
        ("passive_time", ctypes.c_uint16, 16),
        ("rise_time",    ctypes.c_uint16, 16),
        ("fall_time",    ctypes.c_uint16, 16),
    ]

class OutputConfigEntry(ctypes.LittleEndianStructure):
    _pack_ = 1
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
    _pack_ = 1
    _fields_ = [
        ("red",   ctypes.c_uint8, 8),
        ("green", ctypes.c_uint8, 8),
        ("blue",  ctypes.c_uint8, 8),
    ]

class BrightnessLUTEntry(ctypes.LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("brightness", ctypes.c_uint16, 16)
    ]


class Command(ctypes.LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("id",  ctypes.c_uint8,   8),
        ("arg", ctypes.c_uint32, 32),
    ]

class CommandId(Enum):
    NOOP = 0
    CONFIG_LOAD  = 1
    CONFIG_STORE = 2
    CONFIG_ERASE = 3


class Input(Enum):
    FALSE       = 0
    TRUE        = 1
    EXT_IN1     = 2
    EXT_IN2     = 3
    ANO_SW1     = 4
    ANO_SW2     = 5
    ANO_SW3     = 6
    ANO_SW4     = 7
    ANO_SW5     = 8
    BUTTON1     = 9
    BUTTON2     = 10
    BUTTON3     = 11
    BUTTON4     = 12
    BUTTON5     = 13
    BUTTON6     = 14
    BUTTON7     = 15
    BUTTON8     = 16
    USB_LED1    = 17
    USB_LED2    = 18
    USB_LED3    = 19
    USB_LED4    = 20
    USB_LED5    = 21
    VIRTUAL1    = 22
    VIRTUAL2    = 23
    VIRTUAL3    = 24
    VIRTUAL4    = 25
    VIRTUAL5    = 26
    VIRTUAL6    = 27
    VIRTUAL7    = 28
    VIRTUAL8    = 29
    VIRTUAL9    = 30
    VIRTUAL10   = 31


class Output(Enum):
    EXT_OUT1    = 0
    EXT_OUT2    = 1
    LED1        = 2
    LED2        = 3
    LED3        = 4
    LED4        = 5
    NEOPIXEL1   = 6
    NEOPIXEL2   = 7
    NEOPIXEL3   = 8
    NEOPIXEL4   = 9


class Mode(Enum):
    MODE_0  = 0
    MODE_1  = 1
    MODE_2  = 2
    MODE_3  = 3
    MODE_4  = 4
    MODE_5  = 5
    MODE_6  = 6
    MODE_7  = 7
    MODE_8  = 8
    MODE_9  = 9
    MODE_10 = 10
    MODE_11 = 11
    MODE_12 = 12
    MODE_13 = 13
    MODE_14 = 14
    MODE_15 = 15

class LUT(Enum):
    LUT_0  = 0
    LUT_1  = 1
    LUT_2  = 2
    LUT_3  = 3
    LUT_4  = 4
    LUT_5  = 5
    LUT_6  = 6
    LUT_7  = 7
    LUT_8  = 8
    LUT_9  = 9
    LUT_10 = 10
    LUT_11 = 11
    LUT_12 = 12
    LUT_13 = 13
    LUT_14 = 14
    LUT_15 = 15


class FeatureId(Enum):
    COMMAND         = 1
    STATE           = 2
    OUTPUT_CONFIGS  = 3
    MODE_CONFIG     = 4
    COLOR_LUT       = 5
    BRIGHTNESS_LUT  = 6


class Controller:
    COLOR_FORMAT = '<BBB'
    BRIGHTNESS_FORMAT = '<H'

    def __init__(self, dev: hidutil.Device):
        self._device = dev
        self._mode_configs = None
        self._mode_configs_buffer = None
        self._output_configs = None
        self._output_configs_buffer = None
        self._color_lut = None
        self._color_lut_buffer = None
        self._brightness_lut = None
        self._brightness_lut_buffer = None
        self._state = None
        self._state_buffer = None


    def cmd_config_store(self, arg):
        self._command(CommandId.CONFIG_STORE, arg)

    def cmd_config_load(self, arg):
        self._command(CommandId.CONFIG_LOAD, arg)
        self._mode_configs = None
        self._mode_configs_buffer = None
        self._output_configs = None
        self._output_configs_buffer = None
        self._color_lut = None
        self._color_lut_buffer = None
        self._brightness_lut = None
        self._brightness_lut_buffer = None

    def cmd_config_erase(self):
        self._command(CommandId.CONFIG_ERASE, 0)
        self._mode_configs = None
        self._mode_configs_buffer = None
        self._output_configs = None
        self._output_configs_buffer = None
        self._color_lut = None
        self._color_lut_buffer = None
        self._brightness_lut = None
        self._brightness_lut_buffer = None


    def _command(self, command_id: CommandId, arg):
        cmd = Command()
        cmd.id = command_id.value
        cmd.arg = arg
        self._device.set_feature_report(FeatureId.COMMAND.value, bytearray(cmd))


    @property
    def state(self) -> State:
        if not self._state_buffer:
            self._state_buffer = self._device.get_feature_report(FeatureId.STATE.value)
            self._state = State.from_buffer(self._state)
        return self._state

    def update_state(self):
        if self._state_buffer:
            self._device.set_feature_report(FeatureId.STATE.value, self._state_buffer)


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

