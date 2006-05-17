/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGOPERATION_H
#define UDGOPERATION_H

#include "studymask.h"
#include "pacsparameters.h"
#include <QString>

namespace udg {

/** Defineix les possibles operacions que es poden dur a terme amb el PACS, descarregar imatges i visualitzar images (la cerca es tracta en d'altres classes )
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Operation{

public:
    Operation();
    
    /** Especifica la màscara de l'estudi per aquella operacio,
     * @param màscara de l'estudi
     */
    void setStudyMask( StudyMask mask );
    
    /** Estableix la prioritat de l'aplicació si no s'especifica, el setOperation automaticament l'assigna en funció del tipus d'operacio
     * @param prioritat
     * High = 0
     * Normal = 50
     * Low = 100
     */
    void setPriority( int priority );
    
    /** Especifica quina operacio es portara a terma en aquesta operacio, si no s'ha assignat prioritat, aquest mètode l'assigna en funció de l'operació
     * @param operacio a realitzar
     * Retrieve = 1
     * Print = 2
     * Move = 3
     */   
    void setOperation( int operation );
    
    /** Estableix a quin PACS es dura a terme la operacio
     * @param Pacs al qual es dura a terme l'operacio
     */
    void setPacsParameters( PacsParameters parameters );

   /** Retorna la màscara de l'estudi
    * @return màscara de l'estudi
    */        
    StudyMask getStudyMask();
    
    /** Retorna la prioritat de l'operació
     * @return prioritat de l'operació
     */    
    int getPriority();
    
    /** Retorna la operacio a realitzar
     * @return operacio a realitzar
     */
    int getOperation();
    
    /** Retorna el pacsParameters de l'objecte operation
     * @return PacsParameters de l'objecte operation
     */
    PacsParameters getPacsParameters();
    
    /// Operador per ordernar per prioritats
    bool operator < ( Operation ) const;

	///Destructor de la classe
    ~Operation();

private :

    StudyMask m_mask;
    int m_priority;
    int m_operation;
    PacsParameters m_pacsParameters;
    QString m_text;    
};

}

#endif
