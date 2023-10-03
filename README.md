# EagleX Bringup

[EagleX Test Note](https://docs.google.com/presentation/d/1NFBMX0InoTdFPE7k63EsRdLQBOzjHwr1ZK8_0qCyUlE/edit?usp=sharing)

## Bench Settings

Using vcu118, plugin EagleX board to the 400 pin FMC connector (the narrower one).

Provide 0.8V power to the chip through the Banananana connector. Chip should draw ~0.3 A current when running.

Provide 100 MHz clock through the clock generator. The clock mapping is as below:

| CLKSEL[0] | CLKSEL[1] | Clock                                                          |
| --------- | --------- | -------------------------------------------------------------- |
| 0         | 0         | using PLL                                                      |
| 1         | 0         | using 1.8V single ended CCLK0 (on PCB the silkscreen is CCLK2) |
| 0         | 1         | using 1.8V single ended CCLK1 (on PCB the silkscreen is CCLK1) |
| 1         | 1         | using 1.8V single ended FMC clock CCLK2                        |

## vcu118 Hack

the original linux thing does not support UART passthrough, so here we are not using the RISC-V soft core on FPGA. Instead, the bitstream is a simple passthrough, which redirects the JTAG and UART signals from the DUT FMC connector to PMOD header.

## Compiling program

cd into `workspace`

```bash
make
```


# Running programs

## Step 1: boot

Terminal 1
```bash
openocd -f ./bsp/eaglex/debug/eaglex_boot.cfg
```

Terminal 2
```bash
riscv64-unknown-elf-gdb.exe --eval-command="target extended-remote localhost:3020"

(gdb) load
(gdb) c
```

## Step 2: run

Terminal 1
```bash
openocd -f ./bsp/eaglex/debug/eaglex_demo.cfg
```

Terminal 2
```bash
riscv64-unknown-elf-gdb.exe --eval-command="target extended-remote localhost:3020"

(gdb) c
```

Terminal 3
```bash
riscv64-unknown-elf-gdb ./build/firmware.elf --eval-command="target extended-remote localhost:3000"

(gdb) set $pc=0x50000000
(gdb) c
```

Terminal 4
```bash
riscv64-unknown-elf-gdb ./build/firmware.elf --eval-command="target extended-remote localhost:3001"

(gdb) set $pc=0x50000000
(gdb) c
```

...
