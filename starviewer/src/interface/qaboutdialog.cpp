#include "qaboutdialog.h"

#include "starviewerapplication.h"

namespace udg {

QAboutDialog::QAboutDialog(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);

    setWindowTitle(tr("About %1").arg(ApplicationNameString));
    m_applicationNameLabel->setText("<h2>" + ApplicationNameString + "</h2>");

    QString aboutMessage = tr(
        "<p>Copyright &copy; 2005-%2 Graphics & Imaging Laboratory (GILab) and Institut de Diagnòstic per la Imatge (IDI), Girona."
        "<p align='justify'>%1 is a basic but fully featured image review software dedicated to DICOM images produced by medical equipment (MRI,"
        " CT, PET, PET-CT, CR, MG,...) fully compliant with the DICOM standard for image communication and image file formats. It can also read"
        " many other file formats specified by the MetaIO standard (*.mhd files). ").arg(ApplicationNameString).arg(2012);
    // TODO repassar els textos de l'about
#ifdef STARVIEWER_LITE
    aboutMessage += tr("<p align='justify'>%1 has been specifically designed for navigation and visualization of multimodality and"
               " multidimensional images").arg(ApplicationNameString);
#else
    aboutMessage += tr("%1 is able to receive images "
               "transferred by DICOM communication protocol from any PACS or medical imaging modality (STORE SCP - Service Class Provider, "
               "STORE SCU - Service Class User, and Query/Retrieve)."
               "<p align='justify'>%1 enables navigation and visualization of multimodality and multidimensional images through"
               " a complete 2D Viewer which integrates advanced reconstruction techniques such as Thick Slab (including Maximum"
               " Intensity Projection (MIP), Minimum Intensity Projection (MinIP) and average projection), fast orthogonal reconstruction"
               " and 3D navigation tools such as 3D-Cursor. It also incorporates Multi-Planar Reconstruction (MPR) and 3D Viewer for volume rendering."
               "<p align='justify'>%1 is at the same time a DICOM workstation for medical imaging and an image processing"
               " software for medical research (radiology and nuclear imaging), functional imaging and 3D imaging.").arg(ApplicationNameString);
#endif

    aboutMessage += tr("<p align='justify'>%1 is the result of the close collaboration between IDI and GiLab and experience of both entities in the fields of "
                "radiology, medical imaging and image processing.").arg(ApplicationNameString);;
    aboutMessage += tr("<p>Version: %1 </p>").arg(StarviewerVersionString);
    aboutMessage += tr("<p>Support email: <a href=\"mailto:%1\">%1</a></p>").arg(OrganizationEmailString);
    aboutMessage += tr("<p>Web: <a href=\"%1\">%1</a></p>").arg(OrganizationWebURL);
    m_aboutTextLabel->setText(aboutMessage);
}

QAboutDialog::~QAboutDialog()
{
}

}