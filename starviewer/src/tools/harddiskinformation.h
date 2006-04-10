/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef HARDDISKINFORMATION_H
#define HARDDISKINFORMATION_H

#include <QString>

namespace udg {

/**
Classe que ens permet obtenir informació sobre un disc dur. Més concretament ens permet obtenir els Byte, MB, GB d'una partició i quins d'aquests són lliures. Cal tenir present que quan es vol saber l'espai lliure d'un disc dur s'ha d'especificar de quina partició. En windows s'hauria d'especificar la unitat (c:, d:, ...) i un directori i en linux s'ha d'especificar la ruta sensera del directori per saber la partició. Actualment aquesta classe només suporta els sistemes operatius basats en Unix (Linux, Mac OS X ...).

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class HardDiskInformation{
public:
    /**
     * Constructor de la classe
     */
    HardDiskInformation();

    /**
     * Destructor de la classe
     */
    ~HardDiskInformation();

    /**
     * Mètode que ens serveix per saber el número de Bytes total (lliures+ocupats) que hi ha en una partició concreta. Per indicar la
     * partició s'ha d'especificar el path absolut a un fitxer o directori que estigui en aquesta.
     *
     * El mètode no comprova que existeixi el path.
     *
     * Si hi ha qualsevol error el mètode retornarà 0.
     * @param path Indica el path a un directori/fitxer dintre de la partició
     * @return El nombre total de bytes que té una partició
     */
    unsigned long getTotalNumberOfBytes(QString path);

    /**
     * Retorna el número de Bytes d'espai lliure que ens queden en una partició concreta i que poden ser utilitzats per l'usuari. Cal fer
     * notar que aquest número pot ser diferent del nombre real de bytes lliures (per exemple en casos que hi hagi quotes per usuari). Per
     * indicar la partició s'ha d'especificar el path absolut a un fitxer o directori que estigui en aquesta.
     *
     * El mètode no comprova que existeixi el path.
     *
     * Si hi ha qualsevol error el mètode retornarà 0.
     * @param path Indica el path a un directori/fitxer dintre de la partició
     * @return El nombre de bytes lliures que té una partició i poden ser utilitzats per l'usuari que executa el programa
     */
    unsigned long getNumberOfFreeBytes(QString path);

    /**
     * Es comporta exactament igual que getTotalNumberOfBytes() però retorna MBytes en comptes de Bytes. Cal tenir en compte, però, que
     * aquest no és un mètode per saber el número de MBytes amb absoluta precissió (per això fer servir getTotalNumberOfBytes() )
     * @param path Indica el path a un directori/fitxer dintre de la partició
     * @return El nombre de MBytes truncats (ex.: si és 1,9MBytes reals retornarà 1Mbytes)
     */
    unsigned long getTotalNumberOfMBytes(QString path);
    
    /**
     * Es comporta exactament igual que getNumberOfFreeBytes() però retorna MBytes en comptes de Bytes. Cal tenir en compte, però, que
     * aquest no és un mètode per saber el número de MBytes amb absoluta precissió (per això fer servir getNumberOfFreeBytes() )
     * @param path Indica el path a un directori/fitxer dintre de la partició
     * @return El nombre de MBytes lliures truncats (ex.: si és 1,9MBytes reals retornarà 1MByte)
     */
    unsigned long getNumberOfFreeMBytes(QString path);

private:
    quint64 getTotalBytesPlataformEspecific(QString path);
    quint64 getFreeBytesPlataformEspecific(QString path);

};

};  //  end  namespace udg

#endif
