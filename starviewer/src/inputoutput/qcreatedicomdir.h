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
#include "const.h"

class QSignalMapper;

namespace udg {

class DICOMStudy;
class Status;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QCreateDicomdir : public QDialog , private Ui::QCreateDicomdirBase{
Q_OBJECT
public:

    QCreateDicomdir( QWidget *parent = 0 );

    /** Afegeix un estudi per convertir a dicomdir
     * @param study estudi per convertir a dicomdir
     */
    void addStudy( DICOMStudy study );

    ///Neteja el directori temporal utilitzat crear els dicomdir que es gravaran en cd o dvd
    void clearTemporaryDir();

    ~QCreateDicomdir();

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
    /** Carreguem la mida de les columnes del QTreeWidget de l'última vegada que es va tancar la pantalla. La mida de les columnes la tenim guardada al StarviewerSettings
     */
    void setWidthColumns();

    ///Crea les connexions de signals i slots de la interfície
    void createConnections();

    ///crea el menu contextual
    void createContextMenu();

    /**Formata la data
     * @param data de l'estudi
     */
    QString formatDate( const QString );

    /**Formata l'hora
     * @param Hora de l'estudi
     */
    QString formatHour( const QString );

    /** Comprova si l'estudi amb UID passat per paràmetre està dins la llista d'estudis pendents de passa a Dicomdir
     * @param studyUID UID de l'estudi que s'ha de comprovar si existeix dins la llista
     * @return indica si existeix l'estudi a la llista d'estudis pendents de passa a DicomDir
     */
    bool existsStudy( QString studyUID );

    ///Dona valor a l'etiqueta que indica l'espai que ocupa el Dicomdir
    void setDicomdirSize();

    /**comprova si hi ha suficient espai lliure al disc dur per crear el dicomdir, comprova que l'espai lliure sigui superior a l'espai que ocuparà el nou directori dicomdir
     * @return indica si hi ha prou espai lliure al disc per crear el dicomdir
     */
    bool enoughFreeSpace( QString path );

    /** Tracta els errors que s'han produït a la base de dades en general
     * @param state  Estat del mètode
     */
    void databaseError( Status *state );

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

    /** Comprova si aquest directori ja és un dicomdir
     * dicomdir a comprova sir és un directori
     */
    bool dicomdirPathIsADicomdir( QString dicomdirPath );

    /** genera una imatge iso i crida el k3b per gravar la imatge
     * @param device dispositiu al que es grava, pot ser cd o dvd
     */
    void burnDicomdir( recordDeviceDicomDir device );

    /** neteja la pantalla de dicomdir, després que s'hagi creat un dicomdir amb exit
     */
    void clearQCreateDicomdirScreen();

    /** Guarda la mida de les columnes del QTreeWidget al StarviewerSettings
     */
    void saveColumnsWidth();

    /// Inicialitza les QActions
    void createActions();

private slots:
    /// Es passa per paràmetre l'identificador del dispositiu i es fan les pertinents accions
    void deviceChanged( int value );

private:
    unsigned long m_dicomdirSize;
    unsigned long m_DiskSpace;
    QMenu m_contextMenu;///<Menu contextual

    /// Dispositius on es pot gravar el DICOMDIR
    enum DICOMDIRDevices{ CDROM = 0, DVDROM = 1, HardDisk = 2, PenDrive = 3 };

    ///Agrupa les accions dels dispositius on gravarem el DICOMDIR
    QActionGroup *m_devicesActionGroup;

    /// Mapejador d'accions
    QSignalMapper *m_signalMapper;

    /// Accions
    QAction *m_cdromAction;
    QAction *m_dvdromAction;
    QAction *m_hardDiskAction;
    QAction *m_pendriveAction;

    /// Variable que ens diu quin és el dispositiu seleccionat en aquell moment
    int m_currentDevice;
};

}

#endif
