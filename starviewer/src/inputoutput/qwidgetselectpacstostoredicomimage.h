
#ifndef UDGQQWIDGETSELECTPACSTOSTOREDICOMIMAGE_H
#define UDGQQWIDGETSELECTPACSTOSTOREDICOMIMAGE_H

#include "ui_qwidgetselectpacstostoredicomimagebase.h"

namespace udg {

class PacsDevice;

///
class QWidgetSelectPacsToStoreDicomImage : public QWidget , private::Ui::QWidgetSelectPacsToStoreDicomImageBase {
Q_OBJECT

public:
    
    QWidgetSelectPacsToStoreDicomImage(QWidget *parent = 0);

    ///Retorna la llista de PACS Seleccionats
    QList<PacsDevice> getSelectedPacsToStoreDicomImages();

protected :

    void showEvent (QShowEvent * event);

signals:

    ///Es llança signal indicant que s'ha de guardar al pacs passat per paràmetr les imatges de l'estudi indicat que compleixin la DicomMask.
    void selectedPacsToStore();

private slots:

    ///Slot que respón quan fan click al botó Store, fa signal indicant que ja han seleccionat els PACS
    void storeImagesToSelectedPacs();

private :

    void createConnections();

};
} // end namespace udg.

#endif
