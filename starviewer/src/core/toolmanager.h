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
class ToolData;

/**
    Classe que gestiona les Tools que usualment tindrem en una extensió, associades a un o més visualitzadors. 
    Aquesta s'encarrega de determinar les tools i les configuracions que suporten els visualitzadors que hi configurem
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
    // TODO es podria renombrar a setSupportedTools()
    void setViewerTools( QViewer *viewer, const QStringList &toolsList );

    /**
     * Donat un viewer, li assignem el parell tool-configuració.
     * Aquesta crida la farem servir quan volguem donar una configuració per defecte
     * diferent de la que proporciona la tool
     * @param viewer El viewer al que volem assignar la tool
     * @param toolName El nom de la Tool que volem configurar
     * @param configuration Configuració específica per la tool assignada
     */
    void setViewerTool( QViewer *viewer, const QString &toolName, ToolConfiguration *configuration = 0 );

    /**
     * Donat un viewer, li assigna totes les tools que s'hagin registrat amb el ToolManager anteriorment.
     * Té la mateixa funció que setViewerTools() però no cal especificar-li
     * la llista de tools que volem per aquest visor, ja que automàticament hi configura totes 
     * les tools que s'hagin registrat anteriorment, que serà el cas més comú.
     * @param viewer El viewer al que volem assignar les tools registrades
     */
    void setupRegisteredTools( QViewer *viewer );
    
    /**
     * Elimina la tool demanada d'un viewer determinat. És a dir, si després d'aquesta crida li demanem a
     * aquest viewer activar la tool especificada, no es podrà activar aquella tool per aquell viewer.
     * Si aquest viewer no tenia registrada aquesta tool no es farà res.
     * @param viewer Viewer al qual volem deshabilitar la tool
     * @param toolName Nom de la tool a deshabilitar
     */
    void removeViewerTool( QViewer *viewer, const QString &toolName );

    /**
     * Registra la tool al manager i ens retorna la QAction associada
     * Un cop registrada, successives crides a aquest mètode per la
     * mateixa Tool, ens retornarà la mateixa QAction
     * @param toolName Nom de la tool que volem registrar
     * @return QAction de la tool registrada. En cas que la tool demanada no existeixi, 
     *         tindrem una QAction buida.
     */
    QAction *registerTool( const QString &toolName );
    
    /**
     * Ens retorna la QAction associada a la Tool demanada.
     * Només té en compte les Tools que hàgim registrat amb registerTool() prèviament
     * @param toolName Nom de la tool de la qual volem la seva QAction
     * @return QAction associada a la tool si aquesta està registrada, un punter NUL altrament.
     */
    QAction *getRegisteredToolAction( const QString &toolName );

    /// Retorna una llista d'strings amb les Tools registrades amb registerTool()
    QStringList getRegisteredToolsList() const;

    /// Mètode per pre-seleccionar tools, pot servir per definir les tools a activar per defecte. Cal haver-les registrat abans
    /// TODO aquest mètode està perquè no haguem d'accedir directament a l'acció de la tool i dir-li "trigger" perquè quedi seleccionada
    /// Queda pendent una millora "conceptual" sobre aquest aspecte
    void triggerTools( const QStringList &toolsList );
    void triggerTool( const QString &toolName );
    // alres noms addStarterTool(s), setStarterTools(), markStartTools(), setInitialTools()

    /**
     * Afegim una llista de tools dins d'un grup exclusiu.
     * Les tools dins d'un grup, no poden estar actives alhora.
     * Quan s'activa una dins d'un grup, la resta es desactiven.
     * @param groupName Nom del grup on volem posar les tools
     * @param tools Llista de tools que volem dins del grup
     */
    void addExclusiveToolsGroup( const QString &groupName, const QStringList &tools );

    /**
     * Registra una Action Tool i ens retorna la QAction associada. 
     * Un cop registrada, successives crides a aquest mètode per la
     * mateixa Action Tool, ens retornarà la mateixa QAction
     * @param actionToolName Nom de la "Action Tool" que volem registrar
     * @return La QAction associada a la "Action Tool".
     */
    QAction *registerActionTool( const QString &actionToolName );

    /**
     * Activa/desactiva en un determinat viewer les "Action Tool" especificades
     * @param viewer Viewer sobre el qual volem activar/desactivar les "Action Tools"
     * @param actionToolsList Llista de noms d'"Action Tools"
     */
    void enableActionTools( QViewer *viewer, const QStringList &actionToolsList );
    void disableActionTools( QViewer *viewer, const QStringList &actionToolsList );

    /**
     * Activa/desactiva en un determinat viewer les "Action Tool" registrades
     * @param viewer Viewer sobre el qual volem activar/desactivar les "Action Tools" registrades
     */
    void enableRegisteredActionTools( QViewer *viewer );
    void disableRegisteredActionTools( QViewer *viewer );
    
public slots:
    /**
     * Desactiva totes les tools temporalment.
     * Útil per si volem interactuar amb els events de teclat i mouse i no volem que les tools
     * interfereixin.
     * @see undoDisableAllToolsTemporarily()
     */
    void disableAllToolsTemporarily();

    /**
      * Desfà la desactivació temporal de les tools.
      * Per tal de que les tools tornin a funcionar normalment un cop cridat disableAllToolsTemporarily(), cal cridar aquest
      * mètode.
      */
    void undoDisableAllToolsTemporarily();

    /**
     * Activa/Desactiva la tool especificada en tots els viewers registrats
     * Aquests mètodes són de més baix nivell
     * @param toolName Nom de la tool a activar/desactivar
     */
    void activateTool( const QString &toolName );
    void deactivateTool( const QString &toolName );
    // TODO altres noms switchOn/OffTool() turnOn/OffTool(), enable/disableTool().

private slots:
    /**
     * Quan es dispara l'acció d'una tool, aquest slot la rep i a partir del seu nom
     * esbrina si l'acció ha estat d'activar o desactivar la tool (isChecked())
     * En funció d'això activarà o desactivarà la tool
     * @param toolName Nom de la tool disparada
     */
    void triggeredToolAction( const QString &toolName );
    // TODO altres noms: checkToolActivation()

    /**
     * Comprova per cada tool registrada, si l'acció associada està checked o no per activar/desactivar
     * la tool en els viewers indicats. Es pot fer servir per quan per exemple afegim nous viewers en un mateix contexte
     * i volem que s'activin les mateixes tools que estan actives en els altres viewers.
     */
    void refreshConnections();
    // TODO altres noms checkToolsToActivate()

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

    /// Mapa que guarda les ToolData per Tools amb dades compartides
    QMap<QString, ToolData *> m_sharedToolDataRepository;

    /// Mapa que guarda la relació de tools les quals hem registrat la seva acció. Per cada Tool, guardem la seva respectiva QAction
    QMap<QString, QAction *> m_toolsActionsRegistry;

    /// Mapa que guarda la relació d'"Action Tools" amb les respectives parelles QAction/SLOT
    QMap<QString, QPair<QAction *, QString> > m_actionToolRegistry;
};

}

#endif
