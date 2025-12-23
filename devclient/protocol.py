import struct

cmd_READ = 0x00
cmd_WRITE = 0x01

STATUS_OK = 0x00

def pack_read(reg: int) -> bytes:
    return struct.pack("BB",cmd_READ,reg)

def pack_write(reg: int, value: int) -> bytes:
    return struct.pack("<BBi", cmd_WRITE, reg, value)

def unpack_response(data: bytes) -> dict:
    """
    Returns:
      { 'ok': True, 'reg': int, 'value': int | None }
      { 'ok': False, 'status': int, 'error': int }
    """
    if(len(data)<2):
        raise ValueError("Response too short")
    
    status = struct.unpack("b",data[0:1])[0]

    if(status == STATUS_OK):
        if(len(data)==2):
            _, reg = struct.unpack("BB", data)
            return {"ok": True, "reg": reg, "value": None} ##read cmd
        elif len(data) == 6:
            _, reg, value = struct.unpack("<BBi", data)
            return {"ok": True, "reg": reg, "value": value}## write cmd
        else:
            raise ValueError(f"Malformed STATUS_OK response,length: {len(data)}")
    else:
        _, err = struct.unpack("BB", data[:2])
        return {"ok": False, "status": status, "error": err} ## Error response
        