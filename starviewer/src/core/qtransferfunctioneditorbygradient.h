/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech AS. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQTRANSFERFUNCTIONEDITORBYGRADIENT_H
#define UDGQTRANSFERFUNCTIONEDITORBYGRADIENT_H


#include "qtransferfunctioneditor.h"

#include "transferfunction.h"


namespace udg {


class ShadeWidget;


/**
 * Editor de funcions de transferència basat en gràfiques.
 *
 * L'usuari pot modificar 4 gràfiques (una per cada component RGBA)
 * independentment. L'eix X representa els valors de propietat i l'eix Y la
 * intensitat del component. L'usuari ha d'introduir els punts que defineixen la
 * funció de transferència.
 *
 * Un clic amb el botó esquerre crea un punt nou. Un clic amb el botó dret sobre
 * un punt l'esborra. Els punts existents es poden arrossegar mantenint premut
 * el botó esquerre per modificar-los.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class QTransferFunctionEditorByGradient : public QTransferFunctionEditor
{

    Q_OBJECT

public:

    QTransferFunctionEditorByGradient( QWidget * parent = 0 );
    virtual ~QTransferFunctionEditorByGradient();

    /// Assigna una funció de transferència a l'editor.
    virtual void setTransferFunction( const TransferFunction & transferFunction );
    /// Retorna la funció de transferència de l'editor.
    virtual const TransferFunction & getTransferFunction() const;

    void setGradientStops(const QGradientStops &stops);

public slots:
    void pointsUpdated();

signals:
    void gradientStopsChanged(const QGradientStops &stops);

private slots:

    void setTransferFunction( const QGradientStops & stops );

private:
    ShadeWidget *m_red_shade;
    ShadeWidget *m_green_shade;
    ShadeWidget *m_blue_shade;
    ShadeWidget *m_alpha_shade;

    /// Funció de transferència actual.
    TransferFunction m_transferFunction;

};


}


#endif
