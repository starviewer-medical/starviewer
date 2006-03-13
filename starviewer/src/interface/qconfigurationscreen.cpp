/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "qconfigurationscreen.h"

#include <QString>

#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QCursor>
#include <QFileDialog>
#include <QProcess>
#include <QLabel>
#include <q3listview.h>

#include "pacsparameters.h"
#include "status.h"
#include "pacslistdb.h"
#include "pacslist.h"
#include "constkey.h"
#include "cachepacs.h"
#include "cachepool.h"
#include "starviewersettings.h"
#include <math.h>



namespace udg {

/** Constructor de la classe
  */
QConfigurationScreen::QConfigurationScreen( QWidget *parent )
 : QDialog( parent )
{
    int i;

    setupUi( this );
    fillPacsListView(); //emplena el listview amb les dades dels pacs, que tenim configurats
    
    for (i=0;i<=pacsListView->columns();i++)
    {
        pacsListView->setColumnWidthMode(i,Q3ListView::Manual); 
    }
    m_PacsID=0;
    
    loadCacheDefaults();
    loadPacsDefaults();
    m_ButtonCacheApply->setEnabled(false);
    m_ButtonPacsApply->setEnabled(false);
}

/** Carrega les dades de configuració de la cache
  */
void QConfigurationScreen::loadCacheDefaults()
{

    StarviewerSettings settings;


    m_DataBaseRoot->setText(settings.getDatabasePath());
    m_CacheImagePath->setText(settings.getCacheImagePath());

    
    loadCachePoolDefaults();
}

/** calcula les dades del pool
  */
void QConfigurationScreen::loadCachePoolDefaults()
{
    Status state;
    int space,used;
    float result;
    QString text;
    CachePool *pool = CachePool::getCachePool();
    
    //accemdim a la caché a agafar les dades del Pool
    state = pool->getPoolTotalSize(space);   
    if (!state.good())
    {
        databaseError(&state);
        return;
    }
    
    state = pool->getPoolUsedSpace(used);
    if (!state.good())
    {
        databaseError(&state);
        return;
    }
    
    m_PoolSize->setText(text.setNum(space/1000,10));
    
    result=used;
    result = result/1000; //passem Mb a Gb;
    text.setNum(result,'f',2);
    text.append(" Gb");
    m_PoolUsed->setText(text);
    
//     //espai lliure
    result = space - used;
    result = result /1000;
    text.setNum(result,'f',2);
    text.append(" Gb");
    m_PoolFree->setText(text);
    
    //Percentatges
    result = used;
    result = result/space*100;
    text.setNum(result,'f',1);
    text.append(" %");
    m_PoolUsedPerc->setText(text);
    
    result = (space - used);
    result = result/space*100;
    text.setNum(result,'f',1);
    text.append(" %");
    m_PoolFreePerc->setText(text);

}

/** Emplena els textboxs amb les dades del PACS
  */
void QConfigurationScreen::loadPacsDefaults()
{
    QString result;
    StarviewerSettings settings;
    

    m_AETitleMachine->setText(settings.getAETitleMachine());
    m_LocalPort->setText(settings.getLocalPort());
    m_Timeout->setText(settings.getTimeout());
    m_MaxConnections->setText(settings.getMaxConnections());
    m_CheckCountImages->setChecked(settings.getCountImages());
    m_CheckPrevImages->setChecked(settings.getPrevImages());
}

/*************************************************************************************************************************************/
/*                                              PACS DEVICE                                                                          */
/*************************************************************************************************************************************/

/** Neteja els line edit de la pantalla
  */
void QConfigurationScreen:: clear()
{
    m_TextAETitle->setText("");
    m_TextAddress->setText("");
    m_TextPort->setText("");
    m_TextInstitution->setText("");
    m_TextLocation->setText("");
    m_TextDescription->setText("");
    m_CheckDefault->setChecked(false);
    m_PacsID=0;
}

/** Slot que dona d'alta el PACS a la la base de dades
  */
void QConfigurationScreen::addPacs()
{
    PacsParameters pacs;
    Status state;
    PacsListDB pacsList;
    
    if (validatePacsParameters())
    {
        pacs.setAEPacs( m_TextAETitle->text().toStdString() );
        pacs.setPacsPort( m_TextPort->text().toStdString() );
        pacs.setPacsAdr( m_TextAddress->text().toStdString() );
        pacs.setInstitution( m_TextInstitution->text().toStdString() );
        pacs.setLocation( m_TextLocation->text().toStdString() );
        pacs.setDescription( m_TextDescription->text().toStdString() );
        if ( m_CheckDefault->isChecked() )
        {
            pacs.setDefault("S");
        }
        else pacs.setDefault("N");
       
        
        state =  pacsList.insertPacs(&pacs);

        if (!state.good())
        {
            databaseError(&state,&pacs);
        }
        else 
        {
            fillPacsListView();
            clear();
        }
    }

}

/** Slot que s'activa quant seleccionem un Pacs del PacsListView, emplena les caixes de texts amb les dades del Pacs
  */
void QConfigurationScreen::selectedPacs(Q3ListViewItem * item)
{
    PacsList list;
    PacsParameters pacs;
    Status state;
    PacsListDB pacsList;

    if (item!=NULL)
    {
        
        state = pacsList.queryPacsList(list);        
        
        if (!state.good())
        {
            databaseError(&state,&pacs);
            return;
        }
                
        if (list.findPacs( item->text(0).toStdString() ) ) //busquem les dades del PACS
        {   
            pacs = list.getPacs();
            
            //emplenem els textots
            m_TextAETitle->setText( pacs.getAEPacs().c_str() );
            m_TextPort->setText( pacs.getPacsPort().c_str() );
            m_TextAddress->setText( pacs.getPacsAdr().c_str() );
            m_TextInstitution->setText( pacs.getInstitution().c_str() );
            m_TextLocation->setText( pacs.getLocation().c_str() );
            m_TextDescription->setText( pacs.getDescription().c_str() );
            m_PacsID = pacs.getPacsID();
            if (pacs.getDefault() == "S")
            {
                m_CheckDefault->setChecked(true);
            }
            else m_CheckDefault->setChecked(false);
            
            //bloquegem el AETitle perque es camp clau, no el deixem modificar
       }

    }
}

/** Slot que updata les dades d'un pacs
  */
void QConfigurationScreen::updatePacs()
{
    PacsParameters pacs;
    Status state;
    PacsListDB pacsList;
    
    if (m_PacsID==0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Select a Pacs for update"));
        return;
    }
    
    if (validatePacsParameters())
    {
        pacs.setAEPacs( m_TextAETitle->text().toStdString() );
        pacs.setPacsPort( m_TextPort->text().toStdString() );
        pacs.setPacsAdr( m_TextAddress->text().toStdString() );
        pacs.setInstitution( m_TextInstitution->text().toStdString() );
        pacs.setLocation( m_TextLocation->text().toStdString() );
        pacs.setDescription( m_TextDescription->text().toStdString() );
        pacs.setPacsID(m_PacsID);
        if (m_CheckDefault->isChecked())
        {
            pacs.setDefault("S");
        }
        else pacs.setDefault("N");
       
        state = pacsList.updatePacs(&pacs);
         
        if (!state.good())
        {
            databaseError(&state,&pacs);
        }
        else 
        {
            fillPacsListView();
            clear();
        }
            
    }
}

/** Slot que esborra el pacs seleccionat
  */
void QConfigurationScreen::deletePacs()
{

    Status state;
    PacsParameters pacs;
    PacsListDB pacsList;
    
    if (m_PacsID==0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Select a Pacs for delete"));
        return;
    }    
    
    pacs.setPacsID(m_PacsID);//per donar de baixa n'hi prou amb el camp clau    

    state = pacsList.deletePacs(&pacs);
    
    
    if (!state.good())
    {
        databaseError(&state,&pacs);
    }
    else
    {
        fillPacsListView();
        clear();
    }

}

/** Emplena el ListView amb les dades dels PACS que tenim guardades a la bd
  */
void QConfigurationScreen::fillPacsListView()
{

    Status state;
    PacsParameters pacs;
    PacsList list;
    PacsListDB pacsList;
    
    pacsListView->clear();
    
    state = pacsList.queryPacsList(list);
    
    if (state.good())
    {
        list.firstPacs();
        
        while (!list.end())
        {
            Q3ListViewItem* item = new Q3ListViewItem(pacsListView);            
            pacs = list.getPacs();
            item->setText(0,pacs.getAEPacs().c_str() );
            item->setText(1,pacs.getPacsAdr().c_str() );
            item->setText(2,pacs.getPacsPort().c_str() );
            item->setText(3,pacs.getInstitution().c_str() );
            item->setText(4,pacs.getLocation().c_str() );
            item->setText(5,pacs.getDescription().c_str() );
            if (pacs.getDefault() == "S")
            {
                item->setText(6,tr("YES"));
            }
            else item->setText(6,tr("NO"));
            
            list.nextPacs();
        }
    }

}

/** Fa un echo a un pacs seleccionat per saber si aquest està viu
  */
void QConfigurationScreen::test()
{

    PacsParameters pacs;
    
//     pacs.setPacsID(1);
//     
//     m_pacsList.queryPacs(&pacs,"");
    
 
}

/** Comprovem que els paràmetres dels PACS siguin correctes. 
  *            1r Que el AETitle no estigui en blanc,
  *            2n Que l'adreça del PACS no estigui en blanc,
  *            3r Que el Port del Pacs sigui entre 0 i 65535
  *            4t Que l'institució no estigui buida 
  *    @return bool, retorna cert si tots els parametres del pacs son correctes
  */
bool QConfigurationScreen::validatePacsParameters()
{
    QString text;
    
    //Per força tot els pacs han de tenir algun AETitle
    text = m_TextAETitle->text();
    if (text.length() == 0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("AETitle field can't be empty"));
        return false;
    }
    
