class DeviceError(Exception):
    pass

class IllegalRegister(DeviceError):
    pass

class MalformedPacket(DeviceError):
    pass

class UnknownDeviceError(DeviceError):
    pass

ERROR_MAP = {
    0x07: IllegalRegister,
    0x01: MalformedPacket,
}

def raise_for_error(status, error):
    exc = ERROR_MAP.get(error, UnknownDeviceError)
    raise exc(f"Device error status={status} code={error}")
