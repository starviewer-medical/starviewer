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

#ifndef UDGWINDOWLEVEL_H
#define UDGWINDOWLEVEL_H

#include <QString>

class vtkLookupTable;

namespace udg {

/**
    Classe que encapsula el concepte de Window Level.
  */
class WindowLevel {

public:
    WindowLevel();
    WindowLevel(double width, double center, const QString &name = QString());
    ~WindowLevel();

    /// Defineix/obté la descripció window level
    void setName(const QString &name);
    const QString& getName() const;
    
    /// Defineix/obté el width del window level
    void setWidth(double width);
    double getWidth() const;
    
    /// Defineix/obté el center del window level
    void setCenter(double center);
    double getCenter() const;

    /// Ens determina si els valors de width i level són vàlids.
    /// Un WindowLevel serà sempre vàlid excepte en el cas que width sigui 0
    bool isValid() const;

    /// Compara si els valors de window width i window level són iguals amb el WindowLevel passat per paràmetre.
    /// La descripció no es té en compte en aquesta comparació
    bool valuesAreEqual(const WindowLevel &windowLevel) const;

    /// Compara que tots els membres de la classe siguin iguals
    bool operator==(const WindowLevel &windowLevelToCompare) const;

    /// Returns a vtkLookupTable representing this WindowLevel.
    vtkLookupTable* toVtkLookupTable() const;

private:
    QString m_name;
    double m_width;
    double m_center;
};

}

#endif
