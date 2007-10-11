include(../../extensions.inc)

TEMPLATE = subdirs

for(dir, PLAYGROUND_EXTENSIONS) {
    exists($$dir) {
        SUBDIRS += $$dir
    }
}

include(../../compilationtype.inc)
