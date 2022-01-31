
# Check required parameters have been supplied
get_property(FWK_ID_FILE_LIST GLOBAL PROPERTY FWK_ID_FILE_LIST)
get_property(FWK_MSG_FILE_LIST GLOBAL PROPERTY FWK_MSG_FILE_LIST)

# Include application-level includes (if set)
if(DEFINED FWK_APP_ID_FILE_LIST)
    list(APPEND FWK_ID_FILE_LIST ${FWK_APP_ID_FILE_LIST})
endif()
if(DEFINED FWK_APP_MSG_FILE_LIST)
    list(APPEND FWK_MSG_FILE_LIST ${FWK_APP_MSG_FILE_LIST})
endif()

if(NOT DEFINED FWK_ID_FILE_LIST)
    message(FATAL_ERROR "FWK_ID_FILE_LIST variable is not set, this must contain the input file list of framework IDs")
endif()

if(NOT DEFINED FWK_MSG_FILE_LIST)
    message(FATAL_ERROR "FWK_MSG_FILE_LIST variable is not set, this must contain the input file list of framework msgcodes")
endif()

# Setup variables for framework ID/message generation
set(FWK_ID_READ_LIST "")
set(FWK_ID_VAR_LIST "")
set(FWK_ID_COUNT "1")
set(FWK_MSG_READ_LIST "")
set(FWK_MSG_VAR_LIST "")
set(FWK_MSG_COUNT "1")

set(FWK_ID_HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/template/template_ids_top.h)
set(FWK_ID_FOOTER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/template/template_ids_end.h)
set(FWK_MSG_HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/template/template_msgcodes_top.h)
set(FWK_MSG_FOOTER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/template/template_msgcodes_end.h)

string(TIMESTAMP CURRENT_TIME "%d/%m/%Y @ %H:%M")
string(REPLACE ";" "\n * " FWK_ID_FILE_LIST_TEXTUAL "\n * ${FWK_ID_FILE_LIST}")
set(FWK_ID_FILE_HEADER "/* AUTOMATICALLY GENERATED FILE - DO NOT EDIT BY HAND\n * Generated: ${CURRENT_TIME}\n * Input file list:${FWK_ID_FILE_LIST_TEXTUAL}\n */\n")
set(FWK_ID_FILE_FOOTER "\n/* END OF AUTOMATICALLY GENERATED FILE */")
string(REPLACE ";" "\n * " FWK_MSG_FILE_LIST_TEXTUAL "\n * ${FWK_MSG_FILE_LIST}")
set(FWK_MSG_FILE_HEADER "/* AUTOMATICALLY GENERATED FILE - DO NOT EDIT BY HAND\n * Generated: ${CURRENT_TIME}\n * Input file list:${FWK_MSG_FILE_LIST_TEXTUAL}\n */\n")
set(FWK_MSG_FILE_FOOTER "\n/* END OF AUTOMATICALLY GENERATED FILE */")
set(GENERATED_PATH ${PROJECT_BINARY_DIR}/framework)

# Create framework folder
file(MAKE_DIRECTORY ${GENERATED_PATH})

# Remove previous file if present
file(REMOVE ${GENERATED_PATH}/framework_ids.h ${GENERATED_PATH}/framework_msgcodes.h)

# IDs

# Add header
list(APPEND FWK_ID_READ_LIST "set(FWK_ID_FILE_HEADER \"${FWK_ID_FILE_HEADER}\")\n")
list(APPEND FWK_ID_VAR_LIST "\${FWK_ID_FILE_HEADER}")
list(APPEND FWK_ID_READ_LIST "FILE(READ \"${FWK_ID_HEADER_FILE}\" FHEADERIN)\n")
list(APPEND FWK_ID_VAR_LIST "\${FHEADERIN}")

# Parse all framework ID input files
foreach (FWK_ID_FILE IN LISTS FWK_ID_FILE_LIST)
    # Update lists used for building framework ID file
    list(APPEND FWK_ID_READ_LIST "FILE(READ \"${FWK_ID_FILE}\" F${FWK_ID_COUNT}IN)\n")
    list(APPEND FWK_ID_VAR_LIST "\${F${FWK_ID_COUNT}IN}")

    # Increment framework ID file count
    math(EXPR FWK_ID_COUNT "${FWK_ID_COUNT}+1")
