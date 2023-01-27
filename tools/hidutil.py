
import io
import os
import fcntl
import struct
import glob
from dataclasses import dataclass

def _ioctl(fd, EVIOC, code, return_type, buf=None):
    size = struct.calcsize(return_type)
    if buf is None:
        buf = size * '\x00'
    abs = fcntl.ioctl(fd, EVIOC(code, size), buf)
    return struct.unpack(return_type, abs)

# extracted from <asm-generic/ioctl.h>
_IOC_WRITE = 1
_IOC_READ = 2

_IOC_NRBITS = 8
_IOC_TYPEBITS = 8
_IOC_SIZEBITS = 14
_IOC_DIRBITS = 2

_IOC_NRSHIFT = 0
_IOC_TYPESHIFT = _IOC_NRSHIFT + _IOC_NRBITS
_IOC_SIZESHIFT = _IOC_TYPESHIFT + _IOC_TYPEBITS
_IOC_DIRSHIFT = _IOC_SIZESHIFT + _IOC_SIZEBITS

# define _IOC(dir,type,nr,size) \
# 	(((dir)  << _IOC_DIRSHIFT) | \
# 	 ((type) << _IOC_TYPESHIFT) | \
# 	 ((nr)   << _IOC_NRSHIFT) | \
# 	 ((size) << _IOC_SIZESHIFT))
def _IOC(dir, type, nr, size):
    return ((dir << _IOC_DIRSHIFT) |
            (ord(type) << _IOC_TYPESHIFT) |
            (nr << _IOC_NRSHIFT) |
            (size << _IOC_SIZESHIFT))

def _IOR(type, nr, size): return _IOC(_IOC_READ, type, nr, size)
def _IOW(type, nr, size): return _IOC(_IOC_WRITE, type, nr, size)

#define HIDIOCGRDESCSIZE        _IOR('H', 0x01, int)
def _IOC_HIDIOCGRDESCSIZE(none, len):
    return _IOR('H', 0x01, len)


def _HIDIOCGRDESCSIZE(fd):
    """ get report descriptors size """
    type = 'i'
    return int(*_ioctl(fd, _IOC_HIDIOCGRDESCSIZE, None, type))

#define HIDIOCGRDESC            _IOR('H', 0x02, struct hidraw_report_descriptor)
def _IOC_HIDIOCGRDESC(none, len):
    return _IOR('H', 0x02, len)

def _HIDIOCGRDESC(fd, size):
    """ get report descriptors """
    format = "I4096c"
    tmp = struct.pack("i", size) + bytes(4096)
    _buffer = bytearray(tmp)
    fcntl.ioctl(fd, _IOC_HIDIOCGRDESC(None, struct.calcsize(format)), _buffer)
    size, = struct.unpack("i", _buffer[:4])
    value = _buffer[4:size + 4]
    return size, value

#define HIDIOCGRAWINFO          _IOR('H', 0x03, struct hidraw_devinfo)
def _IOC_HIDIOCGRAWINFO(none, len):
    return _IOR('H', 0x03, len)


def _HIDIOCGRAWINFO(fd):
    """ get hidraw device infos """
    type = 'ihh'
    return _ioctl(fd, _IOC_HIDIOCGRAWINFO, None, type)

#define HIDIOCGRAWNAME(len)     _IOC(_IOC_READ, 'H', 0x04, len)
def _IOC_HIDIOCGRAWNAME(none, len): 
    return _IOC(_IOC_READ, 'H', 0x04, len)

def _HIDIOCGRAWNAME(fd):
    """ get device name """
    type = 1024 * 'c'
    cstring = _ioctl(fd, _IOC_HIDIOCGRAWNAME, None, type)
    string = b''.join(cstring).decode('utf-8')
    return "".join(string).rstrip('\x00')

#define HIDIOCGRAWPHYS(len)     _IOC(_IOC_READ, 'H', 0x05, len)
#/* The first byte of SFEATURE and GFEATURE is the report number */
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
def _IOC_HIDIOCGFEATURE(none, len):
    return _IOC(_IOC_WRITE | _IOC_READ, 'H', 0x07, len)


def _HIDIOCGFEATURE(fd, report_id, rsize):
    """ get feature report """
    assert report_id <= 255 and report_id > -1
    # rsize has the report length in it
    buf = bytearray([report_id & 0xff]) + bytearray(rsize - 1)
    fcntl.ioctl(fd, _IOC_HIDIOCGFEATURE(None, len(buf)), buf)
    return buf  # Note: first byte is report ID

#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
def _IOC_HIDIOCSFEATURE(none, len):
    return _IOC(_IOC_WRITE | _IOC_READ, 'H', 0x06, len)


