/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qconfigurationscreen.h"

#include <QMessageBox>
#include <QDir>

#include "pacsserver.h"
#include "status.h"
#include "pacsdevice.h"
#include "pacsdevicemanager.h"
#include "starviewerapplication.h"
#include "logging.h"
#include "utils.h"
#include "localdatabasemanager.h"
#include "inputoutputsettings.h"

namespace udg {

QConfigurationScreen::QConfigurationScreen( QWidget *parent ) : QWidget(parent)
{
    setupUi( this );

    fillPacsListView(); //emplena el listview amb les dades dels pacs, que tenim configurats
    loadPacsDefaults();
    loadInstitutionInformation();
    m_buttonApplyPacs->setEnabled(false);
    m_configurationChanged = false;

    createConnections();
    m_buttonApplyPacs->setIcon( QIcon( ":images/apply.png" ) );

    configureInputValidator();
    
    Settings settings;
    settings.restoreColumnsWidths( InputOutputSettings::ConfigurationScreenPACSList, m_PacsTreeView );
    
    m_PacsTreeView->setColumnHidden(0, true); // amaguem la columna amb l'ID del PACS que és irrellevant per l'usuari
    m_PacsTreeView->sortByColumn(1, Qt::AscendingOrder);//Ordenem per AETitle

    checkIncomingConnectionsPortNotInUse();
}

QConfigurationScreen::~QConfigurationScreen()
{
    Settings settings;
    settings.saveColumnsWidths( InputOutputSettings::ConfigurationScreenPACSList, m_PacsTreeView );
}

void QConfigurationScreen::createConnections()
{
    //connecta el boto aplicar del Pacs amb l'slot apply
    connect( m_buttonApplyPacs , SIGNAL( clicked() ),  SLOT( applyChanges() ) );
    
    connect( m_textAETitleMachine, SIGNAL( textChanged(const QString &) ) , SLOT( enableApplyButtons() ) );
    connect( m_textTimeout, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textLocalPort, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    //En el moment en que ens editen el textBox si apareixia el missatge de port en ús el fem invisible
    connect( m_textLocalPort, SIGNAL(textChanged(const QString &)), SLOT(checkIncomingConnectionsPortNotInUse()));
    connect( m_textMaxConnections, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textInstitutionName, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textInstitutionAddress, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textInstitutionTown, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textInstitutionZipCode, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textInstitutionCountry, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textInstitutionPhoneNumber, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textInstitutionEmail, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );

    connect(m_checkBoxQueryRetrieveEnabled, SIGNAL(stateChanged(int)), SLOT(queryRetrieveServiceEnabledChanged()));
    connect(m_checkBoxStoreEnabled, SIGNAL(stateChanged(int)), SLOT(storeServiceEnabledChanged()));
    
    connect( m_textQueryRetrieveServicePort, SIGNAL(editingFinished()), SLOT(m_textQueryRetrieveServicePortChanged())); 

    //manteniment PACS
    connect( m_buttonAddPacs , SIGNAL( clicked() ), SLOT( addPacs() ) );
    connect( m_buttonDeletePacs , SIGNAL( clicked() ), SLOT( deletePacs() ) );
    connect( m_buttonUpdatePacs , SIGNAL( clicked() ), SLOT( updatePacs() ) );
    connect( m_buttonTestPacs , SIGNAL( clicked() ), SLOT( test() ) );
    connect( m_PacsTreeView , SIGNAL( itemSelectionChanged() ), SLOT( updateSelectedPACSInformation() ) );
}

void QConfigurationScreen::configureInputValidator()
{
    m_textQueryRetrieveServicePort->setValidator( new QIntValidator(0, 65535, m_textQueryRetrieveServicePort) );
    m_textStoreServicePort->setValidator( new QIntValidator(0, 65535, m_textStoreServicePort) );
    m_textLocalPort->setValidator( new QIntValidator(0, 65535, m_textLocalPort) );
    m_textTimeout->setValidator( new QIntValidator(0, 99, m_textTimeout) );
    m_textMaxConnections->setValidator( new QIntValidator(0, 99, m_textMaxConnections) );
}

void QConfigurationScreen::loadPacsDefaults()
{
    QString result;
    Settings settings;
    m_textAETitleMachine->setText( PacsDevice::getLocalAETitle() );
    m_textLocalPort->setText( QString::number(PacsDevice::getIncomingDICOMConnectionsPort()) );
    m_textTimeout->setText( QString::number(PacsDevice::getConnectionTimeout()) );
    m_textMaxConnections->setText( QString::number(PacsDevice::getMaximumConnections()) );
}

void QConfigurationScreen::loadInstitutionInformation()
{
    Settings settings;

    m_textInstitutionName->setText(settings.getValue(InputOutputSettings::InstitutionName).toString());
    m_textInstitutionAddress->setText(settings.getValue(InputOutputSettings::InstitutionAddress).toString());
    m_textInstitutionTown->setText(settings.getValue(InputOutputSettings::InstitutionTown).toString());
    m_textInstitutionZipCode->setText(settings.getValue(InputOutputSettings::InstitutionZipCode).toString());
    m_textInstitutionCountry->setText(settings.getValue(InputOutputSettings::InstitutionCountry).toString());
    m_textInstitutionEmail->setText(settings.getValue(InputOutputSettings::InstitutionEmail).toString());
    m_textInstitutionPhoneNumber->setText(settings.getValue(InputOutputSettings::InstitutionPhoneNumber).toString());
}

void QConfigurationScreen:: clear()
{
    m_textAETitle->clear();
    m_textAddress->clear();
    m_textInstitution->clear();
    m_textLocation->clear();
    m_textDescription->clear();
    m_checkDefault->setChecked( false );
    m_selectedPacsID = "";
    m_textQueryRetrieveServicePort->clear();
    m_textStoreServicePort->clear();
    m_checkBoxQueryRetrieveEnabled->setChecked(true);
    m_checkBoxStoreEnabled->setChecked(true);
}

void QConfigurationScreen::addPacs()
{
    PacsDevice pacs;
    PacsDeviceManager pacsDeviceManager;

    if (validatePacsDevice())
    {
        pacs = getPacsDeviceFromControls();

        INFO_LOG( "Afegir PACS " + pacs.getAETitle() );

        if ( !pacsDeviceManager.addPACS(pacs) )
        {
            QMessageBox::warning(this, ApplicationNameString, tr("This PACS already exists."));
        }
        else
        {
            fillPacsListView();
            clear();
            emit configurationChanged("Pacs/ListChanged");
        }
    }
}

void QConfigurationScreen::updateSelectedPACSInformation()
{
    QList<PacsDevice> pacsList;
    PacsDevice selectedPacs;
    PacsDeviceManager pacsDeviceManager;

    QTreeWidgetItem *selectedItem = 0;
    if( !m_PacsTreeView->selectedItems().isEmpty() )
    {
        // només en podem tenir un de seleccionat
        selectedItem = m_PacsTreeView->selectedItems().first();
        // TODO en comptes d'obtenir del manager, potser es podria obtenir la informació directament del tree widget i estalviar aquest pas de "query"
        selectedPacs = pacsDeviceManager.getPACSDeviceByID( selectedItem->text(0) );// selectedItem->text(0) --> ID del pacs seleccionat al TreeWidget
        if( selectedPacs.isEmpty() )
        {
            ERROR_LOG("No s'ha trobat cap PACS configurat amb l'ID: " + selectedItem->text(0) );
            DEBUG_LOG("No s'ha trobat cap PACS configurat amb l'ID: " + selectedItem->text(0) );
            clear();
        }
        else
        {
            //emplenem els textos
            m_textAETitle->setText( selectedPacs.getAETitle() );
            m_textAddress->setText( selectedPacs.getAddress() );
            m_textInstitution->setText( selectedPacs.getInstitution() );
            m_textLocation->setText( selectedPacs.getLocation() );
            m_textDescription->setText( selectedPacs.getDescription() );
            m_checkDefault->setChecked( selectedPacs.isDefault() );
            m_checkBoxQueryRetrieveEnabled->setChecked(selectedPacs.isQueryRetrieveServiceEnabled());
            m_textQueryRetrieveServicePort->setText( selectedPacs.isQueryRetrieveServiceEnabled() ?  QString().setNum(selectedPacs.getQueryRetrieveServicePort()) : "" );
            m_textQueryRetrieveServicePort->setEnabled( selectedPacs.isQueryRetrieveServiceEnabled() );
            m_checkBoxStoreEnabled->setChecked(selectedPacs.isStoreServiceEnabled());
            m_textStoreServicePort->setText( selectedPacs.isStoreServiceEnabled() ? QString().setNum(selectedPacs.getStoreServicePort()) : ""  );
            m_textStoreServicePort->setEnabled( selectedPacs.isStoreServiceEnabled() );
        }
        // indiquem quin és l'ID del PACS seleccionat
        m_selectedPacsID = selectedPacs.getID();
    }
    else 
        m_selectedPacsID = "";
}

void QConfigurationScreen::updatePacs()
{
    PacsDevice pacs;
    PacsDeviceManager pacsDeviceManager;

    if ( m_selectedPacsID == "" )
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "Select a PACS to update." ) );
        return;
    }

    if ( validatePacsDevice() )
    {
        pacs = getPacsDeviceFromControls();
        
        INFO_LOG( "Actualitzant dades del PACS: " + pacs.getAETitle() );

        pacsDeviceManager.updatePACS(pacs);

        fillPacsListView();
        clear();
        emit configurationChanged("Pacs/ListChanged");
    }
}

