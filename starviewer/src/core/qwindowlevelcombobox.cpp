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

void QWindowLevelComboBox::insertWindowLevelPreset( double window, double level, int position, QString description )
{
    std::vector< double *>::iterator iterator;
    iterator = m_windowLevelArray.begin();
    int i = 0;
    while( iterator != m_windowLevelArray.end() && i < position )
    {
        i++;
        iterator++;
    }
    this->insertItem( i, description );
    double *wl = new double[2];
    wl[0] = window;
    wl[1] = level;
    m_windowLevelArray.insert( iterator, wl );
}

void QWindowLevelComboBox::createConnections()
{
    connect( this , SIGNAL( activated(int) ) , this , SLOT( setActiveWindowLevel(int) ) );
    connect( this , SIGNAL( windowLevel( double , double ) ) , m_customWindowLevelDialog , SLOT( setDefaultWindowLevel( double , double ) ) );
    connect( m_customWindowLevelDialog , SIGNAL( windowLevel(double,double) ) , this , SLOT( updateWindowLevel(double,double) ) );
}

void QWindowLevelComboBox::populate()
{
    this->insertWindowLevelPreset( 2000 , 500, 0, tr("CT Bone") );
    this->insertWindowLevelPreset( 1500 , -650, 1, tr("CT Lung") );
    this->insertWindowLevelPreset( 400 , 40, 2, tr("CT Soft Tissues, Non Contrast") );
    this->insertWindowLevelPreset( 200 , 40, 3, tr("CT Liver, Non Contrast") );
    this->insertWindowLevelPreset( 400 , 70, 4, tr("CT Soft Tissues, Contrast Medium") );
    this->insertWindowLevelPreset( 300 , 60, 5, tr("CT Liver, Contrast Medium") ); // 60-100
    this->insertWindowLevelPreset( 300 , 50, 6, tr("CT Neck, Contrast Medium") );
    this->insertWindowLevelPreset( 500 , 100, 7, tr("Angiography") ); // 100-200
    this->insertWindowLevelPreset( 1000 , 300, 8, tr("Osteoporosis") );// 100-1500:window!
    this->insertWindowLevelPreset( 800 , -800, 9, tr("Emphysema") );
    this->insertWindowLevelPreset( 4000 , 700, 10, tr("Petrous Bone") );
    this->insertWindowLevelPreset( 0 , 0, 11, tr("Custom") );
}

void QWindowLevelComboBox::setActiveWindowLevel( int value )
{
    int customIndex = this->findText( tr("Custom") );
    if( customIndex != value  )
    {
        this->updateWindowLevel( m_windowLevelArray[ value ][0], m_windowLevelArray[ value ][1] );
    }
    else
    {
        m_customWindowLevelDialog->exec();
    }
}

void QWindowLevelComboBox::updateWindowLevel( double window , double level )
{
    double *wl = new double[2];
    wl[0] = window;
    wl[1] = level;
    int customIndex = this->findText( tr("Custom") );
    m_windowLevelArray[ customIndex ] = wl;
    emit windowLevel( wl[0] , wl[1] );
}

};

