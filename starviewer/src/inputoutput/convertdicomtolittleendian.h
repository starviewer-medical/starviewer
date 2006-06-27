/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCONVERTDICOMTOLITTLEENDIAN_H
#define UDGCONVERTDICOMTOLITTLEENDIAN_H

class string;

namespace udg {

class Status;

/** Converteix les imatges guardades al format littleEndian, necessari per crear els DicomDir
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ConvertDicomToLittleEndian
{
public:

    ConvertDicomToLittleEndian();

    /** Converteix el fitxer d'entrada dicom a format little endian i el guarda, amb el nom i directori que s'indiqui a outputfile
     * @param inputFile  ruta completa del fitxer a convertir
     * @param outputFile ruta completa indicant on s'ha de desar el fitxer convertit, s'hi ha d'incloure el nom del fitxer 
     * @return 
     */
    Status convert( std::string inputFile , std::string outputFile );

    ~ConvertDicomToLittleEndian();
};

}

#endif
