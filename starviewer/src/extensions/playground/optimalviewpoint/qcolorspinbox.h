/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQCOLORSPINBOX_H
#define UDGQCOLORSPINBOX_H


#include <QSpinBox>


namespace udg {


/**
 * És un QSpinBox amb un color RGBA associat. El color RGB es visualitza com a
 * color de fons de l'spinbox i l'alfa és el valor de l'spinbox (entre 0 i 255).
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class QColorSpinBox : public QSpinBox
{

    Q_OBJECT

public:

    /// Crea un QColorSpinBox blanc i amb alfa 0.
    QColorSpinBox( QWidget * parent = 0 );
    virtual ~QColorSpinBox();

    /// Retorna el color RGBA associat a l'spinbox.
    const QColor & getColor() const;

public slots:

    /// Assigna un color RGBA a l'spinbox.
    void setColor( const QColor & color );

signals:

    /// S'emet quan canvia el color RGBA associat a l'spinbox.
    void colorChanged( const QColor & color );

private slots:

    /// Assigna una opacitat a l'spinbox.
    void setOpacity( int opacity );

private:

    /// Color RGBA associat a l'spinbox.
    QColor m_color;
    /// Booleà de control per evitar d'emetre dues vegades el signal colorChanged.
    bool m_settingColor;

};


}


#endif
