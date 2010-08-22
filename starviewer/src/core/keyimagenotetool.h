/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGKEYIMAGENOTETOOL_H
#define UDGKEYIMAGENOTETOOL_H

#include "tool.h"
#include <QColor>
#include <QHash>
namespace udg {

class Q2DViewer;
class KeyImageNoteToolWidget;
class DrawerPrimitive;
class KeyImageNoteManager;
class Image;

/** Tool que s'encarrega de pintar una icona a totes les imatges
que formen d'algun Key Image Note de l'estudi donat un volum */
class KeyImageNoteTool : public Tool {
Q_OBJECT
public:
    /// Cal especificar el QViewer que actua
    KeyImageNoteTool(QViewer *viewer, QObject *parent = 0);
    ~KeyImageNoteTool();

    /// Tracta un event de l'usuari
    void handleEvent(long unsigned eventID);

    /// Mostra el Key Image Note Tool Widget que ens informara dels KINS on es referenciada la imatge visualitzada
    void showKeyImageNoteDescriptor();

    /// Estableix el Key Image Note Manager amb el que s'ha de comunicar
    void setKeyImageNoteManager(KeyImageNoteManager *keyImageNoteManager);
    
public slots:

    /// Dibuixa el text de la meva seleccio a la imatge passada per parametre
    void drawMySelectionMark(Image *image);

    /// Actualitza les marques tan del nou KIN com les de KINS de l'estudi
    void updateMarks();

    /// Elimina la marca de la meva seleccio a la imatge amb UID sopInstanceUID
    void removeMySelectionMarkOfImage(const QString &sopInstanceUID);

private slots:
    /// Inicialitzacio de la Tool
    void initialize();

    /// Dibuixa els icones de Key Image Notes al volum
    void drawKeyImageNotesInVolume();

    /// Retorna cert si el click rebut es prou proper i tenim la icona de Key Image Note
    bool hasKeyImageNoteToolWidgetBeDisplayed();


private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Widget per a mostrar els Key Image Notes on hi ha referenciada la imatge
    KeyImageNoteToolWidget *m_keyImageNoteToolWidget;

    /// Llista dels punts que hem pintat als diferents volums
    QList<DrawerPrimitive*> m_pointsOfKeyImageNote;

    /// Key Image Note Manager amb el que interactua la tool
    KeyImageNoteManager *m_keyImageNoteManager;

    /// Llista dels punts que indiquen que una imatge es de la meva seleccio
    QHash<QString,DrawerPrimitive*> m_pointsOfMySelection;

};

}

#endif