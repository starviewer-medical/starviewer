/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

 
#ifndef DIRECTOR_H
#define DIRECTOR_H

#include <qobject.h>

namespace udg{
class QApplicationMainWindow;
}

/**
    Classe base Director. La seva funció serà coordinar les accions que l'usuari realitzi a través de la interfície amb els paràmetres interns de cada mètode, interactuant amb la interfície.
    Aquesta classe fa de pont entre la interfície d'usuari i la implementació pura
    d'un mètode.
    Aquesta és una classe abstracta de la qual s'heredarà una sub-classe per cada mètode implementat
    que proporciona la interfície comuna per a totes.
    
    Aquesta classe s'encarregarà per exemple d'actualitzar els paràmetres d'un mètode de segmentació que
    un usuari està manipul·lant a través d'un usuari, de dirigir la seva execució i validació dels 
    paràmetres, l'apertura de noves finestres, mostrant resultats, vistes, etc.
    
    En el constructor li proporcionem un punter a la finestra principal perquè pugui interactuar amb 
    ella.
    
    L'slot execute és el que s'encarrega de l'exeucució d'un cert algorisme, mostrar finestres, etc.
    aquí és purament virtual, ja que cada sub-classe l'haurà d'implementar de diferent manera.
*/

namespace udg{

class Director : public QObject{

Q_OBJECT

public:
    
    Director( udg::QApplicationMainWindow* app, QObject *parent = 0, const char *name = 0 );
    virtual ~Director();
    
public slots:
    /** 
        Aquest slot s'encarrega d'executar totes les tasques del mètode un cop tenim els paràmetres
        a punt. Entre aquestes tasques tindrem l'execució de l'algorisme i la mostra de resultats.
        Aquest slot és purament virtual i haurà de ser reimplementat en cadascuna de les subclasses
    */
    virtual void execute() = 0;

protected:
    udg::QApplicationMainWindow* m_applicationWindow;
    
};

}; // end namespace udg
#endif

