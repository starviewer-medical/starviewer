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

#ifndef UDGDISPLAYSHUTTER_H
#define UDGDISPLAYSHUTTER_H

#include <QPolygon>

#include "vtkSmartPointer.h"

class QColor;
class QImage;

class vtkImageData;

namespace udg {

/*
    Un display shutter consisteix en una forma geomètrica (cercle, rectangle o polígon)
    que defineix la zona que ha de ser visible en la imatge que s'associa. La part continguda dins de la forma és la visible i la
    que queda fora és la part oclosa.
 */
class DisplayShutter {
public:
    DisplayShutter();
    ~DisplayShutter();

    enum ShapeType { UndefinedShape = 0, RectangularShape, PolygonalShape, CircularShape };
    
    /// Assigna/Obtè la forma del shutter
    void setShape(ShapeType shape);
    ShapeType getShape() const;

    /// Ens retorna la forma en una cadena de text segons els valors establerts pel DICOM
    QString getShapeAsDICOMString() const;
    
    /// Valor de gris amb el que s'hauria de pintar la part opaca del shutter
    void setShutterValue(unsigned short int value);
    unsigned short int getShutterValue() const;

    /// Ens retorna el valor del shutter en forma de QColor
    QColor getShutterValueAsQColor() const;
    
    /// Mètodes per assignar els punts per cada tipus de forma. En cas que el format de punts no es correspongui
    /// amb la forma ja assignada, retornarà fals i no es guardarà cap punt. En cas que no s'hagi definit cap forma
    /// (shape = UndefinedShape) el mètode assignarà automàticament la forma que pertoqui per aquell mètode guardant els punts indicats.
    
    /// Assigna el centre i el radi per la forma circular
    bool setPoints(const QPoint &centre, int radius);
    
    /// Assigna els vèrtexs per la forma poligonal
    bool setPoints(const QVector<QPoint> &vertices);
    
    /// Assigna els punts superior esquerre i inferior dret que defineixen la forma rectangular
    bool setPoints(const QPoint &topLeft, const QPoint &bottomRight);

    /// Assigna els punts en format text. Cada punt tindrà la forma x,x, dos valors enters (x) separats per coma (,) 
    /// Cada punt dins de la llista anirà separat per punt i coma (;)
    /// Per la forma rectangular tindrem 2 punts corresponents al punt superior esquerre i al punt inferior dret del rectangle
    /// Per la forma circular el format serà centre;radi on radi serà un valor enter
    /// Per la forma poligonal el format serà un llistat de punts amb un mínim de 3 elements
    /// En cas que el format de punts no es correspongui amb la forma del shutter, retornarà fals, cert altrament
    /// Si la forma és indefinida, tractarà de veure si es correspon amb algun format de punts definit i li assignarà en cas de trobar-ne correspondència
    bool setPoints(const QString &pointsString);
    
    /// Retorna el shutter en forma de QPolygon
    QPolygon getAsQPolygon() const;

    /// Retorna els punts del shutter en format d'string. El format serà el mateix que el de setPoints(const QString &)
    QString getPointsAsString() const;

    /// Retorna el shutter com una QImage de mides width i height
    QImage getAsQImage(int width, int height) const;

    /// Returns the shutter in vtkImageData format, with extent defined by given width and height.
    vtkSmartPointer<vtkImageData> getAsVtkImageData(int width, int height) const;
    
    /// Donada una llista de shutters, ens retorna el shutter resultant de la intersecció d'aquests. 
    /// En quant al color resultant, serà la mitjana de tots els shutters de la llista.
    static DisplayShutter intersection(const QList<DisplayShutter> &shuttersList);

private:
    /// Comprova que el format de l'string de punts és correcte segons la forma donada
    bool shapeMatchesPointsStringFormat(ShapeType shape, const QString &pointsString);

private:
    /// Forma del shutter
    ShapeType m_shape;

    /// Polígon que defineix el shutter. Independentment de la forma que tingui definida sempre es guardarà internament com un polígon.
    QPolygon m_shutterPolygon;
    
    /// Valor de gris amb el que s'ha de pintar la part opaca del shutter
    unsigned short int m_shutterValue;
};

} // End namespace udg

#endif
