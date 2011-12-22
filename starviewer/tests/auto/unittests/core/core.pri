
SOURCES += $$PWD/test_image.cpp \
           $$PWD/test_imageorientation.cpp \
           $$PWD/test_dicomvaluerepresentationconverter.cpp \
           $$PWD/test_machineidentifier.cpp \
           $$PWD/test_patientorientation.cpp \
           $$PWD/test_anatomicalplane.cpp \
           $$PWD/test_imageorientationoperationsmapper.cpp \
           $$PWD/test_volume.cpp \
           $$PWD/test_q2dviewer.cpp \
           $$PWD/test_machineinformation.cpp \
           $$PWD/test_opacitytransferfunction.cpp \
           $$PWD/test_series.cpp \
           $$PWD/test_study.cpp \
           $$PWD/test_dicomsource.cpp \
           $$PWD/test_patient.cpp \
           $$PWD/test_mathtools.cpp \
           $$PWD/test_hangingprotocol.cpp \
           $$PWD/test_imageoverlay.cpp \
           $$PWD/test_imageoverlayreader.cpp \
           $$PWD/test_drawerbitmap.cpp \
           $$PWD/test_displayshutter.cpp \
           $$PWD/test_firewallaccesstest.cpp \
           $$PWD/test_dicomtagreader.cpp \
           $$PWD/test_patientfillerinput.cpp \
           $$PWD/test_automaticsynchronizationtooldata.cpp \
           $$PWD/test_volumepixeldata.cpp \
           $$PWD/test_drawertext.cpp \
           $$PWD/test_applicationupdatechecker.cpp \
           $$PWD/test_commandlineoption.cpp \
           $$PWD/test_applicationversiontest.cpp \
           $$PWD/test_imageoverlayregionfinder.cpp \
           $$PWD/test_hangingprotocolmanager.cpp \
           $$PWD/test_systemrequerimentstest.cpp \
           $$PWD/test_drawerpolygon.cpp \
           $$PWD/test_diagnosistestresultwriter.cpp

win32 {
    SOURCES += $$PWD/test_windowsfirewallaccess.cpp \
               $$PWD/test_windowssysteminformation.cpp
}
