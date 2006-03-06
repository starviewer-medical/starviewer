/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#ifndef MUTUALINFORMATIONDIRECTOR_H
#define MUTUALINFORMATIONDIRECTOR_H

#include "director.h"
#include "volume.h"
#include "mutualinformationviolawells.h"


namespace udg{

//Fordware declaration
class MutualInformationParameters;

/**
    Sub-classe de Director. En aquest cas fa de pont amb un mètode de registre basat en mesures 
    d'informació mútua.
    
    Aquesta inclou un punter a la classe de paràmetres específics del seu mètode, la classe que 
    encapsula el mètode de registre ( MutualInformationViolaWells )
    
    Totes les classes heredades de Director, inclouran un mètode setParameters on l'indicarem el 
    punter a la classe de paràmetres corresponent.
    També implementaran l'slot virtual apply, on es farà la feina més important de l'execució d'un 
    mètode
    
*/

class MutualInformationDirector : public Director
{

 Q_OBJECT
 
public:

    // Com que el mètode treballa amb dades itk, hem d'extreure el tipus itk
    // per declarar el mètode que fa servir templates.
    typedef udg::Volume::ItkImageType ImageType;
    typedef udg::MutualInformationViolaWells< ImageType, ImageType > MethodType;    
    
    MutualInformationDirector( udg::QApplicationMainWindow* app, QObject *parent = 0, const char *name = 0 );

    ~MutualInformationDirector();

    
    void setParameters( MutualInformationParameters* parameters );
    
    
public slots:
    /**
        Aquest mètode aplicar el mètode de registre. Comprova primer si té els paràmetres
        necessaris, sinó retornarà fals. Slot públic heredat de Director.
    */
    void execute();

private:

    MethodType *m_method;
    MutualInformationParameters *m_parameters;
    
    udg::QApplicationMainWindow* m_applicationWindow;
    
    udg::Volume* getRegisteredImage(); 
};

}; // end namespace udg

#endif
