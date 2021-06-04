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

#ifndef QCEABOUTCONTENT_H
#define QCEABOUTCONTENT_H

#include "ui_qceaboutcontentbase.h"

namespace udg {

class QCeAboutContent : public QWidget, private ::Ui::QCeAboutContentBase
{
    Q_OBJECT

public:
    explicit QCeAboutContent(QWidget *parent = nullptr);

};

} // namespace udg

#endif // QCEABOUTCONTENT_H
