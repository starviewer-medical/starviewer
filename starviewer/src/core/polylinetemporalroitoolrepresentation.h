/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINEROITOOLREPRESENTATION_H
#define UDGPOLYLINEROITOOLREPRESENTATION_H

#include "toolrepresentation.h"
//#include "volume.h"

#include <QPointer>
//#include <vtkImageData.h>
#include <itkImage.h>

namespace udg {
class DrawerPolyline;
class DrawerText;
class PolylineTemporalROIToolData;
class ToolData;

/**
    Tool per calcular l'àrea i la mitjana de grisos d'un polígon
*/
class PolylineTemporalROIToolRepresentation : public ToolRepresentation {
Q_OBJECT
public:
    PolylineTemporalROIToolRepresentation(Drawer *drawer, QObject *parent = 0);
    ~PolylineTemporalROIToolRepresentation();

    /// Se li passa la linia ja dibuixada per un outliner
    void setPolyline(DrawerPolyline *polyline);

    /// Se li passen els parametres necessaris pel calcul
    void setParams(int view, int slice, double *origin, double *spacing);

    /// Fa els calculs pertinents i escriu el text
    void calculate();

    /// Declaració de tipus
    // typedef PolylineTemporalROIToolData::TemporalImageType TemporalImageType;
    typedef itk::Image<double, 4> TemporalImageType;

    /// Retorna les dades pròpies de la seed
    ToolData* getToolData() const;

    /// Assigna les dades pròpies de l'eina (persistent data)
    void setToolData(ToolData *data);

    /// Calcula el voxel a partir de l'espaiat de la imatge i la coordenada i retorna el valor de gris
    //Volume::VoxelType getGrayValue(double *coords);

private:
    /// Mètode que converteix el volum de l'input (que té fases) en imatge itk temporal
    void convertInputImageToTemporalImage();

    /// Mètode per calcular la mitjana temporal de la regio del polyline
    double computeTemporalMean();

    /// Mètode que retorna la serie temporal de mida size en una determinada coordenada 
    QVector<double> getGraySerie(double *coords, int size);

    /// metode per escriure a pantalla les dades calculades.
    //void printData();

    ///metode per buscar els valors de gris que serviran per calcular la mitjana i la desviació estàndard
    //void computeGrayValues();

    /// metode per calcular la mitjana de gris de la regio del polyline
    //double computeGrayMean();

    ///mètode per calcular la desviació estàndard de gris de la regió del polyline
    //double computeStandardDeviation();

    /// Crea els handlers
    void createHandlers();

    /// Mapeja els handlers amb els punts corresponents
    QList<double *> mapHandlerToPoint(ToolHandler *toolHandler);

private slots:
    /// Edita els vèrtexs de les DrawerPrimitive
    void moveAllPoints(double *movement);

private:
    /// Línia que es dibuixa
    QPointer<DrawerPolyline> m_polyline;

    ///Text amb el resultat del càlcul
    //QPointer<DrawerText> m_text;

    ///Parametres necessaris per la representacio
    int m_view;
    int m_slice;
    //const double *m_pixelSpacing;
    double *m_origin;
    double *m_spacing;
    /*Volume *m_input;
    vtkImageData *m_slabProjection;
    int m_use;*/

    ///llista amb els valors de gris per calcular la mitjana i la desviació etàndard
    //QList< double > grayValues;

    /// Dades específiques de la tool
    PolylineTemporalROIToolData *m_myData;
};

}

#endif


