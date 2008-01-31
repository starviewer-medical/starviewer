/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQDICOMDUMPCTLOCALIZERWIDGET_H
#define UDGQDICOMDUMPCTLOCALIZERWIDGET_H

#include <ui_qdicomdumpctlocalizerwidgetbase.h>

namespace udg {

class Series;
class Image;

/** Dicom dump del Localizer d'un CT, mostra els la informació especifica de dicom dump per un CT Localizer
 */
class QDicomDumpCTLocalizerWidget : public QWidget , private Ui::QDicomDumpCTLocalizerWidgetBase{
Q_OBJECT
public:
    
    QDicomDumpCTLocalizerWidget( QWidget *parent = 0 );
    
    ~QDicomDumpCTLocalizerWidget();

    /** Fa el DicomDump dels tags dicom de la imatge 
     * @param image imatge de la que s'ha de fer el dicomdump
     */
    void setCurrentDisplayedImage( Image *currentImage );

private:

    ///Inicalitza tots els labelsValue a "-"
    void initialize();

    /** Estableix el valors dels tags de la imatge
     * @param imageInformation imatge
     */
    void setImageDicomTagsValue( Image *imageInformation );
    
    /** Estableix els valors dels tags de la seire
     * @param seriesInformation  serie
     */
    void setSeriesDicomTagsValue( Series *seriesInformation );
};

}

#endif
