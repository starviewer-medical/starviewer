/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCUSTOMWINDOWLEVELEDITWIDGET_H
#define UDGQCUSTOMWINDOWLEVELEDITWIDGET_H

#include "ui_qcustomwindowleveleditwidgetbase.h"

namespace udg {

/// Widget que permet afegir, modificar i eliminar els WW/WL definits per l'usuari.
/// Un cop acabat s'actualitza el CustomWindowLevelRepository i es reescriu el fitxer. S'hauria de discutir
/// si és aquesta classe l'encarregada de modificar el fitxer.
class QCustomWindowLevelEditWidget : public QDialog, private Ui::QCustomWindowLevelEditWidgetBase
{
Q_OBJECT
public:
    QCustomWindowLevelEditWidget(QWidget *parent = 0);
    ~QCustomWindowLevelEditWidget();

    /// Assigna els valors per defecte que es posarant quan s'afegeixi un custom
    void setDefaultWindowLevel(double width, double level);

private:
    /// Crea les connexions internes
    void createConnections();

    /// Colca els CustomWindowLevels del repository a la taula
    void loadCustomWindowLevelPresets();

    /// Valida la informació de la taula abans d'actualitzar el repository
    bool validate() const;

    /// Afegeix una nova fila a la taula amb els valors passats per paràmetre
    QTreeWidgetItem* addWindowLevelItem(const QString &description, double width, double level);

private slots:
    /// Afegeix una nova fila assignant els valors per defecte guardats als atributs m_defaultWindowWidth i m_defaultWindowLevel
    void addNewWindowLevel();

    /// Elimina les files que estan seleccionades
    void removeWindowLevel();

    /// Actualitza el CustomWindowLevelsRepository si la informació és correcte (no hi ha descripcions repetides ni buides)
    void updatePresetsIfAreValid();

private:
    /// WW/WL per defecte que es posaran al afegir un nova fila
    double m_defaultWindowWidth;
    double m_defaultWindowLevel;
};

}

#endif
