idf_component_register(
    SRCS "protos/logs.pb.c"
    INCLUDE_DIRS "include" "protos"
    REQUIRES log nvs_flash nanopb
)