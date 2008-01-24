/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQDICOMDUMPCTWIDGET_H
#define UDGQDICOMDUMPCTWIDGET_H

#include <ui_qdicomdumpctwidgetbase.h>

namespace udg {

class Series;
class Image;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QDicomDumpCTWidget : public QWidget , private Ui::QDicomDumpCTWidgetBase{
Q_OBJECT
public:
    QDicomDumpCTWidget( QWidget *parent = 0 );

    ~QDicomDumpCTWidget();

    /** Fa el DicomDump dels tags dicom de la imatge 
     * @param image imatge de la que s'ha de fer el dicomdump
     */
    void setCurrentDisplayedImage( Image *image );

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
