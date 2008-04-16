/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSCREENSHOTTOOL_H
#define UDGSCREENSHOTTOOL_H

#include "tool.h"

class vtkWindowToImageFilter;
class vtkRenderWindow;
class QString;

namespace udg {

/**
Fa captures dels visors

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QViewer;

class ScreenShotTool: public Tool
{
Q_OBJECT
public:
    ScreenShotTool(QViewer *viewer, QObject *parent = 0 );

    ~ScreenShotTool();

    void handleEvent( unsigned long eventID );

private slots:
    /// Captura la imatge actual
    void screenShot();
    
    ///llegeix les configuracions que han estat guardades
    void readSettings();

    ///guarda les configuracions
    void writeSettings();
    
    ///decodifica el nom del fitxer a partir de tot el path
    void decodeFileName( QString fileName );
    
    ///composa un nom de fitxer per defecte
    QString compoundSelectedName();

private:
    /// El filtre per obtenir la imatge
    vtkWindowToImageFilter *m_windowToImageFilter;

    ///El render window, necessari per al filtre per desar la imatge
    vtkRenderWindow *m_renderWindow;
    
    ///path de l'última imatge guardada
    QString m_lastScreenShotPath;
    
    ///extensió de l'última imatge guardada
    QString m_lastScreenShotExtension;
    
    ///nom de l'última imatge guardada
    QString m_lastScreenShotName;
};

}

#endif
