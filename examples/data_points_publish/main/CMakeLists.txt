idf_component_register(SRCS "example.c" "protos/shadow.pb.c" "protos/data.pb.c"
                    INCLUDE_DIRS "." "protos"
                    REQUIRES golain-esp-sdk nanopb nvs_flash)
