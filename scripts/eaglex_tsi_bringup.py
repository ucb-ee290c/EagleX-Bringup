import time

import serial



class BringupCommander:
    def __init__(self):
        self.ser = serial.Serial(port="/dev/ttyUSB1", baudrate=115200, timeout=1, write_timeout=1)

    def writeSlowly(self, buffer):
        for b in buffer:
            self.ser.write(bytearray([b]))
            time.sleep(0.01)
        print(self.ser.readline().decode(), end="")

    def sendCommand(self, command):
        self.writeSlowly(command)
        time.sleep(0.1)
    
    def readResult(self):
        buffer = self.ser.readline()
        print(buffer.decode(), end="")
        return buffer
    
    def readResults(self):
        buffer = self.ser.readlines()
        for ln in buffer:
            print(ln.decode(), end="")
        return buffer
    

term = BringupCommander()

# quit gdb
term.sendCommand(b"q\r\n")
time.sleep(0.5)
term.readResults()

term.sendCommand(b"y\r\n")
term.readResults()


# initialize the terminal
term.sendCommand(b"clear\r\n")
term.readResult()

term.sendCommand(b"killall -q -9 openocd\r\n")
term.readResult()

term.sendCommand(b"eagle-reset\r\n")
term.readResult()

term.sendCommand(b"openocd -f /mnt/sdcard/share/openocd/scripts/board/eagleX.cfg >& openocd.log & \r\n")
term.readResult()

term.sendCommand(b"\r\n")
term.readResults()

term.sendCommand(b"gdb\r\n")
time.sleep(2)

term.writeSlowly(b"\r\n")
term.readResults()

term.sendCommand(b"\r\n")
term.readResult()


term.sendCommand(b"target remote localhost:3333\r\n")

time.sleep(2)

term.readResults()



term.writeSlowly(b"\r\n")
term.readResults()


term.sendCommand(b"x 0x00110000\r\n")
time.sleep(0.5)
term.readResults()

term.sendCommand(b"set {int}0x00110000 = 0x0\r\n")
time.sleep(0.5)
term.readResults()

term.sendCommand(b"x 0x00110000\r\n")
time.sleep(0.5)
term.readResults()

