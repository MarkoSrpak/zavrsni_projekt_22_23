set(srcs "src/*")

idf_component_register(SRC_DIRS "src"
                       INCLUDE_DIRS "inc"
                       PRIV_REQUIRES hal driver)
