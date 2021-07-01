set(MANUALS_BASE_DIR ${CMAKE_SOURCE_DIR}/doc/help)
set(MANUALS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

file(GLOB_RECURSE FODT_MANUALS ${MANUALS_BASE_DIR}/*.fodt)

if(WIN32)
    set(LIBREOFFICE "C:/Program Files/LibreOffice/program/soffice.exe")
else()
    set(LIBREOFFICE soffice)
endif()

foreach(FODT_MANUAL ${FODT_MANUALS})
    string(REGEX REPLACE ".*\\[" "${MANUALS_OUTPUT_DIR}/[" PDF_MANUAL ${FODT_MANUAL})
    string(REPLACE "fodt" "pdf" PDF_MANUAL ${PDF_MANUAL})
    list(APPEND PDF_MANUALS ${PDF_MANUAL})
    list(APPEND COMMANDS
        COMMAND "${LIBREOFFICE}"
        ARGS --convert-to pdf ${FODT_MANUAL} --outdir "${MANUALS_OUTPUT_DIR}"
    )
endforeach()

# Note: pdfs must be created sequentially because LibreOffice doesn't allow concurrent instances. That's why a single command is created for all pdfs.
add_custom_command(
    OUTPUT ${PDF_MANUALS}
    ${COMMANDS}
    DEPENDS ${FODT_MANUALS}
    VERBATIM
)
