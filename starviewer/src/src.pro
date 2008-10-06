# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src
# L'objectiu és un subdirectori del projecte 

TEMPLATE = subdirs 

include(compilationtype.inc)


# addSubdirs(subdirs,deps): Adds directories to the project that depend on
# other directories
defineTest(addSubdirs) {
    for(subdirs, 1) {
        entries = $$files($$subdirs)
        for(entry, entries) {
            name = $$replace(entry, [/\\\\], _)
            SUBDIRS += $$name
            eval ($${name}.subdir = $$entry)
            for(dep, 2):eval ($${name}.depends += $$replace(dep, [/\\\\], _))
            export ($${name}.subdir)
            export ($${name}.depends)
        }
    }
    export (SUBDIRS)
}

# Fake subdirs per KDevelop
SUBDIRS = core inputoutput interface extensions main crashreporter
for(DIR, SUBDIRS){
 SUBDIRS -= $$DIR
}

# Afegim els subdirs reals
addSubdirs(core)
addSubdirs(inputoutput, core)
addSubdirs(interface, core inputoutput)
addSubdirs(extensions, core)
addSubdirs(main, interface extensions)

# En linux 64bits, de moment, breakpad no està suportat, per tant, no compilem res relacionat.
include(detectlinux64.inc)
!linux_x86_64 {
  addSubdirs(crashreporter)
}