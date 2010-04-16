/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQANGIOSUBSTRACTION_H
#define UDGQANGIOSUBSTRACTION_H

#include "ui_qangiosubstractionextensionbase.h"

#include <QString>

// FWD declarations
class QAction;
class vtkImageMask;
class vtkActor;

namespace udg {

// FWD declarations
class Volume;
class ToolManager;
class TransDifferenceToolData;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QAngioSubstractionExtension : public QWidget , private Ui::QAngioSubstractionExtensionBase {
Q_OBJECT
public:
    QAngioSubstractionExtension( QWidget *parent = 0 );
    ~QAngioSubstractionExtension();

public slots:
    /// Li assigna el volum principal
    void setInput( Volume *input );

private:
    void initializeTools();

    /// Crea les connexions entre signals i slots
    void createConnections();

	/// Guarda i llegeix les característiques
    void readSettings();
    void writeSettings();

private slots:

	/// Calcula la imatge diferència respecte la imatge imageid
	void computeDifferenceImage( int imageid );

    /// Calcula automàticament (registre) la imatge diferència respecte la imatge imageid
    void computeAutomateSingleImage( );

	///Permet sincronitzar els dos viewers
	void synchronization( Q2DViewerWidget * viewer, bool active );

private:
    /// El volum principal
    Volume *m_mainVolume;
    Volume *m_differenceVolume;

    /// Tool manager
    ToolManager *m_toolManager;
    TransDifferenceToolData *m_tdToolData;

    /// Dades de les transformacions aplicades a cada llesca
    QVector< QPair<int,int> > m_sliceTranslations;

};

} // end namespace udg

#endif
