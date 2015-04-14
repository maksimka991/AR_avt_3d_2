#include <iostream>
#include <glm/gtx/transform.hpp>
#include <SDL.h>
#include <cv.h>
#include <highgui.h>
#include "renderer/CAnimator.hpp"
#include "renderer/CCamera.hpp"
#include "renderer/CLight.hpp"
#include "renderer/CModel.hpp"
#include "renderer/CShader.hpp"
#include "renderer/CUtils.hpp"
#include "loader/TGALoader.hpp"


namespace NApp
{

static const unsigned int MAXBONESPERMESH = 60u;

std::shared_ptr<CModel> CModel::load(const std::string & path)
{
   std::shared_ptr<CModel> model;
   try
   {
      std::shared_ptr<CModel> tmp(new CModel(path));
      model = tmp;
   }
   catch (std::exception & e)
   {
      std::cerr << e.what() << std::endl;
      model.reset();
   }
   return model;
}

CModel::CModel(const std::string & path)
   : mModelMatrix(1.f)
   , mSceneMin(1e10f, 1e10f, 1e10f)
   , mSceneMax(-1e10f, -1e10f, -1e10f)
   , mSceneCenter(0.f, 0.f, 0.f)
   , mScale(1.0f)
   , mUserScale(1.0f)
   , mUserRotate(0.f, 0.f, 0.f)
   , mUserTranslate(0.f, 0.f, 0.f)

{
   if (false == loadScene(path))
   {
      throw std::runtime_error("Can't load file '" + path + "'.");
   }
}

CModel::~CModel()
{
   for (unsigned int i = 0; i < mTextures.size(); ++i)
   {
      if (mTextures[i] != -1)
      {
         glDeleteTextures(1, &mTextures[i]);
      }
   }
}

bool CModel::loadScene(const std::string & path)
{
   //import the model via Assimp
   mStore = aiCreatePropertyStore();
   aiSetImportPropertyInteger(mStore, AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
   aiSetImportPropertyFloat  (mStore, AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
   aiSetImportPropertyInteger(mStore, AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE |
                                                               aiPrimitiveType_POINT);

   mScene = aiImportFileExWithProperties(
      path.c_str(),
      aiProcessPreset_TargetRealtime_Quality |
      aiProcess_FindInstances |
      aiProcess_ValidateDataStructure |
      aiProcess_OptimizeMeshes,
      0,
      mStore);

   if (0 == mScene)
   {
      std::cerr << "Loading model failed." << std::endl;
      return false;
   }
    
   calculateBBox(*mScene->mRootNode, mSceneMin, mSceneMax);
   calculateCenter(mSceneMin, mSceneMax, mSceneCenter);

   //load the textures into the vram
   mTextures.resize(mScene->mNumMaterials, 0);
   for (unsigned int i = 0; i < mScene->mNumMaterials; ++i)
   {
      const aiMaterial & currentMaterial = *mScene->mMaterials[i];
      unsigned int & texture = mTextures[i];

      aiString texturePath;
      aiGetMaterialTexture(
         &currentMaterial,
         aiTextureType_DIFFUSE,
         0,
         &texturePath,
         0, 0, 0, 0, 0, 0);

      cv::Mat image;
      image = cv::imread(
         CUtils::getFullPath(path, std::string(texturePath.data)).c_str(),
         CV_LOAD_IMAGE_COLOR);

      if (false == image.empty())  // Check for invalid input
      {

         IplImage opencvimg2 = (IplImage)image;
         IplImage* opencvimg = &opencvimg2;

         //Convert to SDL_Surface
         SDL_Surface * tex = SDL_CreateRGBSurfaceFrom(
            (void*)opencvimg->imageData,
            opencvimg->width, opencvimg->height,
            opencvimg->depth * opencvimg->nChannels,
            opencvimg->widthStep,
            0x0000ff, 0x00ff00, 0xff0000, 0);

         if (0 != tex)
         {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            gluBuild2DMipmaps(
               GL_TEXTURE_2D,
               3,
               tex->w, tex->h,
               GL_BGR, GL_UNSIGNED_BYTE,
               tex->pixels);
         }
         SDL_FreeSurface(tex);
      }
      else
      {
         Texture TGAtextur;
         TGALoader loader;
         std::string str = CUtils::getFullPath(path, std::string(texturePath.data));

         if (loader.LoadTGA(&TGAtextur, str.c_str()))
         {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            gluBuild2DMipmaps(
               GL_TEXTURE_2D,
               3,
               TGAtextur.width, TGAtextur.height,
               TGAtextur.type,
               GL_UNSIGNED_BYTE,
               TGAtextur.imageData);
          }
          else
          {
             SDL_Surface * tex = SDL_LoadBMP("data/mash.bmp");
			
             if (0 != tex)
             {
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                gluBuild2DMipmaps(
                   GL_TEXTURE_2D,
                   3,
                   tex->w, tex->h,
                   GL_BGR, GL_UNSIGNED_BYTE,
                   tex->pixels);
             }
             SDL_FreeSurface(tex);
          }
       }
    }

   //load the meshes into the vram
   mMeshes.resize(mScene->mNumMeshes);
   for (unsigned int i = 0; i < mScene->mNumMeshes; ++i)
   {
      const aiMesh & currentMesh = *mScene->mMeshes[i];
      Mesh & mesh = mMeshes[i];

       mesh.mNumFaces = currentMesh.mNumFaces;
       mesh.mNumVertices = currentMesh.mNumVertices;
       mesh.mNumBones = currentMesh.mNumBones;
       mesh.mMaterialIndex = currentMesh.mMaterialIndex;

       if (mesh.mNumFaces == 0 || mesh.mNumVertices == 0)
       {
          continue;
       }
       if (!currentMesh.HasPositions())
       {
          std::cerr << "A mesh of the model has no vertices and is not loaded." << std::endl;
          continue;
       }
       if (!currentMesh.HasNormals())
       {
          std::cerr << "A mesh of the model has no normals and is not loaded." << std::endl;
          continue;
       }
       if (!currentMesh.HasFaces())
       {
          std::cerr << "A mesh of the model has no polygon faces and is not loaded." << std::endl;
          continue;
       }

       mesh.mVertices.resize(mesh.mNumVertices);
       mesh.mNormals.resize(mesh.mNumVertices);
       mesh.mColors.resize(mesh.mNumVertices);
       mesh.mTexCoords.resize(mesh.mNumVertices);
       mesh.mIndices.resize(mesh.mNumFaces * 3);
       mesh.mBoneIndices.resize(mesh.mNumVertices);
       mesh.mWeights.resize(mesh.mNumVertices);

       for (int j = 0; j < mesh.mNumVertices; ++j)
       {
         mesh.mVertices[j].x = currentMesh.mVertices[j].x;
         mesh.mVertices[j].y = currentMesh.mVertices[j].y;
         mesh.mVertices[j].z = currentMesh.mVertices[j].z;
         mesh.mVertices[j].w = 1.0f;

         mesh.mNormals[j].x = currentMesh.mNormals[j].x;
         mesh.mNormals[j].y = currentMesh.mNormals[j].y;
         mesh.mNormals[j].z = currentMesh.mNormals[j].z;

         if ( false == currentMesh.HasVertexColors(0)
           || 0 == currentMesh.mColors[j])
         {
            if ( currentMesh.mMaterialIndex < mScene->mNumMaterials
              && 0 != mScene->mMaterials[currentMesh.mMaterialIndex])
            {
               const aiMaterial & currentMaterial = *mScene->mMaterials[currentMesh.mMaterialIndex];

               aiColor4D color;
               aiGetMaterialColor(&currentMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);

               mesh.mColors[j].r = color.r;
               mesh.mColors[j].g = color.g;
               mesh.mColors[j].b = color.b;
               mesh.mColors[j].a = 1.0f;
            }
            else
            {
               mesh.mColors[j] = glm::vec4(1.f, 0.f, 0.f, 1.0f);
            }
         }
         else
         {
            mesh.mColors[j].r = currentMesh.mColors[j]->r;
            mesh.mColors[j].g = currentMesh.mColors[j]->g;
            mesh.mColors[j].b = currentMesh.mColors[j]->b;
            mesh.mColors[j].a = currentMesh.mColors[j]->a;
         }

         if ( 0 != currentMesh.mTextureCoords
           && 0 != currentMesh.mTextureCoords[0])
         {
            mesh.mTexCoords[j].x = currentMesh.mTextureCoords[0][j].x;
            mesh.mTexCoords[j].y = 1.0f - currentMesh.mTextureCoords[0][j].y;
         }
         else
         {
            mesh.mTexCoords[j] = glm::vec2(0.f, 0.f);
         }
      }

      for (int j = 0; j < mesh.mNumFaces; ++j)
      {
         mesh.mIndices[j * 3] = currentMesh.mFaces[j].mIndices[0];
         mesh.mIndices[j * 3 + 1] = currentMesh.mFaces[j].mIndices[1];
         mesh.mIndices[j * 3 + 2] = currentMesh.mFaces[j].mIndices[2];
      }

      //read bone indices and weights for bone animation
      std::vector<std::vector<aiVertexWeight> > vTempWeightsPerVertex;
      vTempWeightsPerVertex.resize(currentMesh.mNumVertices);

      for (unsigned int j = 0; j < currentMesh.mNumBones; ++j)
      {
         const aiBone & bone = *currentMesh.mBones[j];
         for (unsigned int b = 0; b < bone.mNumWeights; ++b)
         {
            std::vector<aiVertexWeight> & weight = vTempWeightsPerVertex[bone.mWeights[b].mVertexId];
            weight.push_back(aiVertexWeight(j, bone.mWeights[b].mWeight));
         }
      }

      for (int j = 0; j < mesh.mNumVertices; ++j)
      {
         mesh.mBoneIndices[j] = glm::uvec4(0, 0, 0, 0);
         mesh.mWeights[j] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

         if (true == currentMesh.HasBones())
         {
            if (4 < vTempWeightsPerVertex[j].size())
            {
               std::cerr << "The model has invalid bone weights and is not loaded." << std::endl;
               return false;
            }

            for (unsigned int k = 0; k < vTempWeightsPerVertex[j].size(); ++k)
            {
               mesh.mBoneIndices[j][k] = (GLfloat) vTempWeightsPerVertex[j][k].mVertexId;
               mesh.mWeights[j][k] = (GLfloat) vTempWeightsPerVertex[j][k].mWeight;
            }
         }
      }

      if (true == mScene->HasAnimations())
      {
         mAnimator = std::make_shared<CAnimator>(mScene, 0);
      }
   }

   mScale = calculateScale();
   return true;
}

float CModel::calculateScale()
{
   float xDistance = sqrt(mSceneMax.x * mSceneMax.x + mSceneMin.x * mSceneMin.x);
   float yDistance = sqrt(mSceneMax.y * mSceneMax.y + mSceneMin.y * mSceneMin.y);
   float zDistance = sqrt(mSceneMax.z * mSceneMax.z + mSceneMin.z * mSceneMin.z);
   float scale = sqrt(xDistance * xDistance + yDistance * yDistance + zDistance * zDistance);

   return 1.5 / scale;
}

void CModel::calculateBBox(
   const aiNode & node,
   aiVector3D & min,
   aiVector3D & max)
{
   for (unsigned int i = 0; i < node.mNumMeshes; ++i)
   {
      const aiMesh & mesh = *mScene->mMeshes[node.mMeshes[i]];

      for (unsigned int j = 0; j < mesh.mNumVertices; ++j)
      {
         const aiVector3D & tmp = mesh.mVertices[j];

         if (min.x > tmp.x) min.x = tmp.x;
         if (min.y > tmp.y) min.y = tmp.y;
         if (min.z > tmp.z) min.z = tmp.z;

         if (max.x < tmp.x) max.x = tmp.x;
         if (max.y < tmp.y) max.y = tmp.y;
         if (max.z < tmp.z) max.z = tmp.z;
      }
   }

   for (unsigned int i = 0; i < node.mNumChildren; ++i)
   {
      calculateBBox(*node.mChildren[i], min, max);
   }
}

void CModel::calculateCenter(
   const aiVector3D & min,
   const aiVector3D & max,
   aiVector3D & center)
{
   center.x = (min.x + max.x) / 2.f;
   //center.y = (min.y + max.y) / 2.f;
   center.y = (min.y);
   center.z = (min.z + max.z) / 2.f;
}

void CModel::render(CShader & shader, unsigned int ellapsedTime)
{
   //set the bone animation to the specified timestamp
   if (0 != mAnimator)
   {
      mAnimator->updateAnimation(ellapsedTime, 20.);
   }

   mModelMatrix = glm::mat4(1.0f);
   mModelMatrix = glm::rotate(mModelMatrix,
                                    90.0f + mUserRotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
   mModelMatrix = glm::rotate(mModelMatrix, mUserRotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
   mModelMatrix = glm::rotate(mModelMatrix, mUserRotate.z, glm::vec3(0.0f, 0.0f, 1.0f));

   mModelMatrix = glm::scale(mModelMatrix, glm::vec3(mScale * mUserScale));

 
   

   mModelMatrix = glm::translate(mModelMatrix, glm::vec3(mUserTranslate.x,
                                                         mUserTranslate.y - mSceneCenter.y,
                                                         mUserTranslate.z));
  
   shader.setUniform("modelMatrix", mModelMatrix);

   if (mScene->mRootNode != 0)
   {
     renderNode(shader, *mScene->mRootNode);
   }
}

void CModel::renderNode(CShader & shader, const aiNode & node)
{
   for (unsigned int i = 0; i < node.mNumMeshes; ++i)
   {
      const aiMesh & currentMesh = *mScene->mMeshes[node.mMeshes[i]];
      std::vector<glm::mat4> matrices;
      matrices.resize(MAXBONESPERMESH);

      //upload bone matrices
      if ((currentMesh.HasBones()) && (0 != mAnimator))
      {
         const std::vector<aiMatrix4x4> & boneMatrices = mAnimator->getBoneMatrices(node, i);

         if (boneMatrices.size() != currentMesh.mNumBones)
         {
            continue;
         }

         for (unsigned int j = 0; j < currentMesh.mNumBones; ++j)
         {
            if (j < MAXBONESPERMESH)
            {
               matrices[j][0][0] = boneMatrices[j].a1;
               matrices[j][0][1] = boneMatrices[j].b1;
               matrices[j][0][2] = boneMatrices[j].c1;
               matrices[j][0][3] = boneMatrices[j].d1;
               matrices[j][1][0] = boneMatrices[j].a2;
               matrices[j][1][1] = boneMatrices[j].b2;
               matrices[j][1][2] = boneMatrices[j].c2;
               matrices[j][1][3] = boneMatrices[j].d2;
               matrices[j][2][0] = boneMatrices[j].a3;
               matrices[j][2][1] = boneMatrices[j].b3;
               matrices[j][2][2] = boneMatrices[j].c3;
               matrices[j][2][3] = boneMatrices[j].d3;
               matrices[j][3][0] = boneMatrices[j].a4;
               matrices[j][3][1] = boneMatrices[j].b4;
               matrices[j][3][2] = boneMatrices[j].c4;
               matrices[j][3][3] = boneMatrices[j].d4;
            }
         }
      }

      //upload the complete bone matrices to the shaders
      shader.setUniform("boneMatrices", matrices);

      drawMesh(shader, node.mMeshes[i]);
   }

   //render all child nodes
   for (unsigned int i = 0; i < node.mNumChildren; ++i)
   {
      renderNode(shader, *node.mChildren[i]);
   }
}

void CModel::drawMesh(CShader & shader, unsigned int index)
{
   const Mesh & mesh = mMeshes[index];

   shader.enableAttributeArray("inPosition");
   shader.setAttributeArray("inPosition", (const float *)mesh.mVertices.data(), 4);

   shader.enableAttributeArray("inNormal");
   shader.setAttributeArray("inNormal", (const float *)mesh.mNormals.data(), 3);

   shader.enableAttributeArray("inBoneWeights");
   shader.setAttributeArray("inBoneWeights", (const float *)mesh.mWeights.data(), 4);

   shader.enableAttributeArray("inBoneIndices");
   shader.setAttributeArray("inBoneIndices", (const float *)mesh.mBoneIndices.data(), 4);

   shader.enableAttributeArray("inTexCoord");
   shader.setAttributeArray("inTexCoord", (const float *)mesh.mTexCoords.data(), 2);

   if (mTextures[mesh.mMaterialIndex] != -1)
   {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mTextures[mesh.mMaterialIndex]);
      shader.setUniform("texture", 0);
      shader.setUniform("useTexture", 1);
   }
   else
   {
      shader.enableAttributeArray("inColor");
      shader.setAttributeArray("inColor", (const float *)mesh.mColors.data(), 4);
      shader.setUniform("useTexture", 0);
   }

   glDrawElements(GL_TRIANGLES, mesh.mNumFaces * 3, GL_UNSIGNED_INT, mesh.mIndices.data());

   shader.disableAttributeArray("inPosition");
   shader.disableAttributeArray("inNormal");
   shader.disableAttributeArray("inBoneWeights");
   shader.disableAttributeArray("inBoneIndices");
   shader.disableAttributeArray("inTexCoord");
}

} /* namespace NApp */
