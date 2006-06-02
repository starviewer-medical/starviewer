/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSTARVIEWERPROCESSIMAGE_H
#define UDGSTARVIEWERPROCESSIMAGE_H

#include <qobject.h>
#include <qwidget.h>
#include <QString>

#include "processimage.h"
#include "cachepacs.h"

namespace udg {

class Image;

/**
@author marc
*/
class StarviewerProcessImage: public QObject, public ProcessImage{

Q_OBJECT

public:

    StarviewerProcessImage();

    void process(Image* image);//virtual significa que els fills la podran reimplementar a la seva classe, i que es cridara la dels fills de process no la del pare, quant es declara virtual només s'ha de fer al .h
    //void setQlistViewItem(QListViewItem *);
    
    void setErrorRetrieving();
    bool getErrorRetrieving();
    
    void setListViewItem();
        
    ~StarviewerProcessImage();
    void studyRetrieved();
    
signals :
    void imageRetrieved(QString studyUID,int);
    void seriesRetrieved(QString studyUID);
    void startRetrieving( QString );
    void seriesView ( QString );
    
private :

    CachePacs *m_localCache;
    int m_downloadedImages , m_downloadedSeries;
    QString m_oldSeriesUID,m_studyUID;
    bool m_error;
    
    QString createImagePath( Image* image );
    
    Status getSeriesInformation( QString imagePath,Series &serie );
    
//
};

};

#endif

