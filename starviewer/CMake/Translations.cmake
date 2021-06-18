find_package(Qt5 COMPONENTS Core LinguistTools)

# This creates rules to call lrelease
function(add_translations OUT_VAR TRANSLATION_BASE_NAME)
    set(LOCALES ca_ES en_GB es_ES)

    list(TRANSFORM LOCALES PREPEND "${TRANSLATION_BASE_NAME}_" OUTPUT_VARIABLE TS_FILES)
    list(TRANSFORM TS_FILES APPEND ".ts")
    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})

    qt5_add_translation(QM_FILES ${TS_FILES})

    set_property(GLOBAL APPEND PROPERTY TS_FILES ${TS_FILES})
    set_property(GLOBAL APPEND PROPERTY QM_FILES ${QM_FILES})

    set(${OUT_VAR} ${QM_FILES} PARENT_SCOPE)
endfunction()






# Objectiu: 2 targets de Makefile: i18n_update, i18n_release
# i18n_update ->    qt5_create_translation(QM_FILES target_source_dir target_source_dir/foo_ca_ES.ts target_source_dir/foo_en_GB.ts target_source_dir/foo_es_ES.ts -no-obsolete)
#       Cal saber: per cada target: target i nom base de la traducció (<- afegir propietat al target)
#
# i18n_release ->   set(TS_FILES helloworld_en.ts helloworld_de.ts)
#                   set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "l10n")
#                   qt5_add_translation(qmFiles ${TS_FILES})

function(add_translation_targets)
    find_package(Qt5 COMPONENTS LinguistTools)

    get_property(I18N_TARGETS GLOBAL PROPERTY I18N_TARGETS)
    set(LOCALES ca_ES en_GB es_ES)

    foreach(TARGET ${I18N_TARGETS})
        get_target_property(TARGET_SOURCE_DIR ${TARGET} SOURCE_DIR)
        get_target_property(TRANSLATION_BASE_NAME ${TARGET} TRANSLATION_BASE_NAME)
        list(TRANSFORM LOCALES PREPEND "${TRANSLATION_BASE_NAME}_" OUTPUT_VARIABLE TS_FILES)
        list(TRANSFORM TS_FILES APPEND ".ts")
        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${TARGET_SOURCE_DIR})

        message("qt5_create_translation(QM_FILES ${TARGET_SOURCE_DIR} ${TS_FILES})")
        qt5_create_translation(QM_FILES ${TARGET_SOURCE_DIR} ${TS_FILES})
        message("Results: ${QM_FILES}")
        message("qt5_add_translation(QM_FILES_2 ${TS_FILES})")
        qt5_add_translation(QM_FILES_2 ${TS_FILES})
        message("Results: ${QM_FILES_2}")
    endforeach()
endfunction()


#[[
Resum del que he descobert fins ara:

add_custom_command crea una regla de makefile per crear un fitxer però només s'arriba a escriure al makefile si algú en depèn

add_custom_target permet crear una regla que sempre arriba al makefile
                  pot servir per assegurar que les regles creades per qt es quedin al makefile encara que no afegim dependències explícites des dels projectes

qt5_create_translation fa add_custom_command per cada .ts que depèn del directori (si es s'afegeix o s'esborra un fitxer del directori, es regenera el .ts) i crida lupdate
                       també crida qt5_add_translation

qt5_add_translation fa add_custom_command per cada .qm que depèn del .ts corresponent i crida lrelease

No es sobreescriure un custom command

Problema:
- la dependència directa del directori farà que es cridi lupdate molt sovint. Ens interessa?

Idees:
- add_custom_target per i18n_update i i18n_release, amb el que calgui
- podem passar de les macros de qt i fer uns commands propis?
  - potser podríem passar només de qt5_create_translation, que és la que crea la dependència del directori, i fer i18n_update a la nostra manera
    - per cridar lupdate: ${Qt5_LUPDATE_EXECUTABLE}
    - mirar què fan la macro de Qt i el que tenim a translations.pri (important -no-obsolete)
- Caldria executar una funció des de cada target per saber els qm i afegir-los com a dependències, i alhora recollir-los en una propietat global per després en una funció final afegir els custom targets
]]
