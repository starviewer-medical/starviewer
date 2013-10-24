#ifndef UDGPATIENTBROWSERMENUBASICITEM_H
#define UDGPATIENTBROWSERMENUBASICITEM_H

#include <QLabel>

namespace udg {

/**
    Object that represents an item of the browser menu
  */
class PatientBrowserMenuBasicItem : public QObject {

    Q_OBJECT
    Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString identifier READ getIdentifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(QString type READ getType)

public:
    PatientBrowserMenuBasicItem(QObject *parent = 0);
    ~PatientBrowserMenuBasicItem(){}

    void setText(const QString &text);
    QString getText();

    void setIdentifier(const QString &identifier);
    QString getIdentifier() const;

    virtual QString getType();

signals:
    void textChanged();
    void identifierChanged();

protected:
    /// Identificador de l'ítem
    QString m_identifier;
    QString m_text;
};

}

#endif
