/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCREATEDICOMDIR_H
#define UDGCREATEDICOMDIR_H

class string;

namespace udg {

class Status;

/** Converteix un directori determinat a dicomdir. Per a que un directori es pugui convertir a dicomdir, els fitxer dicom han de tenir un nom com a molt de 8 caràcters, i han d'estar guardats en little endian
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CreateDicomdir
{
public:

    CreateDicomdir();
        
    /** Crea el fitxer DicomDir amb l'estructura dels estudis del directori passat per paràmetre
     * @param dicomdirPath directori a convertir a dicomdir
     * @return estat de finalització del mètode
     */
    Status create( std::string dicomdirPath );
    
    ~CreateDicomdir();

};

}

#endif
