/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQDICOMDUMPMRWIDGET_H
#define UDGQDICOMDUMPMRWIDGET_H

#include <ui_qdicomdumpmrwidgetbase.h>

namespace udg {

class Image;
class Series;

/** Dicomdump de MR
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QDicomDumpMRWidget : public QWidget , private Ui::QDicomDumpMRWidgetBase{
Q_OBJECT
public:
    QDicomDumpMRWidget( QWidget *parent = 0 );

    ~QDicomDumpMRWidget();

    /** Fa el DicomDump dels tags dicom de la imatge 
     * @param image imatge de la que s'ha de fer el dicomdump
     */
    void setCurrentDisplayedImage( Image *image );

private:

    ///Inicalitza tots els labelsValue a "-"
    void initialize();

    /** Estableix el Tags d'imatge
     * 
     * @param currentImage 
     */
    void setImageDicomTagsValue( Image *currentImage );

    /** Estableix els Tags de serie
     * 
     * @param currentSeries 
     */
    void setSeriesDicomTagsValue( Series *currentSeries );

};

}

#endif
