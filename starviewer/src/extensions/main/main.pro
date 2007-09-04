
TEMPLATE = subdirs 

SUBDIRS = mpr \
          mpr3d \
          mpr3d2d \
	  q2dviewer

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../../compilationtype.inc)
