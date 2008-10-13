/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUME3DFUSEDLIGHT_H
#define UDGVOLUME3DFUSEDLIGHT_H

#include <QObject>

class vtkRenderer;
class vtkVtkLightKit;
namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Volume3DFusedLight : public QObject
{
Q_OBJECT
public:
    Volume3DFusedLight(QObject *parent = 0);

    ~Volume3DFusedLight();

};

}

#endif
