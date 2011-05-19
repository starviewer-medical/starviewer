#include "screenshottool.h"
#include "qviewer.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "coresettings.h"
// Definicions globals d'aplicació
#include "starviewerapplication.h"
// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>
// Qt
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
// Pel "wait cursor"
#include <QApplication>

namespace udg {

const QString ScreenShotTool::PngFileFilter = tr("PNG (*.png)");
const QString ScreenShotTool::JpegFileFilter = tr("Jpeg (*.jpg)");
const QString ScreenShotTool::BmpFileFilter = tr("BMP (*.bmp)");
const QString ScreenShotTool::TiffFileFilter = tr("TIFF (*.tiff)");

ScreenShotTool::ScreenShotTool(QViewer *viewer, QObject *parent) : Tool(viewer, parent)
{
    m_toolName = "ScreenShotTool";
    readSettings();
    m_fileExtensionFilters = PngFileFilter + ";;" + JpegFileFilter + ";;" + BmpFileFilter + ";;" + TiffFileFilter;
    if (!viewer)
    {
        DEBUG_LOG("El viewer proporcionat és NUL!");
    }
}

ScreenShotTool::~ScreenShotTool()
{
}

void ScreenShotTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::KeyPressEvent:
        {
            int key = m_viewer->getInteractor()->GetKeyCode();
            // TODO Cal mirar quina és la manera adequada de com gestionar el Ctrl+tecla amb vtk
            // CTRL+s = key code 19
            // CTRL+a = key code 1
            switch (key)
            {
                // Ctrl+s, "single shot"
                case 19:
                    this->screenShot();
                    break;

                // Ctrl+a, "multiple shot"
                case 1:
                    this->screenShot(false);
                    break;
            }
        }
        break;
    }
}

void ScreenShotTool::singleCapture()
{
    screenShot(true);
}

void ScreenShotTool::completeCapture()
{
    screenShot(false);
}

void ScreenShotTool::screenShot(bool singleShot)
{
    readSettings();

    QString caption;
    if (singleShot)
    {
        caption = tr("Save single screenshot as...");
    }
    else
    {
        QMessageBox::information(0, tr("Information"),
                                 tr("You're going to save several screenshots at one time.\nIt's recommended you save them in an empty folder."));
        caption = tr("Save multiple screenshots as...");
    }
    QString filename = QFileDialog::getSaveFileName(0, caption, m_lastScreenShotPath + "/" + compoundSelectedName(), m_fileExtensionFilters,
                                                    &m_lastScreenShotExtensionFilter);

    if (!filename.isEmpty())
    {
        // Mirem que el nom del fitxer no contingui coses com: nom.png, és a dir, que no es mostri l'extensió
        QString selectedExtension = m_lastScreenShotExtensionFilter.mid(m_lastScreenShotExtensionFilter.lastIndexOf("."));
        selectedExtension.remove(")");

        if (filename.endsWith(selectedExtension))
        {
            filename.remove(filename.lastIndexOf(selectedExtension), selectedExtension.size() + 1);
        }

        // Guardem l'últim path de la imatge per a saber on hem d'obrir per defecte l'explorador per a guardar el fitxer
        m_lastScreenShotPath = QFileInfo(filename).absolutePath();
        // Guardem el nom de l'ultim fitxer
        m_lastScreenShotFileName = QFileInfo(filename).fileName();

        // Pel que pugui trigar el procés
        QApplication::setOverrideCursor(Qt::WaitCursor);
        if (singleShot)
        {
            m_viewer->grabCurrentView();
        }
        else
        {
            Q2DViewer *viewer2D = dynamic_cast<Q2DViewer *>(m_viewer);
            if (viewer2D)
            {
                // Tenim un  Q2DViewer, llavors podem guardar totes les imatges

                // Guardem la llesca i fase actual per restaurar
                int currentSlice = viewer2D->getCurrentSlice();
                int currentPhase = viewer2D->getCurrentPhase();
                int maxSlice = viewer2D->getMaximumSlice() + 1;
                // En cas que tinguem fases farem tantes passades com fases
                int phases = viewer2D->getInput()->getNumberOfPhases();
                for (int i = 0; i < maxSlice; i++)
                {
                    viewer2D->setSlice(i);
                    for (int j = 0; j < phases; j++)
                    {
                        viewer2D->setPhase(j);
                        viewer2D->grabCurrentView();
                    }
                }
                // Restaurem
                viewer2D->setSlice(currentSlice);
                viewer2D->setPhase(currentPhase);
            }
            // Tenim un visor que no és 2D, per tant fem un "single shot"
            else
            {
                m_viewer->grabCurrentView();
            }
        }
        // Determinem l'extensió del fitxer
        QViewer::FileType fileExtension;
        if (m_lastScreenShotExtensionFilter == PngFileFilter)
        {
            fileExtension = QViewer::PNG;
        }
        else if (m_lastScreenShotExtensionFilter == JpegFileFilter)
        {
            fileExtension = QViewer::JPEG;
        }
        else if (m_lastScreenShotExtensionFilter == BmpFileFilter)
        {
            fileExtension = QViewer::BMP;
        }
        else if (m_lastScreenShotExtensionFilter == TiffFileFilter)
        {
            fileExtension = QViewer::TIFF;
        }
        else
        {
            DEBUG_LOG("No coincideix cap patró, no es pot desar la imatge! Assignem PNG, per defecte. Aquest error no hauria de passar MAI! ");
            fileExtension = QViewer::PNG;
            m_lastScreenShotExtensionFilter = PngFileFilter;
        }
        // Guardem totes les imatges capturades
        m_viewer->saveGrabbedViews(filename, fileExtension);
        QApplication::restoreOverrideCursor();

        writeSettings();
    }
    else
    {
        // Cal fer alguna cosa?
        // Si està "empty" és que o bé ha cancelat o bé no ha introduit res
    }
}

