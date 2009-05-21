include(defaultdirectories.inc)

cuda {
    DEFINES += CUDA_AVAILABLE
    # aquest és per activar la funció de debug de cutil
    debug:DEFINES += _DEBUG
    INCLUDEPATH += $${CUDAINCLUDEDIR} $${CUDASDKINCLUDEDIR}

    unix {
        LIBS += -L$${CUDALIBDIR} -lcudart
        QMAKE_CUC = nvcc
    }
    wind32 {
        LIBS += $${CUDALIBDIR}\cuda.lib $${CUDALIBDIR}\cudart.lib
        QMAKE_CUC = $(CUDABINDIR)/nvcc.exe
    }

    CUFLAGS = -arch compute_13 -code compute_13,sm_13 --ptxas-options=-v
    # altres flags: -v -g -deviceemu --keep

    #
    # Cuda extra-compiler for handling files specified in the CUSOURCES variable
    #

    cu.name = Cuda ${QMAKE_FILE_IN}
    cu.input = CUSOURCES
    cu.CONFIG += no_link
    cu.variable_out = OBJECTS

    isEmpty(CU_DIR):CU_DIR = .
    isEmpty(QMAKE_CPP_MOD_CU):QMAKE_CPP_MOD_CU =
    isEmpty(QMAKE_EXT_CPP_CU):QMAKE_EXT_CPP_CU = .cu

    QMAKE_CUFLAGS += $$QMAKE_CXXFLAGS
    DebugBuild:QMAKE_CUFLAGS += $$QMAKE_CXXFLAGS_DEBUG
    ReleaseBuild:QMAKE_CUFLAGS += $$QMAKE_CXXFLAGS_RELEASE
    QMAKE_CUFLAGS += $$QMAKE_CXXFLAGS_RTTI_ON $$QMAKE_CXXFLAGS_WARN_ON $$QMAKE_CXXFLAGS_STL_ON
#    message(QMAKE_CUFLAGS: $$QMAKE_CUFLAGS)

    QMAKE_CUEXTRAFLAGS += -Xcompiler $$join(QMAKE_CUFLAGS, ",") $$CUFLAGS
    QMAKE_CUEXTRAFLAGS += $(DEFINES) $(INCPATH) $$join(QMAKE_COMPILER_DEFINES, " -D", -D)
    QMAKE_CUEXTRAFLAGS += -c
#    QMAKE_CUEXTRAFLAGS += --keep
    QMAKE_EXTRA_VARIABLES += QMAKE_CUEXTRAFLAGS

    cu.commands = $$QMAKE_CUC $(EXPORT_QMAKE_CUEXTRAFLAGS) -o $$OBJECTS_DIR/$${QMAKE_CPP_MOD_CU}${QMAKE_FILE_BASE}$${QMAKE_EXT_OBJ} ${QMAKE_FILE_NAME}$$escape_expand(\n\t)
    cu.output = $$OBJECTS_DIR/$${QMAKE_CPP_MOD_CU}${QMAKE_FILE_BASE}$${QMAKE_EXT_OBJ}
    silent:cu.commands = @echo nvcc ${QMAKE_FILE_IN} && $$cu.commands
    QMAKE_EXTRA_COMPILERS += cu

    build_pass|isEmpty(BUILDS):cuclean.depends = compiler_cu_clean
    else:cuclean.CONFIG += recursive
    QMAKE_EXTRA_TARGETS += cuclean
}
