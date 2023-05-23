
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS "../../nanopb" "../../persistent_logs")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(plog_example)