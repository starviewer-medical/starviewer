win32:CONFIG(debug, debug|release) {
    if (!contains(QMAKE_EXTRA_TARGETS, fixdebug)) {
        fixdebug.depends = Makefile
        fixdebug.target = $(MAKEFILE).Debug
        fixdebug.commands = cscript \"$$PWD/makefixdebug.js\" $${fixdebug.target}
        QMAKE_EXTRA_TARGETS += fixdebug
        OTHER_FILES += makefixdebug.js
    }
}
