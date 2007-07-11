/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGE_H
#define UDGIMAGE_H

#include <QObject>

namespace udg {

/**
Classe que encapsula les propietats d'una imatge d'una sèrie de la classe Series

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Series;

class Image : public QObject
{
Q_OBJECT
public:
    Image(QObject *parent = 0);

    ~Image();

    /// Li indiquem quina és la sèrie pare a la qual pertany
    void setParentSeries( Series *series ){ m_parentSeries = series; };

    /// assigna el path de la imatge \TODO absolut/relatiu????
    void setPath( QString path ){ m_path = path; };

private:
    /// el path de la imatge \TODO absolut/relatiu????
    QString m_path;

    /// La sèrie pare
    Series *m_parentSeries;
};

}

#endif
