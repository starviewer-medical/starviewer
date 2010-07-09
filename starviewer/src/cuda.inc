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
    win32 {
        LIBS += $${CUDALIBDIR}\cuda.lib $${CUDALIBDIR}\cudart.lib
        QMAKE_CUC = $(CUDABINDIR)/nvcc.exe
    }

    #CUFLAGS = -arch compute_13 -code compute_13,sm_13 --ptxas-options=-v
    CUFLAGS = -gencode=arch=compute_13,code=sm_13 -gencode=arch=compute_13,code=compute_13 --ptxas-options=-v
    # altres flags: -v -g -deviceemu --keep
    #CUFLAGS = \
    #   -gencode=arch=compute_10,code=sm_10 \
    #   -gencode=arch=compute_10,code=compute_10 \
    #   -gencode=arch=compute_11,code=sm_11 \
    #   -gencode=arch=compute_11,code=compute_11 \
    #   -gencode=arch=compute_13,code=sm_13 \
    #   -gencode=arch=compute_13,code=compute_13 \
    #   -gencode=arch=compute_20,code=sm_20 \
    #   -gencode=arch=compute_20,code=compute_20

    #
    # Cuda extra-compiler for handling files specified in the CUSOURCES variable
    #

    cu.name = Cuda ${QMAKE_FILE_IN}
    cu.input = CUSOURCES
    cu.CONFIG += no_link
    cu.variable_out = OBJECTS

    # Per g++ 4.4
    #unix:QMAKE_CUFLAGS += -D__builtin_stdarg_start=__builtin_va_start -D__sync_fetch_and_add=

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
#    QMAKE_CUEXTRAFLAGS += -v
#    QMAKE_CUEXTRAFLAGS += -keep
#    QMAKE_CUEXTRAFLAGS += -clean
    QMAKE_EXTRA_VARIABLES += QMAKE_CUEXTRAFLAGS

    cu.commands = $$QMAKE_CUC $(EXPORT_QMAKE_CUEXTRAFLAGS) -o $$OBJECTS_DIR/$${QMAKE_CPP_MOD_CU}${QMAKE_FILE_BASE}$${QMAKE_EXT_OBJ} ${QMAKE_FILE_NAME}$$escape_expand(\n\t)
    cu.output = $$OBJECTS_DIR/$${QMAKE_CPP_MOD_CU}${QMAKE_FILE_BASE}$${QMAKE_EXT_OBJ}
    silent:cu.commands = @echo nvcc ${QMAKE_FILE_IN} && $$cu.commands
    QMAKE_EXTRA_COMPILERS += cu

    build_pass|isEmpty(BUILDS):cuclean.depends = compiler_cu_clean
    else:cuclean.CONFIG += recursive
    QMAKE_EXTRA_TARGETS += cuclean
}
