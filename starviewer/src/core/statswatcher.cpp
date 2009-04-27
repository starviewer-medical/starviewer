#include "statswatcher.h"
#include "logging.h"

#include <QAbstractButton>

namespace udg {

StatsWatcher::StatsWatcher( QObject *parent )
 : QObject(parent), m_context("*")
{
}

StatsWatcher::StatsWatcher( const QString &context, QObject *parent )
 : QObject(parent), m_context(context)
{
}

StatsWatcher::~StatsWatcher()
{
}

void StatsWatcher::addClicksCounter( QAbstractButton *button )
{
    connect( button, SIGNAL( clicked(bool) ), SLOT( registerClick(bool) ) );
}

void StatsWatcher::registerClick(bool checked)
{
    QString statMessage;
    QAbstractButton *button = qobject_cast<QAbstractButton *>( sender() );
    if( button )
    {
        if( button->isCheckable() )
        {
            if( checked )
                statMessage = "S'ha activat amb un click el botó";
            else
                statMessage = "S'ha desactivat amb un click el botó";
        }
        else
            statMessage = "Sha fet un click sobre el botó";
    }
    else // és un altre tipus d'objecte 
    {
        statMessage = "S'ha fet un click sobre l'objecte";
    }

    statMessage = QString( "[%1] %2 %3" ).arg(m_context).arg(statMessage).arg( sender()->objectName() );
    STAT_LOG( statMessage );
}

} //end namespace udg
