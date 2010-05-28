/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCURVEDMPREXTENSION_H
#define UDGCURVEDMPREXTENSION_H

#include "ui_curvedmprextensionbase.h"

// Qt's
#include <QPointer>

namespace udg {

// FWD declarations
class Volume;
class ToolManager;
class DrawerPolyline;

/**
    Aquesta extensió integra les eines per poder realitzar un MPR Curvilini
*/
class CurvedMPRExtension : public QWidget , private ::Ui::CurvedMPRExtensionBase {
Q_OBJECT
public:
    CurvedMPRExtension( QWidget *parent = 0 );
    ~CurvedMPRExtension();

public slots:
    /// Assigna el volum amb el que s'aplica l'MPR Curvilini
    void setInput( Volume *input );

    /// Inicia el procés de creació del reslicedVolume que caldrà visualitzar al segon viewer
    /// Es crida quan l'usuari indica la línia sobre la que caldrà projectar
    void updateReslice( QPointer<DrawerPolyline> polyline );

private:
    /// Inicialitza les tools que tindrà l'extensió
    void initializeTools();

    /// Porta a terme l'MPR Curvilini per obtenir el nou volum amb la reconstrucció
    /// que cal assignar el segon viewer
    Volume* doCurvedReslice( Volume *volume, QPointer<DrawerPolyline> polyline );

    /// Es costrueix una llista amb tots els punts que hi ha sobre la polyline indicada per
    /// l'usuari i que cal tenir en compte al fer la reconstrucció
    void getPointsPath( QPointer<DrawerPolyline> polyline, double pixelsDistance, QList< double * > *pointsPath );

    /// S'inicialitzen i s'emplenen les dades VTK que han de formar el volum de la reconstrucció.
    void initAndFillImageDataVTK( Volume *volume, QList< double * > *pointsPath, vtkImageData *imageDataVTK );

private:
    /// El volum al que se li practica l'MPR Curvilini
    Volume *m_volume;

    /// ToolManager per configurar l'entorn de tools de l'extensió
    ToolManager *m_toolManager;
};

};  //  end  namespace udg

#endif