    //adreça del pacs no pot estar en blanc
    text = m_TextAddress->text();
    if (text.length() == 0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Incorrect address server"));
        return false;
    }   
    
    //el port ha d'estar entre 0 i 65535
    text = m_TextPort->text();
    if (!(text.toInt(NULL,10) >=0 && text.toInt(NULL,10)<=65535) || text.length()==0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("PACS Port has to be between 0 and 65535"));
        return false;    
    }
    
    //la institució no pot estar en blanc
    text = m_TextInstitution->text();
    if (text.length() == 0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Institution field can't be empty"));
        return false;
    }   
    
    
    
    return true;
}

/** Valida que els canvis de la configuració siguin correctes
  *        Port local entre 0 i 65535
  *        Numero màxim de connexions 25
  *        Path de la base de dades i directori dicom's existeix
  *    @return indica si els canvis son correctes
  */
bool QConfigurationScreen::validateChanges()
{
    QDir dir;

    if (m_LocalPort->isModified())
    {
        if (m_LocalPort->text().toInt(NULL,10)<0 || m_LocalPort->text().toInt(NULL,10) > 65535)
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Local Port has to be between 0 and 65535"));
            return false;        
        }
    }
    
    if (m_MaxConnections->isModified())
    {
        if (m_MaxConnections->text().toInt(NULL,10)<1 || m_MaxConnections->text().toInt(NULL,10) > 15 )
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Maximum simultaenious connections has to be between 1 and 15"));
            return false;        
        }
    }    
    
    if (m_DataBaseRoot->isModified())
    {
        if ( !dir.exists(m_DataBaseRoot->text()))
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Invalid database path"));
            return false;             
        }
    }

    if (m_CacheImagePath->isModified())
    {
        if (!dir.exists(m_CacheImagePath->text()))
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Invalid cache image path. The directory doesn't exit"));
            return false;             
        }
    }    

    return true;
}

