/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCREATEDICOMDIR_H
#define UDGQCREATEDICOMDIR_H

#include <QMenu>
#include <ui_qcreatedicomdirbase.h>
#include "createdicomdir.h"

class QSignalMapper;
class QProcess;
class QProgressDialog;

namespace udg {

class Study;
class Status;
class Image;
class IsoImageFileCreator;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QCreateDicomdir : public QDialog , private Ui::QCreateDicomdirBase{
Q_OBJECT
public:
    QCreateDicomdir( QWidget *parent = 0 );
    ~QCreateDicomdir();

    /** Afegeix un estudi per convertir a dicomdir
     * @param study estudi per convertir a dicomdir
     */
    void addStudy(Study* study);

    ///Neteja el directori temporal utilitzat crear els dicomdir que es gravaran en cd o dvd
    void clearTemporaryDir();

    /** Comprova si l'estudi amb UID passat per paràmetre està dins la llista d'estudis pendents de passa a Dicomdir
     * @param studyUID UID de l'estudi que s'ha de comprovar si existeix dins la llista
     * @return indica si existeix l'estudi a la llista d'estudis pendents de passa a DicomDir
     */
    bool studyExists( QString studyUID );

public slots:
    ///Slot que esborra l'estudi seleccionat de la llista
    void removeSelectedStudy();

    ///Slot que esborra tots els estudis de la llista
    void removeAllStudies();

    ///slot que s'activa quan es fa click al botó examinar, obre filedialog, per especificar a quina carpeta es vol guardar el dicomdir
    void examineDicomdirPath();

    ///Slot que s'activa quan es fa click al botó create Dicomdir, i comença el procés de crear el dicomdir
    void createDicomdir();

protected :
    /** Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* ce );

private:
    ///Crea les connexions de signals i slots de la interfície
    void createConnections();

    ///crea el menu contextual
    void createContextMenu();

    ///Dona valor a l'etiqueta que indica l'espai que ocupa el Dicomdir
    void setDicomdirSize();

    /**comprova si hi ha suficient espai lliure al disc dur per crear el dicomdir, comprova que l'espai lliure sigui superior a l'espai que ocuparà el nou directori dicomdir
     * @return indica si hi ha prou espai lliure al disc per crear el dicomdir
     */
    bool enoughFreeSpace( QString path );

    /** Tracta els errors que s'han produït a la base de dades en general
     * @param state  Estat del mètode
     */
    void showDatabaseErrorMessage( const Status &state );

    /** Crea el dicomdir amb els estudis seleccionats, en el directori on se li passa per paràmetre
     * @param dicomdirPath directori on s'ha de crear el dicomdir
     * @return retorna l'estat del mètode
     */
    Status startCreateDicomdir( QString dicomdirPath );

    /** Crear el dicomdir en un cd o dvd
     */
    Status createDicomdirOnCdOrDvd();

    /** Crea el dicomdir al disc dur , dispositiu externs, usb o memòries flash
     */
    void createDicomdirOnHardDiskOrFlashMemories();

    ///Comprova que el directori sigui buit
    bool dicomdirPathIsEmpty(QString dicomdirPath);

    /** Comprova si aquest directori ja és un dicomdir
     * dicomdir a comprova sir és un directori
     */
    bool dicomdirPathIsADicomdir( QString dicomdirPath );

    /** Inicia la generació d'una imatge iso i connecta el signal de finalització de la creació de la imatge
     *  amb l'slot que obrirà o no el programa de gravació segons l'èxit de la creació de la imatge ISO
     */
    void burnDicomdir();

    /**
     * Mostra un msgbox amb el corresponent missatge d'error segons l'estat del procés
     * @param process procés que hem executat
     * @param name nom del procés
     */
    void showProcessErrorMessage( const QProcess &process, QString name );

    /** neteja la pantalla de dicomdir, després que s'hagi creat un dicomdir amb exit
     */
    void clearQCreateDicomdirScreen();

