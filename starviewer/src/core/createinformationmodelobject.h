/***************************************************************************
 *   Copyright (C) 2005-2009 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef CREATEINFORMATIONMODELOBJECT_H
#define CREATEINFORMATIONMODELOBJECT_H

namespace udg {

class Patient;
class Study;
class Series;
class Image;
class DICOMTagReader;

/**
 * Converteix Objects Dataset de dcmtk a objectes d'Starvieweer 
 */
class CreateInformationModelObject{

public:
    ///Crea un patient a partir d'un DICOMTagReader
    static Patient *createPatient(DICOMTagReader *dicomTagReader);

    ///Crea un Study a partir d'un DICOMTagReader
    static Study *createStudy(DICOMTagReader *dicomTagReader);

    ///Crea un Series a partir d'un DICOMTagReader
    static Series *createSeries(DICOMTagReader *dicomTagReader);

    ///Crea un Image a partir d'un DICOMTagReader
    static Image *createImage(DICOMTagReader *dicomTagReader);

};

};  //  end  namespace udg

#endif
