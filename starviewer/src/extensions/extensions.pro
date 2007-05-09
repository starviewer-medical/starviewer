
TEMPLATE = subdirs 

SUBDIRS = testing \
          diffusionperfusionsegmentation

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../compilationtype.inc)
