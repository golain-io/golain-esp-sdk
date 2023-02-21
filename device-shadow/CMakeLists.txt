idf_component_register(
    SRCS "proto/shadow.pb.c" "include/shadow_helper.c"
    INCLUDE_DIRS "proto" "include"
    REQUIRES nanopb log driver nvs_flash 
)
