import time
import datetime
import json

from cc.scpi import Keysight81134A, SiglentSPD3303X


"""
# Bench connection:

## Power:

CH2: 0.8V, connecting to VDD (0.8V) banana jack

## Clock:

CH2: fast clock, connect to CCLK2

CH1: slow clock, connect to CCLK1



"""


data_filename = "./bench-measurements/{0}_shmoo-data.json".format(datetime.datetime.now().strftime("%Y-%m-%d"))

try:
    data_table = json.load(open(data_filename, "r"))
except FileNotFoundError:
    data_table = {}

clock = Keysight81134A("128.32.62.102")
clock.connect()
power = SiglentSPD3303X("128.32.62.100")
power.connect()


clock.setFunction(Keysight81134A.Function.SQUARE, Keysight81134A.Channel.CH2)
clock.setFrequency(100000000)

clock.setFrequencyDivisor(4, Keysight81134A.Channel.CH1)
clock.setFrequencyDivisor(1, Keysight81134A.Channel.CH2)
clock.setVoltageHigh(1.8, Keysight81134A.Channel.CH2)
clock.setVoltageLow(0, Keysight81134A.Channel.CH2)

power.setVoltage(0.8, SiglentSPD3303X.Channel.CH2)
# set current limit
power.setCurrent(1.8, SiglentSPD3303X.Channel.CH2)

time.sleep(1)


FREQ = 100000000
VOLTAGE = 0.85

try:
    power.enableOutput(SiglentSPD3303X.Channel.CH2)
    clock.enableOutputP(Keysight81134A.Channel.CH1)
    clock.enableOutputP(Keysight81134A.Channel.CH2)

    print("using Freq={0}\tVolt={1}".format(FREQ, VOLTAGE))

    clock.setFrequency(FREQ)
    power.setVoltage(VOLTAGE, SiglentSPD3303X.Channel.CH2)

    clock.setFrequencyDivisor(1, Keysight81134A.Channel.CH1)

    while True:
        time.sleep(1)

except KeyboardInterrupt:
    print("quitting")

power.disableOutput(SiglentSPD3303X.Channel.CH2)
clock.disableOutput(Keysight81134A.Channel.CH1)
clock.disableOutput(Keysight81134A.Channel.CH2)