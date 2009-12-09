/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGWINDOWLEVELPRESETSTOOL_H
#define UDGWINDOWLEVELPRESETSTOOL_H

#include "tool.h"

#include <QStringList>
#include <QMap>

namespace udg {

class Q2DViewer;
class WindowLevelPresetsToolData;
/**
Tool per canviar window levels predefinits

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class WindowLevelPresetsTool : public Tool
{
Q_OBJECT
public:
    WindowLevelPresetsTool( QViewer *viewer, QObject *parent = 0 );

    ~WindowLevelPresetsTool();

    void handleEvent( unsigned long eventID );

    /**
     * Ens retorna el nombre de presets que té carregats
     * @return
     */
    int getNumberOfPresets() const;

    void setToolData( ToolData *toolData );

private:
    /**
     * Aplica el window level predeterminat per la tecla donada
     * @param key caràcter rebut
     */
    void applyPreset(char key);

private slots:
    /// Actualitza la llista de window/level definits per defecte
    void updateWindowLevelData();

private:
//     Q2DViewer *m_2DViewer;

    WindowLevelPresetsToolData *m_myToolData;

    QStringList m_standardPresets;
    
    /// Llista de presets per defecte del volum actual
    QStringList m_defaultPresets;
    
    /// Índex de l'últim preset per defecte escollit
    int m_defaultPresetsIndex;

    /// Aquest mapa ens donarà la relació entre el caràcter premut i l'índex corresponent dels presets
    QMap <char, int> m_characterIndexMap;
};

}

#endif