void QConfigurationScreen::deletePacs()
{
    PacsDeviceManager pacsDeviceManager;

    if ( m_selectedPacsID == "" )
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "Select a PACS to delete." ) );
        return;
    }

    INFO_LOG( "Esborrant el PACS: " + m_textAETitle->text() );

    pacsDeviceManager.deletePACS( m_selectedPacsID );

    fillPacsListView();
    clear();
    emit configurationChanged("Pacs/ListChanged");
}

void QConfigurationScreen::fillPacsListView()
{
    QList<PacsDevice> pacsList;
    PacsDeviceManager pacsDeviceManager;

    m_PacsTreeView->clear();

    pacsList = pacsDeviceManager.getPACSList();

    foreach(PacsDevice pacs, pacsList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_PacsTreeView );

        item->setText(0, pacs.getID());
        item->setText(1, pacs.getAETitle());
        item->setText(2, pacs.getAddress());
        item->setText(3, pacs.getInstitution());
        item->setText(4, pacs.isQueryRetrieveServiceEnabled() ? QString().setNum(pacs.getQueryRetrieveServicePort()) : "Disabled" );
        item->setText(5, pacs.isStoreServiceEnabled() ? QString().setNum(pacs.getStoreServicePort()) : "Disabled");
        item->setText(6, pacs.isDefault() ? tr("Yes") : tr("No" ) );
    }
}

