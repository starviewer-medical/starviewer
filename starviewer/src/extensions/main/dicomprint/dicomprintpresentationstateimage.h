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

#ifndef DICOMPRINTPRESENTATIONSTATEIMAGE_H
#define DICOMPRINTPRESENTATIONSTATEIMAGE_H

namespace udg {

/**
    Aquesta classe defineix un Presentation State a aplicar a imatges que s'han d'enviar a imprimir.
  */

class DICOMPrintPresentationStateImage
{
public:
    DICOMPrintPresentationStateImage();

    /// Assigna/Obté el WindowLevel que s'aplicarà a les imatges. Sinó se n'assignat cap retorna 0. Consultar mètode applyDefaultWindowLevelToImage
    void setWindowLevel(double windowCenter, double windowWidth);
    double getWindowCenter() const;
    double getWindowWidth() const;

    /// Indica si s'ha d'aplicat el WindowLevel per defecte que té la Imatge, serà cert en el cas que no s'hagi assignat cap WindowLevel en aquest objecte
    bool applyDefaultWindowLevelToImage() const;

    void setIsFlipped(bool horitzontalFlip);
    bool getIsFlipped() const;

    void setRotateClockWise(int times = 1);
    int getRotateClockWise() const;

private:

    double m_windowWidth;
    double m_windowCenter;

    bool m_applyDefaultWindowLevelToImage;
    bool m_horitzontalFlip;

    int m_rotateClockWiseTimes;
};

}

#endif // DICOMPRINTPRESENTATIONSTATE_H
