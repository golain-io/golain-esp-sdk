idf_component_register(SRCS "mqtt-example.c" "protos/cell_data.pb.c"
                    INCLUDE_DIRS "."
                    REQUIRES nanopb nvs_flash mqtt log driver device-shadow)
