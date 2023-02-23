# Golain ESP SDK

This repo is a set of components created to make it conviniet to get started with an IoT Product with the Golain Platform.

### Works with
| Hardware        | ESP-IDF version | Status | Tested |
| ---             | ---      | ---       | ---                |
| ESP32-WROOM32-D | `v4.4.x` | `dev`     | :heavy_check_mark: |
| ESP32-WROOM32-D | `v5.x`   | `pending` | :x:                |
| ESP32-C3        | `v4.4.x` | `dev`     | :heavy_check_mark: |
| ESP32-C3        | `v5.x`   | `pending` | :x:                |

### Getting started

It is expected that this repository is added as a submodule inside your project under the component directory.
`Sample iot-shell coming soon!`

Assuming that your project is already a git repository, this can be done by:
1. run `git submodule init` inside your project
2. run `git submodule add https://github.com/golain-io/golain-esp-sdk components/`
3. navigate to the cloned repo by executing `cd components/golain-esp-sdk`
4. run `git submodule update --init --recursive` to pull all external components as well
5. Update your CMakeLists.txt to include the components directory

### Details
Usage guides for each individual component are included in the component READMEs, and implmentations can be found under the `example` folders under each component