endforeach()

# Add footer
list(APPEND FWK_ID_READ_LIST "FILE(READ \"${FWK_ID_FOOTER_FILE}\" FFOOTERIN)\n")
list(APPEND FWK_ID_VAR_LIST "\${FFOOTERIN}")
list(APPEND FWK_ID_READ_LIST "set(FWK_ID_FILE_FOOTER \"${FWK_ID_FILE_FOOTER}\")\n")
list(APPEND FWK_ID_VAR_LIST "\${FWK_ID_FILE_FOOTER}")

# Convert lists into strings
string(REPLACE ";" "" FWK_ID_READ_LIST "${FWK_ID_READ_LIST}")
string(REPLACE ";" "" FWK_ID_VAR_LIST "${FWK_ID_VAR_LIST}")

# Create the framework ID cmake file
file(WRITE ${CMAKE_BINARY_DIR}/framework_ids.cmake "${FWK_ID_READ_LIST}
file(WRITE ${GENERATED_PATH}/framework_ids.h \"${FWK_ID_VAR_LIST}\")\n")

# Add a custom command to generate the output merged framework ID file
add_custom_command(
    OUTPUT ${GENERATED_PATH}/framework_ids.h
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/framework_ids.cmake
    DEPENDS ${FWK_ID_FILE_LIST}
)

# Message codes

# Add header
list(APPEND FWK_MSG_READ_LIST "set(FWK_MSG_FILE_HEADER \"${FWK_MSG_FILE_HEADER}\")\n")
list(APPEND FWK_MSG_VAR_LIST "\${FWK_MSG_FILE_HEADER}")
list(APPEND FWK_MSG_READ_LIST "FILE(READ \"${FWK_MSG_HEADER_FILE}\" FHEADERIN)\n")
list(APPEND FWK_MSG_VAR_LIST "\${FHEADERIN}")

# Parse all framework message code input files
foreach (FWK_MSG_FILE IN LISTS FWK_MSG_FILE_LIST)
    # Update lists used for building framework ID file
    list(APPEND FWK_MSG_READ_LIST "FILE(READ \"${FWK_MSG_FILE}\" F${FWK_MSG_COUNT}IN)\n")
    list(APPEND FWK_MSG_VAR_LIST "\${F${FWK_MSG_COUNT}IN}")

    # Increment framework message code file count
    math(EXPR FWK_MSG_COUNT "${FWK_MSG_COUNT}+1")
endforeach()

# Add footer
list(APPEND FWK_MSG_READ_LIST "FILE(READ \"${FWK_MSG_FOOTER_FILE}\" FFOOTERIN)\n")
list(APPEND FWK_MSG_VAR_LIST "\${FFOOTERIN}")
list(APPEND FWK_MSG_READ_LIST "set(FWK_MSG_FILE_FOOTER \"${FWK_MSG_FILE_FOOTER}\")\n")
list(APPEND FWK_MSG_VAR_LIST "\${FWK_MSG_FILE_FOOTER}")

# Convert lists into strings
string(REPLACE ";" "" FWK_MSG_READ_LIST "${FWK_MSG_READ_LIST}")
string(REPLACE ";" "" FWK_MSG_VAR_LIST "${FWK_MSG_VAR_LIST}")

# Create the framework ID cmake file
file(WRITE ${CMAKE_BINARY_DIR}/framework_msgcodes.cmake "${FWK_MSG_READ_LIST}
file(WRITE ${GENERATED_PATH}/framework_msgcodes.h \"${FWK_MSG_VAR_LIST}\")\n")

# Add a custom command to generate the output merged framework ID file
add_custom_command(
    OUTPUT ${GENERATED_PATH}/framework_msgcodes.h
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/framework_msgcodes.cmake
    DEPENDS ${FWK_MSG_FILE_LIST}
)

# Combined

# Make zephyr depend on the framework ID/message code generation as a dependency
add_custom_target(framework_gen DEPENDS ${GENERATED_PATH}/framework_ids.h ${GENERATED_PATH}/framework_msgcodes.h)
add_dependencies(zephyr framework_gen)

# Add the framework ID folder to the list of includes
zephyr_include_directories(${GENERATED_PATH})
