#ifndef UDGPATIENTBROWERMENU_H
#define UDGPATIENTBROWERMENU_H

#include <QWidget>

class QPoint;

namespace udg {

class Patient;
class Volume;
class PatientBrowserMenuExtendedItem;
class PatientBrowserMenuList;

/**
    Widget en forma de menú que desplega la informació del pacient que li donem
    d'entrada de forma que poguem navegar pels seus estudis i corresponents volums
  */
class PatientBrowserMenu : public QWidget {
Q_OBJECT
public:
    PatientBrowserMenu(QWidget *parent = 0);
    ~PatientBrowserMenu();

    /// Assignem un pacient per representar al menu
    void setPatient(Patient *patient);

public slots:
    /// Fem que es mostri el menú en la posició indicada
    void popup(const QPoint &point, const QString &identifier = QString());

signals:
    /// Senyal que notifica el volum corresponent a l'ítem escollit
    void selectedVolume(Volume *);

private slots:
    /// Actualitza les vistes relacionades amb l'ítem actiu (aquell pel qual passa el ratolí per sobre)
    void updateActiveItemView(const QString &identifier);

    /// Donat l'identificador de l'ítem fa les accions pertinents.
    /// En aquest cas s'encarrega d'obtenir el volum seleccionat per l'usuari i notificar-ho
    void processSelectedItem(const QString &identifier);

private:
    /// Crea els widgets dels que es composa el menú
    void createWidgets();

    /// Ens retorna cert si cal alinear el menú a la dreta. Fals en cas que s'hagi d'alinear a la l'esquerra.
    bool shouldAlignMenuToTheRight(const QRect &currentScreenGeometry);
    
    /// Calcula la mida que queda fora de la pantalla del widget amb la llista d'estudis/sèries segons el punt de popup.
    /// Si queda dins de la pantalla, la mida serà (0,0). El tercer paràmetre indica com volem que quedi alineat el menú
    /// respecte la pantalla. Si true, fa els càlculs tenint en compte que s'alinea a la dreta, altrament com si s'alineés a l'esquerra
    void computeListOutsideSize(const QPoint &popupPoint, QSize &out, bool rightAligned);
    
    /// Col·loca el widget d'informació adicional all lloc més adient depenent de la posició del menú principal
    void placeAdditionalInfoWidget();

private:
    /// Atribut que guarda el punter al menú basic que representa les dades del pacient
    PatientBrowserMenuList *m_patientBrowserList;

    /// Atribut que guarda el punter al menú amb informació addicional de l'ítem seleccionat
    PatientBrowserMenuExtendedItem *m_patientAdditionalInfo;

    /// Identificadors de les pantalles respecte on es troba desplegat el menú
    /// L'ID de la pantalla actual, i les pantalles annexes, es calcularà cada cop que es faci el popup del menú
    int m_currentScreenID;
    int m_leftScreenID;
    int m_rightScreenID;
};

}

#endif