void QConfigurationScreen::test()
{
    Status state;
    PacsDevice pacs;
    PacsDeviceManager pacsList;
    PacsServer pacsServer;
    QString message;

    //mirem que hi hagi algun element (pacs) seleccionat per a poder testejar, altrament informem de que cal seleccionar un node
    if ( m_PacsTreeView->selectedItems().count() > 0 )
    {
        QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

        //Agafem les dades del PACS que estan el textbox per testejar
        pacs = getPacsDeviceFromControls();

        pacsServer.setPacs( pacs );

        state = pacsServer.connect( PacsServer::echoPacs );

        if ( !state.good() )
        {
            message = tr( "PACS \"%1\" doesn't respond.\nBe sure that the IP and AETitle of It are correct." ).arg( pacs.getAETitle() );

            QApplication::restoreOverrideCursor();
            QMessageBox::warning( this , ApplicationNameString , message );
            INFO_LOG( "Doing echo PACS " + pacs.getAETitle() + " doesn't responds. PACS ERROR : " + state.text() );
        }
        else
        {
            state = pacsServer.echo();

            QApplication::restoreOverrideCursor();

            if ( state.good() )
            {
                message = tr( "Test of PACS \"%1\" is correct").arg( pacs.getAETitle() );
                QMessageBox::information( this , ApplicationNameString , message );
                // TODO realment cal fer un INFO LOG d'això?
                INFO_LOG( "Test of PACS " + pacs.getAETitle() + "is correct" );
            }
            else
            {
                message = tr( "PACS \"%1\" doesn't respond correclty.\nBe sure that the IP and AETitle of It are correct." ).arg( pacs.getAETitle() );
                QMessageBox::warning( this , ApplicationNameString , message );
                INFO_LOG( "Doing echo PACS " + pacs.getAETitle() + " doesn't responds correctly. PACS ERROR : " + state.text() );
            }
        }
    }
    else
        QMessageBox::information( this , tr("Information") , tr("To test a PACS it is necessary to select an item of the list.") );
}

