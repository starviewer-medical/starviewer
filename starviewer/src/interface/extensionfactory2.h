/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEXTENSIONFACTORY_H
#define UDGEXTENSIONFACTORY_H

#include <QObject>
#include <QString>
#include <map>

#include "extensioncreator.h"

class QWidget;

namespace udg {


/**
Factory de les extensions

@author Grup de Gràfics de Girona  ( GGG )
*/
class ExtensionFactory2 : public QObject{
Q_OBJECT
public:
    typedef std::map< QString , ExtensionCreator* > RegisterMapType;
    
    ExtensionFactory2(QObject *parent = 0, const char *name = 0);

    ~ExtensionFactory2();
    
    /// registra l'extensió al factory. \TODO podríem tenir certes extensions ja registrades aquí per defecte i que aquest mètode es faci servir per les noves extensions que es vagin fent.( estil vtkImageReaderFactory )
    bool registerExtension( QString name , ExtensionCreator *extension );
    /// Ens crea l'extensió, és a dir, ens retorna l'extensió registrada amb el nom que li demanem
    /// \TODO QWidget podria quedar substituit per una classe + genèrica com ExtensionBase
    QWidget *createExtension( QString name );

private:
    RegisterMapType m_registerMap;
};

};  //  end  namespace udg 

#endif
