idf_component_register(
    SRCS "include/shadow_helper.c" "proto/shadow.pb.c"
    INCLUDE_DIRS "proto" "include"
    REQUIRES nanopb log driver nvs_flash 
)
