/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQDICOMDUMP_H
#define UDGQDICOMDUMP_H

#include "ui_qdicomdumpbase.h"

/// Interfície que implementa la llista d'operacions realitzades cap a un PACS
namespace udg {

class Image;

class QDicomDump : public QDialog , private Ui::QDicomDumpBase{
Q_OBJECT
public:

    /** Constructor de la classe
     * @param parent
     * @return
     */
    QDicomDump( QWidget *parent = 0 );

    /// destructor de la classe
    ~QDicomDump();

    ///Displaya les dades de la imatge que se li passa per paràmetre
    void setCurrentDisplayedImage( Image *currentImage );

private:

    ///Crea les connexions dels signals
    void createConnections();

    /// Inicializa a "-" els labelsValue
    void initialize();

    /** Carrega els tags comuns que es mostraran per totes les imatges, independentment de la modalitat
     * @param currentImage image de la que hem d'obtenir els tags
     */
    void setCommonImageTagsValue( Image *currentImage );

    /** Amaga tots els Widgets de diverses modalitats d'imatge que formen part del DicomDumpWidget
     */
    void setNoVisibleAllDicomDumpWidgets();

private slots:
    /// tanquem la finestra
    void closeWindow();

};

};

#endif