def _HIDIOCSFEATURE(fd, data: bytearray):
    """ set feature report """
    sz = fcntl.ioctl(fd, _IOC_HIDIOCSFEATURE(None, len(data)), data)
    return sz

#define HIDIOCGRAWUNIQ(len)     _IOC(_IOC_READ, 'H', 0x08, len)
#/* The first byte of SINPUT and GINPUT is the report number */
#define HIDIOCSINPUT(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x09, len)
#define HIDIOCGINPUT(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x0A, len)
#/* The first byte of SOUTPUT and GOUTPUT is the report number */
#define HIDIOCSOUTPUT(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x0B, len)
#define HIDIOCGOUTPUT(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x0C, len)


def _iter_descriptor(desc: bytearray):
    idx = 0
    while idx < len(desc):
        header = desc[idx]
        size = header & 0x3
        hid = header & 0xfc
        value = 0
        idx+=1
        for i in range(size):
            v = desc[idx]
            idx+=1
            value |= v<<(i*8)
        yield hid, value
    

@dataclass
class ReportData:
    report_id: int = 0
    hid: int = 0
    bits: int = 0

    @property
    def size(self) -> int:
        sz = int((self.bits+7)/8)
        return sz if self.report_id==0 else sz+1


class ReportDescriptor:
    def __init__(self, desc: bytes):
        self.input_reports = {}
        self.output_reports = {}
        self.feature_reports = {}
        report_id = 0
        report_size = 0
        report_count = 0
        for hid,value in _iter_descriptor(memoryview(desc)):
            #print(f"{hid:02x} {value:04x}")
            if hid==0x84: # ReportID
                #print(f"ReportID: {value}")
                report_id = value
            elif hid==0x74: # Report Size
                report_size = value
            elif hid==0x94: # Report Count
                report_count = value
            elif hid==0x80: # Input
                data = self.input_reports[report_id] = self.input_reports.get(report_id, ReportData(report_id, hid))
                data.bits += report_size*report_count
            elif hid==0x90: # Output
                data = self.output_reports[report_id] = self.output_reports.get(report_id, ReportData(report_id, hid))
                data.bits += report_size*report_count
            elif hid==0xb0: # Feature
                data = self.feature_reports[report_id] = self.feature_reports.get(report_id, ReportData(report_id, hid))
                data.bits += report_size*report_count
                #print(f"FeatureReport: {report_id} {data.bits}")




class Device:
    def __init__(self, device: io.FileIO):
        self.device = device
        fd = device.fileno()
        self._name = _HIDIOCGRAWNAME(fd)
        _, self._vendor_id, self._product_id = _HIDIOCGRAWINFO(fd)
        size = _HIDIOCGRDESCSIZE(fd)
        rsize, desc = _HIDIOCGRDESC(fd, size)
        assert rsize == size
        assert len(desc) == rsize
        self.report_descriptor = ReportDescriptor(memoryview(desc))

    def close(self):
        if self.device:
            self.device.close()
            self.device = None

    @property
    def name(self) -> str:
        return self._name

    @property
    def vendor_id(self) -> int:
        return self._vendor_id

    @property
    def product_id(self) -> int:
        return self._product_id


    def get_feature_report(self, report_ID: int) -> bytearray:
        report = self.report_descriptor.feature_reports[report_ID]
        fd = self.device.fileno()
        buf = _HIDIOCGFEATURE(fd, report_ID, report.size)
        return memoryview(buf)[1:]

    def set_feature_report(self, report_id: int, data: bytearray):
        report = self.report_descriptor.feature_reports[report_id]
        assert(report.size==len(data)+1)
        buf = bytearray(struct.pack("<B", report_id)+data)
        assert(report.size==len(buf))
        assert buf[0] == report_id
        fd = self.device.fileno()
        sz = _HIDIOCSFEATURE(fd, buf)
        #print(f"> {buf}")
        if sz != len(buf):
            raise OSError(f"Failed to write data: {data} - bytes written: {sz}")

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def __del__(self):
        self.close()


def find_device(vendor_id: int, product_id: int) -> Device:
    for dev_path in glob.glob("/dev/hidraw*"):
        try:
            with open(dev_path, "r+b") as dev:
                _, vid, pid = _HIDIOCGRAWINFO(dev.fileno())
                if vid==vendor_id and pid==product_id:
                    _dev = io.FileIO(os.dup(dev.fileno()))
                    return Device(_dev)
        except PermissionError:
            pass
    raise FileNotFoundError(f"Device<{vendor_id}:{product_id}> not found")

