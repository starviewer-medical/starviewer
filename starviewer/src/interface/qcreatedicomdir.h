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

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QCreateDicomdir : public QWidget , private Ui::QCreateDicomdirBase{
Q_OBJECT
public:
    QCreateDicomdir(QWidget *parent = 0);

    void addStudy( Study study);

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

    /** Comprova si el path on es vol crear el dicomdir existeix, si no existeix l'intenta crear
     * @return indica si el directori existeix
     */
    bool isCorrectDicomdirPath();

};

}

#endif
