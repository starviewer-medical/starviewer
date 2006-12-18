
TEMPLATE = subdirs 

SUBDIRS = testing

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../compilationtype.inc)
