/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qcustomwindowleveldialog.h"
#include "logging.h"
#include "starviewerapplication.h" // Pel UserDataRootPath
#include "windowlevelpresetstooldata.h"

//Qt's

#include <QTextStream>
#include <QMessageBox>
#include <QLineEdit>
#include <QInputDialog>

namespace udg {

QCustomWindowLevelDialog::QCustomWindowLevelDialog(QDialog *parent)
 : QDialog(parent)
{
    setupUi(this);
    createConnections();
    needToReload = false;
    loadFromFile();
    m_description = "";    
}

QCustomWindowLevelDialog::~QCustomWindowLevelDialog()
{
}

void QCustomWindowLevelDialog::setDefaultWindowLevel(double window , double level)
{
    m_windowSpinBox->setValue(window);
    m_levelSpinBox->setValue(level);
}

void QCustomWindowLevelDialog::createConnections()
{
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(confirmWindowLevel()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_editButton, SIGNAL(clicked()), this, SLOT(editPresets()));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(windowLevelHandler()));
    
}

void QCustomWindowLevelDialog::confirmWindowLevel()
{
    // validar els spin box
    if(m_windowSpinBox->value())
    {
        // \TODO implement me!
        DEBUG_LOG("Falta validar el valor del window");
    }
    if(m_levelSpinBox->value())
    {
        // \TODO implement me!
        DEBUG_LOG("Falta validar el valor del level");
    }

    emit windowLevel(m_windowSpinBox->value(), m_levelSpinBox->value());
    this->close();
}

void QCustomWindowLevelDialog::windowLevelHandler()
{    
    bool ok;    
    m_description = QInputDialog::getText(this, tr("Save Window Level"), tr("Description"), QLineEdit::Normal, "", &ok);
    if (ok)
    {
        double win = m_windowSpinBox->value();
        double lev = m_levelSpinBox->value();
        bool trobat = false;
        do
        {
            if (!m_presetsData->getWindowLevelFromDescription(m_description, win, lev) && !m_description.isEmpty() && m_description.size() <15)
            {
                saveToFile();
                confirmWindowLevel();
                trobat = true;
                ok = true;
            }
            /// Si no s'ha guardat mostrem el perquè
            else
            {
                QMessageBox msgBox;
                if(m_description.isEmpty())
                {
                    msgBox.setText(tr("Preset name can't be empty"));
                }
                else if(m_description.size() >= 15)
                {
                    msgBox.setText(tr("Maximum of 15 characters exceeded"));
                }
                else
                {
                    msgBox.setText(tr("Preset exists! Change the preset name"));
                }
                msgBox.exec();
                m_description = QInputDialog::getText(this, tr("Save Window Level"), tr("Description"), QLineEdit::Normal, m_description, &ok);
                trobat = false;
            }
        }while(!trobat && ok);
    }
}

void QCustomWindowLevelDialog::loadFromFile()
{    
    QString directory = UserDataRootPath + "windowlevels/customwindowlevel.ini";
    QFile customWindowLevel(directory);
    if (customWindowLevel.exists())
    {
        if (!customWindowLevel.open(QIODevice.ReadOnly | QIODevice.Text))
        {
            DEBUG_LOG("ERROR LLEGINT");
            return;
        }
        else
        {
            QTextStream in(&customWindowLevel);
            QString line;
            do
            {
                line = in.readLine();
                /// L'arxiu .ini conté el nom, el window i el level separats per ";"
                QStringList strings = line.split(";");

                if (!line.isNull())
                {
                    DEBUG_LOG(QString("s1: %1 s2: %2 s3: %3").arg(strings[0]).arg(strings[1]).arg(strings[2]));
                    ///Afegim a una estructura per tal de poder-la guardar al m_presets.
                    QVector<double> winLev;
                    winLev.push_back(strings[1].toDouble());
                    winLev.push_back(strings[2].toDouble());
                    DEBUG_LOG(QString("Num valors: %1").arg(winLev.size()));
                    m_presets.insert(strings[0],winLev);                   
                }

            }while (!line.isNull());
        }
    }
    else
    {
        DEBUG_LOG("Encara no s'ha creat cap custom window level");
    }
}

