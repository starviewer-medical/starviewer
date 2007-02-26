/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTARVIEWERSETTINGS_H
#define UDGSTARVIEWERSETTINGS_H

#include <QSettings>
#include <QString>

namespace udg {

/** Aquesta classe permet accedir i guardar els parametres de configuracio de l'starviewer
@author marc
*/

const QString databaseRootKey("/cache/sdatabasePath" ); //indica on es troba la bd
const QString poolSizeKey("/cache/poolSize" );
const QString cacheImagePathKey("/cache/imagePath" );
const QString AETitleMachineKey("/pacsparam/AETitle" );
const QString localPortKey("/pacsparam/localPort" );
const QString timeoutPacsKey("/pacsparam/timeout" );
const QString maxConnectionsKey("/pacsparam/MaxConnects" );
const QString pacsColumnWidthKey("/interface/studyPacsList/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString cacheColumnWidthKey("/interface/studyCacheList/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString dicomdirColumnWidthKey("/interface/studyDicomdirList/columnWidth" );//en aquesta clau a darrera s'hi concatena el número de columna ,  per diferenciar cada columna
const QString MaximumDaysNotViewedStudy("/cache/MaximumDaysNotViewedStudy" );
const QString InstitutionName("/institution/name");
const QString InstitutionAddress("/institution/address");
const QString InstitutionTown("/institution/town");
const QString InstitutionZipCode("/institution/zipCode");
const QString InstitutionCountry("/institution/country");
const QString InstitutionEmail("/institution/Email");
const QString InstitutionPhoneNumber("/institution/phoneNumber");
const QString queryScreenWindowPositionX("/interface/queryscreen/windowPositionX");
const QString queryScreenWindowPositionY("/interface/queryscreen/windowPositionY");
const QString queryScreenWindowWidth("/interface/queryscreen/windowWidth");
const QString queryScreenWindowHeight("/interface/queryscreen/windowHeigth");
const QString queryScreenStudyTreeSeriesListQSplitterState("/interface/queryscreen/StudyTreeSeriesListQSplitterState");

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

	/** estableix la mida de la pool
	 * @param mida de la pool
	 */
    void setPoolSize( QString );

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

	/** retorna la mida de la pool
	 * @return retorna la mida de la pool
	 */
    QString getPoolSize();

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


private :
    /// Nom del grup on es guarda la configuració d'aquesta classe
    const QString GroupSettingsName;

    QSettings m_starviewerSettings;

};

};

#endif
