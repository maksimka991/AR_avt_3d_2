#include "renderer/CAnimator.hpp"

namespace NApp
{

static const float ANIMATION_TICKS_PER_SECOND = 20.f;

/**
 * A little tree structure to match the scene's node structure,
 * but holding additional data.
 */
struct AnimationNode
{
   std::string Name;
   AnimationNode * Parent;
   std::vector<AnimationNode *> Children;
   aiMatrix4x4 LocalTransform;   // most recently calculated local transform
   aiMatrix4x4 GlobalTransform;  // same, but in world space
   unsigned int ChannelIndex;    // index in the current animation's channel array.

   AnimationNode()
      : Parent(0)
      , ChannelIndex(UINT_MAX)
   {
   }

   AnimationNode(const std::string & name)
      : Name(name)
      , Parent(0)
      , ChannelIndex(UINT_MAX)
   {
   }

   ~AnimationNode()
   {
      for (std::vector<AnimationNode *>::iterator it = Children.begin(); it != Children.end(); ++it)
      {
         delete *it;
      }
   }
};

CAnimator::CAnimator(const aiScene * scene, unsigned int animationIndex) {
   mScene = scene;
   mLastTime = 0.0;
   mRootNode = 0;
   mCurrentAnimationIndex = UINT_MAX;
   mCurrentAnimation = 0;
   mLastFramePosition = 0;
   mIdentityMatrix = aiMatrix4x4();

   //build the nodes-for-bones table
   for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
   {
      const aiMesh & currentMesh = *scene->mMeshes[i];
      for (unsigned int j = 0; j < currentMesh.mNumBones; ++j)
      {
         const aiBone & currentBone = *currentMesh.mBones[j];
         mMapBoneNodesByName[currentBone.mName.data] = mScene->mRootNode->FindNode(currentBone.mName);
      }
   }

  //changing the current animation also creates the node tree for this animation
  setAnimationIndex(animationIndex);
}

CAnimator::~CAnimator()
{
   if (mRootNode != 0) {
      delete mRootNode;
   }

   if (mLastFramePosition != 0)
   {
      delete [] mLastFramePosition;
   }
}

void CAnimator::setAnimationIndex(unsigned int animationIndex) {
   if (animationIndex == mCurrentAnimationIndex)
   {
      return;
   }

   //kill data of the previous anim
   if (mRootNode != 0)
   {
      delete mRootNode;
      mRootNode = 0;
   }

   if (mLastFramePosition != 0)
   {
      delete[] mLastFramePosition;
   }

   mMapNodesByName.clear();
   mCurrentAnimation = 0;

   mCurrentAnimationIndex = animationIndex;
   mCurrentAnimation = mScene->mAnimations[mCurrentAnimationIndex];

   //create the internal node tree. Do this even in case of invalid animation index
   //so that the transformation matrices are properly set up to mimic the current scene
   mRootNode = createNodeTree(mScene->mRootNode, 0);

   //invalid anim index
   if (animationIndex >= mScene->mNumAnimations)
   {
      mCurrentAnimationIndex = 0;
      mCurrentAnimation = mScene->mAnimations[mCurrentAnimationIndex];
   }

   mLastFramePosition = new glm::uvec3[mCurrentAnimation->mNumChannels];
}

AnimationNode * CAnimator::createNodeTree(aiNode * node, AnimationNode * parent)
{
   //create a node
   AnimationNode * internalNode = new AnimationNode(node->mName.data);
   internalNode->Parent = parent;
   mMapNodesByName[node] = internalNode;

   //copy its transformation
   internalNode->LocalTransform = node->mTransformation;
   calculateGlobalTransform(internalNode);

   //find the index of the animation track affecting this node, if any
   if (mCurrentAnimationIndex < mScene->mNumAnimations)
   {
      internalNode->ChannelIndex = -1;

      for (unsigned int a = 0; a < mCurrentAnimation->mNumChannels; a++)
      {
         if (mCurrentAnimation->mChannels[a]->mNodeName.data == internalNode->Name)
         {
            internalNode->ChannelIndex = a;
            break;
         }
      }
   }

   //continue for all child nodes and assign the created internal nodes as our children
   for (unsigned int a = 0; a < node->mNumChildren; a++)
   {
      AnimationNode * childNode = createNodeTree(node->mChildren[a], internalNode);
      internalNode->Children.push_back(childNode);
   }

   return internalNode;
}

const std::vector<aiMatrix4x4> & CAnimator::getBoneMatrices(
   const aiNode & node,
   unsigned int nodeMeshIndex)
{
   if (nodeMeshIndex < node.mNumMeshes)
   {
      unsigned int meshIndex = node.mMeshes[nodeMeshIndex];

      if ((mScene != 0) && (meshIndex < mScene->mNumMeshes))
      {
         const aiMesh & mesh = *mScene->mMeshes[meshIndex];

         for (unsigned int i = 0; i < mTransforms.size(); ++i)
         {
            mTransforms[i] = aiMatrix4x4();
         }

         //resize array and initialise it with identity matrices
         mTransforms.resize(mesh.mNumBones, aiMatrix4x4());

         //calculate the mesh's inverse global transform
         aiMatrix4x4 mGlobalInverseMeshTransform = getGlobalTransform(node);
         mGlobalInverseMeshTransform.Inverse();

         //Bone matrices transform from mesh coordinates in bind pose to mesh coordinates in skinned pose
         //Therefore the formula is offsetMatrix * currentGlobalTransform * inverseCurrentMeshTransform
         for (unsigned int i = 0; i < mesh.mNumBones; ++i)
         {
            const aiBone & bone = *mesh.mBones[i];
            const aiMatrix4x4 & mCurrentGlobalTransform = getGlobalTransform(*mMapBoneNodesByName[bone.mName.data]);
            mTransforms[i] = mGlobalInverseMeshTransform * mCurrentGlobalTransform * bone.mOffsetMatrix;
         }
      }
   }

   //and return the result
   return mTransforms;
}

void CAnimator::calculateGlobalTransform(AnimationNode * internalNode)
{
   //concatenate all parent transforms to get the global transform for this node
   internalNode->GlobalTransform = internalNode->LocalTransform;
   AnimationNode * node = internalNode->Parent;

    while (0 != node)
    {
      internalNode->GlobalTransform = node->LocalTransform * internalNode->GlobalTransform;
      node = node->Parent;
   }
}

const aiMatrix4x4& CAnimator::getGlobalTransform(const aiNode & node) const
{
   tNodeMap::const_iterator it = mMapNodesByName.find(&node);
   if (it == mMapNodesByName.end())
   {
      return mIdentityMatrix;
   }
   return it->second->GlobalTransform;
}

void CAnimator::updateAnimation(long elapsedTime, double ticksPerSecond)
{
   if ((mCurrentAnimation) && (mCurrentAnimation->mDuration > 0.0))
   {
      double time = ((double) elapsedTime) / 1000.0;

      //calculate current local transformations
      //extract ticks per second. Assume default value if not given
      double ticksPerSecondCorrected = ticksPerSecond != 0.0 ? ticksPerSecond : ANIMATION_TICKS_PER_SECOND;

      //map into anim's duration
      double timeInTicks = 0.0f;

      if (mCurrentAnimation->mDuration > 0.0) {
         timeInTicks = fmod(time * ticksPerSecondCorrected, mCurrentAnimation->mDuration);
      }

      if (mTransforms.size() != mCurrentAnimation->mNumChannels)
      {
         mTransforms.resize(mCurrentAnimation->mNumChannels);
      }

      //calculate the transformations for each animation channel
      for (unsigned int i = 0; i < mCurrentAnimation->mNumChannels; ++i)
      {
         const aiNodeAnim* channel = mCurrentAnimation->mChannels[i];

         //******** Position *****
         aiVector3D presentPosition(0.f, 0.f, 0.f);
         if (channel->mNumPositionKeys > 0)
         {
            //Look for present frame number. Search from last position if time is after the last time, else from beginning
            //Should be much quicker than always looking from start for the average use case.
            unsigned int frame = (timeInTicks >= mLastTime) ? mLastFramePosition[i].x : 0;

            while (frame < channel->mNumPositionKeys - 1)
            {
               if (timeInTicks < channel->mPositionKeys[frame + 1].mTime)
               {
                  break;
               }
               frame++;
            }

            //interpolate between this frame's value and next frame's value
            unsigned int nextFrame = (frame + 1) % channel->mNumPositionKeys;
            const aiVectorKey & key = channel->mPositionKeys[frame];
            const aiVectorKey & nextKey = channel->mPositionKeys[nextFrame];
            double timeDifference = nextKey.mTime - key.mTime;

            if (timeDifference < 0.0)
            {
               timeDifference += mCurrentAnimation->mDuration;
            }

            if (timeDifference > 0)
            {
               float interpolationFactor = (float) ((timeInTicks - key.mTime) / timeDifference);
               presentPosition = key.mValue + (nextKey.mValue - key.mValue) * interpolationFactor;
            }
            else
            {
               presentPosition = key.mValue;
            }

            mLastFramePosition[i].x = frame;
         }

         //******** Rotation *********
         aiQuaternion presentRotation(1, 0, 0, 0);
         if (channel->mNumRotationKeys > 0)
         {
            unsigned int frame = (timeInTicks >= mLastTime) ? mLastFramePosition[i].y : 0;

            while (frame < channel->mNumRotationKeys - 1)
            {
               if (timeInTicks < channel->mRotationKeys[frame + 1].mTime)
               {
                  break;
               }

               frame++;
            }

            //interpolate between this frame's value and next frame's value
            unsigned int nextFrame = (frame + 1) % channel->mNumRotationKeys;
            const aiQuatKey & key = channel->mRotationKeys[frame];
            const aiQuatKey & nextKey = channel->mRotationKeys[nextFrame];
            double timeDifference = nextKey.mTime - key.mTime;

            if (timeDifference < 0.0)
            {
               timeDifference += mCurrentAnimation->mDuration;
            }

            if (timeDifference > 0)
            {
               float interpolationFactor = (float)((timeInTicks - key.mTime) / timeDifference);
               aiQuaternion::Interpolate(presentRotation, key.mValue, nextKey.mValue, interpolationFactor);
            }
            else
            {
               presentRotation = key.mValue;
            }

            mLastFramePosition[i].y = frame;
         }

         //******** Scaling **********
         aiVector3D presentScaling(1.f, 1.f, 1.f);
         if (channel->mNumScalingKeys > 0)
         {
            unsigned int frame = (timeInTicks >= mLastTime) ? mLastFramePosition[i].z : 0;
            while (frame < channel->mNumScalingKeys - 1)
            {
               if (timeInTicks < channel->mScalingKeys[frame + 1].mTime)
               {
                  break;
               }

               frame++;
            }

            presentScaling = channel->mScalingKeys[frame].mValue;
            mLastFramePosition[i].z = frame;
         }

         //build a transformation matrix from it
         aiMatrix4x4 & mTransformation = mTransforms[i];
         mTransformation = aiMatrix4x4(presentRotation.GetMatrix());
         mTransformation.a1 *= presentScaling.x;
         mTransformation.b1 *= presentScaling.x;
         mTransformation.c1 *= presentScaling.x;
         mTransformation.a2 *= presentScaling.y;
         mTransformation.b2 *= presentScaling.y;
         mTransformation.c2 *= presentScaling.y;
         mTransformation.a3 *= presentScaling.z;
         mTransformation.b3 *= presentScaling.z;
         mTransformation.c3 *= presentScaling.z;
         mTransformation.a4 = presentPosition.x;
         mTransformation.b4 = presentPosition.y;
         mTransformation.c4 = presentPosition.z;
      }

      mLastTime = timeInTicks;

      //and update all node transformations with the results
      updateTransforms(mRootNode, mTransforms);
   }
}

void CAnimator::updateTransforms(AnimationNode * node, const std::vector<aiMatrix4x4> & transforms)
{
   //update node local transform
   if (node->ChannelIndex != UINT_MAX)
   {
      if (node->ChannelIndex >= transforms.size())
      {
         return;
      }
      node->LocalTransform = transforms[node->ChannelIndex];
   }

   //concatenate all parent transforms to get the global transform for this node
   node->GlobalTransform = node->LocalTransform;
   AnimationNode * parentNode = node->Parent;
   while (0 != parentNode)
   {
      node->GlobalTransform = parentNode->LocalTransform * node->GlobalTransform;
      parentNode = parentNode->Parent;
   }

   //continue for all children
   std::vector<AnimationNode *>::iterator it;
   for (it = node->Children.begin(); it != node->Children.end(); ++it)
   {
      updateTransforms(*it, transforms);
   }
}

} /* namespace NApp */
