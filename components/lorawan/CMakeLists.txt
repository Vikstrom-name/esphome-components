# For debugging and see if this CMakeLists.txt is being used
message(STATUS "CMakeLists.txt for my_component is being used!")

# idf_component_register(
#     SRCS "swl2001/lbm_lib/smtc_modem_core/smtc_modem.c"
#     INCLUDE_DIRS "swl2001/lbm_lib/smtc_modem_core"
# )

# Include directories for header files (if any)
set(COMPONENT_ADD_INCLUDEDIRS "swl2001/lbm_lib/smtc_modem_api")

# Source files for the component
set(COMPONENT_SRCS "swl2001/lbm_lib/smtc_modem_core/smtc_modem.c")

# Add dependencies if needed
# list(APPEND REQUIRES SPI)

# Register the component so ESPHome will compile and link it
register_component()
