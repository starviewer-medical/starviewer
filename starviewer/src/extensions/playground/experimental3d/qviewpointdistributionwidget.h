#ifndef UDGQVIEWPOINTDISTRIBUTIONWIDGET_H
#define UDGQVIEWPOINTDISTRIBUTIONWIDGET_H


#include "ui_qviewpointdistributionwidgetbase.h"

#include <QVector>

#include "vector3.h"


namespace udg {


class ViewpointGenerator;


/**
 * Widget que permet seleccionar un nombre de punts de vista distribuïts uniformement o quasi-uniforme.
 */
class QViewpointDistributionWidget : public QWidget, private ::Ui::QViewpointDistributionWidgetBase {

    Q_OBJECT

public:

    QViewpointDistributionWidget( QWidget *parent = 0 );
    ~QViewpointDistributionWidget();

    /// Selecciona la distribució uniforme amb 4 punts.
    void setToUniform4();
    /// Selecciona la distribució uniforme amb 6 punts.
    void setToUniform6();
    /// Selecciona la distribució uniforme amb 8 punts.
    void setToUniform8();
    /// Selecciona la distribució uniforme amb 12 punts.
    void setToUniform12();
    /// Selecciona la distribució uniforme amb 20 punts.
    void setToUniform20();
    /// Selecciona la distribució quasi-uniforme amb el nivell de recursió \a recursionLevel.
    void setToQuasiUniform( int recursionLevel );

    /// Retorna cert si s'ha seleccionat una distribució uniforme.
    bool isUniform() const;
    /// Retorna el nombre de punts de vista seleccionat.
    unsigned int numberOfViewpoints() const;
    /// Retorna el nivell de recursió seleccionat.
    int recursionLevel() const;
    /// Retorna la llista de punts de vista d'acord amb les opcions seleccionades.
    QVector<Vector3> viewpoints() const;
    /// Retorna un generador de punts de vista configurat d'acord amb les opcions seleccionades.
    ViewpointGenerator viewpointGenerator( float radius = 1.0f ) const;

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