    /// Inicialitza les QActions
    void createActions();

private slots:
    /// Es passa per paràmetre l'identificador del dispositiu i es fan les pertinents accions
    void deviceChanged( int value );

    /// Slot que s'activa quan s'ha acabat de generar la imatge del dicomdir i per tant es pot executar el programa de gravació
    void openBurningApplication(bool createIsoResult);

private:

    static const int CDRomSizeMb = 700;
    static const int DVDRomSizeMb = 4800;
    static const int HardDiskSizeMb = 9999999;

    static const quint64 CDRomSizeBytes = ( quint64 ) CDRomSizeMb * ( quint64 ) ( 1024 * 1024 );
    static const quint64 DVDRomSizeBytes = ( quint64 ) DVDRomSizeMb * ( quint64 ) ( 1024 * 1024 );
    static const quint64 HardDiskSizeBytes = ( quint64 ) HardDiskSizeMb * ( quint64 ) ( 1024 * 1024 );

    //Indiquem de mitjana que ocupa una capçalera dicom d'una imatga, en el ticket #766 indiquem com s'ha obtingut el càcul;
    static const int dicomHeaderSizeBytes = 23000;

    quint64 m_dicomdirSizeBytes;
    quint64 m_DiskSpaceBytes;
    QMenu m_contextMenu;///<Menu contextual

    ///Agrupa les accions dels dispositius on gravarem el DICOMDIR
    QActionGroup *m_devicesActionGroup;

    /// Mapejador d'accions
    QSignalMapper *m_signalMapper;

    /// Accions
    QAction *m_cdromAction;
    QAction *m_dvdromAction;
    QAction *m_hardDiskAction;
    QAction *m_pendriveAction;

    QString m_lastDicomdirDirectory;//Guarda l'últim directori on s'ha creat el Dicomdir

    /// Permet mostrar una barra de progrés
    QProgressDialog *m_progressBar;
    /// Timer 
    QTimer *m_timer;
    /// Permet genera fitxers d'imatge ISO
    IsoImageFileCreator *m_isoImageFileCreator;

    /// Variable que ens diu quin és el dispositiu seleccionat en aquell moment
    CreateDicomdir::recordDeviceDicomDir m_currentDevice;

    /**En windows la opció de crear dicomdir en cd/dvd ara mateix està deshabilitat perquè no tenim implementada la part 
      *de comunicar-nos algun cap sw de crear cd/dvd, el que es fa en Windows és avisar de que està desactivat el servei i que el
      *que es pot fer es crear el dicomdir al disc dur i després gravar-lo a cd, i s'activa per defecte la creació de dicomdir al disc dur
      */
    void dvdCdDicomdirDesactivatedOnWindows();

    /// Permet comprovar si la configuració es correcte per poder gravar el dicomdir en un cd o dvd
    void checkDICOMDIRBurningApplicationConfiguration();

    /**Retorna la mida que l'estudi ocuparà en el dicomdir, al fer el càlcul ja té en compta si les imatges que s'afegiran al DICOMDIR
      *han de conventir-se a transfer syntax LittleEndian o mantenen la seva transfer syntax, per calcular correctament la mida que ocuparà l'estudi.
      *Si les imatges s'han de convertir a LittleEndian el resultat que dona aquesta funció és una estimació del que ocuparà l'estudi, si 
      *conserva seva transfer syntax origina el càlcul del que ocuparà l'estudi és un càlcul real.
      */
    quint64 getStudySizeInBytes(bool transferSyntaxInLittleEndian, QString studyInstanceUID);

    ///Retorna el que ocuparà la imatge passada per paràmetre en transfer syntax Little Endian, la mida que retorna és un càlcul aproximat del que ocuparà
    quint64 getImageSizeInBytesInLittleEndianTransferSyntax(Image *image);

};

}

#endif
