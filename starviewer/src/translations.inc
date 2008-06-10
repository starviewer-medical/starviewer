# Include on hi ha tota la "lògica" de les traducions. Si es fa un include d'aquest fitxer
# i es respecta l'ubicació del directori extensions respecte on es fa l'include, en el Makefile
# que es generi hi apareixerà 2 targets nous: i18n_release i i18n_update per generar els .qm i actualitzar
# els .ts respectivament.
# El i18n_release es crida automàticament sempre que es necessita.

include(extensions.inc)

for(directory, PLAYGROUND_EXTENSIONS) {
    exists(extensions/playground/$$directory) {
        I18NPROJECTS += ../extensions/playground/$$directory
        I18NPROJECTS_TSFILES += ../extensions/playground/$$directory/*.ts
        I18NPROJECTS_QMFILES += ../extensions/playground/$$directory/*.qm
}
}

for(directory, CONTRIB_EXTENSIONS) {
    exists(extensions/contrib/$$directory) {
        I18NPROJECTS += ../extensions/contrib/$$dir
        I18NPROJECTS_TSFILES += ../extensions/contrib/$$directory/*.ts
        I18NPROJECTS_QMFILES += ../extensions/contrib/$$directory/*.qm
}
}

for(directory, MAIN_EXTENSIONS) {
    exists(extensions/main/$$directory) {
        I18NPROJECTS += ../extensions/main/$$directory
        I18NPROJECTS_TSFILES += ../extensions/main/$$directory/*.ts
        I18NPROJECTS_QMFILES += ../extensions/main/$$directory/*.qm
}
}

I18NPROJECTS += ../core ../interface ../inputoutput
I18NPROJECTS_TSFILES += ../core/*.ts ../interface/*.ts ../inputoutput/*.ts
I18NPROJECTS_QMFILES += ../core/*.qm ../interface/*.qm ../inputoutput/*.qm

unix {
i18n_update.commands = @for directory in $$I18NPROJECTS; do \
                             cd \$$directory; \
                             $$[QT_INSTALL_PREFIX]/bin/lupdate -noobsolete *.pro; \
                             cd -; \
                         done
}
win32 {
i18n_update.commands = for %d in ($$I18NPROJECTS) do \
                             pushd . &\
                             cd %d & \
                             $$[QT_INSTALL_PREFIX]\bin\lupdate -noobsolete %~nxd.pro & \
                             popd
}
i18n_release.target = i18n_release
i18n_release.depends = $$I18NPROJECTS_TSFILES

unix {
i18n_release.commands = @for directory in $$I18NPROJECTS; do \
                             cd \$$directory; \
                             $$[QT_INSTALL_PREFIX]/bin/lrelease *.pro; \
                             cd -; \
                         done \
                         && touch $$i18n_release.target
}
win32 {
i18n_release.commands = for %d in ($$I18NPROJECTS) do \
                             pushd . &\
                             cd %d & \
                             $$[QT_INSTALL_PREFIX]\bin\lrelease %~nxd.pro & \
                             popd &\
                             echo touch > $$i18n_release.target
}
QMAKE_EXTRA_TARGETS += i18n_update i18n_release
PRE_TARGETDEPS += i18n_release
QMAKE_CLEAN += $$I18NPROJECTS_QMFILES $$i18n_release.target
