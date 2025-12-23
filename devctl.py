#!/usr/bin/env python3
import argparse
import sys

from devclient.client import DeviceClient
from devclient.errors import DeviceError

REG_STATUS = 0
REG_CONTROL = 7

def main():
    parser = argparse.ArgumentParser(
        prog="devctl",
        description="Device control CLI for networked embedded device"
    )

    parser.add_argument("--host", default="127.0.0.1",
                        help="Device IP address")
    parser.add_argument("--port", type=int, default=8080,
                        help="Device TCP port")

    sub = parser.add_subparsers(dest="cmd", required=True)

    # read
    p_read = sub.add_parser("read", help="Read device register")
    p_read.add_argument("reg", type=int, help="Register number")

    # write
    p_write = sub.add_parser("write", help="Write device register")
    p_write.add_argument("reg", type=int, help="Register number")
    p_write.add_argument("value", type=int, help="Value to write")

    # status
    sub.add_parser("status", help="Read device status")

    args = parser.parse_args()

    dev = DeviceClient(args.host, args.port)

    try:
        if args.cmd == "read":
            val = dev.read(args.reg)
            print(f"REG[{args.reg}] = {val}")

        elif args.cmd == "write":
            dev.write(args.reg, args.value)
            if(args.reg == 7):
                print(f"Wrote {args.value} to Control")
            else:
                print(f"Wrote {args.value} to REG[{args.reg}]")

        elif args.cmd == "status":
            val = dev.read(REG_STATUS)
            state = "ACTIVE" if val else "IDLE"
            print(f"STATUS = {state} ({val})")

    except DeviceError as e:
        print(f"Device error: {e}", file=sys.stderr)
        sys.exit(1)

    except Exception as e:
        print(f"Fatal error: {e}", file=sys.stderr)
        sys.exit(2)


if __name__ == "__main__":
    main()