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

    /** 
     * Afegeix una llista d'estudis per convertir a DICOMDIR
     * @param studies Llista d'estudis per convertir a DICOMDIR
     */
    void addStudies(const QList<Study *> &studies);

    /** 
     * Comprova si l'estudi amb UID passat per paràmetre està dins la llista d'estudis pendents de passa a DICOMDIR
     * @param studyUID UID de l'estudi que s'ha de comprovar si existeix dins la llista
     * @return indica si existeix l'estudi a la llista d'estudis pendents de passa a DICOMDIR
     */
    bool studyExistsInDICOMDIRList( QString studyUID );

public slots:
    /// Slot que esborra l'estudi seleccionat de la llista
    void removeSelectedStudy();

    /// Slot que esborra tots els estudis de la llista i reinicia les variables de tamany del DICOMDIR
    void resetDICOMDIRList();

    /// Slot que s'activa quan es fa click al botó examinar, obre filedialog, per especificar a quina carpeta es vol guardar el DICOMDIR
    void examineDicomdirPath();

    /// Slot que s'activa quan es fa click al botó create Dicomdir, i comença el procés de crear el DICOMDIR
    void createDicomdir();

protected:
    /** 
     * Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* ce );

private:
    /// Crea les connexions de signals i slots de la interfície
    void createConnections();

    /// Crea el menú contextual
    void createContextMenu();

    /// Dóna valor a l'etiqueta que indica l'espai que ocupa el DICOMDIR
    void showDICOMDIRSize();

    /**
     * Comprova si hi ha suficient espai lliure al disc dur per crear el DICOMDIR, comprova que l'espai 
     * lliure sigui superior a l'espai que ocuparà el nou directori DICOMDIR
     * @return indica si hi ha prou espai lliure al disc per crear el DICOMDIR
     */
    bool enoughFreeSpace( QString path );

    /** 
     * Tracta els errors que s'han produït a la base de dades en general
     * @param state  Estat del mètode
     */
    void showDatabaseErrorMessage( const Status &state );

    /** 
     * Crea el DICOMDIR amb els estudis seleccionats, en el directori on se li passa per paràmetre
     * @param dicomdirPath directori on s'ha de crear el DICOMDIR
     * @return retorna l'estat del mètode
     */
    Status startCreateDicomdir( QString dicomdirPath );

    /// Crea el DICOMDIR en un CD o DVD
    Status createDicomdirOnCdOrDvd();

    /// Crea el DICOMDIR al disc dur, dispositius externs usb o memòries flash
    void createDicomdirOnHardDiskOrFlashMemories();

    /// Comprova que el directori sigui buit
    bool dicomdirPathIsEmpty(QString dicomdirPath);

    /** 
     * Comprova si aquest directori ja és un DICOMDIR
     * @param dicomdirPath Directori a comprovar
     */
    bool dicomdirPathIsADicomdir( QString dicomdirPath );

    /** 
     * Inicia la generació d'una imatge ISO i connecta el signal de finalització de la creació de la imatge
     * amb l'slot que obrirà o no el programa de gravació segons l'èxit de la creació de la imatge ISO
     */
    void burnDicomdir();

    /**
     * Mostra un messagebox amb el corresponent missatge d'error segons l'estat del procés
     * @param process procés que hem executat
     * @param name nom del procés
     */
    void showProcessErrorMessage( const QProcess &process, QString name );

    /// Neteja la pantalla de DICOMDIR, després que s'hagi creat un DICOMDIR amb exit
    void clearQCreateDicomdirScreen();

    /// Inicialitza les QActions
    void createActions();

    /// Actualitza la variable que comptabilitza l'espai disponible per grabar amb el dispositiu actual
    void updateAvailableSpaceToRecord();

    /// Neteja el directori temporal utilitzat per crear els DICOMDIR que es gravaran en CD o DVD
    void clearTemporaryDICOMDIRPath();

private slots:
    /// Es passa per paràmetre l'identificador del dispositiu i es fan les pertinents accions
    void deviceChanged( int value );

    /// Slot que s'activa quan s'ha acabat de generar la imatge del DICOMDIR i per tant es pot executar el programa de gravació
    void openBurningApplication(bool createIsoResult);

