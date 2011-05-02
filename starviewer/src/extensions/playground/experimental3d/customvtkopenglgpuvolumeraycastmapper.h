#ifndef CUSTOMVTKOPENGLGPUVOLUMERAYCASTMAPPER_H
#define CUSTOMVTKOPENGLGPUVOLUMERAYCASTMAPPER_H


#include <vtkOpenGLGPUVolumeRayCastMapper.h>


namespace udg {


class CustomVtkOpenGLGPUVolumeRayCastMapper : public vtkOpenGLGPUVolumeRayCastMapper {

public:

    static CustomVtkOpenGLGPUVolumeRayCastMapper *New();
    vtkTypeRevisionMacro(CustomVtkOpenGLGPUVolumeRayCastMapper, vtkOpenGLGPUVolumeRayCastMapper);
    //virtual void PrintSelf(ostream &os, vtkIndent indent);

protected:

    CustomVtkOpenGLGPUVolumeRayCastMapper();
    //virtual ~CustomVtkOpenGLGPUVolumeRayCastMapper();

    // Methods called by the AMR Volume Mapper.
    virtual void PreRender(vtkRenderer *renderer, vtkVolume *volume, double datasetBounds[6], double scalarRange[2], int numberOfScalarComponents, unsigned int numberOfLevels);

private:

    CustomVtkOpenGLGPUVolumeRayCastMapper(const CustomVtkOpenGLGPUVolumeRayCastMapper&);    // Not implemented.
    void operator =(const CustomVtkOpenGLGPUVolumeRayCastMapper&);   // Not implemented.

};


} // namespace udg


#endif // CUSTOMVTKOPENGLGPUVOLUMERAYCASTMAPPER_H
