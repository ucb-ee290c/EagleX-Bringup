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


FREQ_LOW = 100000000
FREQ_HIGH = 1600000000
VOLTAGE = 0.85

print("This run covers frequencies {0} to {1} with voltage {2}".format(FREQ_LOW, FREQ_HIGH, VOLTAGE))

try:
    power.enableOutput(SiglentSPD3303X.Channel.CH2)
    clock.enableOutputP(Keysight81134A.Channel.CH1)
    clock.enableOutputP(Keysight81134A.Channel.CH2)




    frequencies = list(range(FREQ_LOW, FREQ_HIGH+1, 100000000))
    voltage = VOLTAGE

    for freq in frequencies:
        print("using Freq={0}\tVolt={1}".format(freq, voltage))

        if freq == 600000000:
            freq = 601000000

        clock.setFrequency(freq)
        power.setVoltage(voltage, SiglentSPD3303X.Channel.CH2)

        if freq <= 300000000:
            clock.setFrequencyDivisor(2, Keysight81134A.Channel.CH1)
        elif freq <= 700000000:
            clock.setFrequencyDivisor(4, Keysight81134A.Channel.CH1)
        elif freq <= 1500000000:
            clock.setFrequencyDivisor(8, Keysight81134A.Channel.CH1)
        else:
            clock.setFrequencyDivisor(16, Keysight81134A.Channel.CH1)


        time.sleep(1)


        is_working = input("is it working? (y/n)\n")
        voltage_draw = power.getVoltage(SiglentSPD3303X.Channel.CH2)
        current_draw = power.getCurrent(SiglentSPD3303X.Channel.CH2)
        power_draw = power.getPower(SiglentSPD3303X.Channel.CH2)
        if is_working == "y" or is_working == "Y" or is_working == "1":
            is_working = 1
        else:
            is_working = 0
        print(voltage_draw, current_draw, power_draw, is_working)

        if not data_table.get(voltage):
            data_table[voltage] = {}
        data_table[voltage][freq] = {
            "is_working": is_working,
            "voltage": voltage_draw,
            "current": current_draw,
            "power": power_draw,
        }

    print("Data:")
    print(data_table)

    json.dump(data_table, open(data_filename, "w"))

except KeyboardInterrupt:
    print("quitting")

power.disableOutput(SiglentSPD3303X.Channel.CH2)
clock.disableOutput(Keysight81134A.Channel.CH1)
clock.disableOutput(Keysight81134A.Channel.CH2)