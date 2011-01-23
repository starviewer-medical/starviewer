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

const QString WindowLevelDirectory("windowlevels/");
const QString WindowLevelFileName("customwindowlevel.ini");

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
    if (m_windowSpinBox->value())
    {
        // \TODO implement me!
        DEBUG_LOG("Falta validar el valor del window");
    }
    if (m_levelSpinBox->value())
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
        double window = m_windowSpinBox->value();
        double level = m_levelSpinBox->value();
        bool founded = false;
        do
        {
            if (!m_presetsData->getWindowLevelFromDescription(m_description, window, level) && !m_description.isEmpty() && m_description.size() <15)
            {
                saveToFile();
                confirmWindowLevel();
                founded = true;
                ok = true;
            }
            /// Si no s'ha guardat mostrem el perquè
            else
            {
                QMessageBox messageBox;
                if (m_description.isEmpty())
                {
                    messageBox.setText(tr("Preset name can't be empty"));
                }
                else if (m_description.size() >= 15)
                {
                    messageBox.setText(tr("Maximum of 15 characters exceeded"));
                }
                else
                {
                    messageBox.setText(tr("Preset exists! Change the preset name"));
                }
                messageBox.exec();
                m_description = QInputDialog::getText(this, tr("Save Window Level"), tr("Description"), QLineEdit::Normal, m_description, &ok);
                founded = false;
            }
        }while(!founded && ok);
    }
}

void QCustomWindowLevelDialog::loadFromFile()
{    
    QString directory = UserDataRootPath;
    directory.append(WindowLevelDirectory);
    if (!QDir(directory).exists())
    {
        QDir(UserDataRootPath).mkdir(WindowLevelDirectory);
    }
    directory.append(WindowLevelFileName);
    QFile customWindowLevelFile(directory);
    if (customWindowLevelFile.exists())
    {
        if (!customWindowLevelFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            DEBUG_LOG("ERROR LLEGINT");
            return;
        }
        else
        {
            QTextStream inputStream(&customWindowLevelFile);
            QString inputLine;
            do
            {
                inputLine = inputStream.readLine();
                /// L'arxiu .ini conté el nom, el window i el level separats per ";"
                QStringList strings = inputLine.split(";");

                if (!inputLine.isNull())
                {
                    DEBUG_LOG(QString("s1: %1 s2: %2 s3: %3").arg(strings[0]).arg(strings[1]).arg(strings[2]));
                    ///Afegim a una estructura per tal de poder-la guardar al m_presets.
                    QVector<double> windowLevel;
                    windowLevel.push_back(strings[1].toDouble());
                    windowLevel.push_back(strings[2].toDouble());
                    DEBUG_LOG(QString("Num valors: %1").arg(windowLevel.size()));
                    m_presets.insert(strings[0],windowLevel);                   
                }

            }while (!inputLine.isNull());
        }
    }
    else
    {
        DEBUG_LOG("Encara no s'ha creat cap custom window level");
    }
}

void QCustomWindowLevelDialog::saveToFile()
{
    QString directory = UserDataRootPath;
    directory.append(WindowLevelDirectory);
    if (!QDir(directory).exists())
    {
        QDir(UserDataRootPath).mkdir(WindowLevelDirectory);
    }
    directory.append(WindowLevelFileName);
    QFile customWindowLevelFile(directory);
    if (customWindowLevelFile.exists())
    {
        if (!customWindowLevelFile.open(QIODevice::Append | QIODevice::Text))
        {
            DEBUG_LOG("ERROR LLEGINT");
            return;
        }
        else
        {
            QTextStream outputStream(&customWindowLevelFile);
            QVector<double> windowLevel;
            windowLevel.push_back(m_windowSpinBox->value());
            windowLevel.push_back(m_levelSpinBox->value());

            /// L'arxiu .ini conté el nom, el window i el level separats per ";"
            outputStream << m_description << ";" << windowLevel.at(0)<< ";" << windowLevel.at(1) << "\n";
            m_presets.insert(m_description, windowLevel);
            m_presetsData->addPreset(m_description, windowLevel.at(0), windowLevel.at(1),2);
        }
    }
    else
    {
        if (!customWindowLevelFile.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            DEBUG_LOG("ERROR A L' ESCRIURE");
            return;
        }
        QTextStream outputStream(&customWindowLevelFile);
        QVector<double> windowLevel;
        windowLevel.push_back(m_windowSpinBox->value());
        windowLevel.push_back(m_levelSpinBox->value());

        /// L'arxiu .ini conté el nom, el window i el level separats per ";"
        outputStream << m_description << ";" << windowLevel.at(0)<< ";" << windowLevel.at(1) << "\n";
        m_presets.insert(m_description, windowLevel);
        m_presetsData->addPreset(m_description, windowLevel.at(0), windowLevel.at(1),2);      
    }
}

