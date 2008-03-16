/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTOREIMAGES_H
#define UDGSTOREIMAGES_H

#include <list>

class T_ASC_Association;
class T_ASC_Network;

namespace udg {

class Status;
class ImageList;
class PacsConnection;

//codi d'error que retorna l'store a part del OFCondition, aquest status code també s'ha de comprovar
static int m_lastStatusCode;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class StoreImages{
public:
    StoreImages();

    /** This action sets the connection that we will use to connect to the pacs
     * @param   connection [in] Study's Open connection to the pacs
     */
   void setConnection( PacsConnection connection );

   /** Sets the network to use for retrieve the images
    * @param Network to use
    */
   void setNetwork ( T_ASC_Network * network );

   /** Guarda les imatges que s'especifiquen a la llista en el pacs establert per la connexió
    * @param imagePath llista amb el path de les imatges a guardar
    * @return indica estat del mètode
    */
   Status store( ImageList imageList );

    ~StoreImages();

private :
    T_ASC_Association *m_assoc; // request DICOM association;
    T_ASC_Network *m_net;

};

}

#endif
