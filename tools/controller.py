from typing import Tuple,List,Optional, ClassVar
from enum import Enum, IntEnum, IntFlag, auto
import struct
import ctypes
import hidutil


def _clamp(x,lim_l, lim_h): return min(max(x, lim_l), lim_h)

class _ControllerStructure(ctypes.LittleEndianStructure):
    def __setattr__(self, key, value):
        if not hasattr(self, key):
            raise AttributeError( "%r is not a known attribute." % key)
        object.__setattr__(self, key, value)
    
    @classmethod
    def fields(cls):
        return [field[0] for field in cls._fields_ if not field[0].startswith("_")]
    @classmethod
    def fields_ext(cls):
        return [(field[0],field[1],field[2]) for field in cls._fields_ if not field[0].startswith("_")]
    def __str__(self):
        fields = ", ".join(f"{field[0]}={getattr(self,field[0])}" for field in self._fields_ if not field[0].startswith("_"))
        return "<" + fields + ">"
    def __repr__(self):
        return str(self.__class__.__name__) + str(self)


class _ControllerIntFlag(IntFlag):
    @classmethod
    def list(cls):
        return list(map(lambda c: c.name.lower(), cls))
    @classmethod
    def from_string(cls, s: str):
        return cls[s.upper()]
    @classmethod
    def from_list(cls, values: List[str]) -> IntFlag:
        res = 0
        for val in values:
            res |= cls.from_string(val)
        return res

class _ControllerIntEnum(IntEnum):
    @classmethod
    def list(cls):
        return list(map(lambda c: c.name.lower(), cls))
    @classmethod
    def from_string(cls, s: str):
        return cls[s.upper()]
    @classmethod
    def from_list(cls, values: List[str]) -> IntFlag:
        res = 0
        for val in values:
            res |= cls.from_string(val)
        return res



class SoftInputState(_ControllerStructure):
    _pack_ = 1
    _fields_ = [
        ('input',  ctypes.c_uint16, 10),
        ('_pack1', ctypes.c_uint16, 6),
    ]

class SoftInput(_ControllerIntFlag):
    IN1 = auto()
    IN2 = auto()
    IN3 = auto()
    IN4 = auto()
    IN5 = auto()
    IN6 = auto()
    IN7 = auto()
    IN8 = auto()
    IN9 = auto()
    IN10 = auto()


class ModeConfig(_ControllerStructure):
    _pack_ = 1
    _fields_ = [
        ("active_time",  ctypes.c_uint16, 16),
        ("passive_time", ctypes.c_uint16, 16),
        ("rise_time",    ctypes.c_uint16, 16),
        ("fall_time",    ctypes.c_uint16, 16),
    ]

class OutputConfigEntry(_ControllerStructure):
    _pack_ = 1
    _fields_ = [
        ("_pack1"       , ctypes.c_uint8, 2),
        ("enable_inv"   , ctypes.c_uint8, 1),
        ("enable"       , ctypes.c_uint8, 5),
        ("_pack2"       , ctypes.c_uint8, 2),
        ("active_inv"   , ctypes.c_uint8, 1),
        ("active"       , ctypes.c_uint8, 5),
        ("active_mode"  , ctypes.c_uint8, 4),
        ("passive_mode" , ctypes.c_uint8, 4),
        ("primary_lut"   , ctypes.c_uint8, 4),
        ("secondary_lut"  , ctypes.c_uint8, 4),
    ]


class OutputConfig(_ControllerStructure):
    ENTRIES_PER_OUTPUT = 4
    _pack_ = 1
    _fields_ = [
        ('configs', OutputConfigEntry*ENTRIES_PER_OUTPUT)
    ]
    def __getitem__(self, idx: int) -> OutputConfigEntry:
        return self.configs[idx]
    def __str__(self):
        return "[\n" + ",\n".join("  "+str(config) for config in self.configs) + "\n]"

class OutputConfigSlot(IntEnum):
    SLOT1 = auto()
    SLOT2 = auto()
    SLOT3 = auto()
    SLOT4 = auto()


class Colors(Enum):
    BLACK   = "000000"
    WHITE   = "FFFFFF"
    RED     = "FF0000"
    GREEN   = "00FF00"
    BLUE    = "0000FF"
    YELLOW  = "FFFF00"
    CYAN    = "FF00FF"
    MAGENTA = "00FFFF"
    @classmethod
    def from_string(cls, s: str):
        return cls[s.upper()]
    @classmethod
    def list(cls):
        return list(map(lambda c: c.name.lower(), cls))

