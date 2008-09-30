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

	/// Constructor de la casse
    StarviewerSettings();

	///Destructor de la classe
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

	/** Estableix el número de dies màxim que un estudi pot estar a la cache sense ser vist ,  a partir d'aquest número de dies l'estudi és esborrat
	 * @param número maxim de dies
	 */
    void setMaximumDaysNotViewedStudy( QString  );

	/** retorna el path de la base de dades
	 * @return retorna el path de la base de dades
	 */
    QString getDatabasePath();

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

    //interficie

	/** guarda la mida de la columna que se li passa per paràmetre del QStudyListView ,  encarregat de mostrar les dades del Pacs
	 * @param número de columna
	 * @param amplada de la columna
	 */
    void setStudyPacsListColumnWidth( int col , int width );

	/** guarda la mida de la columna que se li passa per paràmetre del QStudyListView ,  encarregat de mostrar les dades de la cache
	 * @param número de columna
	 * @param amplada de la columna
	 */
    void setStudyCacheListColumnWidth( int col , int width );

    /** guarda la mida de la columna que se li passa per paràmetre del QStudyListView ,  encarregat de mostrar les dades d'un dicomdir
     * @param número de columna
     * @param amplada de la columna
     */
    void setStudyDicomdirListColumnWidth( int col , int width );

    /** guarda en quina és la posició en l'eix de coordenades X en que es troba la interficie queryscreen
     * @param positionX posició en l'eix de coordenades X de la pantalla queryScreen
     */
    void setQueryScreenWindowPositionX( int positionX );

    /** guarda en quina és la posició en l'eix de coordenades Y en que es troba la interficie queryscreen
     * @param positionY posició en l'eix de coordenades Y de la pantalla queryScreen
     */
    void setQueryScreenWindowPositionY( int positionY );

    /** guarda quina és l'amplada de la queryScreen
     * @param width amplada de la queryScreen
     */
    void setQueryScreenWindowWidth( int width );

    /** guarda quina és l'allargada de la pantalla queryscreen
     * @param positionY posició en l'eix de coordenades Y de la pantalla queryScreen
     */
    void setQueryScreenWindowHeight( int height );

    /** guarda l'estat en que es troba el QSplitter que divideix a la pestany de local el StudyList i el SeriesList
     * @param state estat del QSplitter
     */
    void setQueryScreenStudyTreeSeriesListQSplitterState( QByteArray state );

    /** guarda la mida de la columna que se li passa per paràmetre del QTreeWidget de QOperationStateScreen ,  encarregat de mostrar l'estat de les operacions relacionades amb el PACS
     * @param número de columna
     * @param amplada de la columna
     */
    void setQOperationStateColumnWidth( int col , int width );

    /** guarda la mida de la columna que se li passa per paràmetre del QTreeWidget QCreatoDicomdir ,  on s'hi mostren quins estudis es convertiran en dicomdirs
     * @param número de columna
     * @param amplada de la columna
     */
    void setQCreateDicomdirColumnWidth( int col , int width );

    /** guarda la mida de la columna que se li passa per paràmetre del QTreeWidget de la QConfigurationScreen ,  on s'hi mostren quins pacs tenim per connectar-nos
     * @param número de columna
     * @param amplada de la columna
     */
    void setQConfigurationPacsDeviceColumnWidth( int col , int width );

	/** retorna l'amplada del número de columna de la llista d'estudis del PACS ,  passat per paràmetre
	 * @return amplada de la columna
	 */
   	int getStudyPacsListColumnWidth( int column );

	/** retorna l'amplada del número de columna de la llista d'estudis de la cache ,  passat per paràmetre
	 * @return amplada de la columna
	 */
   	int getStudyCacheListColumnWidth( int column );

    /** retorna l'amplada del número de columna de la llista d'estudis de Dicomdir,  passat per paràmetre
     * @return amplada de la columna
     */
    int getStudyDicomdirListColumnWidth( int column );

    /** retorna en quin posició de l'eix estava la pantalla queryScreen guardada al QSettings l'aplicació
     * @return posicó en l'eix X que es trobava la interficie queryScreen
     */
    int getQueryScreenWindowPositionX();

    /** retorna en quin posició de l'eix estava la pantalla queryScreen guardada al QSettings l'aplicació
     * @return posicó en l'eix Y que es trobava la interficie queryScreen
     */
    int getQueryScreenWindowPositionY();

    /** retorna quina era l'amplada de la pantalla queryScreen guardada al QSettings
     * @return amplada de la pantalla
     */
    int getQueryScreenWindowWidth();

    /** retorna l'allargada la pantalla queryScreen guardada al QSettings
     * @return posicó en l'eix Y que es trobava la interficie queryScreen
     */
    int getQueryScreenWindowHeight();

    /** retorna l'estat amb que estava el QSplitter que separa el StudyTree i el SeriesList
     * @return estat del QSpliltter
     */
    QByteArray getQueryScreenStudyTreeSeriesListQSplitterState();

    /** retorna l'amplada del número de columna de la QTreeWidget de la QOperationStateScreen
     * @return amplada de la columna
     */
    int getQOperationStateColumnWidth( int column );

    /** retorna l'amplada del número de columna de la QTreeWidget de la QCreatoDicomdir
     * @return amplada de la columna
     */
    int getQCreateDicomdirColumnWidth( int column );

    /** retorna l'amplada del número de columna de la QTreeWidget de la QConfigurationScree
     * @return amplada de la columna
     */
    int getQConfigurationPacsDeviceColumnWidth( int column );


    /** Guarda el nom de la institució
     * @param institutionName nom de l'institució
     */
    void setInstitutionName( QString institutionName );

    /** Guarda l'adreça de la institució
     * @param institutionAddress adreça de la institució
     */
    void setInstitutionAddress( QString institutionNameString );

    /** Guarda la poblacio de la institució
     * @param institutionTown població de la institució
     */
    void setInstitutionTown( QString institutionTownString );

    /** Guarda el codi postal de la institució
     * @param institutionZipCode codi postal del a institució
     */
    void setInstitutionZipCode( QString institutionZipCodeString );

    /** Guarda el país de la institució
     * @param institutionCountry
     */
    void setInstitutionCountry( QString institutionCountryString );

    /** Guarda el correu electrònic de la institució
     * @param institutionEmail correu electrònic
     */
    void setInstitutionEmail( QString institutionEmailString );

    /** Guarda el telefon de la institució
     * @param institutionPhoneNumber telèfon
     */
    void setInstitutionPhoneNumber( QString institutionPhoneNumberString );

    /** retorna el nom de la institució
     * @return nom de la institució
     */
    QString getInstitutionName();

    /** retorna el adreça de la institució
     * @return adreça de la institució
     */
    QString getInstitutionAddress();

    /** retorna la ciutat de la institució
     * @return ciutat de la institució
     */
    QString getInstitutionTown();

    /** retorna el codi postal de la institució
     * @return codi postal de la institució
     */
    QString getInstitutionZipCode();

    /** retorna el país de la institució
     * @return país de la institució
     */
    QString getInstitutionCountry();

    /** retorna el correu electrònic de la institució
     * @return correu electrònic de la institució
     */
    QString getInstitutionEmail();

    /** retorna el telèfon de la institució
     * @return telèfon de la institució
     */
    QString getInstitutionPhoneNumber();

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

private: 
    /// Ens dóna el nom del host local. 
    /// El mètode l'hem extret del propi codi de Qt (QtNetwork) per així evitar haver de 
    /// linkar contra una altre llibreria més.
    /// TODO el mètode està aquí temporalment, fins que tinguem una classe de caràcter general
    /// per utilitats a nivell de sistema
    QString getLocalHostName();

private:
    /// Nom del grup on es guarda la configuració d'aquesta classe
    QString GroupSettingsName;

    QSettings m_starviewerSettings;

};

};

#endif