void QCustomWindowLevelDialog::saveToFile()
{
    QDir dir (UserDataRootPath );
    dir.mkdir("windowlevels");
    QString directory = UserDataRootPath + "windowlevels/customwindowlevel.ini";
    DEBUG_LOG(directory);
    QFile customWindowLevel(directory);
    if (customWindowLevel.exists())
    {
        if (!customWindowLevel.open(QIODevice.Append | QIODevice.Text))
        {
            DEBUG_LOG("ERROR LLEGINT");
            return;
        }
        else
        {
            QTextStream out(&customWindowLevel);
            QVector<double> winLev;
            winLev.push_back(m_windowSpinBox->value());
            winLev.push_back(m_levelSpinBox->value());

            /// L'arxiu .ini conté el nom, el window i el level separats per ";"
            out << m_description << ";" << winLev.at(0)<< ";" << winLev.at(1) << "\n";
            m_presets.insert(m_description, winLev);
            m_presetsData->addPreset(m_description, winLev.at(0), winLev.at(1),2);
        }
    }
    else
    {
        if (!customWindowLevel.open(QIODevice.ReadWrite | QIODevice.Text))
        {
            DEBUG_LOG("ERROR A L' ESCRIURE");
            return;
        }
        QTextStream out(&customWindowLevel);
        QVector<double> winLev;
        winLev.push_back(m_windowSpinBox->value());
        winLev.push_back(m_levelSpinBox->value());

        /// L'arxiu .ini conté el nom, el window i el level separats per ";"
        out << m_description << ";" << winLev.at(0)<< ";" << winLev.at(1) << "\n";
        m_presets.insert(m_description, winLev);
        m_presetsData->addPreset(m_description, winLev.at(0), winLev.at(1),2);      
    }
}

void QCustomWindowLevelDialog::updatePresets()
{
    QMap<QString, QVector<double> >::const_iterator i = m_presets.constBegin();
    /// Si hem borrat un preset hem de borrar-los de tots els visors. Ara mateix no tenim accés al visor
    /// directament, per tant borrem tots els custom presets (grup 2) i els afegim tots un altre cop.
    /// Una alternativa seria crear una llista de borrats i comprovar.
    /// A més ara mateix intenta afegir tots els presets i potser només caldria que afegis els nous custom
    if(needToReload)
    {
        m_presetsData->removePresetsFromGroup(2);
    }
    while (i != m_presets.constEnd())
    {
        m_presetsData->addPreset(i.key(), i.value().at(0), i.value().at(1), 2);
        ++i;
    }
}

void QCustomWindowLevelDialog::editPresets()
{
    QStringList items;
    QString deleteItem = "";
    QMap<QString, QVector<double>>::const_iterator i = m_presets.constBegin();
    ///Variable per fer el setNum(asignar un Numero a un String)
    QString aux;
    while (i != m_presets.constEnd())
    {
        QString cadena = i.key();
        aux.setNum(i.value().at(0));
        cadena.append("\tw( ");
        cadena.append(aux);
        cadena.append(" ), l( ");
        aux.setNum(i.value().at(1));
        cadena.append(aux);
        cadena.append(" )");
        items << cadena;
        ++i;
    }
    if (m_presets.size() > 0)
    {
        bool ok;
        deleteItem = QInputDialog::getItem(this, tr("Delete Window Level"), tr("Preset"), items, 0, false, &ok);    
        
        if (ok )
        {
            QMessageBox msgBox;
            msgBox.setText("The Window Level will be deleted");
            msgBox.setInformativeText("Do you want to continue?");
            msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int retorn = msgBox.exec();
            
            if (retorn == QMessageBox::Ok)
            {
                int index = deleteItem.indexOf("\t");
                deleteItem = deleteItem.left(index);
                QMap<QString, QVector<double> >::const_iterator i = m_presets.constBegin();
                QString bac;
                bac.setNum(deleteItem.size());        
                bac.setNum(i.key().size());
                while (i != m_presets.constEnd())
                {
                    if(i.key() == deleteItem)
                    {
                        m_presetsData->removePreset(i.key());
                        DEBUG_LOG(QString("Borrem el preset %1").arg(deleteItem));
                        needToReload = true;
                        //m_presets.remove(i.key());
                    }            
                    ++i;
                }
                m_presets.remove(deleteItem);
            }
            refreshFile();        
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("There are not Custom Window Level presets!"));
        msgBox.exec();
    }
}

void QCustomWindowLevelDialog::refreshFile()
{
    QDir dir (UserDataRootPath);
    dir.mkdir("windowlevels");
    QString directory = UserDataRootPath + "windowlevels/customwindowlevel.ini";
    QFile customWindowLevel(directory);
    if (customWindowLevel.exists())
    {
        if (!customWindowLevel.open(QIODevice.ReadWrite | QIODevice.Truncate | QIODevice.Text))
        {
            DEBUG_LOG("ERROR LLEGINT");
            return;
        }
        else
        {            
            QMap<QString, QVector<double>>::const_iterator i = m_presets.constBegin();
            while (i != m_presets.constEnd())
            {
                QTextStream out(&customWindowLevel);
                out << i.key() << ";" << i.value().at(0) << ";" << i.value().at(1) << "\n";
                ++i;
            }            
        }
    }
}

void QCustomWindowLevelDialog::setPresetsData(WindowLevelPresetsToolData *windowLevelData)
{
    m_presetsData = windowLevelData;
    updatePresets();
}

};
