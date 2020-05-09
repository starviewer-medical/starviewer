# TL;DR do fixdebug unless nofixdebug is specified in CONFIG+=nofixdebug
# The following code is used to compile Starviewer on Windows with debug mode on a release compiled SDK.
win32:!nofixdebug:CONFIG(debug, debug|release) {
    if (!contains(QMAKE_EXTRA_TARGETS, fixdebug)) {
		message("Applying FIXDEBUG")
        isfixed.depends = Makefile.Debug
        isfixed.target = Makefile.Debug.isfixed
        isfixed.commands = cscript \"$$PWD/makefixdebug.js\" $(MAKEFILE).Debug & echo > $${isfixed.target}
        fixdebug.depends = $${isfixed.target}
        fixdebug.target = FORCE
        QMAKE_EXTRA_TARGETS += fixdebug isfixed
        OTHER_FILES += makefixdebug.js
    }
}
