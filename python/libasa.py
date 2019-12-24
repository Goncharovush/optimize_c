from ctypes import c_char_p, CDLL, Structure, Array, c_ubyte, c_int, \
    c_double, c_uint32, byref, c_size_t
lib = CDLL('libasa')
MAX_NUM_POINTS = 10000
_host = '172.16.3.194'
_port = '8888'
HOST = c_char_p(_host.encode('utf-8'))
PORT = c_char_p(_port.encode('utf-8'))


class _IterableStructure(Structure):
    def __iter__(self):
        return (getattr(self, n) for n, t in self._fields_)


def _normalize_arg(value, desired_ctype):
    from collections import Sequence

    if isinstance(value, desired_ctype):
        return value
    elif issubclass(desired_ctype, Array) and isinstance(value, Sequence):
        member_type = desired_ctype._type_

        if desired_ctype._length_ < len(value):
            raise ValueError()

        if issubclass(member_type, c_ubyte) and isinstance(value, bytes):
            return desired_ctype.from_buffer_copy(value)
        elif issubclass(member_type, c_ubyte) and isinstance(value, bytearray):
            return value
        else:
            return desired_ctype(*value)
    else:
        return value


class Version(_IterableStructure):

    _fields_ = (
          ('major', c_int),
          ('minor', c_int),
          ('bugfix', c_int)
     )


class Server(_IterableStructure):

    _fields_ = (
          ('host', c_char_p),
          ('port', c_char_p)
     )


class AsaSettings(_IterableStructure):
    _fields_ = (
        ('disc_freq_hz', c_double),
        ('probe_signal_frequency_hz', c_double),
        ('number_points', c_uint32),
        ('number_charge_points', c_uint32),
        ('voltage_ampl_v', c_double),
        ('max_current_m_a', c_double),
        ('measure_flags', c_uint32),
        ('debug_model_type', c_uint32),
        ('debug_model_nominal', c_double),
        ('trigger_mode', c_uint32)
    )


class IvCurve(_IterableStructure):
    _fields_ = (
        ('voltages', c_double*MAX_NUM_POINTS),
        ('currents', c_double*MAX_NUM_POINTS)
    )


def GetLibraryVersion():
    version = lib.GetLibraryVersion
    version.argtype = None
    version.restype = Version
    res = version()
    res.major = _normalize_arg(res.major, c_int)
    res.minor = _normalize_arg(res.minor, c_int)
    res.bugfix = _normalize_arg(res.bugfix, c_int)
    return res.major, res.minor, res.bugfix


def SetSettings(server, settings):
    lib_func = lib.SetSettings
    lib_func.argtype = Server, AsaSettings
    lib_func.restype = c_int
    res = _normalize_arg(lib_func(byref(server), byref(settings)), c_int)
    return res


def GetSettings(server, settings):
    lib_func = lib.GetSettings
    lib_func.argtype = Server, AsaSettings
    lib_func.restype = c_int
    res = _normalize_arg(lib_func(byref(server), byref(settings)), c_int)
    return res


def GetIVCurve(server, iv_curve, size):
    lib_func = lib.GetIVCurve
    lib_func.argtype = Server, IvCurve, c_size_t
    lib_func.restype = c_int
    res = _normalize_arg(lib_func(byref(server), byref(iv_curve), c_size_t(size)), c_int)
    return res


def SetMinVC(min_var_v, min_var_c):
    lib_func = lib.SetMinVC
    lib_func.argtype = c_double, c_double
    lib_func(c_double(min_var_v),  c_double(min_var_c))


def CompareIvc(first_iv_curve, second_iv_curve, size):
    lib_func = lib.CompareIvc
    lib_func.argtype = IvCurve, IvCurve, c_size_t
    lib_func.restype = c_double
    res = lib_func(byref(first_iv_curve), byref(second_iv_curve), c_size_t(size))
    res = _normalize_arg(res, c_double)
    return res


if __name__ == '__main__':
    server = Server(HOST, PORT)
    settings = AsaSettings()
    settings.disc_freq_hz = c_double(10000)
    settings.number_points = c_uint32(100)
    settings.number_charge_points = c_uint32(400)
    settings.measure_flags = c_uint32(0)
    settings.probe_signal_frequency_hz = c_double(100)
    settings.voltage_ampl_v = c_double(5)
    settings.max_current_m_a = c_double(5)
    settings.debug_model_type = c_uint32(1)
    settings.debug_model_nominal = c_double(100)
    settings.trigger_mode = c_uint32(0)
    SetSettings(server, settings)
    iv_curve = IvCurve()
    status = GetIVCurve(server, iv_curve, settings.number_points)
    settings.voltage_ampl_v = c_double(15)
    SetSettings(server, settings)
    ivc_curve = IvCurve()
    status = GetIVCurve(server, ivc_curve, settings.number_points)
    SetMinVC(0, 0)
    f = CompareIvc(iv_curve, ivc_curve, settings.number_points)
    print(f)
    for i in range(settings.number_points):
        print(iv_curve.currents[i], iv_curve.voltages[i])
