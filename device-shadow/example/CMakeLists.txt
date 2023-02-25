# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# must be set before project() is called
set(EXTRA_COMPONENT_DIRS "../../")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(blemesh_shadow)


configure_file(${CMAKE_SOURCE_DIR}/../proto/shadow.proto ${CMAKE_SOURCE_DIR}/../../nanopb/generator/proto COPYONLY)

execute_process(COMMAND pip install protobuf grpcio-tools)

execute_process(COMMAND ${PYTHON_EXECUTABLE} nanopb/generator/nanopb_generator.py shadow.proto
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../..)


configure_file(${CMAKE_SOURCE_DIR}/../../shadow.pb.h ${CMAKE_SOURCE_DIR}/../proto COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/../../shadow.pb.c ${CMAKE_SOURCE_DIR}/../proto COPYONLY)
