# The following four lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS ../../)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(example_project)

target_add_binary_data(${PROJECT_NAME}.elf "main/certs/device_cert.pem" TEXT)
target_add_binary_data(${PROJECT_NAME}.elf "main/certs/device_private_key.pem" TEXT)
target_add_binary_data(${PROJECT_NAME}.elf "main/certs/mqtt_broker_cert.pem" TEXT)
target_add_binary_data(${PROJECT_NAME}.elf "main/certs/root_ca_cert.pem" TEXT)

