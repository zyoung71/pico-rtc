import sys
from datetime import datetime
import serial
import serial.tools.list_ports

def finddev():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "Pico" in port.description or "RP2040" in port.description:
            return port.device
    return None

port = finddev()
if port is None:
    sys.exit(1)

print(f"Found device on port {port}")

com = serial.Serial(port, 115200, timeout=1)
now = datetime.now()

timestamp = f"{now.year} {now.month} {now.day} {now.isoweekday()} {now.hour} {now.minute} {now.second}"

command = f"/synctime \"{timestamp}\"\n"
print(f"Sent command: {command}")
com.write(command.encode())