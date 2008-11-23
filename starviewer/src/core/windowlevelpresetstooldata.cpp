/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "windowlevelpresetstooldata.h"

#include <QStringList>

namespace udg {

WindowLevelPresetsToolData::WindowLevelPresetsToolData(QObject *parent)
 : ToolData(parent)
{
    // Per defecte afegirem els window levels predeterminats estàndar.
    // TODO Més endavant aquests valors podrien estar continguts en algun determinat fitxer amb un format definit
    addPreset( tr("CT Bone"), 2000, 500, StandardPresets );
    addPreset( tr("CT Lung"), 1500, -650, StandardPresets );
    addPreset( tr("CT Soft Tissues, Non Contrast"), 400, 40, StandardPresets );
    addPreset( tr("CT Soft Tissues, Contrast Medium"), 400, 70, StandardPresets );
    addPreset( tr("CT Liver, Contrast Medium"), 300, 60, StandardPresets ); // 60-100
    addPreset( tr("CT Liver, Non Contrast"), 200, 40, StandardPresets );
    addPreset( tr("CT Neck, Contrast Medium"), 300, 50, StandardPresets );
    addPreset( tr("Angiography"), 500, 100, StandardPresets ); // 100-200
    addPreset( tr("Osteoporosis"), 1000, 300, StandardPresets ); //100-1500:window!
    addPreset( tr("Emphysema"), 800, -800, StandardPresets );
    addPreset( tr("Petrous Bone"), 4000, 700, StandardPresets );
    // TODO ara caldria afegir els presets que tinguem guardats en QSettins, o altres tipus d'arxius tipus XML o ".ini"
}

WindowLevelPresetsToolData::~WindowLevelPresetsToolData()
{
}

void WindowLevelPresetsToolData::addPreset( const QString &description, double window, double level, int group )
{
    if( !m_presets.contains(description) )
    {
        WindowLevelStruct data = { window, level, group };
        m_presets.insert( description, data );
        emit presetAdded( description );
    }
}

void WindowLevelPresetsToolData::removePreset( const QString &description )
{
    if( m_presets.contains(description) )
    {
        m_presets.remove( description );
        emit presetRemoved( description );
    }
}

void WindowLevelPresetsToolData::removePresetsFromGroup( int group )
{
    QMutableMapIterator<QString, WindowLevelStruct> iterator( m_presets );
    while( iterator.hasNext() )
    {
        iterator.next();
        if( iterator.value().m_group == group )
        {
            emit presetRemoved( iterator.key() );
            iterator.remove();
        }
    }
}

bool WindowLevelPresetsToolData::getWindowLevelFromDescription( const QString &description, double &window, double &level )
{
    bool ok = true;
    if( m_presets.contains(description) )
    {
        WindowLevelStruct data = m_presets.value( description );
        window = data.m_window;
        level = data.m_level;
    }
    else
    {
        ok = false;
        window = 0.0;
        level = 0.0;
    }
    return ok;
}

bool WindowLevelPresetsToolData::getGroup( const QString &description, int &group )
{
    bool ok = true;
    if( m_presets.contains(description) )
    {
        WindowLevelStruct data = m_presets.value( description );
        group = data.m_group;
    }
    else
    {
        ok = false;
        group = Other;
    }
    return ok;
}

QStringList WindowLevelPresetsToolData::getDescriptionsFromGroup( int group )
{
    QStringList descriptionList;
    QMapIterator< QString, WindowLevelStruct > iterator( m_presets );
    while( iterator.hasNext() )
    {
        iterator.next();
        if( iterator.value().m_group == group )
        {
            descriptionList << iterator.key();
        }
    }
    return descriptionList;
}

QString WindowLevelPresetsToolData::getCurrentPreset() const
{
    return m_currentPreset;
}

void WindowLevelPresetsToolData::setCustomWindowLevel( double window, double level )
{
    WindowLevelStruct data = { window, level, StandardPresets };
    m_presets.insert( tr("Custom"), data );
    emit currentWindowLevel( window, level );
    emit presetChanged( tr("Custom") );
    m_currentPreset = tr("Custom");
}

void WindowLevelPresetsToolData::activatePreset(const QString &preset)
{
    double window, level;
    if( this->getWindowLevelFromDescription( preset, window, level ) )
    {
        emit currentWindowLevel( window, level );
        emit presetChanged(preset);
        m_currentPreset = preset;
    }
}

}
