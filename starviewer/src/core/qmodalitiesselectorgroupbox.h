#ifndef UDGQMODALITIESSELECTORGROUPBOX_H
#define UDGQMODALITIESSELECTORGROUPBOX_H

#include "ui_qmodalitiesselectorgroupboxbase.h"

class QButtonGroup;

namespace udg {

/**
    Combo box per seleccionar modalitats
 */
class QModalitiesSelectorGroupBox : public QGroupBox, private Ui::QModalitiesSelectorGroupBoxBase {
Q_OBJECT
public:
    QModalitiesSelectorGroupBox(QWidget *parent = 0);
    ~QModalitiesSelectorGroupBox();

    /// Permet escollir quines opcions adicionals estan disponibles. Per defecte totes ho estan.
    void enableAllModalitiesCheckBox(bool enable);
    void enableOtherModalitiesCheckBox(bool enable);
    
    /// Desmarca tots els check box
    void clear();
    
    /// Indica si podem seleccionar una sola modalitat o múltiples
    void setExclusive(bool exlusive);

    /// Ens retorna una llista amb les modalitats seleccionades
    QStringList getCheckedModalities();

    /// Selecciona les modalitats de la llista. Si la modalitat no és vàlida no marca res.
    void checkModalities(const QStringList &modalities);

    /// Fa que el check box especial "All" es marqui o desmarqui
    void setAllModalitiesCheckBoxChecked(bool checked);

    /// Ens indica si el check box de "All" està marcat o no, independentment de si està habilitat o no
    bool isAllModalitiesCheckBoxChecked() const;

private:
    void initialize();

private:
    /// Grup per poder fer que els check box siguin exclusius o no
    QButtonGroup *m_buttonGroup;
};

} // End namespace udg

#endif