void QCustomWindowLevelDialog::updatePresets()
{
    QMap<QString, QVector<double> >::const_iterator presetsIterator = m_presets.constBegin();
    /// Si hem borrat un preset hem de borrar-los de tots els visors. Ara mateix no tenim accés al visor
    /// directament, per tant borrem tots els custom presets (grup 2) i els afegim tots un altre cop.
    /// Una alternativa seria crear una llista de borrats i comprovar.
    /// A més ara mateix intenta afegir tots els presets i potser només caldria que afegis els nous custom
    if (needToReload)
    {
        m_presetsData->removePresetsFromGroup(2);
    }
    while (presetsIterator != m_presets.constEnd())
    {
        m_presetsData->addPreset(presetsIterator.key(), presetsIterator.value().at(0), presetsIterator.value().at(1), 2);
        ++presetsIterator;
    }
}

void QCustomWindowLevelDialog::editPresets()
{
    QStringList itemList;
    QString deleteItem = "";
    QMap<QString, QVector<double> >::const_iterator presetsIterator = m_presets.constBegin();
    ///Variable per fer el setNum(asignar un Numero a un String)
    QString numToText;
    while (presetsIterator != m_presets.constEnd())
    {
        QString item = presetsIterator.key();
        numToText.setNum(presetsIterator.value().at(0));
        item.append("\tw( ");
        item.append(numToText);
        item.append(" ), l( ");
        numToText.setNum(presetsIterator.value().at(1));
        item.append(numToText);
        item.append(" )");
        itemList << item;
        ++presetsIterator;
    }
    if (m_presets.size() > 0)
    {
        bool ok;
        deleteItem = QInputDialog::getItem(this, tr("Delete Window Level"), tr("Preset"), itemList, 0, false, &ok);    
        
        if (ok )
        {
            QMessageBox messageBox;
            messageBox.setText("The Window Level will be deleted");
            messageBox.setInformativeText("Do you want to continue?");
            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            messageBox.setDefaultButton(QMessageBox::Ok);
            int retorn = messageBox.exec();
            
            if (retorn == QMessageBox::Ok)
            {
                int index = deleteItem.indexOf("\t");
                deleteItem = deleteItem.left(index);
                QMap<QString, QVector<double> >::const_iterator presetsIterator = m_presets.constBegin();
                while (presetsIterator != m_presets.constEnd())
                {
                    if (presetsIterator.key() == deleteItem)
                    {
                        m_presetsData->removePreset(presetsIterator.key());
                        needToReload = true;
                        //m_presets.remove(presetsIterator.key());
                    }            
                    ++presetsIterator;
                }
                m_presets.remove(deleteItem);
            }
            refreshFile();        
        }
    }
    else
    {
        QMessageBox messageBox;
        messageBox.setText(tr("There are not Custom Window Level presets!"));
        messageBox.exec();
    }
}

void QCustomWindowLevelDialog::refreshFile()
{
    QString directory = UserDataRootPath;
    directory.append(WindowLevelDirectory);
    if (!QDir(directory).exists())
    {
        QDir(UserDataRootPath).mkdir(WindowLevelDirectory);
    }
    directory.append(WindowLevelFileName);
    QFile customWindowLevelFile(directory);
    if (customWindowLevelFile.exists())
    {
        if (!customWindowLevelFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        {
            DEBUG_LOG("ERROR LLEGINT");
            return;
        }
        else
        {            
            QMap<QString, QVector<double> >::const_iterator presetsIterator = m_presets.constBegin();
            while (presetsIterator != m_presets.constEnd())
            {
                QTextStream outputStream(&customWindowLevelFile);
                outputStream << presetsIterator.key() << ";" << presetsIterator.value().at(0) << ";" << presetsIterator.value().at(1) << "\n";
                ++presetsIterator;
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
