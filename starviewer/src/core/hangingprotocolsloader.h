#ifndef UDGHANGINGPROTOCOLSLOADER_H
#define UDGHANGINGPROTOCOLSLOADER_H

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

namespace udg {

/**

*/
class HangingProtocolsLoader: public QObject
{
Q_OBJECT
public:
    HangingProtocolsLoader(QObject *parent = 0);

    ~HangingProtocolsLoader();

    /// Càrrega de hanging protocols per defecte
    void loadDefaults();

    /// Càrrega des d'un directori de hanging protocols o un fitxer XML
    void loadXMLFiles(const QString &path);

private:
    /// Mètodes privats per fer la lectura dels hanging protocols
    QStringList loadXMLFiles(const QFileInfo &fileInfo);
    QString loadXMLFile(const QFileInfo &fileInfo);
};

}

#endif
