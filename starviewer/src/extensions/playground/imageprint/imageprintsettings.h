#ifndef UDGIMAGEPRINTSETTINGS_H
#define UDGIMAGEPRINTSETTINGS_H

#include "defaultsettings.h"

namespace udg {

class ImagePrintSettings : public DefaultSettings
{
public:
    ImagePrintSettings();
    ~ImagePrintSettings();

    void init();

    ///Directori de l'Spool on es creen les imatges a imprimir a través d'una impressora DICOM
    static const QString SpoolDirectory;

};

} // end namespace udg 

#endif
