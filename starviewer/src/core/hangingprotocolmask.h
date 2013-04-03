#ifndef UDGHANGINGPROTOCOLMASK_H
#define UDGHANGINGPROTOCOLMASK_H

#include <QObject>
#include <QStringList>

namespace udg {

class HangingProtocolMask : public QObject {
Q_OBJECT
public:
    HangingProtocolMask(QObject *parent = 0);

    ~HangingProtocolMask();

    /// Llista de protocols a tractar del hanging protocol
    void setProtocolsList(const QStringList &protocols);

    /// Obtenir la llista de protocols
    QStringList getProtocolList() const;

private:
    QStringList m_protocolsList;
};

}

#endif
