import json

import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter

matplotlib.rcParams.update({"font.size": 10})


data_filename = "./bench-measurements/2023-10-02.json"
data_filename = "./bench-measurements/2023-10-02_shmoo-data.json"

data_table = json.load(open(data_filename, "r"))


voltages = []
frequencies = []
working_status = []
powers = []

for v in sorted(data_table.keys(), reverse=True):
    voltages.append(float(v))
    data_entry = data_table[v]

    s = []
    p = []
    c = []
    for f in data_entry.keys():
        f_display = float(f) / 1000000000
        if f_display not in frequencies:
            frequencies.append(f_display)
        s.append(data_entry[f].get("is_working"))
        p.append(data_entry[f].get("power"))
        c.append(data_entry[f].get("current"))

    working_status.append(s)
    powers.append(p)


'''
fig, ax1 = plt.subplots()

cax = ax1.matshow(working_status, cmap="RdYlGn", vmin=0, vmax=1.25)

#ax2 = ax1.secondary_xaxis("bottom")
#ax2.set_xlabel("period [s]")

ax1.set_xticks(range(len(frequencies)), frequencies, rotation=45)
ax1.xaxis.set_ticks_position("bottom")
ax1.set_xlabel("Frequency (GHz)")
# plt.gca().set_xticklabels(['{:.0f}'.format(x) for x in frequencies])
ax1.set_yticks(range(len(voltages)), voltages)
ax1.set_ylabel("Voltage (V)")


#fig.colorbar(cax)
plt.title("EagleX Core Booting Status Shmoo Plot - 2 harts")

plt.show()

'''


fig, ax1 = plt.subplots()

cax = ax1.matshow(powers, cmap="hot")


ax1.set_xticks(range(len(frequencies)), frequencies, rotation=45)
ax1.xaxis.set_ticks_position("bottom")
ax1.set_xlabel("Frequency (MHz)")
ax1.set_yticks(range(len(voltages)), voltages)
ax1.set_ylabel("Voltage (V)")

fig.colorbar(cax)
plt.title("EagleX Core Power Consumption Shmoo Plot (Watt)")

plt.show()


