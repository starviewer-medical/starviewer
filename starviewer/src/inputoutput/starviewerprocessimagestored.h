/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTARVIEWERPROCESSIMAGESTORED_H
#define UDGSTARVIEWERPROCESSIMAGESTORED_H

#include <QObject>
#include "processimage.h"
#include "image.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class StarviewerProcessImageStored : public QObject, public ProcessImage
{
Q_OBJECT
public:
    StarviewerProcessImageStored(QObject *parent = 0);

    void process( Image * image );

    ~StarviewerProcessImageStored();

signals :

    /** signal que s'emet a QExcuteOperationThread per indica que s'ha descarregat una image
      *@param uid de l'estudi
      *@param número d'imatge
      */
    void imageStored( QString studyUID , int );

    /** signal que s'emet a QExcuteOperationThread per indica que s'ha descarregat una sèroe
      *@param número d'imatge
      */
    void seriesStored( QString studyUID );

    /** signal que s'emet a QExcuteOperationThread per indica que s'ha iniciat la descàrrega d'un estudi
      * @param número d'imatge
      */
    void startStoring( QString );

    bool getError();

    void setError();

    private:

    int m_imagesStored;
    QString m_oldSeriesUID, m_studyUID;
};

}

#endif
