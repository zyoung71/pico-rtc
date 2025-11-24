import sys
from datetime import datetime
import serial.tools.list_ports

def find_port():
    for port in serial.tools.list_ports.comports():
        if "Pico" in port.description or "RP2040" in port.description:
            return port.device
    return None

port = find_port()
if port is None:
    print("Device not detected.")
    sys.exit(1)

print(f"Device detected! Found on port {port}")

com = serial.Serial(port, 115200, timeout=1)

now = datetime.now()

year = now.year
month = now.month
day = now.day
hour = now.hour
minute = now.minute
second = now.second

weekday = now.isoweekday()

timestamp = f"{year} {month} {day} {weekday} {hour} {minute} {second}"

com.write((f"cmd synctime {timestamp}\n").encode())

print("Timestamp sent.")
com.close()