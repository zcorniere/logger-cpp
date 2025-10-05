function(generate_export_header macro_name output_path)

    configure_file("${CMAKE_SOURCE_DIR}/cmake/EngineExport.hxx.in" "${output_path}" @ONLY)
endfunction(generate_export_header)

function(generate_build_config_header)
    set(options)
    set(oneValueArgs MACRO_NAME OUTPUT_PATH VERSION TEMPLATE)
    set(multiValueArgs)
    cmake_parse_arguments(
        GBCH
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    # Compose the define name from macro_name
    string(REPLACE "_API" "_EXPORTS" macro_define "${GBCH_MACRO_NAME}")

    # Use set_property to cache the values for configure_file
    set(EXPORT_MACRO "${GBCH_MACRO_NAME}")
    set(EXPORT_DEFINE "${macro_define}")

    # Get git commit hash (short)
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GBCH_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    configure_file("${GBCH_TEMPLATE}" "${GBCH_OUTPUT_PATH}" @ONLY)

    message(STATUS "Generated build config header at ${GBCH_OUTPUT_PATH}:")
    message(STATUS "  Version: ${GBCH_VERSION}")
    message(STATUS "  Git Hash: ${GBCH_GIT_HASH}")
endfunction(generate_build_config_header)
