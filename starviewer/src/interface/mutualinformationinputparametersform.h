/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGMUTUALINFORMATIONINPUTPARAMETERSFORM_H
#define UDGMUTUALINFORMATIONINPUTPARAMETERSFORM_H
 
#include "ui_mutualinformationinputparametersformbase.h" // defines Ui::MutualInformationInputParametersFormBase
#include "qinputparameters.h"

#include "identifier.h"

namespace udg {

//forward declaration
class MutualInformationParameters;

/**
@author Grup de Gràfics de Girona  ( GGG )
*/
class MutualInformationInputParametersForm : public QInputParameters , private Ui::MutualInformationInputParametersFormBase{
Q_OBJECT
public:
    MutualInformationInputParametersForm( QWidget *parent = 0 );

    ~MutualInformationInputParametersForm();
    
    void setParameters( MutualInformationParameters* parameters );

public slots:
    /**
        Aquests són els slots virtuals que heredem i que hem d'implementar
    */
    void readParameter(int index);
    
    void writeAllParameters();
    
private:
    MutualInformationParameters* m_parameters;

};

};  // end namespace udg

#endif
