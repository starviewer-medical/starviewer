/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQPOPURISREQUESTSCREEN_H
#define UDGQPOPURISREQUESTSCREEN_H

#include <QDialog>
#include "ui_qpopuprisrequestsscreenbase.h"

class QTimer;

/** Classe que mostra un PopUp indicant que s'ha rebut una petició del RIS per descarregar un estudi
 *
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

namespace udg {


class QPopUpRisRequestsScreen : public QDialog, private Ui::QPopUpRisRequestsScreenBase{
Q_OBJECT
public:

    ///Constructor
    QPopUpRisRequestsScreen( QWidget *parent = 0 );

    ///Destructor
    ~QPopUpRisRequestsScreen();

    ///Especifiquem l'accession number del estudi a descarregar
    void setAccessionNumber(QString text);

protected:

    ///Quan es fa un show al popup s'activa un timer, que passat un temps amaga automàticament el PopUp
    void showEvent(QShowEvent *);

private slots :

    ///Slot que s'activa quan acaba el timer per amagar el popup
    void timeoutTimer();

private:

    QTimer *m_qTimer;

};

};

#endif

