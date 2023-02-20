idf_component_register(
    SRCS "protos/shadow.pb.c" "include/shadow_helper.c"
    INCLUDE_DIRS "protos" "include"
    REQUIRES nanopb log driver nvs_flash 
)
