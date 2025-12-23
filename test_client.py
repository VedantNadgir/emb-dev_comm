from devclient.client import DeviceClient

dev = DeviceClient("127.0.0.1", 8080)

dev.write(1, 42)
print("Wrote 42")

v = dev.read(1)
print("Read:", v)
