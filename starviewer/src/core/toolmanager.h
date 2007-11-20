/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLMANAGER_H
#define UDGTOOLMANAGER_H

#include <QObject>
#include <QMultiMap>
#include <QPair>

class QAction;
class QSignalMapper;
class QActionGroup;

namespace udg {

class QViewer;
class ToolConfiguration;
class ToolRegistry;

/**
Manager de Tools que tindrem per cada extensio, Aquest s'encarregara de determinar les tools i les configuracions que suporten els visualitzadors d'una determinada extensio

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolManager : public QObject
{
Q_OBJECT
public:
    ToolManager(QObject *parent = 0);

    ~ToolManager();

    /**
     * Donat un viewer, li assignem la llista de tools que suporta
     * @param viewer El viewer al que volem assignar les tools suportades
     * @param toolsList Llista de noms de tools que li volem assignar
     */
    void setViewerTools( QViewer *viewer, const QStringList &toolsList );

    /**
     * Donat un viewer, li assignem el parell tool-configuració.
     * Aquesta crida la farem servir quan volguem donar una configuració per defecte
     * diferent de la que proporciona la tool
     * @param viewer
     * @param toolName
     * @param configuration
     */
    void setViewerTool( QViewer *viewer, const QString &toolName, ToolConfiguration *configuration = 0 );

    /**
     * Elimina la tool demanada d'un viewer determinat. És a dir, si després d'aquesta crida li demanem a
     * aquest viewer activar la tool especificada, no es podrà activar aquella tool per aquell viewer.
     * Si aquest viewer no tenia registrada aquesta tool no es farà res.
     * @param viewer Viewer al qual volem deshabilitar la tool
     * @param toolName Nom de la tool a deshabilitar
     */
    void removeViewerTool( QViewer *viewer, const QString &toolName );

    /**
     * Obtenim l'acció que defineix una tool a través del ToolRegistry
     * @param toolName Nom de la tool de la qual volem l'acció
     * @return QAction de la tool demanada
     */
    QAction *getToolAction( const QString &toolName );

    /**
     * Afegim una llista de tools dins d'un grup exclusiu.
     * Les tools dins d'un grup, no poden estar actives alhora.
     * Quan s'activa una dins d'un grup, la resta es desactiven.
     * @param groupName Nom del grup on volem posar les tools
     * @param tools Llista de tools que volem dins del grup
     */
    void addExclusiveToolsGroup( const QString &groupName, const QStringList &tools );

public slots:
    /**
     * Activa/Desactiva la tool especificada en tots els viewers registrats
     * @param toolName Nom de la tool a activar/desactivar
     */
    void activateTool( const QString &toolName );
    void deactivateTool( const QString &toolName );

    /**
     * Comprova per cada tool registrada, si l'acció associada està checked o no per activar/desactivar
     * la tool en els viewers indicats. Es pot fer servir per quan per exemple afegim nous viewers en un mateix contexte
     * i volem que s'activin les mateixes tools que estan actives en els altres viewers.
     */
    void refreshConnections();

private slots:
    /**
     * Quan es dispara l'acció d'una tool, aquest slot la rep i a partir del seu nom
     * esbrina si l'acció ha estat d'activar o desactivar la tool (isChecked())
     * En funció d'això activarà o desactivarà la tool
     * @param toolName Nom de la tool disparada
     */
    void triggeredToolAction( const QString &toolName );

private:
    /// Registre que ens proporcionarà tools i accions associades
    ToolRegistry *m_toolRegistry;

    /// Definició de tipus del parell Viewer-ToolConfig
    typedef QPair<QViewer *,ToolConfiguration *> ViewerToolConfigurationPairType;

    /// Mapa en el que per cada nom de tool (clau), associem el viewer i la configuració que li correspon
    /// Una mateixa tool pot tenir més d'un parell <QViewer *,ToolConfiguration *>, és a dir, la mateixa tool pot
    /// estar en diferents viewers amb diverses configuracions
    QMultiMap< QString, ViewerToolConfigurationPairType > m_toolViewerMap;

    /// Signal mapper encarregat de mapejar les senyals de les accions de les tools
    QSignalMapper *m_toolsActionSignalMapper;

    /// Mapa que associa el nom del grup amb les accions agrupades de les tools exclusives
    QMap<QString, QActionGroup *> m_toolsGroups;
};

}

#endif
