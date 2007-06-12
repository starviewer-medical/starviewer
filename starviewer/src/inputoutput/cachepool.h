/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHEPOOL_H
#define UDGCACHEPOOL_H

#include "databaseconnection.h"

namespace udg {

class Status;

/** Classe que s'encarrega de gestionar l'spool de la caché
@author Grup de Gràfics de Girona  ( GGG )
*/

class CachePool
{
public:

    static const unsigned int MinimumMBytesOfDiskSpaceRequired = 1000;///<Espai mínim lliure requerit al disc dur
    static const unsigned int MBytesToEraseWhenDiskOrCacheFull = 2000;///<Si la cache esta plena, s'allibera aquesta quanttiat de Mb en estudis vells

    /// Constructor de la classe
    CachePool();

    /** Esborra un estudi de l'spool de l'aplicació
     * @param path absolut de l'estudi
     */
    void removeStudy( QString studyUID );

    /** actualitza l'espai utiltizat de la cache a 0 bytes
     * @return estat el mètode
     */
    Status resetPoolSpace();

    /** Actualitza l'espai utilitzat de caché, amb la quantitat de bytes passats per paràmetre
     * @param mida a actualitzar
     * @return retorna estat del mètode
     */
    Status updatePoolSpace( int );

    /** Retorna l'espai que estem utilitzan de l'spool en Mb
     * @param  Espai ocupat del Pool (la caché) actualment en Mb
     * @return estat el mètode
     */
    Status getPoolUsedSpace( unsigned int &usedSpace );

    /** Retorna l'espai màxim que pot ocupar el Pool(Tamany de la caché)
     * @param  Espai assignat en Mb que pot ocupar el Pool (la caché)
     * @return estat el mètode
     */
    Status getPoolTotalSize( unsigned int &totalSize );

    /** actualitza l'espai màxim disponible per a la caché
     * @param  Espai en Mb que pot ocupar la caché
     * @return estat el mètode
     */
    Status updatePoolTotalSize( int );

    /** Calcula l'espai lliure disponible a la Pool
     * @param space  Espai lliure en Mb de la Pool (la caché)
     * @return estat el mètode
     */
    Status getPoolFreeSpace( unsigned int &freeSpace );

    ///Destructor de classe
    ~CachePool();

private:

    DatabaseConnection *m_DBConnect;

};
};

#endif