private:
    /// Constants per definir les mides de CD/DVD i disc
    static const int CDRomSizeMb = 700;
    static const int DVDRomSizeMb = 4800;

    static const quint64 CDRomSizeBytes = ( quint64 ) CDRomSizeMb * ( quint64 ) ( 1024 * 1024 );
    static const quint64 DVDRomSizeBytes = ( quint64 ) DVDRomSizeMb * ( quint64 ) ( 1024 * 1024 );

    /// Indiquem de mitjana que ocupa una capçalera dicom d'una imatge, en el ticket #766 indiquem com s'ha obtingut el càcul
    static const int dicomHeaderSizeBytes = 23000;

    /// Mida en bytes del que ocupen els estudis que es volen incloure en el DICOMDIR
    quint64 m_dicomdirSizeBytes;
    
    /// Espai lliure en bytes del dispositiu de disc escollit sobre el que es vol grabar el DICOMDIR
    quint64 m_availableSpaceToRecordInBytes;

    /// Menu contextual
    QMenu m_contextMenu;

    ///Agrupa les accions dels dispositius on gravarem el DICOMDIR
    QActionGroup *m_devicesActionGroup;

    /// Mapejador d'accions
    QSignalMapper *m_signalMapper;

    /// Accions
    QAction *m_cdromAction;
    QAction *m_dvdromAction;
    QAction *m_hardDiskAction;
    QAction *m_pendriveAction;

    /// Guarda l'últim directori on s'ha creat el DICOMDIR
    QString m_lastDicomdirDirectory;

    /// Permet mostrar una barra de progrés
    QProgressDialog *m_progressBar;
    
    /// Timer 
    QTimer *m_timer;
    
    /// Permet genera fitxers d'imatge ISO
    IsoImageFileCreator *m_isoImageFileCreator;

    /// Variable que ens diu quin és el dispositiu seleccionat en aquell moment
    CreateDicomdir::recordDeviceDicomDir m_currentDevice;

    /// Variable que indica si a la mida del DICOMDIR s'hi ha sumat el tamany que ocupa la carpeta a copiar el DICOMDIR
    bool m_folderToCopyToDICOMDIRSizeAddedToDICOMDIRSize;

    /**
     * Indica si la configuració és correcte per poder gravar el DICOMDIR en un CD o DVD. No comprova que sigui 
     * un programa vàlid, simplement comprova que la ruta ens ha indicat com programa per gravar CD/DVD existeix
     */
    bool checkDICOMDIRBurningApplicationConfiguration();

    /**
     * Retorna la mida que l'estudi ocuparà en el DICOMDIR, al fer el càlcul ja té en compte si les imatges que s'afegiran al DICOMDIR
     * han de conventir-se a transfer syntax LittleEndian o mantenen la seva transfer syntax, per calcular correctament la mida que ocuparà l'estudi.
     * Si les imatges s'han de convertir a LittleEndian el resultat que dona aquesta funció és una estimació del que ocuparà l'estudi, si 
     * conserva seva transfer syntax origina el càlcul del que ocuparà l'estudi és un càlcul real.
     */
    quint64 getStudySizeInBytes(bool transferSyntaxInLittleEndian, QString studyInstanceUID);

    /// Retorna el que ocuparà la imatge passada per paràmetre en transfer syntax Little Endian, la mida que retorna és un càlcul aproximat del que ocuparà
    quint64 getImageSizeInBytesInLittleEndianTransferSyntax(Image *image);

    ///Retorna el path temporal on poder crear el DICOMDIR
    QString getTemporaryDICOMDIRPath();

    ///Retorna la mida del visor de la carpeta a copiar al DICOMDIR, si no tenim cap carpeta configurada al Settings, retorna 0
    quint64 getFolderToCopyToDICOMDIRSizeInBytes();

    /** Starviewer té l'opció de copiar el contingut d'una carpeta al DICOMDIR. Aquest mètode indica si aquesta carpeta
        s'ha de copiar al DICOMDIR.*/
    bool haveToCopyFolderContentToDICOMDIR();

    /** Starviewer té l'opció de copiar el contingut d'una carpeta al DICOMDIR. Aquest mètode té en compte la mida d'aquesta carpeta
      * i en funció de si s'ha de copiar o no el contingut de la carpeta i el dispositiu actualitza la mida del DICODIR afegint/restant
      * la mida de la carpeta a copiar.
      */
    void updateDICOMDIRSizeWithFolderToCopyToDICOMDIRSize();
};

}

#endif
