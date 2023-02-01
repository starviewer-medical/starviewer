find_package(Qt5 COMPONENTS Core LinguistTools)

# Creates rules to call lupdate and lrelease for a folder. Additional directories can be given as optional parameters.
function(add_translations OUT_VAR TARGET TRANSLATION_BASE_NAME)
    set(LOCALES ca_ES en_GB es_ES)

    list(TRANSFORM LOCALES PREPEND "${TRANSLATION_BASE_NAME}_" OUTPUT_VARIABLE TS_FILES)
    list(TRANSFORM TS_FILES APPEND ".ts")
    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})

    qt5_add_translation(QM_FILES ${TS_FILES})

    list(TRANSFORM TS_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
    set(ADDITIONAL_DIRECTORIES ${ARGN})
    list(TRANSFORM ADDITIONAL_DIRECTORIES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

    # Add custom command to call lupdate
    add_custom_target(${TARGET}_i18n_update
        COMMAND ${Qt5_LUPDATE_EXECUTABLE} -no-obsolete ${CMAKE_CURRENT_SOURCE_DIR} ${ADDITIONAL_DIRECTORIES} -ts ${TS_FILES}
        VERBATIM
    )

    set_property(GLOBAL APPEND PROPERTY I18N_UPDATE_TARGETS ${TARGET}_i18n_update)

    set(${OUT_VAR} ${QM_FILES} PARENT_SCOPE)
endfunction()

# Creates a global target i18n_update to call lupdate for all targets that have translations
function(add_translation_targets)
    get_property(I18N_UPDATE_TARGETS GLOBAL PROPERTY I18N_UPDATE_TARGETS)

    add_custom_target(i18n_update
        DEPENDS ${I18N_UPDATE_TARGETS}
    )
endfunction()
