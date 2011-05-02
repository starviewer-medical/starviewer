#include "customvtkopenglgpuvolumeraycastmapper.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>


namespace udg {


//#ifndef VTK_IMPLEMENT_MESA_CXX
//vtkCxxRevisionMacro(CustomVtkOpenGLGPUVolumeRayCastMapper, "$Revision: 1.9 $");
vtkStandardNewMacro(CustomVtkOpenGLGPUVolumeRayCastMapper);
//#endif


CustomVtkOpenGLGPUVolumeRayCastMapper::CustomVtkOpenGLGPUVolumeRayCastMapper()
{
}


//CustomVtkOpenGLGPUVolumeRayCastMapper::~CustomVtkOpenGLGPUVolumeRayCastMapper()
//{
//}


//-----------------------------------------------------------------------------
// Standard print method
//-----------------------------------------------------------------------------
//void vtkOpenGLGPUVolumeRayCastMapper::PrintSelf(ostream &os, vtkIndent indent)
//{
//    this->Superclass::PrintSelf(os, indent);
//}


//extern class vtkUnsupportedRequiredExtensionsStringStream;


//-----------------------------------------------------------------------------
// Rendering initialization including making the context current, loading
// necessary extensions, allocating frame buffers, updating transfer function,
// computing clipping regions, and building the fragment shader.
//
// Pre-conditions:
//   - renderer != NULL
//   - volume != NULL
//   - renderer->GetRenderWindow() != NULL
//   - 1 <= numberOfScalarComponents <= 4
//   - numberOfLevels >= 1
//-----------------------------------------------------------------------------
void CustomVtkOpenGLGPUVolumeRayCastMapper::PreRender(vtkRenderer *renderer, vtkVolume *volume, double datasetBounds[], double scalarRange[], int numberOfScalarComponents, unsigned int numberOfLevels)
{
/*    // Inici còpia ---------------------------------------------------------------------------------------------------------------------------------------------
    // make sure our window is the current OpenGL context.
    renderer->GetRenderWindow()->MakeCurrent();

    // If we haven't already succeeded in loading the extensions,
    // try to load them
    if(!this->LoadExtensionsSucceeded)
    {
        this->LoadExtensions(renderer->GetRenderWindow());
    }

    // If we can't load the necessary extensions, provide
    // feedback on why it failed.
    if(!this->LoadExtensionsSucceeded)
    {
        vtkErrorMacro("Rendering failed because the following OpenGL extensions are required but not supported: " << (this->UnsupportedRequiredExtensions->Stream.str()).c_str());
        return;
        }

      // Create the OpenGL object that we need
      this->CreateOpenGLObjects();

      // Compute the reduction factor that may be necessary to get
      // the interactive rendering rate that we want
      this->ComputeReductionFactor(vol->GetAllocatedRenderTime());

      // Allocate the frame buffers
      if(!this->AllocateFrameBuffers(ren))
        {
        vtkErrorMacro("Not enough GPU memory to create a framebuffer.");
        return;
        }

      // Save the scalar range - this is what we will use for the range
      // of the transfer functions
      this->TableRange[0]=scalarRange[0];
      this->TableRange[1]=scalarRange[1];


      if(this->RGBTable==0)
        {
        this->RGBTable=new vtkRGBTable;
        }

      if(this->MaskInput!=0)
        {
        if(this->Mask1RGBTable==0)
          {
          this->Mask1RGBTable=new vtkRGBTable;
          }
        if(this->Mask2RGBTable==0)
          {
          this->Mask2RGBTable=new vtkRGBTable;
          }
        }

       // Update the color transfer function
      this->UpdateColorTransferFunction(vol,numberOfScalarComponents);

      // Update the noise texture that will be used to jitter rays to
      // reduce alliasing artifacts
      this->UpdateNoiseTexture();

      // We are going to change the blending mode and blending function -
      // so lets push here so we can pop later
      glPushAttrib(GL_COLOR_BUFFER_BIT);

      // If this is the canonical view - we don't want to intermix so we'll just
      // start by clearing the z buffer.
      if ( this->GeneratingCanonicalView )
        {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

      // See if the volume transform is orientation-preserving
      vtkMatrix4x4 *m=vol->GetMatrix();
      double det=vtkMath::Determinant3x3(
        m->GetElement(0,0),m->GetElement(0,1),m->GetElement(0,2),
        m->GetElement(1,0),m->GetElement(1,1),m->GetElement(1,2),
        m->GetElement(2,0),m->GetElement(2,1),m->GetElement(2,2));

      this->PreserveOrientation=det>0;

      // If we have clipping planes, render the back faces of the clipped
      // bounding box of the whole dataset to set the zbuffer.
      if(this->ClippingPlanes && this->ClippingPlanes->GetNumberOfItems()!=0)
        {
        // push the model view matrix onto the stack, make sure we
        // adjust the mode first
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        this->TempMatrix[0]->DeepCopy(vol->GetMatrix());
        this->TempMatrix[0]->Transpose();
        glMultMatrixd(this->TempMatrix[0]->Element[0]);
        this->ClipBoundingBox(ren,datasetBounds,vol);
        glEnable (GL_CULL_FACE);
        glCullFace (GL_FRONT);
        glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        this->RenderClippedBoundingBox(0,0,1,ren->GetRenderWindow());
        glDisable (GL_CULL_FACE);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        //glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        }
      // Check if everything is OK
      this->CheckFrameBufferStatus();

      // Intermixed geometry: Grab the depth buffer into a texture

      int size[2];
      int lowerLeft[2];
      ren->GetTiledSizeAndOrigin(size,size+1,lowerLeft,lowerLeft+1);

      vtkgl::ActiveTexture( vtkgl::TEXTURE3 );
      glBindTexture(GL_TEXTURE_2D,
                    static_cast<GLuint>(
                      this->TextureObjects[
                        vtkOpenGLGPUVolumeRayCastMapperTextureObjectDepthMap]));
      glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,lowerLeft[0],lowerLeft[1],size[0],
                          size[1]);


      vtkgl::ActiveTexture( vtkgl::TEXTURE0 );

      int parallelProjection=ren->GetActiveCamera()->GetParallelProjection();

      // initialize variables to prevent compiler warnings.
      int rayCastMethod=vtkOpenGLGPUVolumeRayCastMapperMethodMIP;
      int shadeMethod=vtkOpenGLGPUVolumeRayCastMapperShadeNotUsed;
      int componentMethod=vtkOpenGLGPUVolumeRayCastMapperComponentNotUsed;

      switch(this->BlendMode)
        {
        case vtkVolumeMapper::COMPOSITE_BLEND:
          switch(numberOfScalarComponents)
            {
            case 1:
              componentMethod=vtkOpenGLGPUVolumeRayCastMapperComponentOne;
              break;
            case 4:
              componentMethod=vtkOpenGLGPUVolumeRayCastMapperComponentFour;
              break;
            default:
              assert("check: impossible case" && false);
              break;
            }
          if(this->MaskInput!=0)
            {
            rayCastMethod=
              vtkOpenGLGPUVolumeRayCastMapperMethodCompositeMask;
            }
          else
            {
            //cout<<"program is composite+shade"<<endl;
            rayCastMethod=vtkOpenGLGPUVolumeRayCastMapperMethodComposite;
            }
          if ( vol->GetProperty()->GetShade() )
            {
            shadeMethod=vtkOpenGLGPUVolumeRayCastMapperShadeYes;
            assert("check: only_1_component_todo" && numberOfScalarComponents==1);
            }
          else
            {
            shadeMethod=vtkOpenGLGPUVolumeRayCastMapperShadeNo;
            //cout<<"program is composite"<<endl;
            }
          break;
        case vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND:
          shadeMethod=vtkOpenGLGPUVolumeRayCastMapperShadeNotUsed;
          componentMethod=vtkOpenGLGPUVolumeRayCastMapperComponentNotUsed;
          switch(numberOfScalarComponents)
            {
            case 1:
              rayCastMethod=vtkOpenGLGPUVolumeRayCastMapperMethodMIP;
              break;
            case 4:
              rayCastMethod=
                vtkOpenGLGPUVolumeRayCastMapperMethodMIPFourDependent;
              break;
            default:
              assert("check: impossible case" && false);
              break;
            }
          break;
        case vtkGPUVolumeRayCastMapper::MINIMUM_INTENSITY_BLEND:
          shadeMethod=vtkOpenGLGPUVolumeRayCastMapperShadeNotUsed;
          componentMethod=vtkOpenGLGPUVolumeRayCastMapperComponentNotUsed;
          switch(numberOfScalarComponents)
            {
            case 1:
              rayCastMethod=vtkOpenGLGPUVolumeRayCastMapperMethodMinIP;
              break;
            case 4:
              rayCastMethod=
                vtkOpenGLGPUVolumeRayCastMapperMethodMinIPFourDependent;
              break;
            default:
              assert("check: impossible case" && false);
              break;
            }
          break;
        default:
          assert("check: impossible case" && 0);
          rayCastMethod=0;
          break;
        }

      this->ComputeNumberOfCroppingRegions(); // TODO AMR vs single block
      if(this->AMRMode)
        {
        NumberOfCroppingRegions=2; // >1, means use do compositing between blocks
        }
      this->BuildProgram(parallelProjection,rayCastMethod,shadeMethod,
                         componentMethod);
      this->CheckLinkage(this->ProgramShader);

      vtkgl::UseProgram(this->ProgramShader);

      // for active texture 0, dataset

      if(numberOfScalarComponents==1)
        {
        // colortable
        vtkgl::ActiveTexture(vtkgl::TEXTURE1);
        this->RGBTable->Bind();

         if(this->MaskInput!=0)
           {
           vtkgl::ActiveTexture(vtkgl::TEXTURE8);
           this->Mask1RGBTable->Bind();
           vtkgl::ActiveTexture(vtkgl::TEXTURE9);
           this->Mask2RGBTable->Bind();
           }
        }

      GLint uDataSetTexture;

      uDataSetTexture=vtkgl::GetUniformLocation(
        static_cast<GLuint>(this->ProgramShader),"dataSetTexture");

      if(uDataSetTexture!=-1)
        {
        vtkgl::Uniform1i(uDataSetTexture,0);
        }
      else
        {
        vtkErrorMacro(<<"dataSetTexture is not a uniform variable.");
        }

      if ( this->MaskInput)
        {
        // Make the mask texture available on texture unit 7
        GLint uMaskTexture;

        uMaskTexture=vtkgl::GetUniformLocation(
          static_cast<GLuint>(this->ProgramShader),"maskTexture");

        if(uMaskTexture!=-1)
          {
          vtkgl::Uniform1i(uMaskTexture,7);
          }
        else
          {
          vtkErrorMacro(<<"maskTexture is not a uniform variable.");
          }
        }

      if(numberOfScalarComponents==1)
        {
        GLint uColorTexture;
        uColorTexture=vtkgl::GetUniformLocation(
          static_cast<GLuint>(this->ProgramShader),"colorTexture");

        if(uColorTexture!=-1)
          {
          vtkgl::Uniform1i(uColorTexture,1);
          }
        else
          {
          vtkErrorMacro(<<"colorTexture is not a uniform variable.");
          }

        if(this->MaskInput!=0)
          {
          GLint uMask1ColorTexture;
          uMask1ColorTexture=vtkgl::GetUniformLocation(
            static_cast<GLuint>(this->ProgramShader),"mask1ColorTexture");

          if(uMask1ColorTexture!=-1)
            {
            vtkgl::Uniform1i(uMask1ColorTexture,8);
            }
          else
            {
            vtkErrorMacro(<<"mask1ColorTexture is not a uniform variable.");
            }

          GLint uMask2ColorTexture;
          uMask2ColorTexture=vtkgl::GetUniformLocation(
            static_cast<GLuint>(this->ProgramShader),"mask2ColorTexture");

          if(uMask2ColorTexture!=-1)
            {
            vtkgl::Uniform1i(uMask2ColorTexture,9);
            }
          else
            {
            vtkErrorMacro(<<"mask2ColorTexture is not a uniform variable.");
            }

          GLint uMaskBlendFactor;
          uMaskBlendFactor=vtkgl::GetUniformLocation(
            static_cast<GLuint>(this->ProgramShader),"maskBlendFactor");
          if(uMaskBlendFactor!=-1)
            {
            vtkgl::Uniform1f(uMaskBlendFactor,this->MaskBlendFactor);
            }
          else
            {
            vtkErrorMacro(<<"maskBlendFactor is not a uniform variable.");
            }
          }

        }

      GLint uOpacityTexture;

      uOpacityTexture=vtkgl::GetUniformLocation(
        static_cast<GLuint>(this->ProgramShader),"opacityTexture");

      if(uOpacityTexture!=-1)
        {
        vtkgl::Uniform1i(uOpacityTexture,2);
        }
      else
        {
        vtkErrorMacro(<<"opacityTexture is not a uniform variable.");
        }

      // depthtexture
      vtkgl::ActiveTexture( vtkgl::TEXTURE3 );
      glBindTexture(GL_TEXTURE_2D,static_cast<GLuint>(this->TextureObjects[vtkOpenGLGPUVolumeRayCastMapperTextureObjectDepthMap]));

      GLint uDepthTexture;

      uDepthTexture=vtkgl::GetUniformLocation(
        static_cast<GLuint>(this->ProgramShader),"depthTexture");

      if(uDepthTexture!=-1)
        {
        vtkgl::Uniform1i(uDepthTexture,3);
        }
      else
        {
        vtkErrorMacro(<<"depthTexture is not a uniform variable.");
        }

      // noise texture
      vtkgl::ActiveTexture( vtkgl::TEXTURE6 );
      glBindTexture(GL_TEXTURE_2D,static_cast<GLuint>(this->NoiseTextureId));


      GLint uNoiseTexture;

      uNoiseTexture=vtkgl::GetUniformLocation(
        static_cast<GLuint>(this->ProgramShader),"noiseTexture");

      if(uNoiseTexture!=-1)
        {
        vtkgl::Uniform1i(uNoiseTexture,6);
        }
      else
        {
        vtkErrorMacro(<<"noiseTexture is not a uniform variable.");
        }

      this->CheckFrameBufferStatus();

      if(this->NumberOfCroppingRegions>1)
        {
        // framebuffer texture
        if(rayCastMethod!=vtkOpenGLGPUVolumeRayCastMapperMethodMIP && rayCastMethod!=vtkOpenGLGPUVolumeRayCastMapperMethodMinIP)
          {
          vtkgl::ActiveTexture( vtkgl::TEXTURE4 );
          glBindTexture(GL_TEXTURE_2D,static_cast<GLuint>(this->TextureObjects[vtkOpenGLGPUVolumeRayCastMapperTextureObjectFrameBufferLeftFront]));

          GLint uFrameBufferTexture;

          uFrameBufferTexture=vtkgl::GetUniformLocation(
            static_cast<GLuint>(this->ProgramShader),"frameBufferTexture");

          this->PrintError("framebuffertexture 1");
          if(uFrameBufferTexture!=-1)
            {
            vtkgl::Uniform1i(uFrameBufferTexture,4);
            }
          else
            {
            vtkErrorMacro(<<"frameBufferTexture is not a uniform variable.");
            }
          this->PrintError("framebuffertexture 2");
          }

        this->CheckFrameBufferStatus();
        // max scalar value framebuffer texture
        if(this->BlendMode==vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND
           || this->BlendMode==vtkGPUVolumeRayCastMapper::MINIMUM_INTENSITY_BLEND)
          {
          vtkgl::ActiveTexture( vtkgl::TEXTURE5 );
          glBindTexture(GL_TEXTURE_2D,static_cast<GLuint>(this->MaxValueFrameBuffer2));

          GLint uScalarBufferTexture;

          uScalarBufferTexture=vtkgl::GetUniformLocation(
            static_cast<GLuint>(this->ProgramShader),"scalarBufferTexture");

          this->PrintError("scalarbuffertexture 1");
          if(uScalarBufferTexture!=-1)
            {
            vtkgl::Uniform1i(uScalarBufferTexture,5);
            }
          else
            {
            vtkErrorMacro(<<"scalarBufferTexture is not a uniform variable.");
            }
          this->PrintError("scalarbuffertexture 2");
          }
        }
      this->CheckFrameBufferStatus();

      GLint uWindowLowerLeftCorner;

      uWindowLowerLeftCorner=vtkgl::GetUniformLocation(
        static_cast<GLuint>(this->ProgramShader),"windowLowerLeftCorner");

      if(uWindowLowerLeftCorner!=-1)
        {
        vtkgl::Uniform2f(uWindowLowerLeftCorner,static_cast<GLfloat>(lowerLeft[0]),
                         static_cast<GLfloat>(lowerLeft[1]));
        }
      else
        {
        vtkErrorMacro(<<"windowLowerLeftCorner is not a uniform variable.");
        }
      GLint uInvOriginalWindowSize;

      uInvOriginalWindowSize=vtkgl::GetUniformLocation(
        static_cast<GLuint>(this->ProgramShader),"invOriginalWindowSize");

      if(uInvOriginalWindowSize!=-1)
        {
        vtkgl::Uniform2f(uInvOriginalWindowSize,
                         static_cast<GLfloat>(1.0/size[0]),
                         static_cast<GLfloat>(1.0/size[1]));
        }
      else
        {
        // yes it is not error. It is only actually used when there is some
        // complex cropping (this->NumberOfCroppingRegions>1). Some GLSL compilers
        // may remove the uniform variable for optimization when it is not used.
        vtkDebugMacro(
          <<"invOriginalWindowSize is not an active uniform variable.");
        }

      size[0] = static_cast<int>(size[0]*this->ReductionFactor);
      size[1] = static_cast<int>(size[1]*this->ReductionFactor);

      GLint uInvWindowSize;

      uInvWindowSize=vtkgl::GetUniformLocation(
        static_cast<GLuint>(this->ProgramShader),"invWindowSize");

      if(uInvWindowSize!=-1)
        {
        vtkgl::Uniform2f(uInvWindowSize,static_cast<GLfloat>(1.0/size[0]),
                         static_cast<GLfloat>(1.0/size[1]));
        }
      else
        {
        vtkErrorMacro(<<"invWindowSize is not a uniform variable.");
        }


      this->PrintError("after uniforms for textures");


      this->CheckFrameBufferStatus();

      GLint savedFrameBuffer;
      glGetIntegerv(vtkgl::FRAMEBUFFER_BINDING_EXT,&savedFrameBuffer);
      this->SavedFrameBuffer=static_cast<unsigned int>(savedFrameBuffer);

      vtkgl::BindFramebufferEXT(vtkgl::FRAMEBUFFER_EXT,
                                static_cast<GLuint>(this->FrameBufferObject));

      GLenum buffer[4];
      buffer[0] = vtkgl::COLOR_ATTACHMENT0_EXT;
      if(this->NumberOfCroppingRegions>1 &&
         this->BlendMode==vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND)
        {
        // max scalar frame buffer
        buffer[1] = vtkgl::COLOR_ATTACHMENT1_EXT;
        }
      else
        {
        buffer[1] = GL_NONE;
        }

      vtkgl::DrawBuffers(2,buffer);

      this->CheckFrameBufferStatus();

      // Use by the composite+shade program
      double shininess=vol->GetProperty()->GetSpecularPower();
      if(shininess>128.0)
        {
        shininess=128.0; // upper limit for the OpenGL shininess.
        }
      glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,static_cast<GLfloat>(shininess));

      glDisable(GL_COLOR_MATERIAL); // other mapper may have enable that.

      GLfloat values[4];
      values[3]=1.0;

      values[0]=0.0;
      values[1]=values[0];
      values[2]=values[0];
      glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,values);

      values[0]=static_cast<float>(vol->GetProperty()->GetAmbient());
      values[1]=values[0];
      values[2]=values[0];
      glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,values);

      values[0]=static_cast<float>(vol->GetProperty()->GetDiffuse());
      values[1]=values[0];
      values[2]=values[0];
      glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,values);
      values[0]=static_cast<float>(vol->GetProperty()->GetSpecular());
      values[1]=values[0];
      values[2]=values[0];
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,values);

    //  cout << "pingpong=" << this->PingPongFlag << endl;

      // To initialize the second color buffer
      vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT,
                                     vtkgl::COLOR_ATTACHMENT0_EXT,
                                     GL_TEXTURE_2D,
                                     this->TextureObjects[vtkOpenGLGPUVolumeRayCastMapperTextureObjectFrameBufferLeftFront],
                                     0);

      vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT,
                                     vtkgl::COLOR_ATTACHMENT0_EXT+1,
                                     GL_TEXTURE_2D,
                                     this->TextureObjects[vtkOpenGLGPUVolumeRayCastMapperTextureObjectFrameBufferLeftFront+1],
                                     0);
      buffer[0] = vtkgl::COLOR_ATTACHMENT0_EXT;
      buffer[1] = vtkgl::COLOR_ATTACHMENT1_EXT;
      vtkgl::DrawBuffers(2,buffer);

    //  cout << "check before setup" << endl;
      this->CheckFrameBufferStatus();
      this->SetupRender(ren,vol);

      // restore in case of composite with no cropping or streaming.
      buffer[0] = vtkgl::COLOR_ATTACHMENT0_EXT;
      buffer[1] = GL_NONE;
      vtkgl::DrawBuffers(2,buffer);
      vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT,
                                     vtkgl::COLOR_ATTACHMENT0_EXT+1,
                                     GL_TEXTURE_2D,0,0);
    //  cout << "check after color init" << endl;
      this->CheckFrameBufferStatus();

      if(this->NumberOfCroppingRegions>1 &&
         (this->BlendMode==vtkGPUVolumeRayCastMapper::MINIMUM_INTENSITY_BLEND
          || this->BlendMode==vtkGPUVolumeRayCastMapper::MAXIMUM_INTENSITY_BLEND))
        {
    //    cout << "this->MaxValueFrameBuffer="<< this->MaxValueFrameBuffer <<endl;
    //    cout << "this->MaxValueFrameBuffer2="<< this->MaxValueFrameBuffer2 <<endl;

        vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT,
                                       vtkgl::COLOR_ATTACHMENT0_EXT,
                                       GL_TEXTURE_2D,
                                       this->MaxValueFrameBuffer,0);

        vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT,
                                       vtkgl::COLOR_ATTACHMENT0_EXT+1,
                                       GL_TEXTURE_2D,
                                       this->MaxValueFrameBuffer2,0);

        buffer[0] = vtkgl::COLOR_ATTACHMENT0_EXT;
        buffer[1] = vtkgl::COLOR_ATTACHMENT1_EXT;
        vtkgl::DrawBuffers(2,buffer);

        if(this->BlendMode==vtkGPUVolumeRayCastMapper::MINIMUM_INTENSITY_BLEND)
          {
          glClearColor(1.0, 0.0, 0.0, 0.0);
          }
        else
          {
          glClearColor(0.0, 0.0, 0.0, 0.0); // for MAXIMUM_INTENSITY_BLEND
          }
    //    cout << "check before clear on max" << endl;
        this->CheckFrameBufferStatus();
        glClear(GL_COLOR_BUFFER_BIT);
        }

      if(this->NumberOfCroppingRegions>1)
        {
        // color buffer target in the color attachement 0
        vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT,
                                       vtkgl::COLOR_ATTACHMENT0_EXT,
                                       GL_TEXTURE_2D,
                                       this->TextureObjects[vtkOpenGLGPUVolumeRayCastMapperTextureObjectFrameBufferLeftFront],
                                       0);

        // color buffer input is on texture unit 4.
        vtkgl::ActiveTexture(vtkgl::TEXTURE4);
        glBindTexture(GL_TEXTURE_2D,this->TextureObjects[vtkOpenGLGPUVolumeRayCastMapperTextureObjectFrameBufferLeftFront+1]);

        if(this->BlendMode==vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND
           || this->BlendMode==vtkGPUVolumeRayCastMapper::MINIMUM_INTENSITY_BLEND)
          {
          // max buffer target in the color attachment 1
          vtkgl::FramebufferTexture2DEXT(vtkgl::FRAMEBUFFER_EXT,
                                         vtkgl::COLOR_ATTACHMENT0_EXT+1,
                                         GL_TEXTURE_2D,
                                         this->MaxValueFrameBuffer,0);

          // max buffer input is on texture unit 5.
          vtkgl::ActiveTexture(vtkgl::TEXTURE5);
          glBindTexture(GL_TEXTURE_2D,this->MaxValueFrameBuffer2);
          }
        vtkgl::ActiveTexture(vtkgl::TEXTURE0);
        }

      this->CheckFrameBufferStatus();

      if(this->OpacityTables!=0 &&
         this->OpacityTables->Vector.size()!=numberOfLevels)
        {
        delete this->OpacityTables;
        this->OpacityTables=0;
        }
      if(this->OpacityTables==0)
        {
        this->OpacityTables=new vtkOpacityTables(numberOfLevels);
        }

      // debug code
      // DO NOT REMOVE the following commented line
    //  this->ValidateProgram();

      glCullFace (GL_BACK);
      // otherwise, we are rendering back face to initialize the zbuffer.


      if(!this->GeneratingCanonicalView && this->ReportProgress)
        {
        // initialize the time to avoid a progress event at the beginning.
        this->LastProgressEventTime=vtkTimerLog::GetUniversalTime();
        }

      this->PrintError("PreRender end");*/
}


} // namespace udg
