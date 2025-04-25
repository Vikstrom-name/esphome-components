message(STATUS "CMakeLists.txt for my_component is being used!")
idf_component_register(
    SRCS "swl2001/lbm_lib/smtc_modem_core/smtc_modem.c"
    INCLUDE_DIRS "swl2001/lbm_lib/smtc_modem_core"
)