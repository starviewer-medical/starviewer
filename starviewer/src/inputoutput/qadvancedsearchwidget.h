
#ifndef UDGQADVANCEDSEARCHWIDGET_H
#define UDGQADVANCEDSEARCHWIDGET_H

#include <ui_qadvancedsearchwidgetbase.h>

#include <QPair>
#include <QTime>
#include <QDate>

class QString;

namespace udg {

class DicomMask;

/**
    Classe de cerca bàsica d'elemens
  */
class QAdvancedSearchWidget : public QWidget, private Ui::QAdvancedSearchWidgetBase {
Q_OBJECT
public:
    QAdvancedSearchWidget(QWidget *parent = 0);

    /// Neteja els camps de la cerca bàsica
    void clear();
    /// Construeix la màscara de cerca
    DicomMask buildDicomMask();

private slots:
    /// Posa un * al tab el qual pertany el lineedit que se li ha donat valor, per a que l'usuari sàpiga quin tabs ha posat valors de cerca
    void updateAdvancedSearchModifiedStatus();

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    ///Converteix un string en format de data DICOM YYYYMMDD-YYYYMMDD, YYYYMMDD-, YYYYMMDD, ... a un Rang de dates en que s'ha de fer la cerca. 
    ///Si l'String no té el format DICOM de data correcte Retorna QDate() invàlids (Consultar PS 3.5 el Value Representation DA per veure quin format pot tenir)    
    QPair<QDate, QDate> getDateRangeToSearchFromString(QString dateRangeAsDICOMFormat);

    ///Converteix un string en format de temps DICOM HHMMSS-HHMMSS, HHMMSS-, HHMMSS, ... a un rang de Time en que s'ha de fer la cerca
    ///Si l'String no té el format DICOM de temps correcte Retorna QTime() invàlids (Consultar PS 3.5 el Value Representation TM per veure quin format pot tenir)
    QPair<QTime, QTime> getTimeRangeToSearchFromString(QString timeRangeAsDICOMFormat);
};

}

#endif
