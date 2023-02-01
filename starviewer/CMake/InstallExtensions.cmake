# Adds an extension to the extensions list that is later used to configure the extensions header.
function(add_extension EXTENSION)
    set_property(GLOBAL APPEND PROPERTY EXTENSIONS ${EXTENSION})
endfunction()

# Configures the extensions header used in main.cpp.
function(generate_extensions_header)
    get_property(EXTENSIONS GLOBAL PROPERTY EXTENSIONS)

    list(TRANSFORM EXTENSIONS PREPEND "#include \"" OUTPUT_VARIABLE EXTENSIONS_INCLUDES)
    list(TRANSFORM EXTENSIONS_INCLUDES APPEND "extensionmediator.h\"")
    string(REPLACE ";" "\n" EXTENSIONS_INCLUDES "${EXTENSIONS_INCLUDES}")

    list(TRANSFORM EXTENSIONS PREPEND "    Q_INIT_RESOURCE(" OUTPUT_VARIABLE INIT_EXTENSIONS_RESOURCES)
    list(TRANSFORM INIT_EXTENSIONS_RESOURCES APPEND ");")
    string(REPLACE ";;" ";\n" INIT_EXTENSIONS_RESOURCES "${INIT_EXTENSIONS_RESOURCES}")

    configure_file(extensions.h.in extensions.h)

    # Add include directories of each extension globally
    foreach(EXTENSION ${EXTENSIONS})
        get_target_includes(${EXTENSION}_INCLUDES ${EXTENSION} YES)
        target_include_directories(starviewer PRIVATE ${${EXTENSION}_INCLUDES})
    endforeach()

    target_link_libraries(starviewer ${EXTENSIONS})
endfunction()
