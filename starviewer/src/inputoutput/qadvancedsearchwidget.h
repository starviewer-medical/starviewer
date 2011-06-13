
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
    /// Posa un * al tab el qual pertany el control que se li ha donat valor, per a que l'usuari sàpiga quin tabs ha posat valors de cerca
    void updateAdvancedSearchModifiedStatus();

    ///Quan canvia la data del QDateEdit a partir quina data de naixament s'ha de cercar, actualizem el QDateEdit de fins quina data s'ha de cercar amb
    ///la mateixa data
    void fromPatientBirthDateChanged();
    //Quan canvia la data comprova que la data desde no sigui major a aquesta, si ho és assigna a la data des de la mateixa que la data fins
    void toPatientBirthDateChanged();

private:
    
    void initialize();

    /// Crea les connexions entre signals i slots
    void createConnections();

    ///Converteix un string en format de data DICOM YYYYMMDD-YYYYMMDD, YYYYMMDD-, YYYYMMDD, ... a un Rang de dates en que s'ha de fer la cerca. 
    ///Si l'String no té el format DICOM de data correcte Retorna QDate() invàlids (Consultar PS 3.5 el Value Representation DA per veure quin format pot tenir)    
    QPair<QDate, QDate> getDateRangeToSearchFromString(QString dateRangeAsDICOMFormat);

    ///Converteix un string en format de temps DICOM HHMMSS-HHMMSS, HHMMSS-, HHMMSS, ... a un rang de Time en que s'ha de fer la cerca
    ///Si l'String no té el format DICOM de temps correcte Retorna QTime() invàlids (Consultar PS 3.5 el Value Representation TM per veure quin format pot tenir)
    QPair<QTime, QTime> getTimeRangeToSearchFromString(QString timeRangeAsDICOMFormat);

    ///Ens indica si el QObject és un QLineEdit enabled i conté text
    bool isQLineEditEnabledAndIsNotEmpty(QObject *);
    ///Ens indica si el QOjbect és un QCheckBox i conté text
    bool isQCheckboxAndIsChecked(QObject *);

    ///Retorna el Widgets del control que he afegit nosaltres a la UI
    QList<QObject*> getMemberWidgets();

private:

    static const QRegExp regExpGetMemberWidgets;
};

}

#endif
