win32:CONFIG(debug, debug|release) {
    if (!contains(QMAKE_EXTRA_TARGETS, fixdebug)) {
        isfixed.depends = Makefile.Debug
        isfixed.target = Makefile.Debug.isfixed
        isfixed.commands = cscript \"$$PWD/makefixdebug.js\" $(MAKEFILE).Debug & echo > $${isfixed.target}
        fixdebug.depends = $${isfixed.target}
        fixdebug.target = FORCE
        QMAKE_EXTRA_TARGETS += fixdebug isfixed
        OTHER_FILES += makefixdebug.js
    }
}
