# TODO Do it with configure_file (example: https://stackoverflow.com/a/60644020)

set(MAIN_EXTENSIONS
    q2dviewer
    q3dviewer
    mpr2d
    dicomprint
    pdf
)

set(CONTRIB_EXTENSIONS
)

set(PLAYGROUND_EXTENSIONS
    angiosubstraction
    diffusionperfusionsegmentation
    edemasegmentation
    example
    experimental3d
    perfusionmapreconstruction
    rectumsegmentation
)

if(STARVIEWER_CE)
    set(ALL_EXTENSIONS
        ${MAIN_EXTENSIONS}
    )
elseif(STARVIEWER_LITE)
    set(ALL_EXTENSIONS
        q2dviewer
    )
else()
    set(ALL_EXTENSIONS
        ${MAIN_EXTENSIONS}
        ${CONTRIB_EXTENSIONS}
        ${PLAYGROUND_EXTENSIONS}
    )
endif()

set(EXTENSIONS_HEADER ${CMAKE_CURRENT_BINARY_DIR}/extensions.h)

file(WRITE ${EXTENSIONS_HEADER}
"#ifndef EXTENSIONS_H
#define EXTENSIONS_H

")

foreach(EXTENSION ${ALL_EXTENSIONS})
    file(APPEND ${EXTENSIONS_HEADER} "#include \"${EXTENSION}extensionmediator.h\"\n")
endforeach()

file(APPEND ${EXTENSIONS_HEADER}
"
void initExtensionsResources()
{
")

foreach(EXTENSION ${ALL_EXTENSIONS})
    file(APPEND ${EXTENSIONS_HEADER} "    Q_INIT_RESOURCE(${EXTENSION});\n")
endforeach()

file(APPEND ${EXTENSIONS_HEADER}
"}

#endif
")

# Add include directories of each extension globally
# TODO Probably there's a better way
foreach(EXTENSION ${ALL_EXTENSIONS})
    get_target_includes(${EXTENSION}_INCLUDES ${EXTENSION} YES)
    include_directories(${${EXTENSION}_INCLUDES})
endforeach()

target_link_libraries(starviewer ${ALL_EXTENSIONS})