QString ScreenShotTool::compoundSelectedName()
{
    // TODO Això estaria millor si es fes amb la classe QRegExp,
    // produint un codi molt més net i clar
    QString compoundFile = "";

    if (!m_lastScreenShotFileName.isEmpty())
    {
        QChar lastChar = m_lastScreenShotFileName[m_lastScreenShotFileName.length() - 1];

        if (lastChar.isNumber())
        {
            int i = m_lastScreenShotFileName.length() - 1;

            do
            {
                i--;
                lastChar = m_lastScreenShotFileName[i];
            }while (i > 0 && lastChar.isNumber());

            bool ok;
            int sufix = m_lastScreenShotFileName.right(m_lastScreenShotFileName.length() - (i + 1)).toInt(&ok, 10);

            if (ok)
            {
                compoundFile = m_lastScreenShotFileName.mid(0, i + 1) + QString::number(sufix + 1, 10);
            }
            else
            {
                compoundFile = m_lastScreenShotFileName;
            }
        }
        else
        {
            compoundFile = m_lastScreenShotFileName + "1";
        }
    }
    return compoundFile;
}

void ScreenShotTool::readSettings()
{
    Settings settings;

    m_lastScreenShotPath = settings.getValue(CoreSettings::ScreenShotToolFolder).toString();
    m_lastScreenShotExtensionFilter = settings.getValue(CoreSettings::ScreenShotToolFileExtension).toString();
    m_lastScreenShotFileName = settings.getValue(CoreSettings::ScreenShotToolFilename).toString();
}

void ScreenShotTool::writeSettings()
{
    Settings settings;

    settings.setValue(CoreSettings::ScreenShotToolFolder, m_lastScreenShotPath);
    settings.setValue(CoreSettings::ScreenShotToolFileExtension, m_lastScreenShotExtensionFilter);
    settings.setValue(CoreSettings::ScreenShotToolFilename, m_lastScreenShotFileName);
}

}