class Color(_ControllerStructure):
    _pack_ = 1
    _fields_ = [
        ("red",   ctypes.c_uint8, 8),
        ("green", ctypes.c_uint8, 8),
        ("blue",  ctypes.c_uint8, 8),
    ]
    def set(self, r: int, g: int, b:int):
        self.red = r
        self.green = g
        self.blue = b
    def set_from_string(self, c: str):
        try:
            cc = Colors.from_string(c)
            c = cc.value
        except KeyError:
            pass
        if len(c)==6:
            self.red   = int(c[0:2], 16)
            self.green = int(c[2:4], 16)
            self.blue  = int(c[4:6], 16)
            return
        raise ValueError(f"Invalid color {c} (must be 'RRGGBB')")
    def __str__(self):
        return f"{self.red:02x}{self.green:02x}{self.blue:02x}"
    def __repr__(self):
        return str(self)


class Brightness(_ControllerStructure):
    _pack_ = 1
    _fields_ = [
        ("brightness", ctypes.c_uint16, 16)
    ]
    def set(self, brightness):
        self.brightness = brightness
    def set_from_string(self, b: str):
        self.brightness = _clamp(int(b, 16), 0, 0xFFFF)
    def __str__(self):
        return f"<{self.brightness:04x}>"
    def __repr__(self):
        return str(self)


class Command(_ControllerStructure):
    _pack_ = 1
    _fields_ = [
        ("id",  ctypes.c_uint8,   8),
        ("arg", ctypes.c_uint32, 32),
    ]

class CommandId(IntEnum):
    NOOP = auto()
    CONFIG_LOAD  = 1
    CONFIG_STORE = 2
    CONFIG_ERASE = 3

class CommandLoadStoreArg(_ControllerIntFlag):
    OUTPUT_CONFIGS = auto()
    MODE_CONFIGS   = auto()
    COLOR_LUT      = auto()
    BRIGHTNESS_LUT = auto()
    ALL            = 0xFFFFFFFF
    NONE           = 0



class Input(_ControllerIntEnum):
    FALSE            = 0
    TRUE             = auto()
    EXT_IN1          = auto()
    EXT_IN2          = auto()
    ANO_SW1          = auto()
    ANO_SW2          = auto()
    ANO_SW3          = auto()
    ANO_SW4          = auto()
    ANO_SW5          = auto()
    BUTTON1          = auto()
    BUTTON2          = auto()
    BUTTON3          = auto()
    BUTTON4          = auto()
    BUTTON5          = auto()
    BUTTON6          = auto()
    BUTTON7          = auto()
    BUTTON8          = auto()
    USB_LED_STANDBY  = auto()
    USB_LED_SUSPEND  = auto()
    USB_LED_CHARGING = auto()
    USB_LED_MESSAGE  = auto()
    USB_LED_GENERIC  = auto()
    SOFT1            = auto()
    SOFT2            = auto()
    SOFT3            = auto()
    SOFT4            = auto()
    SOFT5            = auto()
    SOFT6            = auto()
    SOFT7            = auto()
    SOFT8            = auto()
    SOFT9            = auto()
    SOFT10           = auto()


class Output(_ControllerIntEnum):
    EXT_OUT1    = 0
    EXT_OUT2    = auto()
    LED1        = auto()
    LED2        = auto()
    LED3        = auto()
    LED4        = auto()
    NEOPIXEL1   = auto()
    NEOPIXEL2   = auto()
    NEOPIXEL3   = auto()
    NEOPIXEL4   = auto()


class Mode(_ControllerIntEnum):
    MODE_0  = 0
    MODE_1  = auto()
    MODE_2  = auto()
    MODE_3  = auto()
    MODE_4  = auto()
    MODE_5  = auto()
    MODE_6  = auto()
    MODE_7  = auto()
    MODE_8  = auto()
    MODE_9  = auto()
    MODE_10 = auto()
    MODE_11 = auto()
    MODE_12 = auto()
    MODE_13 = auto()
    MODE_14 = auto()
    MODE_15 = auto()

class LUT(IntEnum):
    LUT_0  = 0
    LUT_1  = auto()
    LUT_2  = auto()
    LUT_3  = auto()
    LUT_4  = auto()
    LUT_5  = auto()
    LUT_6  = auto()
    LUT_7  = auto()
    LUT_8  = auto()
    LUT_9  = auto()
    LUT_10 = auto()
    LUT_11 = auto()
    LUT_12 = auto()
    LUT_13 = auto()
    LUT_14 = auto()
    LUT_15 = auto()


