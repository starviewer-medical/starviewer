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

#ifndef UDG_QLICENSEDIALOG_H
#define UDG_QLICENSEDIALOG_H

#include <QDialog>
#include "ui_qlicensedialogbase.h"

namespace udg {

/**
 * @brief The QLicenseDialog class shows information about the licenses used in the application.
 */
class QLicenseDialog : public QDialog, private ::Ui::QLicenseDialogBase
{
    Q_OBJECT

public:
    explicit QLicenseDialog(QWidget *parent = 0);

};


} // namespace udg

#endif // UDG_QLICENSEDIALOG_H
