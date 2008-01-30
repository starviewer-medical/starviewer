/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQTRANSFERFUNCTIONEDITOR_H
#define UDGQTRANSFERFUNCTIONEDITOR_H


#include <QWidget>


namespace udg {


class TransferFunction;


/**
 * Classe base per a qualsevol editor de funcions de transferència.
 *
 * Declara mètodes per assignar i obtenir la funció de transferència.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class QTransferFunctionEditor : public QWidget
{

    Q_OBJECT

public:

    QTransferFunctionEditor( QWidget * parent = 0 );
    virtual ~QTransferFunctionEditor();

    /// Retorna el valor de propietat màxim mapat per aquest editor.
    virtual unsigned short maximum() const;
    /// Assigna el valor de propietat màxim mapat per aquest editor.
    virtual void setMaximum( unsigned short maximum );

    /// Assigna una funció de transferència a l'editor.
    virtual void setTransferFunction( const TransferFunction & transferFunction ) = 0;
    /// Retorna la funció de transferència de l'editor.
    virtual const TransferFunction & getTransferFunction() const = 0;

protected:

    /// Valor de propietat màxim mapat per aquest editor.
    unsigned short m_maximum;

};


}


#endif
