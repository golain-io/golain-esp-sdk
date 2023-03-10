# Device Shadow Library

## ***Config structure***

#### ```ShadowCfg``` : Config structure for shadow functionality
```shadow_update_cb``` : Callback function for when the shadow is updated.

```clean_on_error``` : When set to HIGH or 1, clear NVS when invalid buffer is present. The buffer will be considered invalid when it cannot be decoded into the shadow protobuffer.

## ***Functions***

#### ```UpdatewithStruct``` : A function to commit war crimes against the nvs by writing the global shadow onto it. This is useful for using during runtime.

#### ```UpdatewithBuff(buff,size)``` : Update the global shadow and the NVS simultaneously with a buffer. This is particularly useful when receiving an encoded shadow protobuff via protocols like mqtt or ble. 

``buff`` : Buffer that contains encoded pb message.

``len`` : Length of the pb message.

#### ```InitDeviceShadow(temp_cfg)``` : Initialise the shadow service.

``temp_cfg`` : Config structure required for initialising the shadow service.


#### ```GetShadow( buff, buff_len, encoded_size)```: Get the encoded version of the shadow protobuffer.
``buff`` The buffer you want to store the encoded protobuffer in. 

``buff_len`` Length of the above buffer.

``message_len`` Length of the shadow you are using. 

# Example 
It contains a program that uses the shadow to work with our BLE Mesh component. Currently, the shadow has three `int32` members. You can store and read these members via BLE mesh by triggering _Control set_ and _Control get_ repectively.

## How to use example

### Hardware required

The example has been tested on `ESP32-WROOM32-D` and `ESP32-C3` on esp-idf `v4.4`. 

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py build 
idf.py -p PORT flash monitor

```
(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

