/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTARVIEWERPROCESSIMAGERETRIEVED_H
#define UDGSTARVIEWERPROCESSIMAGERETRIEVED_H

#include <QObject>

#include "processimage.h"

namespace udg {

/**
@author marc
*/
class StarviewerProcessImageRetrieved: public QObject, public ProcessImage{

Q_OBJECT

public:

    ///Constructor de la classe
    StarviewerProcessImageRetrieved();

    void process(DICOMTagReader *dicomTagReader);

    /// Retorna si s'ha produit algun error intentant guardar alguna de les imatges al disc dur
    bool getError();

    ///Destructor de la classe
    ~StarviewerProcessImageRetrieved();

signals:
    ///signal que s'ement quant s'ha descarregat un fitxer
    void fileRetrieved(DICOMTagReader *dicomTagReader);

private:
    /// Indica que s'ha produit algun error intentant guardar alguna de les imatges al disc dur
    void setError();

private:

    bool m_error;

};

};

#endif

