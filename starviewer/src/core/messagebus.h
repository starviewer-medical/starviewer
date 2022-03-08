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

#ifndef UDG_MESSAGEBUS_H
#define UDG_MESSAGEBUS_H

#include <QObject>
#include "singleton.h"

#include <QVariant>

namespace udg {

/**
 * @brief The MessageBus class implements a global message bus to communicate simple messages with a key and a value between unrelated classes.
 */
class MessageBus : public QObject, public Singleton<MessageBus>
{
    Q_OBJECT

public:
    /// Sends the given message through the bus.
    void send(const QString &key, const QVariant &value = QVariant());

signals:
    /// Emitted when a message is sent.
    void message(const QString &key, const QVariant &value);

private:
    friend Singleton<MessageBus>;
    MessageBus() = default;
};

} // namespace udg

#endif // UDG_MESSAGEBUS_H
