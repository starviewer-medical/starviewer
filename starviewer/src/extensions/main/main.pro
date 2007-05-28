
TEMPLATE = subdirs 

#SUBDIRS = 

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../../compilationtype.inc)
