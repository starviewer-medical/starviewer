/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qwindowlevelcombobox.h"
#include "qcustomwindowleveldialog.h"
#include "logging.h"

namespace udg {

QWindowLevelComboBox::QWindowLevelComboBox(QWidget *parent)
 : QComboBox(parent)
{
    m_customWindowLevelDialog = new QCustomWindowLevelDialog;
    populate();
    createConnections();
}

QWindowLevelComboBox::~QWindowLevelComboBox()
{
}

void QWindowLevelComboBox::createConnections()
{
    connect( this , SIGNAL( activated(int) ) , this , SLOT( processSelected(int) ) );
    connect( this , SIGNAL( windowLevel( double , double ) ) , m_customWindowLevelDialog , SLOT( setDefaultWindowLevel( double , double ) ) );
    connect( m_customWindowLevelDialog , SIGNAL( windowLevel(double,double) ) , this , SLOT( updateWindowLevel(double,double) ) );
}

void QWindowLevelComboBox::populate()
{
    this->addItem( tr("Default") );
    this->addItem( tr("CT Bone") );
    this->addItem( tr("CT Lung") );
    this->addItem( tr("CT Soft Tissues, Non Contrast") );
    this->addItem( tr("CT Liver, Non Contrast") );
    this->addItem( tr("CT Soft Tissues, Contrast Medium") );
    this->addItem( tr("CT Liver, Contrast Medium") );
    this->addItem( tr("CT Neck, Contrast Medium") );
    this->addItem( tr("Angiography") );
    this->addItem( tr("Osteoporosis") );
    this->addItem( tr("Emphysema") );
    this->addItem( tr("Petrous Bone") );
    this->addItem( tr("Custom...") );
}

void QWindowLevelComboBox::processSelected( int value )
{
    switch( value )
    {
    case 0:
        emit defaultValue();
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Defecte");
        return;
    break;

    case 1:
        m_window = 2000;
        m_level = 500;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Bone");
    break;

    case 2:
        m_window = 1500;
        m_level = -650;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Lung");
    break;

    case 3:
        m_window = 400;
        m_level = 40;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> SoftTissuesNC");
    break;

    case 4:
        m_window = 200;
        m_level = 40;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> LiverNC");
    break;

    case 5:
        m_window = 400;
        m_level = 70;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> SoftTissuesCM");
    break;

    case 6:
        m_window = 300;
        m_level = 60; // 60-100
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> LiverCM");
    break;

    case 7:
        m_window = 300;
        m_level = 50;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> NeckCM");
    break;

    case 8:
        m_window = 500;
        m_level = 100; // 100-200
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Angiography");
    break;

    case 9:
        m_window = 1000; // 100-1500
        m_level = 300;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Osteoporosis");
    break;

    case 10:
        m_window = 800;
        m_level = -800;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Emfisema");
    break;
    
    case 11:
        m_window = 4000;
        m_level = 700;
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Petrous Bone");
    break;

    case 12:
        // custom
        m_customWindowLevelDialog->exec();
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Custom");
    break;

    default:
        emit defaultValue();
        INFO_LOG("QWindowLevelComboBox: Canviem Window Level >> Defecte");
        return;
    break;
    }
    emit windowLevel( m_window , m_level );
}

void QWindowLevelComboBox::updateWindowLevel( double window , double level )
{
    m_window = window;
    m_level = level;
    emit windowLevel( m_window , m_level );
}

};

