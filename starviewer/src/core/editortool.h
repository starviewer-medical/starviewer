/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGEDITORTOOL_H
#define UDGEDITORTOOL_H

#include "tool.h"

class vtkActor;

namespace udg {

class Q2DViewer;
class Volume;
class EditorToolData;

/**
Tool que serveix per editar el volum sobreposat en un visor 2D

    @author Laboratori de Gràfics i Imatge  ( GILab ) <vismed@ima.udg.es>
*/
class EditorTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NoEditor , Paint , Erase , EraseSlice , EraseRegion };

    EditorTool( QViewer *viewer, QObject *parent = 0 );
    ~EditorTool();

    void handleEvent( unsigned long eventID );

    /**
     * Retorna les dades de la tool
     */
    ToolData* getToolData() const;

//private slots:

private:

    /**
     * Pinta el quadrat vert si fa falta (estats Paint i Erase) a la posició del cursor
     * si el botó esquerre està premut també es fa l'acció de pintar
     */
    void setPaintCursor();

    /**
     * Canvia l'estat a Erase
     */
    void setErase();

    /**
     * Canvia l'estat a Paint
     */
    void setPaint();

    /**
     * Canvia l'estat a Erase Slice
     */
    void setEraseSlice();

    /**
     * Canvia l'estat a Erase Region
     */
    void setEraseRegion();

    /**
     * Crida a l'acció que convingui seegons l'estat
     */
    void setEditorPoint();

    /**
     * Esborra una porció quadrada de la màscara de mida m_editorSize 
     */
    void eraseMask();

    /**
     * Pinta una porció quadrada de la màscara de mida m_editorSize 
     */
    void paintMask();

    /**
     * Esborra una llesca de la màscara 
     */
    void eraseSliceMask();

    /**
     * Esborra una porció conectada de la màscara (en 2D) 
     */
    void eraseRegionMask();

    /**
     * Crida recursiva de la funció eraseRegionMask 
     * @param a, @param b, @param c índex del volum de la màscara que estem mirant en cada crida
     */
    void eraseRegionMaskRecursive(int a, int b, int c);

    /**
     * Decrementa un estat de la tool. Ordre: Paint, Erase, EraseRegion, EraseSlice 
     */
    void decreaseState();

    /**
     * Incrementa un estat de la tool. Ordre: Paint, Erase, EraseRegion, EraseSlice 
     */
    void increaseState();

    /**
     * Incrementa la mida de l'editor
     */
    void increaseEditorSize();

    /**
     * Decrementa la mida de l'editor
     */
    void decreaseEditorSize();

    /**
     * Inicialitza diverses dades necessàries per la tool: Valors de m_insideValue, m_outsideValue i m_volumeCont
     */
    void initialize();

private:

    /// Ens guardem aquest punter per ser més còmode
    Q2DViewer *m_2DViewer;

    /// El mode en que editem l'overlay.
    /// De moment podrà tenir 4 tipus: escriure, esborrar, esborrar regió i esborrar llesca, per defecte escriure
    int m_editorState;

    ///Mida de la tool
    int m_editorSize;
    vtkActor *m_squareActor;
    //\TODO:Aquesta variable no caldria, ja que hauria d'estar només al ToolData
    int m_volumeCont;
    EditorToolData* m_myData;
    int m_insideValue;
    int m_outsideValue;
    bool m_isLeftButtonPressed;

};

}

#endif
