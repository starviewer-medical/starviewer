/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qwindowlevelcombobox.h"
#include "qcustomwindowleveldialog.h"
#include "logging.h"
#include "windowlevelpresetstooldata.h"

namespace udg {

QWindowLevelComboBox::QWindowLevelComboBox(QWidget *parent)
 : QComboBox(parent), m_presetsData(0)
{
    m_customWindowLevelDialog = new QCustomWindowLevelDialog;
    connect( this, SIGNAL( activated(int) ), SLOT( setActiveWindowLevel(int) ) );
}

QWindowLevelComboBox::~QWindowLevelComboBox()
{
}

void QWindowLevelComboBox::setPresetsData( WindowLevelPresetsToolData *windowLevelData )
{
    if( m_presetsData )
    {
        // desconectem tot el que teníem connectat aquí
        disconnect( m_presetsData, 0, this, 0 );
        disconnect( m_presetsData, 0, m_customWindowLevelDialog, 0 );
        disconnect( m_customWindowLevelDialog, 0, m_presetsData, 0 );
    }
    m_presetsData = windowLevelData;
    populateFromPresetsData();
    connect( m_presetsData, SIGNAL(presetAdded(QString)), SLOT( addPreset(QString) ) );
    connect( m_presetsData, SIGNAL(presetRemoved(QString)), SLOT( removePreset(QString) ) );
    connect( m_presetsData, SIGNAL(presetChanged(QString)), SLOT( selectPreset(QString) ) );
    connect( m_presetsData, SIGNAL( currentWindowLevel( double , double ) ), m_customWindowLevelDialog, SLOT( setDefaultWindowLevel( double , double ) ) );
    // TODO això es podria substituir fent que el CustomWindowLevelDialog també contingués les dades
    // de window level i directament li fes un setCustomWindowLevel() a WindowLevelPresetsToolData
    connect( m_customWindowLevelDialog, SIGNAL( windowLevel(double,double) ), m_presetsData, SLOT( setCustomWindowLevel(double,double) ) );
}

void QWindowLevelComboBox::addPreset(QString preset)
{
    int group;
    if( m_presetsData->getGroup( preset, group ) )
    {
        int index;
        switch( group )
        {
        case WindowLevelPresetsToolData::FileDefined:
            index = m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::FileDefined ).count() - 1;
        break;

        case WindowLevelPresetsToolData::StandardPresets:
            index = m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::FileDefined ).count() +
                m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::StandardPresets ).count() - 1;
        break;

        case WindowLevelPresetsToolData::UserDefined:
            index = m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::FileDefined ).count() +
                m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::StandardPresets ).count() +
                m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::UserDefined ).count() - 1;
        break;

        case WindowLevelPresetsToolData::Other:
            index = m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::FileDefined ).count() +
                m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::StandardPresets ).count() +
                m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::UserDefined ).count() +
                m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::Other ).count() - 1;
        break;
        }
        this->insertItem( index, preset );
    }
    else
    {
        DEBUG_LOG("El preset " + preset + " no està present en les dades de window level proporcionades" );
    }
}

void QWindowLevelComboBox::removePreset(QString preset)
{
    int index = this->findText( preset );
    if( index > -1 )
        this->removeItem( index );
}

void QWindowLevelComboBox::selectPreset( const QString &preset )
{
    int index = this->findText( preset );
    if( index > -1 )
        this->setCurrentIndex( index );
    else
        this->setCurrentIndex( this->findText( tr("Custom") ) );
}

void QWindowLevelComboBox::populateFromPresetsData()
{
    if( !m_presetsData )
        return;

    this->clear();
    this->addItems( m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::FileDefined ) );
    this->addItems( m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::StandardPresets ) );
    this->addItems( m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::UserDefined ) );
    this->addItems( m_presetsData->getDescriptionsFromGroup( WindowLevelPresetsToolData::Other ) );
    this->addItem( tr("Custom") );
}

void QWindowLevelComboBox::setActiveWindowLevel( int value )
{
    int customIndex = this->findText( tr("Custom") );
    if( customIndex != value  )
    {
        m_presetsData->activatePreset( this->itemText( value ) );
    }
    else
    {
        m_customWindowLevelDialog->exec();
    }
}

};

