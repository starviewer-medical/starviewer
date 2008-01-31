/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQDICOMDUMP_H
#define UDGQDICOMDUMP_H

#include "ui_qdicomdumpbase.h"

/** Aquest classe és l'encarregada de realitzar el dicomdump de la imatge que s'està visualizant per part de l'usuari.
  *Totes les imatges tenen una sèrie de tags que es descodifiquen en aquesta classe que són comuns a totes les imatges, llavors
  *en funció de la modalitat d'imatge i del Vendor mostrem el QWidget que conté els camps més adients per generar el dicom dump.
 * per aquella modalitat
 */
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

    ///Genera el dicomdump de la iamtge que se li passa per paràmetre
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
