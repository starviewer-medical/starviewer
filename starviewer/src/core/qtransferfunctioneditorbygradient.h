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

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
    Contact: Qt Software Information (qt-info@nokia.com)

    This file is part of the demonstration applications of the Qt Toolkit.

    $QT_BEGIN_LICENSE:LGPL$
    Commercial Usage
    Licensees holding valid Qt Commercial licenses may use this file in
    accordance with the Qt Commercial License Agreement provided with the
    Software or, alternatively, in accordance with the terms contained in
    a written agreement between you and Nokia.

    GNU Lesser General Public License Usage
    Alternatively, this file may be used under the terms of the GNU Lesser
    General Public License version 2.1 as published by the Free Software
    Foundation and appearing in the file LICENSE.LGPL included in the
    packaging of this file. Please review the following information to
    ensure the GNU Lesser General Public License version 2.1 requirements
    will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.

    In addition, as a special exception, Nokia gives you certain
    additional rights. These rights are described in the Nokia Qt LGPL
    Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
    package.

    GNU General Public License Usage
    Alternatively, this file may be used under the terms of the GNU
    General Public License version 3.0 as published by the Free Software
    Foundation and appearing in the file LICENSE.GPL included in the
    packaging of this file. Please review the following information to
    ensure the GNU General Public License version 3.0 requirements will be
    met: http://www.gnu.org/copyleft/gpl.html.

    If you are unsure which license is appropriate for your use, please
    contact the sales department at qt-sales@nokia.com.
    $QT_END_LICENSE$
 *************************************************************************************/

#ifndef UDGQTRANSFERFUNCTIONEDITORBYGRADIENT_H
#define UDGQTRANSFERFUNCTIONEDITORBYGRADIENT_H

#include "qtransferfunctioneditor.h"

#include <QLineEdit>
#include "transferfunction.h"

namespace udg {

class ShadeWidget;

/**
    Editor de funcions de transferència basat en gràfiques.

    L'usuari pot modificar 4 gràfiques (una per cada component RGBA)
    independentment. L'eix X representa els valors de propietat i l'eix Y la
    intensitat del component. L'usuari ha d'introduir els punts que defineixen la
    funció de transferència.

    Un clic amb el botó esquerre crea un punt nou. Un clic amb el botó dret sobre
    un punt l'esborra. Els punts existents es poden arrossegar mantenint premut
    el botó esquerre per modificar-los.

    \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
  */
class QTransferFunctionEditorByGradient : public QTransferFunctionEditor {
Q_OBJECT

public:
    QTransferFunctionEditorByGradient(QWidget *parent = 0);
    virtual ~QTransferFunctionEditorByGradient();

    /// Assigna una funció de transferència a l'editor.
    virtual void setTransferFunction(const TransferFunction &transferFunction);
    /// Retorna la funció de transferència de l'editor.
    virtual const TransferFunction& getTransferFunction() const;

    void setGradientStops(const QGradientStops &stops);

public slots:
    void pointsUpdated();

signals:
    void gradientStopsChanged(const QGradientStops &stops);

private slots:
    void setTransferFunction(const QGradientStops &stops);
    void setTransferFunctionName(const QString &name);

private:
    QLineEdit *m_nameLineEdit;
    ShadeWidget *m_red_shade;
    ShadeWidget *m_green_shade;
    ShadeWidget *m_blue_shade;
    ShadeWidget *m_alpha_shade;

    /// Funció de transferència actual.
    TransferFunction m_transferFunction;

};

}

#endif