/** Guarda els canvis a la configuració dels paràmetres del PACS
  */
void QConfigurationScreen::acceptChanges()
{
    if (validateChanges())
    {    
        applyChangesPacs();
        applyChangesCache();
        this->hide();
    }
}

/** Tanca la pantalla de configuració
  */
void QConfigurationScreen::cancelChanges()
{   
    this->hide();
}

/** Aplica els canvis de la configuració
  */
void QConfigurationScreen::applyChanges()
{
    if (validateChanges())
    {
        applyChangesPacs();
        applyChangesCache();
    }
}

/** Guarda els canvis a la configuració dels paràmetres del PACS
  */
void QConfigurationScreen::applyChangesPacs()
{


    StarviewerSettings settings;
    
    
    if (m_AETitleMachine->isModified())
    {
        settings.setAETitleMachine(m_AETitleMachine->text());
    }
    
    if (m_Timeout->isModified())
    {
        settings.setTimeout(m_Timeout->text());
    }
    
    if (m_LocalPort->isModified())
    {
        settings.setLocalPort(m_LocalPort->text());
    }
    
    if (m_MaxConnections->isModified())
    {
        settings.setMaxConnections( m_MaxConnections->text());
    }
    
    settings.setCountImages(m_CheckCountImages->isChecked());
    settings.setPrevImages(m_CheckPrevImages->isChecked());
    
    m_ButtonPacsApply->setEnabled(false);
    
}


/** Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
  */
void QConfigurationScreen::configurationChanged (int)
{
    m_ButtonPacsApply->setEnabled(true);
    m_ButtonCacheApply->setEnabled(true);
}


/** Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
  */
void QConfigurationScreen::configurationChanged (const QString&)
{
    m_ButtonPacsApply->setEnabled(true);
    m_ButtonCacheApply->setEnabled(true);
}


