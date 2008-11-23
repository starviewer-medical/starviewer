#ifndef UDGCOMBININGVOXELSHADER_CPP
#define UDGCOMBININGVOXELSHADER_CPP


#include "combiningvoxelshader.h"


namespace udg {


template <class VS1, class VS2>
CombiningVoxelShader<VS1, VS2>::CombiningVoxelShader()
 : VoxelShader()
{
    m_voxelShader1 = 0;
    m_voxelShader2 = 0;
}


template <class VS1, class VS2>
CombiningVoxelShader<VS1, VS2>::~CombiningVoxelShader()
{
}


template <class VS1, class VS2>
void CombiningVoxelShader<VS1, VS2>::setVoxelShaders( VS1 *voxelShader1, VS2 *voxelShader2 )
{
    m_voxelShader1 = voxelShader1;
    m_voxelShader2 = voxelShader2;
}


template <class VS1, class VS2>
QString CombiningVoxelShader<VS1, VS2>::toString() const
{
    QString string1 = m_voxelShader1 ? m_voxelShader1->toString() : "null";
    QString string2 = m_voxelShader2 ? m_voxelShader2->toString() : "null";
    return "CombiningVoxelShader<" + string1 + ", " + string2 + ">";
}


}


#endif
