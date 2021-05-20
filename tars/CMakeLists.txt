cmake_minimum_required(VERSION 3.15)

project(Tars-Service-Protocols-TARS)

file(GLOB_RECURSE TARS_INPUT *.tars)
if (TARS_INPUT)
    foreach(TARS_FILE ${TARS_INPUT})
        get_filename_component(TARS_NAME ${TARS_FILE} NAME_WE)
        get_filename_component(TARS_PATH ${TARS_FILE} PATH)

        add_custom_command(
            OUTPUT ${TARS_HEADER_DIR}/${TARS_NAME}.h
            WORKING_DIRECTORY ${TARS_PATH}
            COMMAND ${TARS2CPP} ${TARS_TOOL_FLAG} ${TARS_FILE} --dir=${TARS_HEADER_DIR}
            COMMENT "${TARS2CPP} ${TARS_TOOL_FLAG} ${TARS_FILE} --dir=${TARS_HEADER_DIR}"
            DEPENDS ${TARS2CPP} ${TARS_FILE}
        )
        list(APPEND OUT_TARS_H_LIST ${TARS_HEADER_DIR}/${TARS_NAME}.h)
    endforeach()
endif()

add_custom_target(bcostars ALL DEPENDS ${OUT_TARS_H_LIST})
add_dependencies(bcostars tarscpp)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${OUT_TARS_H_LIST}")