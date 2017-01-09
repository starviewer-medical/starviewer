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

#ifndef QDPICONFIGURATIONSCREEN_H
#define QDPICONFIGURATIONSCREEN_H

#include "ui_qdpiconfigurationscreenbase.h"

#include <QtGlobal>

namespace udg {

/**
    @brief Magnification configuration screen.

    Magnification changes the Qt's device to pixel ratio when the application
    opens. This configuration screen shows a preview of the font and icon
    sizes on the next start.

    A setting named "ScaleFactor" is used.

    That setting is an integer number that has to be converted to a
    multiplication factor.

    The integer is splitted in eight 0.125 increment steps, so the scale factor
    passed to Qt is defined by:

    QT_SCALE_FACTOR = 1 + (scaleFactor * 0.125)

 */
class QDPIConfigurationScreen : public QWidget, private Ui::QDPIConfigurationScreenBase {
Q_OBJECT
public:
    /// @brief Constructs the GUI and reads the current configuration.
    /// Some runtime information that will be changed during the preview
    /// interaction is stored in the members.
    /// @param parent
    explicit QDPIConfigurationScreen(QWidget *parent = 0);
    ~QDPIConfigurationScreen();

protected:
    /// @brief On close the configuration is saved.
    /// Make sure that you call the close() function before destroying this class.
    ///
    /// If the factor is different a message box asking the user to restart the
    /// application will be shown.
    /// @param event
    void closeEvent(QCloseEvent *event);

private slots:
    void checkboxChanged(bool enabled);
    /// @brief Updates the preview.
    /// Updates the prevew and recalculates everything for the given factor.
    /// @param value Scale factor.
    void factorChanged(int value);

private:
    qreal m_devToPixelRatio;
    qreal m_devDPI;

    // Actual sizes before applying any scaling on the preview.
    qreal m_smallReference;
    qreal m_mediumReference;
    qreal m_largeReference;
    qreal m_fontReference;

    // Pixel sizes will be multiplied by this.
    int m_factor;
    int m_initialFactor; // If m_factor != m_initialFactor a restart message has to be shown

};

} // End namespace udg

#endif
