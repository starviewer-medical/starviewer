/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCREATEDICOMDIR_H
#define UDGQCREATEDICOMDIR_H

#include <ui_qcreatedicomdirbase.h>

namespace udg {

class Study;
class Status;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QCreateDicomdir : public QWidget , private Ui::QCreateDicomdirBase{
Q_OBJECT

    enum recordDevice { cd , dvd };
    
public:
    QCreateDicomdir( QWidget *parent = 0 );

    void addStudy( Study study );

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

public slots:
        
    /** Slot que s'activa quan canviem l'operació seleccionada a combobox
     * @param index del combobox
     */
    void changedAction( int index );
    
private:

    ///Crea les connexions de signals i slots de la interfície    
    void createConnections();

    /**Formata la data
     * @param data de l'estudi
     */
    QString formatDate( const std::string );
    
    /**Formata l'hora
     * @param Hora de l'estudi
     */
    QString formatHour( const std::string );

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

    /** Crea el dicomdir al disc dur o en un pendrive
     */
    void createDicomdirOnHard();

    /** Comprova si aquest directori ja és un dicomdir
     * dicomdir a comprova sir és un directori
     */
    bool dicomdirPathIsADicomdir( QString dicomdirPath );

    /** executa el k3b per gravar en un cd o dvd depenent de l'opció escollida
     * @param device dispositiu al que es grava, pot ser cd o dvd
     */
    void QCreateDicomdir::executek3b( recordDevice device );

    
    /** neteja la pantalla de dicomdir, després que s'hagi creat un dicomdir amb exit
     */
    void clearQCreateDicomdirScreen();

    unsigned long m_dicomdirSize;
    unsigned long m_DiskSpace;
};

}

#endif
