function(get_target_includes OUT_VAR TARGET INCLUDE_UIC)
    get_target_property(TARGET_SOURCE_DIR ${TARGET} SOURCE_DIR)

    if(INCLUDE_UIC)
        get_target_property(TARGET_NAME ${TARGET} NAME)
        get_target_property(TARGET_BINARY_DIR ${TARGET} BINARY_DIR)
        set(${OUT_VAR}
            ${TARGET_SOURCE_DIR}                                # normal headers
            ${TARGET_BINARY_DIR}/${TARGET_NAME}_autogen/include # uic-generated headers
            PARENT_SCOPE)
    else()
        set(${OUT_VAR} ${TARGET_SOURCE_DIR} PARENT_SCOPE)
    endif()
endfunction()
