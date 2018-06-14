#Afegim els projectes de tercers que s'han de compilar que utilitza Starviewer.

TEMPLATE = subdirs 
SUBDIRS = qtsingleapplication breakpad easylogging++

include(../compilationtype.pri)
