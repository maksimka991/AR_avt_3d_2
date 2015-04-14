#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>

namespace NApp
{

struct AnimationNode;

/**
 * Calculates the animated node transformations for a given scene and timestamp.
 *
 *  Create an instance for a aiScene you want to animate and set the current animation
 *  to play. You can then have the instance calculate the current pose for all nodes
 *  by calling Calculate() for a given timestamp. After this you can retrieve the
 *  present transformation for a given node by calling GetLocalTransform() or
 *  GetGlobalTransform().
 */
class CAnimator
{
public:
   /** Constructor for a given scene.
    *
    * The object keeps a reference to the scene during its lifetime, but
    * ownership stays at the caller.
    * @param scene The scene to animate.
    * @param animIndex Index of the animation to play.
    */
   CAnimator(const aiScene * scene, unsigned int animIndex);
   ~CAnimator();

   /** Sets the animation to use for playback. This also recreates the internal
    * mapping structures, which might take a few cycles.
    * @param animIndex Index of the animation in the scene's animation array
    */
   void setAnimationIndex(unsigned int animIndex);

   /** Calculates the node transformations for the scene. Call this to get
    * up-to-date results before calling one of the getters.
    * Evaluates the animation tracks for a given time stamp. The calculated pose can be retrieved as a
    * array of transformation matrices afterwards by calling GetTransformations().
    * @param elapsedTime Elapsed time since animation start in ms.
    */
   void updateAnimation(long elapsedTime, double ticksPerSecond);

   /** Calculates the bone matrices for the given mesh.
    *
    * Each bone matrix transforms from mesh space in bind pose to mesh space in
    * skinned pose, it does not contain the mesh's world matrix. Thus the usual
    * matrix chain for using in the vertex shader is
    * @code
    * projMatrix * viewMatrix * worldMatrix * boneMatrix
    * @endcode
    * @param node The node carrying the mesh.
    * @param meshIndex Index of the mesh in the node's mesh array. The NODE's
    *   mesh array, not  the scene's mesh array! Leave out to use the first mesh
    *   of the node, which is usually also the only one.
    * @return A reference to a vector of bone matrices. Stays stable till the
    *   next call to GetBoneMatrices();
    */
   const std::vector<aiMatrix4x4> & getBoneMatrices(const aiNode & node, unsigned int meshIndex = 0);

   /** Retrieves the most recent global transformation matrix for the given node.
    *
    * The returned matrix is in world space, which is the same coordinate space
    * as the transformation of the scene's root node. If the node is not animated,
    * the node's original transformation is returned so that you can safely use or
    * assign it to the node itsself. If there is no node with the given name, the
    * identity matrix is returned. All transformations are updated whenever
    * Calculate() is called.
    * @param pNodeName Name of the node
    * @return A reference to the node's most recently calculated global
    *   transformation matrix.
    */
   const aiMatrix4x4 & getGlobalTransform(const aiNode & node) const;

private:
   /** Recursively creates an internal node structure matching the current scene and animation. */
   AnimationNode * createNodeTree(aiNode * node, AnimationNode * parent);

   /** Calculates the global transformation matrix for the given internal node */
   void calculateGlobalTransform(AnimationNode * internalNode);

   /** Recursively updates the internal node transformations from the given matrix array */
   void updateTransforms(AnimationNode* node, const std::vector<aiMatrix4x4> & transforms);

   /** Name to node map to quickly find nodes by their name */
   typedef std::map<const aiNode *, AnimationNode *> tNodeMap;
   tNodeMap mMapNodesByName;

   /** Name to node map to quickly find nodes for given bones by their name */
   typedef std::map<const char *, const aiNode *> tBoneMap;
   tBoneMap mMapBoneNodesByName;

   /** At which frame the last evaluation happened for each channel.
    * Useful to quickly find the corresponding frame for slightly increased time stamps
    */
   double mLastTime;
   glm::uvec3 * mLastFramePosition;

   /** Array to return transformations results inside. */
   std::vector<aiMatrix4x4> mTransforms;

   /** Identity matrix to return a reference to in case of error */
   aiMatrix4x4 mIdentityMatrix;

   const aiScene * mScene;
   AnimationNode * mRootNode;
   unsigned int mCurrentAnimationIndex;
   const aiAnimation * mCurrentAnimation;
};

} /* namespace NApp */
