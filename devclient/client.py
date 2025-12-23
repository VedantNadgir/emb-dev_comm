import socket
from devclient.protocol import *
from devclient.errors import raise_for_error

class DeviceClient:
    def __init__(self, host, port, timeout=1.0):
        self.host = host
        self.port = port
        self.timeout = timeout
    
    def _send_and_recv(self,payload: bytes) -> dict:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(self.timeout)
            s.connect((self.host,self.port))
            s.sendall(payload)

            resp = s.recv(1024)
            if not resp:
                raise RuntimeError("No response from device")
            
            result = unpack_response(resp)
            
            if not result["ok"]:
                raise_for_error(result["status"], result["error"])

            return result

    def read(self,reg: int) -> int:
        res = self._send_and_recv(pack_read(reg))
        return res["value"]
    def write(self,reg: int,value: int):
        self._send_and_recv(pack_write(reg,value))