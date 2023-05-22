idf_component_register(
    SRCS "protos/logs.pb.c" "src/persistent_logs.h"
    INCLUDE_DIRS "include" "protos"
    REQUIRES log nvs_flash nanopb
)