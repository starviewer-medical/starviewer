# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src
# L'objectiu és un subdirectori del projecte 

TEMPLATE = subdirs 

include(compilationtype.pri)


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

# Afegim els subdirs a compilar amb les seves dependències
addSubdirs(thirdparty)
addSubdirs(core)
addSubdirs(inputoutput, core)
addSubdirs(interface, core inputoutput)
addSubdirs(extensions, core inputoutput)
addSubdirs(main, interface extensions thirdparty)
addSubdirs(starviewersapwrapper)
addSubdirs(crashreporter, main core)
