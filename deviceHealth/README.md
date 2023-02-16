# Device health read example



This example demonstrates how to read various device parameters such as number of reboots, its reason and many more.


  | Metric | Definition | variable name |
| --- | --- | --- |
| Reset count | tracks number of soft and hard restarts | `deviceHealth.reset_counter` |
| Number of Errors | tracks number of errors occured since last reboot | `deviceHealth.errorCountSinceLastReset` |
| Reboot reason | tells the reason behind last reboot | `deviceHealth.rebootReason` |
| Chip revision | Gives chip revision number | `deviceHealth.chipRevision` |
| User data | There are two user data fields, one for numeric data and another for string data | `deviceHealth.userStringData` && `deviceHealth.userNumericData` |


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
