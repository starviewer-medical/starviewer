/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROITOOLREPRESENTATION_H
#define UDGROITOOLREPRESENTATION_H

#include "toolrepresentation.h"
#include "volume.h"

#include <QPointer>

class vtkImageData;

namespace udg {

class DrawerPolygon;
class DrawerText;

/**
    Representació base per tools de ROI
*/
class ROIToolRepresentation : public ToolRepresentation {
Q_OBJECT
public:
    ROIToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~ROIToolRepresentation();

    /// Se li passa el polígon ja dibuixat per un outliner
    void setPolygon(DrawerPolygon *polygon);

    /// Se li passen els paràmetres necessaris pels càlculs de la ROI
    void setParameters(int view, int slice, Volume *input, vtkImageData *slabProjection, bool m_thickSlabInUse);

    /// Fa els càlculs pertinents i escriu el text
    void calculate();

protected:
    /// Mètode per escriure a pantalla les dades calculades.
    void printData();

    /// Col·loca el texte al seu lloc corresponent segons els límits i forma de la ROI
    void placeText();
    
    virtual void createHandlers() = 0;

    QList<double *> mapHandlerToPoint(ToolHandler *toolHandler);

protected:
    /// Polígon que conté la forma final dibuixada
    QPointer<DrawerPolygon> m_roiPolygon;

    /// Paràmetres necessaris per la representació
    int m_view;
    int m_slice;
    // TODO Fer que amb m_slabProjection sigui suficient i no necessitar m_thickSlabInUse
    Volume *m_input;
    vtkImageData *m_slabProjection;
    bool m_thickSlabInUse;

private:
    /// Calcula el voxel a partir de l'espaiat de la imatge i la coordenada i retorna el valor de gris
    Volume::VoxelType getGrayValue(double *coordinate);
    
    /// Calcula les dades estadístiques de la ROI. 
    /// Serà necessari cridar aquest mètode abans si volem obtenir la mitjana i/o la desviació estàndar
    void computeStatisticsData();

private slots:
    /// Edita els vèrtexs de les DrawerPrimitive
    void moveAllPoints(double *movement);

private:
    /// Texte amb el resultat del càlcul
    QPointer<DrawerText> m_text;

    /// Llista amb els valors de gris per calcular la mitjana i la desviació etàndard
    QList<double> m_grayValues;

    /// Controlarà si cal o no recalcular les dades estadístiques
    /// Quan modifiquem la roi (afegint punts, creant una de nova, etc) el valor serà true
    /// Un cop s'hagin calculat, serà false fins que no es torni a modificar
    /// Per defecte el valor és true
    bool m_hasToComputeStatisticsData;
    
    /// Mitjana de valors de la ROI
    double m_mean;

    /// Desviació estàndar de la ROI
    double m_standardDeviation;
};

}

#endif


