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

#ifndef UDG_QMEDICALDEVICEINFORMATIONDIALOG_H
#define UDG_QMEDICALDEVICEINFORMATIONDIALOG_H

#include "ui_qmedicaldeviceinformationdialogbase.h"

namespace udg {

/**
 *  @brief The QMedicalDeviceInformationDialog class is a dialog containing important information regarding the use of the application as medical device.
 */
class QMedicalDeviceInformationDialog : public QDialog, private ::Ui::QMedicalDeviceInformationDialogBase
{
    Q_OBJECT

public:
    explicit QMedicalDeviceInformationDialog(QWidget *parent = nullptr);
    ~QMedicalDeviceInformationDialog();

};

} // namespace udg

#endif // UDG_QMEDICALDEVICEINFORMATIONDIALOG_H