class FeatureId(Enum):
    COMMAND         = auto()
    SOFT_INPUT      = auto()
    OUTPUT_CONFIGS  = auto()
    MODE_CONFIG     = auto()
    COLOR_LUT       = auto()
    BRIGHTNESS_LUT  = auto()


class Controller:
    VID = 0x0483
    PID = 0x5740

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
        self._soft_input = None
        self._soft_input_buffer = None


    def cmd_config_store(self, arg: CommandLoadStoreArg):
        self._command(CommandId.CONFIG_STORE, arg.value)

    def cmd_config_load(self, arg: CommandLoadStoreArg):
        self._command(CommandId.CONFIG_LOAD, arg.value)
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
    def soft_input(self) -> SoftInput:
        return SoftInput(self.soft_input_state.input)

    @soft_input.setter
    def soft_input_input(self, input: SoftInput):
        self.soft_input_state.input = input.value

    def soft_input_set(self, input: SoftInput):
        self.soft_input_state.input |= input

    def soft_input_clear(self, input: SoftInput):
        self.soft_input_state.input &= ~input

    @property
    def soft_input_state(self) -> SoftInputState:
        if not self._soft_input_buffer:
            self._soft_input_buffer = self._device.get_feature_report(FeatureId.SOFT_INPUT.value)
            self._soft_input = SoftInputState.from_buffer(self._soft_input_buffer)
        return self._soft_input

    def update_soft_input(self):
        if self._soft_input_buffer:
            self._device.set_feature_report(FeatureId.SOFT_INPUT.value, self._soft_input_buffer)


    @property
    def mode_configs(self) -> List[ModeConfig]:
        if not self._mode_configs_buffer:
            sz = ctypes.sizeof(ModeConfig)
            self._mode_configs_buffer = self._device.get_feature_report(FeatureId.MODE_CONFIG.value)
            self._mode_configs = [ModeConfig.from_buffer(self._mode_configs_buffer, offset) for offset in range(0, len(self._mode_configs_buffer), sz)]
        return self._mode_configs

    def update_mode_configs(self):
        if self._mode_configs_buffer:
            self._refresh(self._mode_configs, self._mode_configs_buffer)
            self._device.set_feature_report(FeatureId.MODE_CONFIG.value, self._mode_configs_buffer)


    @property
    def output_configs(self) -> List[OutputConfig]:
        if not self._output_configs_buffer:
            sz = ctypes.sizeof(OutputConfig)
            self._output_configs_buffer = self._device.get_feature_report(FeatureId.OUTPUT_CONFIGS.value)
            self._output_configs = [OutputConfig.from_buffer(self._output_configs_buffer, offset) for offset in range(0, len(self._output_configs_buffer), sz)]
        return self._output_configs
    
    def update_output_configs(self):
        if self._output_configs_buffer:
            self._refresh(self._output_configs, self._output_configs_buffer)
            self._device.set_feature_report(FeatureId.OUTPUT_CONFIGS.value, self._output_configs_buffer)


    @property
    def color_lut(self) -> List[Color]:
        if not self._color_lut_buffer:
            sz = ctypes.sizeof(Color)
            self._color_lut_buffer = self._device.get_feature_report(FeatureId.COLOR_LUT.value)
            self._color_lut = [Color.from_buffer(self._color_lut_buffer, offset) for offset in range(0, len(self._color_lut_buffer), sz)]
        return self._color_lut

    def update_color_lut(self):
        if self._color_lut_buffer:
            self._refresh(self._color_lut, self._color_lut_buffer)
            self._device.set_feature_report(FeatureId.COLOR_LUT.value, self._color_lut_buffer)


    @property
    def brightness_lut(self) -> List[Brightness]:
        if not self._brightness_lut_buffer:
            sz = ctypes.sizeof(Brightness)
            self._brightness_lut_buffer = self._device.get_feature_report(FeatureId.BRIGHTNESS_LUT.value)
            self._brightness_lut = [Brightness.from_buffer(self._brightness_lut_buffer, offset) for offset in range(0, len(self._brightness_lut_buffer), sz)]
        return self._brightness_lut

    def update_brightness_lut(self):
        if self._brightness_lut_buffer:
            self._refresh(self._brightness_lut, self._brightness_lut_buffer)
            self._device.set_feature_report(FeatureId.BRIGHTNESS_LUT.value, self._brightness_lut_buffer)


    def _refresh(self, source, buffer):
        off = 0
        for idx, obj in enumerate(source):
            sz = ctypes.sizeof(obj)
            if not obj._objects:
                b = bytearray(obj)
                buffer[off:off+len(b)] = b
                source[idx] = obj.__class__.from_buffer(buffer, off)
            off+=sz
    