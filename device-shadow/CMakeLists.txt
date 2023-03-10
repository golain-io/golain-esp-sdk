idf_component_register(
    SRCS "include/shadow_helper.c" "protos/shadow.pb.c"
    INCLUDE_DIRS "include" "protos"
    REQUIRES nanopb log driver nvs_flash 
)

# This file should be in the component directory
# project/component/CMakeLists.txt

# so the include directory is added to the include path
# ../../