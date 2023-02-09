# Device health read example



This example demonstrates how to read various device parameters such as number of reboots, its reason and many more.

  * Reset count - tracks number of soft and hard restarts.
  * Number of errors since last reboot - tracks number of errors occured since last reboot. (This feature can be used only if you use persistent logs)
  * Reboot reason - Tells the reason behind last reboot. (check proto file for more explaination)
  * Chip revision - Gives the chip revision number
  * User data - There are two user data fields, one for numeric data and another for string data.
## How to use example

### Hardware required

This example can be run on most common development boards which have an active button connected to boot mode pin. On most boards, this button is labeled as "Boot". When pressed, the button connects boot mode pin to ground.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

I (43) boot: ESP-IDF v4.4.3-dirty 2nd stage bootloader
I (43) boot: compile time 19:28:27
I (43) boot: chip revision: 3
I (45) boot.esp32c3: SPI Speed      : 80MHz
I (50) boot.esp32c3: SPI Mode       : DIO
I (54) boot.esp32c3: SPI Flash Size : 4MB
I (59) boot: Enabling RNG early entropy source...
I (65) boot: Partition Table:
I (68) boot: ## Label            Usage          Type ST Offset   Length
I (75) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (83) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (90) boot:  2 factory          factory app      00 00 00010000 00100000
I (98) boot: End of partition table
I (102) esp_image: segment 0: paddr=00010020 vaddr=3c020020 size=082b0h ( 33456) map
I (116) esp_image: segment 1: paddr=000182d8 vaddr=3fc8ac00 size=01404h (  5124) load
I (120) esp_image: segment 2: paddr=000196e4 vaddr=40380000 size=06934h ( 26932) load
I (132) esp_image: segment 3: paddr=00020020 vaddr=42000020 size=19f9ch (106396) map
I (152) esp_image: segment 4: paddr=00039fc4 vaddr=40386934 size=041b0h ( 16816) load
I (156) esp_image: segment 5: paddr=0003e17c vaddr=50000010 size=00010h (    16) load
I (162) boot: Loaded app from partition at offset 0x10000
I (164) boot: Disabling RNG early entropy source...
I (181) cpu_start: Pro cpu up.
I (190) cpu_start: Pro cpu start user code
I (190) cpu_start: cpu freq: 160000000
I (190) cpu_start: Application information:
I (193) cpu_start: Project name:     deviceHealth
I (198) cpu_start: App version:      v4.4.3-dirty
I (203) cpu_start: Compile time:     Feb  9 2023 19:28:17
I (209) cpu_start: ELF file SHA256:  587846bf0fcb352e...
I (215) cpu_start: ESP-IDF:          v4.4.3-dirty
I (221) heap_init: Initializing. RAM available for dynamic allocation:
I (228) heap_init: At 3FC8CEF0 len 0004F820 (318 KiB): DRAM
I (234) heap_init: At 3FCDC710 len 00002950 (10 KiB): STACK/DRAM
I (241) heap_init: At 50000020 len 00001FE0 (7 KiB): RTCRAM
I (248) spi_flash: detected chip: generic
I (252) spi_flash: flash io: dio
I (256) sleep: Configure to isolate all GPIO pins in sleep state
I (263) sleep: Enable automatic switching of GPIO sleep configuration
I (270) cpu_start: Starting scheduler.
E (279) main: (app_main)-> This is not an error!
E (279) main: (app_main)-> This is not an error!
E (289) main: (app_main)-> This is not an error!
E (289) main: (app_main)-> This is not an error!
E (299) main: (app_main)-> This is not an error!
E (299) main: (app_main)-> This is not an error!
E (309) main: (app_main)-> This is not an error!
E (309) main: (app_main)-> This is not an error!
E (319) main: (app_main)-> This is not an error!
E (319) main: (app_main)-> This is not an error!
I (339) main: device health Stored successfully
I (339) debug: was called from decode_message
I (339) decode: number of errors since last reboot: 10
I (339) decode: last reboot reason 0
I (349) decode: number of reboots: 9
I (349) decode: chip revision: 3
I (359) decode: user numeric data: 569.780029
I (359) decode: user string data: Zephyr is better