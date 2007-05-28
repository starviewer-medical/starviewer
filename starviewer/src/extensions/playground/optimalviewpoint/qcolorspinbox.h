/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef QCOLORSPINBOX_H
#define QCOLORSPINBOX_H

#include <QSpinBox>

/**
	@author Grup de Gràfics de Girona  ( GGG ) <jspinola@gmail.com>
*/
class QColorSpinBox : public QSpinBox
{
Q_OBJECT
public:
    QColorSpinBox(QWidget *parent = 0);

    virtual ~QColorSpinBox();

    QColor getColor() const;

public slots:

    void setColor( const QColor & color );

private slots:

    void setOpacity( int opacity );

private:

    QColor m_color;

};

#endif
