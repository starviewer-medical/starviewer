include(../../extensions.inc)

TEMPLATE = subdirs

for(dir, CONTRIB_EXTENSIONS) {
    exists($$dir) {
        SUBDIRS += $$dir
    }
}

include(../../compilationtype.inc)
