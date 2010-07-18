#include "thumbnailimagedisplayer.h"
#include "image.h"
#include "series.h"

namespace udg {

ThumbnailImageDisplayer::ThumbnailImageDisplayer(QWidget *parent): QWidget(parent)
{
    setupUi(this);
}

ThumbnailImageDisplayer::~ThumbnailImageDisplayer()
{

}

void ThumbnailImageDisplayer::setThumbnailSize(ThumbnailSize thumbnailSize)
{
    QSize size;

    switch (thumbnailSize)
    {
        case ThumbnailImageDisplayer::Small:
            size.setHeight(ScaledSmallThumbnailsSizeY);
            size.setWidth(ScaledSmallThumbnailsSizeX);
        case ThumbnailImageDisplayer::Medium:
            size.setHeight(ScaledMediumThumbnailsSizeY);
            size.setWidth(ScaledMediumThumbnailsSizeX);
        case ThumbnailImageDisplayer::Big:
            size.setHeight(ScaledBigThumbnailsSizeY);
            size.setWidth(ScaledBigThumbnailsSizeX);
        default:
            size.setHeight(ScaledMediumThumbnailsSizeY);
            size.setWidth(ScaledMediumThumbnailsSizeX);
    }

    m_listImagesDisplayer->setIconSize(size);
}

void ThumbnailImageDisplayer::addImage(Image *image)
{
    QListWidgetItem *item = new QListWidgetItem();
    QIcon icon(image->getThumbnail());
    item->setIcon(icon);
    QString text;

    if (image->getInstanceNumber() != NULL)
    {
        text = tr("Image") + " " + image->getInstanceNumber();
        text += "\n";
        text += tr("Series") + " " + image->getParentSeries()->getSeriesNumber();
    }
    else
    {
        text = tr("SOP Instance UID") + " " + image->getSOPInstanceUID();
        text += "\n";
    }

    item->setText(text);
    m_listImagesDisplayer->addItem(item);
}

void ThumbnailImageDisplayer::clearAllThumbnails()
{
    m_listImagesDisplayer->clear();
}

}