#ifndef UDGQVIEWPOINTDISTRIBUTIONWIDGET_H
#define UDGQVIEWPOINTDISTRIBUTIONWIDGET_H


#include "ui_qviewpointdistributionwidgetbase.h"


namespace udg {


/**
 * Widget que permet seleccionar un nombre de punts de vista distribuïts uniformement o quasi-uniforme.
 */
class QViewpointDistributionWidget : public QWidget, private ::Ui::QViewpointDistributionWidgetBase {

    Q_OBJECT

public:

    QViewpointDistributionWidget( QWidget *parent = 0 );
    ~QViewpointDistributionWidget();

    /// Retorna cert si s'ha seleccionat una distribució uniforme.
    bool isUniform() const;
    /// Retorna el nombre de punts de vista seleccionat.
    unsigned int numberOfViewpoints() const;
    /// Retorna el nivell de recursió seleccionat.
    int recursionLevel() const;

signals:

    /// Es llança quan canvia el nombre de punts de vista.
    void numberOfViewpointsChanged( int numberOfViewpoints );

private:

    /// Crea les connexions de signals i slots.
    void createConnections();
    /// Retorna el nombre de punts de vista quasi-uniformes.
    unsigned int numberOfQuasiUniformViewpoints() const;

private slots:

    /**
     * Actualitza l'etiqueta del nombre de plans quasi-uniforme segons el nivell de recursió i emet el signal amb el nou nombre de punts de
     * vista.
     */
    void updateNumberOfViewpoints();

};


}


#endif
