#include <glm/gtc/type_ptr.hpp>
#include "detector/CDetector.hpp"

namespace NApp
{

CDetector::CDetector()
{
}

bool CDetector::initialize()
{
   return true;
}

std::shared_ptr<CMarkersData> CDetector::detect(const CFrame & frame)
{
   std::vector<Marker> markers;

   cv::Mat img = frame.getMat();
   //cv::flip(img, img, 1); // 0 - around x

   IplImage iplImg = img;
   mMarkerDetector.Detect(&iplImg, &mCamera, true, false);

   for (size_t i = 0; i < mMarkerDetector.markers->size(); ++i)
   {
      alvar::MarkerData & marker = (*mMarkerDetector.markers)[i];

      unsigned int id = marker.GetId();
      alvar::Pose & pose = marker.pose;

      double tmpQuat[4];
      CvMat quat = cvMat(4, 1, CV_64F, tmpQuat);
      pose.GetQuaternion(&quat);

      double tmpTrans[3];
      CvMat trans = cvMat(3, 1, CV_64F, tmpTrans);
      pose.GetTranslation(&trans);

      double tmpView[16] = { 0 };
      pose.GetMatrixGL(tmpView);
      float tmpViewF[16] = {
         tmpView[0],  tmpView[1],  tmpView[2],  tmpView[3],
         tmpView[4],  tmpView[5],  tmpView[6],  tmpView[7],
         tmpView[8],  tmpView[9],  tmpView[10], tmpView[11],
         tmpView[12], tmpView[13], tmpView[14], tmpView[15],
      };
      glm::mat4 view = glm::make_mat4(tmpViewF);

      markers.push_back(Marker(
         id,
         view,
         glm::quat(
            (float)cvmGet(&quat, 0, 0),
            (float)cvmGet(&quat, 1, 0),
            (float)cvmGet(&quat, 2, 0),
            (float)cvmGet(&quat, 3, 0)),
         glm::vec3(
            (float)cvmGet(&trans, 0, 0),
            (float)cvmGet(&trans, 1, 0),
            (float)cvmGet(&trans, 2, 0))
         ));
   }

   return std::shared_ptr<CMarkersData>(new CMarkersData(markers));
}

} /* namespace NApp */
