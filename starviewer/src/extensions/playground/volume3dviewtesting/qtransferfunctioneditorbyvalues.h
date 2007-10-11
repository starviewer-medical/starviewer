/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQTRANSFERFUNCTIONEDITORBYVALUES_H
#define UDGQTRANSFERFUNCTIONEDITORBYVALUES_H


#include "qtransferfunctioneditor.h"
#include "ui_qtransferfunctioneditorbyvaluesbase.h"

#include "transferfunction.h"


namespace udg {


class QTransferFunctionIntervalEditor;


/**
 * Editor de funcions de transferència basat en valors.
 *
 * L'usuari pot definir punts o intervals amb el valor RGBA que els correspon.
 * Aquests punts o intervals són sempre disjunts i n'hi pot haver un màxim de
 * 256. Els valors de propietat són enters en l'interval [0,255] (el primer
 * sempre comença a 0 i l'últim sempre acaba a 255). El color es pot triar en un
 * diàleg de selecció de color i l'opacitat en el mateix diàleg o directament
 * modificant el valor d'un SpinBox.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class QTransferFunctionEditorByValues
    : public QTransferFunctionEditor, public Ui::QTransferFunctionEditorByValuesBase
{

    Q_OBJECT

public:

    /// Crea l'editor amb un sol interval de 0 a 255 amb RGBA = (0,0,0,0).
    QTransferFunctionEditorByValues( QWidget * parent = 0 );
    virtual ~QTransferFunctionEditorByValues();

    /// Assigna una funció de transferència a l'editor.
    virtual void setTransferFunction( const TransferFunction & transferFunction );
    /// Retorna la funció de transferència de l'editor.
    virtual const TransferFunction & getTransferFunction() const;

public slots:

    /// Afegeix un interval a l'editor (al final). Si ja n'hi ha 256 no fa res.
    void addInterval();
    /// Esborra un interval de l'editor (l'últim). Si n'hi ha només 1 no fa res.
    void removeInterval();

private:

    /// Afegeix un interval al final i el retorna. Si ja n'hi ha 256 no fa res.
    QTransferFunctionIntervalEditor * addIntervalAndReturnIt();

private slots:

    /// Posa m_changed a cert.
    void markAsChanged();

private:

    /// Widget on es situaran els editors d'interval, dins d'una àrea d'scroll.
    QWidget * m_intervalEditorsWidget;
    /// El layout del widget anterior.
    QVBoxLayout * m_intervalEditorsLayout;

    /// Nombre d'intervals de l'editor.
    unsigned short m_numberOfIntervals;

    /// Memoria cau de la funció de transferència representada actualment per l'editor (si m_changed és fals).
    mutable TransferFunction m_transferFunction;
    /// Indica si hi ha hagut canvis des de l'últim cop que s'ha actualitzat la memòria cau de la funció de transferència.
    mutable bool m_changed;

};


}


#endif
