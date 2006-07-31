
TEMPLATE = subdirs 

SUBDIRS = testing testing2

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}
