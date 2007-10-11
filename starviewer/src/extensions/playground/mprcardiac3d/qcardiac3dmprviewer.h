/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQCARDIAC3DMPRVIEWER_H
#define UDGQCARDIAC3DMPRVIEWER_H

#include "q3dmprviewer.h"

class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImagePlaneWidget;
class vtkActor;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;
class vtkLookupTable;
class vtkInteractorStyle;

namespace udg {

/**
    Visor de Reconstrucció multiplanar 3D pel cor

@author Grup de Gràfics de Girona  ( GGG )
*/

// FWD declarations


class QCardiac3DMPRViewer : public Q3DMPRViewer{
Q_OBJECT
public:
    QCardiac3DMPRViewer( QWidget *parent = 0 );

    ~QCardiac3DMPRViewer();

    virtual void setInput( Volume *inputImage );

    virtual void resetPlanes();

public slots:

    void setSubVolume( int index );

private:

    int m_actualSubVolume;

protected:
    virtual void createOutline();

    virtual void initializeWindowLevel();

    virtual void updatePlanesData();

};

}; //  end  namespace udg

#endif
