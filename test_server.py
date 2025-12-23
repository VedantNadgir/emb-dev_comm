import socket
import struct

# Configuration
SERVER_IP = "127.0.0.1"
PORT = 8080 # Change this to the port you pass to server_run()

def send_packet(packet):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((SERVER_IP, PORT))
        s.sendall(packet)
        response = s.recv(1024)
        return response

print("--- Starting Server Integration Tests ---")

# Test 1: Write to REG_RW0 (Value: 42)
# Packet format: [Cmd(1), Reg(1), Value(4 bytes)]
# '<BIi' means Little-Endian, Unsigned Char, Unsigned Char, Integer
cmd_write = 0x01
reg_rw0 = 1
val_42 = 42
packet_write = struct.pack('<BBi', cmd_write, reg_rw0, val_42)

print(f"\n1. Testing WRITE 42 to REG_RW0...")
resp = send_packet(packet_write)
print(f"   Response received: {resp.hex().upper()}")
# Expected: 0001 (Status OK, Reg 1)

# Test 2: Read from REG_RW0
cmd_read = 0x00
packet_read = struct.pack('<BB', cmd_read, reg_rw0)

print(f"\n2. Testing READ from REG_RW0...")
resp = send_packet(packet_read)
if len(resp) >= 6:
    status, reg, val = struct.unpack('<BBi', resp)
    print(f"   Status: {status}, Reg: {reg}, Value: {val}")
else:
    print(f"   Error: Response too short! {resp.hex()}")

# Test 3: Read Write-Only Register (REG_CONTROL = 7)
packet_bad_read = struct.pack('<BB', cmd_read, 7)
print(f"\n3. Testing illegal READ from REG_CONTROL...")
resp = send_packet(packet_bad_read)
print(f"   Response: {resp.hex().upper()} (First byte should be FE/-2)")

# Test 4: Malformed Packet (Wrong length)
print(f"\n4. Testing malformed packet (too short)...")
resp = send_packet(b'\x01\x01') # Write cmd but no data
print(f"   Response (should be empty/no response): {resp.hex()}")