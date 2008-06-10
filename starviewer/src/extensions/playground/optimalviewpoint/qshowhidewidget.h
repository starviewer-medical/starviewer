/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQSHOWHIDEWIDGET_H
#define UDGQSHOWHIDEWIDGET_H

#include <ui_qshowhidewidgetbase.h>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QShowHideWidget : public QWidget, private ::Ui::QShowHideWidgetBase
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked)

public:
    QShowHideWidget(QWidget *parent = 0);

    ~QShowHideWidget();

public slots:

    QString title() const;
    void setTitle( const QString & title );
    bool isChecked() const;
    void setChecked( bool checked );

protected:

    virtual void childEvent( QChildEvent * event );

private:

    bool m_inited;
    QObject * m_frameLayout;

};

}

#endif
