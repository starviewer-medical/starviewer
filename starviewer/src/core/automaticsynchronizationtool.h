#ifndef UDGAUTOMATICSYNCHRONIZATIONTOOL_H
#define UDGAUTOMATICSYNCHRONIZATIONTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class AutomaticSynchronizationToolData;
class ImagePlane;

/**
    Tool de sincronització automatica entre visualitzadors.
*/

class AutomaticSynchronizationTool : public Tool {
Q_OBJECT

public:

    /// Constructor i destructor
    AutomaticSynchronizationTool(QViewer *viewer, QObject *parent = 0);
    ~AutomaticSynchronizationTool();

    /// Processament dels events
    void handleEvent(unsigned long eventID);

    /// Per posar les dades de  (compartides)
    virtual void setToolData(ToolData *data);

    /// Per obtenir les dades (per compartir)
    virtual ToolData* getToolData() const;

public slots:

    /// Actualitza les dades si el visor està actiu
    void changePositionIfActive();

private slots:

    /// Actualitza la posició segons les dades de la toolData
    void updatePosition();

    /// Reinicialitza les dades
    void reset();

private:

    /// Posa la posició actual a les dades, sobreescrivint l'existent si n'hi ha
    void setPositionToToolData();

private:
    
    /// Dades de la tool
    AutomaticSynchronizationToolData *m_toolData;

    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Ultima llesca a partir de la qual calcular l'increment
    int m_lastSlice;

    /// Espai entre llesques perdut per arrodoniment
    double m_roundLostSpacingBetweenSlices;

};

}

#endif
