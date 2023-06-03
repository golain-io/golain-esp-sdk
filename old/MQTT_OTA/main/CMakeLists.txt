set(srcs "main.c"
    )

idf_component_register(SRCS "main.c" "${srcs}"
                    INCLUDE_DIRS  ".") #This is going to cause conflicts with our mqtt component
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
