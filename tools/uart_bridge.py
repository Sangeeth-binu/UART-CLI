import serial
import time
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--uart0", required=True)
parser.add_argument("--uart1", required=True)
parser.add_argument("--baud", type=int, required=True)
parser.add_argument("--input", required=True)
parser.add_argument("--output", required=True)

args = parser.parse_args()

uart0 = serial.Serial(args.uart0, args.baud, timeout=1)
uart1 = serial.Serial(args.uart1, args.baud, timeout=1)

time.sleep(2)

with open(args.input, "r") as infile:
    for ch in infile.read():
        uart0.write(ch.encode())
        uart0.flush()
        time.sleep(0.05)

time.sleep(1)

with open(args.output, "w") as outfile:
    while uart1.in_waiting > 0:
        data = uart1.read(1)
        outfile.write(data.decode(errors="ignore"))

uart0.close()
uart1.close()
