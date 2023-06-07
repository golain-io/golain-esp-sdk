idf_component_register(SRCS "main.c" "example.c" "protos/shadow.pb.c"
                    INCLUDE_DIRS "." "protos"
                    REQUIRES golain-esp-sdk)
