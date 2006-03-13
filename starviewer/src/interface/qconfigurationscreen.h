/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGQCONFIGURATIONSCREEN_H
#define UDGQCONFIGURATIONSCREEN_H

#include "ui_qconfigurationscreenbase.h"
#include "pacslistdb.h"
using namespace Ui; // \TODO això s'hauria d'evitar!

namespace udg {

/** Interfície que permet configurar els paràmetres del pacs i de la caché
@author marc
*/
class QConfigurationScreen : public QDialog , private QConfigurationScreenBase{
Q_OBJECT

private :

    int m_PacsID;

    bool validatePacsParameters();
    bool validateChanges();
    void fillPacsListView();

    void databaseError(Status *,PacsParameters *);
    void databaseError(Status *);
    
    void loadCacheDefaults();
    void loadPacsDefaults();
    void loadCachePoolDefaults();
    
    void applyChangesPacs();
    void applyChangesCache();
   
     
public slots :

    /** Configuració de la llista de Pacs */
    void clear();
    void addPacs();
    void selectedPacs( Q3ListViewItem * item );
    void updatePacs();
    void deletePacs();
    void test();
    
    /*Configuració de la caché*/
    void examinateDataBaseRoot();
    void examinateCacheImagePath();

    void deleteStudies();
    void compactCache();
    
    /*Configuració dels paràmetres del Pacs */
    void applyChanges();
    void acceptChanges();
    void cancelChanges();

    void configurationChanged(int);
    void configurationChanged( const QString& );
       
public:
    QConfigurationScreen( QWidget *parent = 0 );

    ~QConfigurationScreen();

};

};

#endif
