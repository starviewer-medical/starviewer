# Include on hi ha tota la "lògica" de les traducions. Si es fa un include d'aquest fitxer
# i es respecta l'ubicació del directori extensions respecte on es fa l'include, en el Makefile
# que es generi hi apareixerà 2 targets nous: i18n_release i i18n_update per generar els .qm i actualitzar
# els .ts respectivament.
# El i18n_release es crida automàticament sempre que es necessita.

include(extensions.inc)

for(dir, PLAYGROUND_EXTENSIONS) {
    exists(extensions/playground/$$dir) {
        I18NPROJECTS += ../extensions/playground/$$dir
        I18NPROJECTS_TSFILES += ../extensions/playground/$$dir/*.ts
        I18NPROJECTS_QMFILES += ../extensions/playground/$$dir/*.qm
}
}

for(dir, CONTRIB_EXTENSIONS) {
    exists(extensions/contrib/$$dir) {
        I18NPROJECTS += ../extensions/contrib/$$dir
        I18NPROJECTS_TSFILES += ../extensions/contrib/$$dir/*.ts
        I18NPROJECTS_QMFILES += ../extensions/contrib/$$dir/*.qm
}
}

for(dir, MAIN_EXTENSIONS) {
    exists(extensions/main/$$dir) {
        I18NPROJECTS += ../extensions/main/$$dir
        I18NPROJECTS_TSFILES += ../extensions/main/$$dir/*.ts
        I18NPROJECTS_QMFILES += ../extensions/main/$$dir/*.qm
}
}

I18NPROJECTS += ../core ../interface ../inputoutput
I18NPROJECTS_TSFILES += ../core/*.ts ../interface/*.ts ../inputoutput/*.ts
I18NPROJECTS_QMFILES += ../core/*.qm ../interface/*.qm ../inputoutput/*.qm


i18n_update.commands = @for dir in $$I18NPROJECTS; do \
                             pushd .; \
                             cd \$$dir; \
                             $$[QT_INSTALL_PREFIX]/bin/lupdate -noobsolete *.pro; \
                             popd; \
                         done

i18n_release.target = i18n_release
i18n_release.depends = $$I18NPROJECTS_TSFILES
i18n_release.commands = @for dir in $$I18NPROJECTS; do \
                             pushd .; \
                             cd \$$dir; \
                             $$[QT_INSTALL_PREFIX]/bin/lrelease *.pro; \
                             popd; \
                         done \
                         && touch $$i18n_release.target

QMAKE_EXTRA_TARGETS += i18n_update i18n_release
PRE_TARGETDEPS += i18n_release
QMAKE_CLEAN += $$I18NPROJECTS_QMFILES