bool QConfigurationScreen::validatePacsDevice()
{
    QString text;

    //Per força tot els pacs han de tenir algun AETitle
    text = m_textAETitle->text();
    if ( text.length() == 0 )
    {
         QMessageBox::warning( this , ApplicationNameString , tr("AETitle field can't be empty.") );
        return false;
    }

    //adreça del pacs no pot estar en blanc
    text = m_textAddress->text();
    if ( text.length() == 0 )
    {
        QMessageBox::warning( this , ApplicationNameString , tr ( "Incorrect server address." ) );
        return false;
    }

    if (!m_checkBoxQueryRetrieveEnabled->isChecked() && !m_checkBoxStoreEnabled->isChecked())
    {
        QMessageBox::warning( this, ApplicationNameString, tr( "At least one of the service Query/Retrieve or Send has to be enabled." ) );
        return false;
    }

    if (m_checkBoxQueryRetrieveEnabled->isChecked() && m_textQueryRetrieveServicePort->text().isEmpty())
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "Query/Retrieve Port value has to be between 0 and 65535." ) );
        return false;
    }

    if (m_checkBoxStoreEnabled->isChecked() && m_textStoreServicePort->text().isEmpty())
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "Send Port value has to be between 0 and 65535." ) );
        return false;
    }

    //la institució no pot estar en blanc
    text = m_textInstitution->text();
    if ( text.length() == 0 )
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "Institution field can't be empty." ) );
        return false;
    }

    return true;
}

bool QConfigurationScreen::validateChanges()
{
    QDir dir;

    if ( m_textLocalPort->isModified() )
    {
        if ( m_textLocalPort->text().toInt( NULL , 10 ) < 0 || m_textLocalPort->text().toInt( NULL , 10 ) > 65535 )
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Local Port value has to be between 0 and 65535." ) );
            return false;
        }
    }

    if ( m_textMaxConnections->isModified() )
    {
        if ( m_textMaxConnections->text().toInt( NULL , 10 ) < 1 || m_textMaxConnections->text().toInt( NULL , 10 ) > 15 )
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Maximum simultaenious connections has to be between 1 and 15." ) );
            return false;
        }
    }

    return true;
}

bool QConfigurationScreen::applyChanges()
{
    if (validateChanges())
    {
        if (isIncomingConnectionsPortInUseByAnotherApplication() && m_textLocalPort->isModified())
        {
            QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString, tr( "The port %1 for incoming connections is in use by another application. Are you sure you want to apply the changes?" ).arg(m_textLocalPort->text()),
                                                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            if (response == QMessageBox::No) return false;
        }
        applyChangesPacs();
        applyChangesInstitutionInformation();
        m_configurationChanged = false;

        return true;
    }
    else
        return false;
}

void QConfigurationScreen::applyChangesPacs()
{
    // TODO realment cal fer INFO LOGS d'això?
    Settings settings;

    if ( m_textAETitleMachine->isModified() )
    {
        INFO_LOG( "Modificació del AETitle de la màquina: " + m_textAETitleMachine->text() );
        settings.setValue( InputOutputSettings::LocalAETitle, m_textAETitleMachine->text() );
    }

    if ( m_textTimeout->isModified() )
    {
        INFO_LOG( "Modificació del valor del timeout " + m_textTimeout->text() );
        settings.setValue(InputOutputSettings::PACSConnectionTimeout, m_textTimeout->text());
    }

    if ( m_textLocalPort->isModified() )
    {
        INFO_LOG( "Modificació del Port d'entrada dels estudis" + m_textLocalPort->text() );
        settings.setValue( InputOutputSettings::QueryRetrieveLocalPort, m_textLocalPort->text() );
        m_textLocalPort->setModified(false);
    }

    if ( m_textMaxConnections->isModified() )
    {
        INFO_LOG( "Modificació del nombre màxim de connexions " + m_textMaxConnections->text() );
        settings.setValue( InputOutputSettings::MaximumPACSConnections, m_textMaxConnections->text() );
    }

    m_buttonApplyPacs->setEnabled( false );
}

