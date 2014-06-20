win32:CONFIG(debug, debug|release) {
    if (!contains(QMAKE_EXTRA_TARGETS, fixdebug)) {
        fixdebug.target = FORCE
        fixdebug.commands = cscript \"$$PWD/makefixdebug.js\" $(MAKEFILE).Debug
        QMAKE_EXTRA_TARGETS += fixdebug
        OTHER_FILES += makefixdebug.js
    }
}
