#ifndef UDGDISPLAYSHUTTER_H
#define UDGDISPLAYSHUTTER_H

#include <QPolygon>

class QColor;

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

    /// Retorna el shutter en forma de QPolygon
    QPolygon getAsQPolygon() const;
    
    /// Donada una llista de shutters, ens retorna el shutter resultant de la intersecció d'aquests. 
    /// En quant al color resultant, serà la mitjana de tots els shutters de la llista.
    static DisplayShutter intersection(const QList<DisplayShutter> &shuttersList);

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
