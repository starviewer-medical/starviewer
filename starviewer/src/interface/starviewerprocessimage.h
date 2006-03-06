/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSTARVIEWERPROCESSIMAGE_H
#define UDGSTARVIEWERPROCESSIMAGE_H
//#include <qstring.h>
#include <qobject.h>
#include <qwidget.h>
#include "processimage.h"
#include "cachepacs.h"
#include "image.h"
#include <string.h>

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
    void imageRetrieved(Image *,int);
    void seriesRetrieved(QString studyUID);

private :

    CachePacs *m_localCache;
    int m_downloadedImages;
    std::string m_studyUID,m_oldSeriesUID;
    bool m_error;
//
};

};

#endif

