/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGMAGICROITOOL_H
#define UDGMAGICROITOOL_H

#include "roitool.h"

#include "sliceorientedvolumepixeldata.h"
#include "vector3.h"
#include "voxelindex.h"

#include <QVector>

namespace udg {

class Q2DViewer;
class Volume;
class DrawerText;
class DrawerPolygon;
class VolumePixelData;
class VoxelIndex;

/**
    Tool que serveix per editar el volum sobreposat en un visor 2D
*/
class MagicROITool : public ROITool {
Q_OBJECT
public:

    MagicROITool(QViewer *viewer, QObject *parent = 0);
    ~MagicROITool();

    void handleEvent(unsigned long eventID);

protected:
    virtual void setTextPosition(DrawerText *text);

private:
    /// Returns the current pixel data from the selected input.
    SliceOrientedVolumePixelData& getPixelData();

    /// Crida a la generació de la regió màgica
    void generateRegion();

    /// Gets the index of the input where the magic ROI has to be drawn
    int getROIInputIndex() const;
    
    /// Returns the slice oriented voxel index in the selected pixel data corresponding to the current picked position.
    const VoxelIndex& getStartVoxelIndex() const;
    
    /// Calcula el rang de valors d'intensitat vàlid a partir de \sa #m_magicSize i \see #m_magicFactor
    std::array<double, 2> computeLevelRange();

    /// Versió iterativa del region Growing
    std::array<int, 2> computeRegionMask();

    /// Fer un moviment des d'un índex cap a una direcció
    /// @param a, @param b índex del volum de la màscara que estem mirant en cada crida
    /// @param movement direcció en el moviment
    void doMovement(int &a, int &b, int movement);

    /// Desfer un moviment dses d'un índex cap a una direcció
    /// @param a, @param b índex del volum de la màscara que estem mirant en cada crida
    /// @param movement direcció en el moviment
    void undoMovement(int &a, int &b, int movement);

    /// Genera el polígon a partir de la màscara
    void computePolygon(const std::array<int, 2> &firstTrueInMask);

    /// Mètodes auxiliar per la generació del polígon
    void getNextIndex(int direction, int x, int y, int &nextX, int &nextY);
    int getNextDirection(int direction);
    int getInverseDirection(int direction);
    void addPoint(int direction, int x, int y);
    bool isLoopReached();

    /// Retorna la desviació estàndard dins la regió marcada per la magicSize
    double getStandardDeviation();

    /// Comença la generació de la regió màgica
    void startRegion();

    /// Calcula la regió definitiva i mostra per pantalla les mesures
    void endRegion();

    /// Modifica el Magic Factor #m_magicFactor segons el desplaçament del ratolí
    void modifyRegionByFactor();

    /// Calcula els bounds de la màscara
    void computeMaskBounds();

    /// Returns the value of the voxel at the given slice oriented voxel index. Only the first component is considered.
    double getVoxelValue(const VoxelIndex &index);

    /// Returns the mask index corresponding to the given x and y image indices.
    int getMaskVectorIndex(int x, int y) const;

    /// Returns the mask value at the given x and y image indices. If the indices are out of bounds, returns false.
    bool getMaskValue(int x, int y) const;

private slots:

    /// Resets this tool to its initial state.
    void reset();

    /// Reinicia la regió, invalidant l'anterior que hi hagués en curs si existia
    void restartRegion();

private:
    /// Possible states of the tool.
    enum State { Ready, Drawing };

    double m_magicFactor;

    /// Màscara de la regió que formarà el polígon
    QVector<bool> m_mask;

    /// Bounds de la màscara
    int m_minX, m_maxX, m_minY, m_maxY;

    /// Coordenades de món a on s'ha fet el click inicial
    Vector3 m_startWorldCoordinate;

    /// Coordenades de pantalla a on s'ha fet el click inicial
    QPoint m_startEventPosition;

    /// Slice oriented voxel index of the first clicked voxel.
    VoxelIndex m_startVoxelIndex;

    /// Polígon ple que es mostrarà durant l'edició de la ROI.
    QPointer<DrawerPolygon> m_filledRoiPolygon;

    /// Index of the input to draw the magic ROI on
    int m_inputIndex;

    /// Current state of the tool.
    State m_state;

    /// Cached pixel data to speed up access to it.
    SliceOrientedVolumePixelData m_cachedPixelData;

};

}

#endif
