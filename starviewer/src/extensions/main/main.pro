include(../../extensions.inc)

TEMPLATE = subdirs 

for(dir, MAIN_EXTENSIONS) {
    exists($$dir) {
        SUBDIRS += $$dir
    }
}

include(../../compilationtype.inc)
