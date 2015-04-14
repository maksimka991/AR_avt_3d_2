#pragma once

#include <assimp/cimport.h>
#include <assimp/config.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector> 


namespace NApp
{

class CShader;
class CAnimator;
class TGALoader;

struct Mesh
{
   std::vector<glm::vec4> mVertices;
   std::vector<glm::vec3> mNormals;
   std::vector<glm::vec4> mColors;
   std::vector<glm::vec2> mTexCoords;
   std::vector<glm::vec4> mWeights;
   std::vector<glm::vec4> mBoneIndices;
   std::vector<int> mIndices;
   int mMaterialIndex;
   int mNumFaces;
   int mNumVertices;
   int mNumBones;
   unsigned int mTexture;
};

class CModel
{
public:
   static std::shared_ptr<CModel> load(const std::string & path);

public:
   ~CModel();

   void render(CShader & shader, unsigned int ellapsedTime);

   void rotate(const glm::vec3 & value);
   void scale(const glm::vec3 & value);
   void translate(const glm::vec3 & value);

private:
   explicit CModel(const std::string & path);

   bool loadScene(const std::string & path);

   void drawMesh(CShader & shader, unsigned int index);
   void renderNode(CShader & shader, const aiNode & node);

   float calculateScale();
   void calculateBBox(const aiNode & node, aiVector3D & min, aiVector3D & max);
   void calculateCenter(const aiVector3D & min, const aiVector3D & max, aiVector3D & center);

private:

   const aiScene * mScene;
   aiPropertyStore * mStore;

   aiVector3D mSceneMin;
   aiVector3D mSceneMax;
   aiVector3D mSceneCenter;

   float mScale;
   glm::vec3 mUserScale;
   glm::vec3 mUserRotate;
   glm::vec3 mUserTranslate;

   std::vector<Mesh> mMeshes;
   std::vector<unsigned int> mTextures;
  
   std::shared_ptr<CAnimator> mAnimator;

   glm::mat4 mModelMatrix;
};

inline
void CModel::rotate(const glm::vec3 & value)
{  
   mUserRotate = value;
}

inline
void CModel::scale(const glm::vec3 & value)
{
	mUserScale = value;
    
}

inline
void CModel::translate(const glm::vec3 & value)
{
   mUserTranslate = value;
}

} /* namespace NApp */
