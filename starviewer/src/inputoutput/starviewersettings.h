/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTARVIEWERSETTINGS_H
#define UDGSTARVIEWERSETTINGS_H

#include <QSettings>

class QString;

namespace udg {

/** Aquesta classe permet accedir i guardar els parametres de configuracio de l'starviewer
@author marc
*/
class StarviewerSettings{
public:
    StarviewerSettings();
    ~StarviewerSettings();

    //Cache
	/** estableix el path de la base de dades
	 * @param path de la base de dades local
	 */
    void setDatabasePath( QString );

    ///Espai lliure mínim necessari expressat en Gbytes per poder descarregar objectes dicom
    void setMinimumSpaceRequiredToRetrieveInGbytes(uint minimumSpaceRequired);

	/** estableix el path on es guarden les imatges de la cache
	 * @param path de la cache
	 */
    void setCacheImagePath( QString );

    /**Permet indicar si s'han d'esborrar o no els estudis que no han estat visualitzats uns determinats passat un determinats dies.
      *El número de dies s'especifica a través del mètode setMaximumDaysNotViewedStudy
      */
    void setDeleteOldStudiesHasNotViewedInDays(bool deleteOldStudies);

    /**Permet indicar si s'ha d'alliberar espai de forma automàtica esborrant estudis vells quan no hi ha suficient 
      *espai lliure per més descàrregues/importacions
      */
    void setDeleteOldStudiesIfNotEnoughSpaceAvailable(bool deletedOldStudiesIfNotEnoughSpaceAvailable);

    /** Permet Indicar el Gbytes d'objectes dicom que s'han d'esborrar en cas que no hi hagi suficient espai per noves
      * descàrregues/importacions
      */
    void setGbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable(uint minimumSpaceRequired);

	/** Estableix el número de dies màxim que un estudi pot estar a la cache sense ser vist ,  a partir d'aquest número de dies l'estudi és esborrat
	 * @param número maxim de dies
	 */
    void setMaximumDaysNotViewedStudy( QString  );

	/** retorna el path de la base de dades
	 * @return retorna el path de la base de dades
	 */
    QString getDatabasePath();

    ///retorna si s'han d'esborrar els estudis que no han estat visualitzats en un determinat número de dies    
    bool getDeleteOldStudiesHasNotViewedInDays();

    ///retorna si s'han d'esborrar estudis vell si no hi ha suficient espai per descarregar/importar nous estudis
    bool getDeleteOldStudiesIfNotEnoughSpaceAvailable();

    ///Retorna el Gbytes a esborrar d'objectes dicom di no hi ha suficent espai per descarregar-ne/importar-ne de nous
    uint getGbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable();

    ///Retorna l'espai mínim necessari en Gbytes al disc dur per poder descàrregar objectes dicom
    uint getMinimumSpaceRequiredToRetrieveInGbytes();
    ///Retorna l'espai mínim necessari en Mbytes al disc dur per poder descàrregar objectes dicom
    uint getMinimumSpaceRequiredToRetrieveInMbytes();

	/** retorna el Path on es guarden les imatges
	 * @return retorn el path de la cache
	 */
    QString getCacheImagePath();

	/** retorna el número màxim de dies que un estudi pot estar a la caché sense ser visualitzat
	 * @return retorn el path de la cache
	 */
    QString getMaximumDaysNotViewedStudy();

    //Pacs
	/** guarda el AETitle de la màquina
	 * @param AETitle de la màquina
	 */
    void setAETitleMachine( QString );

	/** estableix el Time out
	 * @param Time out
	 */
    void setTimeout( QString );

	/** guarda el port Local pel qual rebrem les imatges descarregades
	 * @param port local per la descarrega d'imatges
	 */
    void setLocalPort( QString );

	/** Nombre màxim de connexions simultànies al PACS
	 * @param nombre màxim de connexions
  	 */
    void setMaxConnections( QString );

	/** retorna el AEtitle de la màquina
	 * @return AETitle de la màquina
	 */
    QString getAETitleMachine();

	/** retorna el time out de l'aplicacio
	 * @return timeout
 	 */
    QString getTimeout();

	/** retorna el port pel qual esperem rebre les imatges descarregades
	 * @return port Local
	 */
    QString getLocalPort();

	/** retorna el nombre màxim de connexions simultànies que es poden tenir atacant a un PACS
	 * @return nombre màxim de connexions
	 */
    QString getMaxConnections();

    /** Estableix si l'aplicació s'ha d'executar en mode verbose o no (les comunicacions del pacs surten per cònsola)
     * @param VerboseMode indica si l'aplicació s'executa en mode verbose
     */
    void setLogCommunicationPacsVerboseMode(bool VerboseMode);

    /** retorn si hem d'executar l'aplicació en verbose Mode,
      * @return indica si s'executa l'aplicació en verbode Mode
      */
    bool getLogCommunicationPacsVerboseMode();

	/// Path amb l'últim directori on hem obert un DICOMDIR
	QString getLastOpenedDICOMDIRPath() const;
	void setLastOpenedDICOMDIRPath( QString const & path );

    ///Especifica/Indica si s'han d'escoltar les peticions del RIS
    void setListenRisRequests(bool listenRisRequests);
    bool getListenRisRequests();

    ///Especifica/Indica per quin port s'escolten les peticions del RIS
    void setListenPortRisRequests(int portListenRisRequests);
    int getListenPortRisRequests();

    ///Indica/Especifica si una vegada descarregat un estudi com a petició del RIS 
    void setViewAutomaticallyAStudyRetrievedFromRisRequest(bool viewAutomaticallyStudy);
    bool getViewAutomaticallyAStudyRetrievedFromRisRequest();

private:
    /// Nom del grup on es guarda la configuració d'aquesta classe
    QString GroupSettingsName;

    QSettings m_starviewerSettings;

};

};

#endif