/** Mostra un QDialog per especificar on es troba la base de dades de la caché
  */
void QConfigurationScreen::examinateDataBaseRoot()
{
    QProcess process;
//     dirname , filter , parent , name , modal
//     QFileDialog *dlg = new QFileDialog( process.workingDirectory().dirName(), "*.sdb (StarViewer Database)", 0, 0, TRUE );
    
//     parent , caption ,  dir , filter
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , process.workingDirectory(), tr("*.sdb (StarViewer Database)") );
    QString directory;

    dlg->setFileMode( QFileDialog::ExistingFile );
    
    if ( dlg->exec() == QDialog::Accepted ) {
        m_DataBaseRoot->setText( dlg->selectedFile() );
//         directory = dlg->url();
// \TODO : serveix d'algo aquest directori??????????????
        directory = dlg->directory().absolutePath();
    }
    
    delete dlg;
}

/** Mostra un QDialog per especificar on s'han de guardar les imatges descarregades
  */
void QConfigurationScreen::examinateCacheImagePath()
{
    QProcess process;
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , process.workingDirectory(), tr("*.sdb (StarViewer Database)") );
    QString directory;
    
    dlg->setMode( QFileDialog::DirectoryOnly );
    
    if ( dlg->exec() == QDialog::Accepted ) {
        m_CacheImagePath->setText(dlg->selectedFile());
//         directory = dlg->url();
// \TODO : serveix d'algo aquest directori??????????????
        directory = dlg->directory().absolutePath();
    }
       
    delete dlg;
}


/**  Aplica els canvis fets a la configuració de la cache
  */
void QConfigurationScreen::applyChangesCache()
{

    StarviewerSettings settings;
    CachePool * pool = CachePool::getCachePool();
    Status state;
    
    //Aquest els guardem sempre 
    settings.setCacheImagePath(m_CacheImagePath->text());
    settings.setDatabasePath(m_DataBaseRoot->text());
    
    if (m_PoolSize->isModified())
    {   
        state = pool->updatePoolTotalSize(m_PoolSize->text().toInt(NULL,10)*1000);//Passem l'espai a Mb
        databaseError(&state);
    }
    
    if (m_CacheImagePath->isModified())
    {
        settings.setCacheImagePath(m_CacheImagePath->text());
    }
    
    m_ButtonCacheApply->setEnabled(false);
}


/** Esborra tota la caché
  */
void QConfigurationScreen::deleteStudies()
{
    CachePacs * localCache;
    Status state;
            
   
   switch( QMessageBox::information( this, tr("Starviewer"),
				      tr("Are you sure you want to delete all Studies of the cache ?"),
				      tr("Yes"), tr("No"),
				      0, 1 ) ) 
    {
    case 0:
        this->setCursor(QCursor(Qt::WaitCursor));
            
        localCache = CachePacs::getCachePacs();
        
        state = localCache->clearCache();
        
        if (!state.good())
        {
            databaseError(&state);
        }
        
        loadCachePoolDefaults();
        this->setCursor(QCursor(Qt::ArrowCursor));
	break;

    }

}


/** Compacta la base de dades de la cache
  */
void QConfigurationScreen::compactCache()
{
    CachePacs * localCache;
    Status state;
    
    this->setCursor(QCursor(Qt::WaitCursor));    
    localCache = CachePacs::getCachePacs();
    
    state = localCache->compactCachePacs();
    
    if (!state.good())
    {
        databaseError(&state);
    }

    this->setCursor(QCursor(Qt::ArrowCursor));    
}


/** Tracta els errors que s'han produït durant els accessos a la base dades
  *           @param state [in] Estat de l'acció 
  */
void QConfigurationScreen::databaseError(Status *state)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {   
            case 2019 : text.insert(0,tr("Contraint Violation "));
                        text.append("\n");
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2001 : text.insert(0,tr("Database not found."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
        }
    }    

}


/** Tracta els errors que s'han produït durant els accessos a la base dades
  *           @param state [in] Estat de l'acció 
  *           @param PacsParameters que ha produït l'error
  */
void QConfigurationScreen::databaseError(Status *state,PacsParameters *pacs)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {   
            case 2019 : text.insert(0,tr("AETitle "));
                        text.append(pacs->getAEPacs().c_str() );
                        text.append(tr(" exists"));
                        text.append("\n");
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2001 : text.insert(0,tr("Database missing."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
        }
    }    

}

/** destructor de la classe*/
QConfigurationScreen::~QConfigurationScreen()
{
}


};
