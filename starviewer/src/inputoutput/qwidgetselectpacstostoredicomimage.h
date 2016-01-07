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


#ifndef UDGQQWIDGETSELECTPACSTOSTOREDICOMIMAGE_H
#define UDGQQWIDGETSELECTPACSTOSTOREDICOMIMAGE_H

#include "ui_qwidgetselectpacstostoredicomimagebase.h"

namespace udg {

class PacsDevice;

///
class QWidgetSelectPacsToStoreDicomImage : public QWidget, private Ui::QWidgetSelectPacsToStoreDicomImageBase {
Q_OBJECT

public:
    QWidgetSelectPacsToStoreDicomImage(QWidget *parent = 0);

    /// Retorna la llista de PACS Seleccionats
    QList<PacsDevice> getSelectedPacsToStoreDicomImages();

protected:
    void showEvent (QShowEvent *event);

signals:
    /// Es llança signal indicant que s'ha de guardar al pacs passat per paràmetr les imatges de l'estudi indicat que compleixin la DicomMask.
    void selectedPacsToStore();

private slots:
    /// Enables the store button if any PACS is selected, and disables it when no PACS is selected.
    void updateStoreButton();
    /// Slot que respón quan fan click al botó Store, fa signal indicant que ja han seleccionat els PACS
    void storeImagesToSelectedPacs();

private:
    void createConnections();

};
} // end namespace udg.

#endif
