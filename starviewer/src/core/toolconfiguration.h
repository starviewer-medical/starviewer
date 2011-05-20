#ifndef UDGTOOLCONFIGURATION_H
#define UDGTOOLCONFIGURATION_H

#include <QObject>

#include <QMap>
#include <QVariant>

namespace udg {

/**
    Classe encarregada de definir els atributs que configuren una tool. Els guarda de forma genèrica.
  */
class ToolConfiguration : public QObject {
Q_OBJECT
public:
    ToolConfiguration(QObject *parent = 0);
    ~ToolConfiguration();

    /// Afegeix un atribut a la tool
    void addAttribute(const QString &attributeName, const QVariant &value);

    /// Posa el valor a un atribut
    void setValue(const QString &attributeName, const QVariant &value);

    /// Obté el valor d'un atribut
    QVariant getValue(const QString &attributeName);

    /// Retorna cert si conté l'atribut, fals altrament
    bool containsValue(const QString &attributeName);

private:
    /// Map per guardar els atributs amb els seus corresponents valors
    QMap<QString, QVariant> attributeMap;
};

}

#endif
