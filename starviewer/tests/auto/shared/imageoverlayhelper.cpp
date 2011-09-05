#include "imageoverlayhelper.h"

#include "imageoverlay.h"

using namespace udg;

namespace testing {

bool ImageOverlayHelper::areEqual(const ImageOverlay &overlay1, const ImageOverlay &overlay2)
{
    bool equal = overlay1.getColumns() == overlay2.getColumns() && overlay1.getRows() == overlay2.getRows() && overlay1.getXOrigin() == overlay2.getXOrigin()
        && overlay1.getYOrigin() == overlay2.getYOrigin();

    if (!equal)
    {
        return false;
    }
    
    // Tots els atributs són iguals, comparem els buffers
    unsigned char *buffer1 = overlay1.getData();
    unsigned char *buffer2 = overlay2.getData();
    
    if (buffer1 && buffer2)
    {
        int length = overlay1.getColumns() * overlay1.getRows();
        int index = 0;
        while (length-- > 0)
        {
            if (buffer1[index] == buffer2[index])
            {
                ++index;
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    else if (!buffer1 && !buffer2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

}
