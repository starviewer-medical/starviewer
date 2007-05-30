
TEMPLATE = subdirs 

SUBDIRS = mpr \
          mpr3d \
          mpr3d2d

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../../compilationtype.inc)
