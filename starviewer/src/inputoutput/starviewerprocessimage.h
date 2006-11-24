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

namespace udg {

class Status;
class Image;
class Series;

/**
@author marc
*/
class StarviewerProcessImage: public QObject, public ProcessImage{

Q_OBJECT

public:

    ///Constructor de la classe
    StarviewerProcessImage();

    /** Processa la informacio de la imatge a descarregar, la guarda a la cache, si la imatge pertany a una nova sèrie també guarda la informació de la sèrie
     * @param imatge a processar 
     */
    void process( Image* image );
    
    /// Retorna si s'ha produit algun error intentant guardar alguna de les imatges al disc dur
    bool getError();

    ///Destructor de la classe    
    ~StarviewerProcessImage();

signals :

    /** signal que s'emet a QExcuteOperationThread per indica que s'ha descarregat una image
      *@param uid de l'estudi
      *@param número d'imatge
      */
    void imageRetrieved( QString studyUID , int );

    /** signal que s'emet a QExcuteOperationThread per indica que s'ha descarregat una sèroe
      *@param número d'imatge
      */
    void seriesRetrieved( QString studyUID );

    /** signal que s'emet a QExcuteOperationThread per indica que s'ha inicia la descàrrega d'un estudi
      *@param número d'imatge
      */
    void startRetrieving( QString );

    /** signal que s'emet a QExcuteOperationThread per indicar que hi ha una sèrie a punt per ser visualitzada
      *@para UID de l'estudi
      */
    void seriesView ( QString );
    
private :

    int m_downloadedImages , m_downloadedSeries;
    QString m_oldSeriesUID , m_studyUID;
    bool m_error;
    
    /** Crea el path de la imatge d'on obtenir la informació de les series
     * @param imatge de la que s'ha d'obtenir el path
     */
    QString createImagePath( Image* image );
    
    /** Retorna la informació de la sèrie de la imatge que es troba al path del paràmetre
     * @param path de la imatge d'on obtenir la informació de la sèrie
     */
    Status getSeriesInformation( QString imagePath , Series &serie );

    /// Indica que s'ha produit algun error intentant guardar alguna de les imatges al disc dur
    void setError();
};

};

#endif