void QConfigurationScreen::applyChangesInstitutionInformation()
{
    Settings settings;

    if (m_textInstitutionName->isModified())
    {
        settings.setValue(InputOutputSettings::InstitutionName, m_textInstitutionName->text());
    }

    if (m_textInstitutionAddress->isModified())
    {
        settings.setValue(InputOutputSettings::InstitutionAddress, m_textInstitutionAddress->text());
    }

    if (m_textInstitutionTown->isModified())
    {
        settings.setValue(InputOutputSettings::InstitutionTown, m_textInstitutionTown->text());
    }

    if (m_textInstitutionZipCode->isModified())
    {
        settings.setValue(InputOutputSettings::InstitutionZipCode, m_textInstitutionZipCode->text());
    }

    if (m_textInstitutionCountry->isModified())
    {
        settings.setValue(InputOutputSettings::InstitutionCountry, m_textInstitutionCountry->text());
    }

    if (m_textInstitutionEmail->isModified())
    {
        settings.setValue(InputOutputSettings::InstitutionEmail, m_textInstitutionEmail->text());
    }

    if (m_textInstitutionPhoneNumber->isModified())
    {
        settings.setValue(InputOutputSettings::InstitutionPhoneNumber, m_textInstitutionPhoneNumber->text());
    }
}

void QConfigurationScreen::enableApplyButtons()
{
    m_buttonApplyPacs->setEnabled( true );
    m_configurationChanged = true;
}

bool QConfigurationScreen::isIncomingConnectionsPortInUseByAnotherApplication()
{
    //Comprovem que el port estigui o no en ús i que en el cas que estigui en ús, no sigui utilitzat per l'Starviewer
    return Utils::isPortInUse(m_textLocalPort->text().toInt()) && !LocalDatabaseManager().isStudyRetrieving();
}

void QConfigurationScreen::checkIncomingConnectionsPortNotInUse()
{
    m_warningFrameIncomingConnectionsPortInUse->setVisible(isIncomingConnectionsPortInUseByAnotherApplication()); ///Si està en ús el frame que conté el warning es fa visible
}

void QConfigurationScreen::queryRetrieveServiceEnabledChanged()
{
    m_textQueryRetrieveServicePort->setEnabled(m_checkBoxQueryRetrieveEnabled->isChecked());
    m_textQueryRetrieveServicePort->setText("");
    m_checkDefault->setEnabled(m_checkBoxQueryRetrieveEnabled->isChecked());//"Default query PACS" no té sentit que estigui activat si no es pot fer query en el PACS

    if (!m_checkBoxQueryRetrieveEnabled->isChecked())
    {
        m_checkDefault->setChecked(false);
    }
}

void QConfigurationScreen::storeServiceEnabledChanged()
{
    m_textStoreServicePort->setEnabled(m_checkBoxStoreEnabled->isChecked());
    /*Si ens indiquen que el servei d'Store està permés li donem el mateix port que del Query/Retrieve, ja que la majoria de 
      PACS utilitzen el mateix port per Q/R que per store*/
    m_textStoreServicePort->setText(m_checkBoxStoreEnabled->isChecked() ? m_textQueryRetrieveServicePort->text() : "" );
}

void QConfigurationScreen::m_textQueryRetrieveServicePortChanged()
{
    if (!m_textQueryRetrieveServicePort->text().isEmpty())
    {
        if (m_checkBoxStoreEnabled->isChecked() && m_textStoreServicePort->text().isEmpty())
        {
            //Si s'ha indicat que el servei d'store està permés i aquest no té el port configurat li donem per defecte el valor del port de Q/R
            m_textStoreServicePort->setText(m_textQueryRetrieveServicePort->text());
        }
    }
}

PacsDevice QConfigurationScreen::getPacsDeviceFromControls()
{
    PacsDevice pacsDevice;

    pacsDevice.setAETitle( m_textAETitle->text() );
    pacsDevice.setAddress( m_textAddress->text() );
    pacsDevice.setInstitution( m_textInstitution->text() );
    pacsDevice.setLocation( m_textLocation->text() );
    pacsDevice.setDescription( m_textDescription->text() );
    pacsDevice.setID( m_selectedPacsID );
    pacsDevice.setQueryRetrieveServiceEnabled( m_checkBoxQueryRetrieveEnabled->isChecked() );
    pacsDevice.setQueryRetrieveServicePort( m_textQueryRetrieveServicePort->text().toInt() );
    pacsDevice.setStoreServiceEnabled( m_checkBoxStoreEnabled->isChecked() );
    pacsDevice.setStoreServicePort( m_textStoreServicePort->text().toInt() );
    pacsDevice.setDefault( m_checkDefault->isChecked() );

    return pacsDevice;
}

};
