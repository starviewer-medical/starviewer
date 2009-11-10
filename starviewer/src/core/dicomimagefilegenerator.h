/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMIMAGEFILEGENERATOR_H
#define UDGDICOMIMAGEFILEGENERATOR_H

#include "dicomfilegenerator.h"


namespace udg {

class Volume;
class Image;


class DICOMImageFileGenerator : public DICOMFileGenerator
{
public:
    DICOMImageFileGenerator();

    ~DICOMImageFileGenerator();

    /// Afegir el volume a partir del qual s'ha de generar els fitxers
    void setInput( Volume * volume );

    /** Mètode encarregat de generar el/s fitxer/s a partir del volume introduït
        @pre Cal haver afegit un Volume \sa setInput
    */
    virtual bool generateDICOMFiles();

private:
    bool generateSCDICOMFiles();
    void fillGeneralImageInfo( DICOMWriter * writer , Image * image);
    void fillImagePixelInfo( DICOMWriter * writer , Image * image);
    void fillSCInfo(DICOMWriter * writer , Image * image );
    void fillSOPInfo(DICOMWriter * writer , Image * image );

private:
    Volume * m_input;

};

}

#endif // UDGDICOMIMAGEFILEGENERATOR_H
