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
#include <QTreeView>

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
    setupUi( this );
    fillPacsListView(); //emplena el listview amb les dades dels pacs, que tenim configurats

    m_PacsID = 0;
    
    loadCacheDefaults();
    loadPacsDefaults();
    m_buttonApplyCache->setEnabled(false);
    m_buttonApplyPacs->setEnabled(false);
    
    connectSignalAndSlots();
}

void QConfigurationScreen::connectSignalAndSlots()
{
    //connecta el boto examinar de la cache amb el dialog per escollir el path de la base de dades
    connect(m_buttonExaminateDataBase, SIGNAL(clicked()), this, SLOT(examinateDataBaseRoot()));
    
    //connecta el boto examinar de la cache amb el dialog per escollir el path de la base de dades
    connect(m_buttonExaminateCacheImage, SIGNAL(clicked()), this, SLOT(examinateCacheImagePath()));
    
    //connecta el boto acceptar de la cache amb l'slot accept
    connect(m_buttonAcceptCache, SIGNAL(clicked()), this, SLOT(acceptChanges()));
    
    //connecta el boto cancelar de la cache amb l'slot cancel
    connect(m_buttonCancelCache, SIGNAL(clicked()), this, SLOT(cancelChanges()));
    
    //connecta el boto aplicar de la cache amb l'slot apply
    connect(m_buttonApplyCache, SIGNAL(clicked()), this, SLOT(applyChanges()));
    
    //connecta el boto acceptar del pacs amb l'slot accept
    connect(m_buttonAcceptPacs, SIGNAL(clicked()), this, SLOT(acceptChanges()));
    
    //connecta el boto cancelar del pacs amb l'slot cancel
    connect(m_buttonCancelPacs, SIGNAL(clicked()), this, SLOT(cancelChanges()));
    
    //connecta el boto aplicar del Pacs amb l'slot apply
    connect(m_buttonApplyPacs, SIGNAL(clicked()), this, SLOT(applyChanges()));
    
    //activen el boto apply quant canvia el seu valor
    connect(m_textDatabaseRoot,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_textCacheImagePath,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_textPoolSize,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_textAETitleMachine,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_textTimeout,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_textDatabaseRoot,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_textLocalPort,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_textMaxConnections,SIGNAL(textChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    connect(m_checkPrevImages,SIGNAL(stateChanged(int)),this,SLOT(configurationChanged( int )));
    connect(m_checkCountImages,SIGNAL(stateChanged(int)),this,SLOT(configurationChanged( int )));
    connect(m_comboLanguage,SIGNAL(editTextChanged(const QString &)),this,SLOT(configurationChanged( const QString& )));
    
    //mateniment base de dades
    connect(m_buttonDeleteStudies,SIGNAL(clicked()),this,SLOT(deleteStudies()));
    connect(m_buttonCompactDatabase,SIGNAL(clicked()),this,SLOT(compactCache()));
    
    //afegeix la / al final del Path de la cache d'imatges
    connect(m_textCacheImagePath,SIGNAL(editingFinished()),this,SLOT(cacheImagePathEditingFinish()));
    
    //manteniment PACS
    connect(m_buttonAddPacs,SIGNAL(clicked()),this,SLOT(addPacs()));
    connect(m_buttonDeletePacs,SIGNAL(clicked()),this,SLOT(deletePacs()));
    connect(m_buttonUpdatePacs,SIGNAL(clicked()),this,SLOT(updatePacs()));
    connect(m_buttonClear,SIGNAL(clicked()),this,SLOT(clear()));
    connect(m_buttonTestPacs,SIGNAL(clicked()),this,SLOT(test()));
    connect(m_PacsTreeView,SIGNAL(itemClicked ( QTreeWidgetItem *, int)),this,SLOT(selectedPacs( QTreeWidgetItem *, int)));
    
    
    
}

/** Carrega les dades de configuració de la cache
  */
void QConfigurationScreen::loadCacheDefaults()
{

    StarviewerSettings settings;

    m_textDatabaseRoot->setText(settings.getDatabasePath());
    m_textCacheImagePath->setText(settings.getCacheImagePath());
    
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
    
    m_textPoolSize->setText(text.setNum(space/1000,10));
    
    result = used;
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

    m_textAETitleMachine->setText(settings.getAETitleMachine());
    m_textLocalPort->setText(settings.getLocalPort());
    m_textTimeout->setText(settings.getTimeout());
    m_textMaxConnections->setText(settings.getMaxConnections());
    m_checkCountImages->setChecked(settings.getCountImages());
    m_checkPrevImages->setChecked(settings.getPrevImages());
}


/************************************************************************************************************************/
/*                                              PACS DEVICE                                                             */
/************************************************************************************************************************/

/** Neteja els line edit de la pantalla
  */
void QConfigurationScreen:: clear()
{
    m_textAETitle->setText("");
    m_textAddress->setText("");
    m_textPort->setText("");
    m_textInstitution->setText("");
    m_textLocation->setText("");
    m_textDescription->setText("");
    m_checkDefault->setChecked(false);
    m_PacsID = 0;
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
        pacs.setAEPacs( m_textAETitle->text().toStdString() );
        pacs.setPacsPort( m_textPort->text().toStdString() );
        pacs.setPacsAdr( m_textAddress->text().toStdString() );
        pacs.setInstitution( m_textInstitution->text().toStdString() );
        pacs.setLocation( m_textLocation->text().toStdString() );
        pacs.setDescription( m_textDescription->text().toStdString() );
        if ( m_checkDefault->isChecked() )
        {
            pacs.setDefault("S");
        }
        else pacs.setDefault("N");
       
        
        state =  pacsList.insertPacs(&pacs);

        if (!state.good())
        {
            if (state.code() == 2019)
            {
            
                QMessageBox::warning( this, tr("StarViewer"),tr("AETitle ") + pacs.getAEPacs().c_str() + tr(" exists") +"\n");
            }else databaseError(&state);

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
void QConfigurationScreen::selectedPacs(QTreeWidgetItem * item,int )
{
    PacsList list;
    PacsParameters pacs;
    Status state;
    PacsListDB pacsList;

    if (item != NULL)
    {
        
        state = pacsList.queryPacsList(list);        
        
        if (!state.good())
        {
            databaseError(&state);
            return;
        }
                
        if (list.findPacs( item->text(0).toStdString() ) ) //busquem les dades del PACS
        {   
            pacs = list.getPacs();
            
            //emplenem els textots
            m_textAETitle->setText( pacs.getAEPacs().c_str() );
            m_textPort->setText( pacs.getPacsPort().c_str() );
            m_textAddress->setText( pacs.getPacsAdr().c_str() );
            m_textInstitution->setText( pacs.getInstitution().c_str() );
            m_textLocation->setText( pacs.getLocation().c_str() );
            m_textDescription->setText( pacs.getDescription().c_str() );
            m_PacsID = pacs.getPacsID();
            if (pacs.getDefault() == "S")
            {
                m_checkDefault->setChecked(true);
            }
            else m_checkDefault->setChecked(false);
            
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
    
    if (m_PacsID == 0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Select a Pacs for update"));
        return;
    }
    
    if (validatePacsParameters())
    {
        pacs.setAEPacs( m_textAETitle->text().toStdString() );
        pacs.setPacsPort( m_textPort->text().toStdString() );
        pacs.setPacsAdr( m_textAddress->text().toStdString() );
        pacs.setInstitution( m_textInstitution->text().toStdString() );
        pacs.setLocation( m_textLocation->text().toStdString() );
        pacs.setDescription( m_textDescription->text().toStdString() );
        pacs.setPacsID(m_PacsID);
        if (m_checkDefault->isChecked())
        {
            pacs.setDefault("S");
        }
        else pacs.setDefault("N");
       
        state = pacsList.updatePacs(&pacs);
         
        if (!state.good())
        {
            databaseError(&state);
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
    
    if (m_PacsID == 0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Select a Pacs for delete"));
        return;
    }    
    
    pacs.setPacsID(m_PacsID);//per donar de baixa n'hi prou amb el camp clau    

    state = pacsList.deletePacs(&pacs);
    
    
    if (!state.good())
    {
        databaseError(&state);
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
    
    m_PacsTreeView->clear();
    
    state = pacsList.queryPacsList(list);
    
    if (state.good())
    {
        list.firstPacs();
        
        while (!list.end())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(m_PacsTreeView);            
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
    text = m_textAETitle->text();
    if (text.length() == 0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("AETitle field can't be empty"));
        return false;
    }
    
    //adreça del pacs no pot estar en blanc
    text = m_textAddress->text();
    if (text.length() == 0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("Incorrect address server"));
        return false;
    }   
    
    //el port ha d'estar entre 0 i 65535
    text = m_textPort->text();
    if (!(text.toInt(NULL,10) >=0 && text.toInt(NULL,10)<=65535) || text.length()==0)
    {
        QMessageBox::warning( this, tr("StarViewer"),tr("PACS Port has to be between 0 and 65535"));
        return false;    
    }
    
    //la institució no pot estar en blanc
    text = m_textInstitution->text();
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

    if (m_textLocalPort->isModified())
    {
        if (m_textLocalPort->text().toInt(NULL,10)<0 || m_textLocalPort->text().toInt(NULL,10) > 65535)
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Local Port has to be between 0 and 65535"));
            return false;        
        }
    }
    
    if (m_textMaxConnections->isModified())
    {
        if (m_textMaxConnections->text().toInt(NULL,10)<1 || m_textMaxConnections->text().toInt(NULL,10) > 15 )
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Maximum simultaenious connections has to be between 1 and 15"));
            return false;        
        }
    }    
    
    if (m_textDatabaseRoot->isModified())
    {
        if ( !dir.exists(m_textDatabaseRoot->text()))
        {
            QMessageBox::warning( this, tr("StarViewer"),tr("Invalid database path"));
            return false;             
        }
    }

    if (m_textCacheImagePath->isModified())
    {
        if (!dir.exists(m_textCacheImagePath->text()))
        {
            switch (QMessageBox::question(this,
                    tr("Create directory ?"),
                    tr("The cache image directory doesn't exists. Do you want to create it ?"),
                    tr("&Yes"), tr("&No"), 0, 1))
                    
            {
                case 0:
                    return dir.mkpath(m_textCacheImagePath->text());
                case 1: 
                    return false;
            }
                
        
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
    
    
    if (m_textAETitleMachine->isModified())
    {
        settings.setAETitleMachine(m_textAETitleMachine->text());
    }
    
    if (m_textTimeout->isModified())
    {
        settings.setTimeout(m_textTimeout->text());
    }
    
    if (m_textLocalPort->isModified())
    {
        settings.setLocalPort(m_textLocalPort->text());
    }
    
    if (m_textMaxConnections->isModified())
    {
        settings.setMaxConnections( m_textMaxConnections->text());
    }
    
    settings.setCountImages(m_checkCountImages->isChecked());
    settings.setPrevImages(m_checkPrevImages->isChecked());
    
    m_buttonApplyPacs->setEnabled(false);
    
}


/** Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
  */
void QConfigurationScreen::configurationChanged (int)
{
    m_buttonApplyPacs->setEnabled(true);
    m_buttonApplyCache->setEnabled(true);
}


/** Slot que s'utilitza quant es fa algun canvi a la configuració, per activar els buttons apply
  */
void QConfigurationScreen::configurationChanged (const QString&)
{
    m_buttonApplyPacs->setEnabled(true);
    m_buttonApplyCache->setEnabled(true);
}


/** Mostra un QDialog per especificar on es troba la base de dades de la caché
  */
void QConfigurationScreen::examinateDataBaseRoot()
{
    // \TODO canviar la manera d'escollir els fitxers , perque ara ens tornen una llista i nomes en volem un fitxer
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./", "Starviewer Database (*.sdb)" );

    dlg->setFileMode( QFileDialog::ExistingFile );
    
    if ( dlg->exec() == QDialog::Accepted ) 
    {
        m_textDatabaseRoot->setText( dlg->selectedFiles().takeFirst() );
    }
    
    delete dlg;
}

/** Mostra un QDialog per especificar on s'han de guardar les imatges descarregades
  */
void QConfigurationScreen::examinateCacheImagePath()
{
    // \TODO canviar la manera d'escollir els fitxers , perque ara ens tornen una llista i nomes en volem un fitxer
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./", tr("Cache Directory"));
    
    dlg->setFileMode( QFileDialog::DirectoryOnly );
    
    if ( dlg->exec() == QDialog::Accepted )
    {
        m_textCacheImagePath->setText(dlg->selectedFiles().takeFirst() );
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
    settings.setCacheImagePath(m_textCacheImagePath->text());
    settings.setDatabasePath(m_textDatabaseRoot->text());
    
    if (m_textPoolSize->isModified())
    {   
        state = pool->updatePoolTotalSize(m_textPoolSize->text().toInt(NULL,10)*1000);//Passem l'espai a Mb
        databaseError(&state);
    }
    
    if (m_textCacheImagePath->isModified())
    {
        settings.setCacheImagePath(m_textCacheImagePath->text());
    }
    
    m_buttonApplyCache->setEnabled(false);
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


/** Afegeix la '/' al final del path del directori si l'usuari no l'ha escrit
  */
void QConfigurationScreen::cacheImagePathEditingFinish()
{
    QString path;
    cout<<"entro\n";
    if (!m_textCacheImagePath->text().endsWith('/',Qt::CaseInsensitive))
    {
        cout<<"passo\n";
        path = m_textCacheImagePath->text();
        path.append("/");
        m_textCacheImagePath->setText(path);
    }
}


/** Tracta els errors que s'han produït a la base de dades en general
  *           @param state [in] Estat del mètode
  */
void QConfigurationScreen::databaseError(Status *state)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {  case 2001 : text.insert(0,tr("Database not found."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        break;
            case 2019 : text.insert(0,tr("Register duplicated."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        break;
            case 2050 : text.insert(0,"Not Connected to database");
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        break;            
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
        }
        QMessageBox::critical( this, tr("StarViewer"),text);
    }    

}



/** destructor de la classe*/
QConfigurationScreen::~QConfigurationScreen()
{
}


};
