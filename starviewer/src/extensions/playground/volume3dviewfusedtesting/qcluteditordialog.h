/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html?langu=uk                           *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQCLUTEDITORDIALOG_H
#define UDGQCLUTEDITORDIALOG_H


#include <QDialog>
#include "ui_qcluteditordialogbase.h"

#include <QDir>
#include <QMap>


class QAbstractButton;


namespace udg {


/**
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class QClutEditorDialog : public QDialog, private ::Ui::QClutEditorDialogBase {

    Q_OBJECT

public:

    QClutEditorDialog( QWidget * parent = 0 );
    virtual ~QClutEditorDialog();

    void setCluts( const QDir & clutsDir, const QMap<QString, QString> & clutNameToFileName );
    void setMaximum( unsigned short maximum );

signals:

    void clutApplied( const TransferFunction & clut );

private slots:

    /// Carrega una funció de transferència predefinida.
    void loadPresetClut( const QString & clutName );
    void loadClut();
    void saveClut();
    void manageClick( QAbstractButton * button );
    void switchEditor();

private:

    QDir m_clutsDir;
    QMap<QString, QString> m_clutNameToFileName;

};


}


#endif
