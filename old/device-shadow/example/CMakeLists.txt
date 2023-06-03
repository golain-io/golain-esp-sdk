# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# must be set before project() is called
set(EXTRA_COMPONENT_DIRS "../../")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(blemesh_shadow)

execute_process(COMMAND pip install protobuf grpcio-tools)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/protos/shadow.proto ${CMAKE_CURRENT_SOURCE_DIR}/../protos/shadow.proto COPYONLY)

execute_process(COMMAND ${PYTHON_EXECUTABLE} ../../nanopb/generator/nanopb_generator.py shadow.proto
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../protos)


# REPLACE WITH YOUR OWN PATHS. Typically, nanopb would go in your project's components directory. Like so:
##  
##
# execute_process(COMMAND ${PYTHON_EXECUTABLE} components/nanopb/generator/nanopb_generator.py shadow.proto
#                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/components/device-shadow/protos)