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

#ifndef UDGDRAWERBITMAP_H
#define UDGDRAWERBITMAP_H

#include "drawerprimitive.h"

#include <vtkSmartPointer.h>

class QColor;
class vtkImageActor;

namespace udg {

/**
    Classe que representa una primitiva per mapa de bits. Aquest mapa té una produnditat de color d'1 bit,
    per tant només accepta dos valors diferents. El bitmap vindrà definit per la seva longitud, alçada i dades (array de bytes).
    
    Ens permet definir quin és el color de fons (background), la seva opacitat i quin és el color principal (foreground).
    Es prendrà el valor 0 com a valor de fons, qualsevol altre valor diferent, serà pres com el color principal.

    També ens permet assignar un origen i spacing per poder-lo ajustar sobre la imatge que l'estem veient.
 */
class DrawerBitmap : public DrawerPrimitive {
Q_OBJECT
public:
    DrawerBitmap(QObject *parent = 0);
    ~DrawerBitmap();

    /// Assigna l'origen TODO Potser amb dimensió 2 seria suficient
    void setOrigin(double origin[3]);
    
    /// Assigna l'espaiat TODO Potser amb dimensió 2 seria suficient
    void setSpacing(double spacing[3]);

    /// Assigna les mides del bitmap i les seves dades. El nombre d'elements que es comptabilitzaran de data seran width * height
    void setData(unsigned int width, unsigned int height, unsigned char *data);
    
    /// Determina l'opacitat del valor de background
    void setBackgroundOpacity(double opacity);

    /// Determina el color amb que es mapeja el color de fons
    void setBackgroundColor(const QColor &color);

    /// Determina el color amb que es mapeja el color principal
    void setForegroundColor(const QColor &color);

    virtual vtkProp* getAsVtkProp() override;

    virtual double getDistanceToPointInDisplay(const Vector3 &displayPoint, Vector3 &closestDisplayPoint,
                                               std::function<Vector3(const Vector3&)> worldToDisplay) override;

    virtual std::array<double, 4> getDisplayBounds(std::function<Vector3(const Vector3&)> worldToDisplay) override;

public slots:
    virtual void update() override;

protected slots:
    virtual void updateVtkProp() override;

private:
    /// Origen del bitmap
    double m_origin[3];

    /// Espaiat del bitmap
    double m_spacing[3];

    /// Mides del bitmap
    unsigned int m_width, m_height;
    
    /// Array amb les dades del bitmap
    unsigned char *m_data;
    
    /// Opacitat del valor de background. El seu valor per defecte serà 0.0, és a dir transparent
    double m_backgroundOpacity;

    /// Color amb el que es pintarà el valor de fons. El seu valor per defecte serà Qt::black
    QColor m_backgroundColor;

    /// Color amb el que es pintarà el que no sigui el fons. El seu valor per defecte serà Qt::white
    QColor m_foregroundColor;

    /// Actor vtk amb el que pintarem el bitmap
    vtkSmartPointer<vtkImageActor> m_imageActor;
};

} // end namespace udg

#endif // UDGDRAWERBITMAP_H
