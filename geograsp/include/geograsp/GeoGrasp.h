#ifndef GEOGRASP_H_
#define GEOGRASP_H_

#include <iostream>
#include <math.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/common/centroid.h>
#include <pcl/common/geometry.h>

#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/extract_indices.h>

#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/visualization/point_cloud_color_handlers.h>

#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_line.h>
#include <pcl/sample_consensus/sac_model_plane.h>

#include <pcl/segmentation/sac_segmentation.h>

#include <pcl/features/normal_3d.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/features/moment_of_inertia_estimation.h>


struct GraspConfiguration {
  pcl::PointNormal firstPoint;
  pcl::PointNormal secondPoint;
};

class GeoGrasp {
  public:
    GeoGrasp();
    ~GeoGrasp();

    void setBackgroundCloud(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr &cloud);
    void setObjectCloud(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr &cloud);
    void setGrasps(const int & grasps);
    void setGripTipSize(const int & size);

    GraspConfiguration getGrasp(const int & index) const;
    GraspConfiguration getBestGrasp() const;
    
    pcl::ModelCoefficients getObjectAxisCoeff() const;
    
    pcl::PointCloud<pcl::PointNormal> getFirstPointRadiusNormalCloud() const;
    pcl::PointCloud<pcl::PointNormal> getSecondPointRadiusNormalCloud() const;

    float getRanking(const int & index) const;
    float getBestRanking() const;

    void compute();

  private:
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr backgroundCloud;
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr objectCloud;
    pcl::PointCloud<pcl::PointNormal>::Ptr objectNormalCloud;
    pcl::PointCloud<pcl::PointNormal>::Ptr graspPlaneCloud;

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr firstPointRadiusCloud;
    pcl::PointCloud<pcl::PointNormal>::Ptr firstPointRadiusNormalCloud;
    pcl::PointCloud<pcl::PointNormal>::Ptr firstNormalCloudVoxel;

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr secondPointRadiusCloud;
    pcl::PointCloud<pcl::PointNormal>::Ptr secondPointRadiusNormalCloud;
    pcl::PointCloud<pcl::PointNormal>::Ptr secondNormalCloudVoxel;

    pcl::ModelCoefficients::Ptr backgroundPlaneCoeff;
    pcl::ModelCoefficients::Ptr objectAxisCoeff;
    pcl::ModelCoefficients::Ptr graspPlaneCoeff;

    pcl::PointXYZ objectCentroidPoint;
    pcl::PointNormal firstGraspPoint;
    pcl::PointNormal secondGraspPoint;

    std::vector<GraspConfiguration> graspPoints;
    std::vector<float> rankings;
    std::vector<float> pointsDistance;

    int numberBestGrasps;
    int gripTipSize; //mm

    // Auxiliary functions

    void computeCloudPlane(const pcl::PointCloud<pcl::PointXYZRGB>::Ptr & cloud,
      pcl::ModelCoefficients::Ptr backPlaneCoeff);

    void filterOutliersFromCloud(
      const pcl::PointCloud<pcl::PointXYZRGB>::Ptr & inputCloud,
      const int & meanNeighbours, const float & distanceThreshold,
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr outputCloud);

    void computeCloudNormals(
      const pcl::PointCloud<pcl::PointXYZRGB>::Ptr & inputCloud,
      const float & searchRadius,
      pcl::PointCloud<pcl::PointNormal>::Ptr cloudNormals);

    void computeCloudGeometry(
      const pcl::PointCloud<pcl::PointXYZRGB>::Ptr & inputCloud,
      pcl::ModelCoefficients::Ptr objAxisCoeff, pcl::PointXYZ & objCenterMass);

    template<typename T, typename U>
    void extractInliersCloud(const T & inputCloud,
      const pcl::PointIndices::Ptr & inputCloudInliers, T outputCloud);

    void buildGraspingPlane(const pcl::PointXYZ & planePoint,
      const Eigen::Vector3f & planeNormalVector, const float & distanceThreshold,
      const pcl::PointCloud<pcl::PointNormal>::Ptr & inputCloud,
      pcl::ModelCoefficients::Ptr graspPlaneCoeff,
      pcl::PointCloud<pcl::PointNormal>::Ptr graspPlaneCloud);

    void getClosestPointsByRadius(const pcl::PointNormal & point,
      const float & radius,
      const pcl::PointCloud<pcl::PointXYZRGB>::Ptr & inputCloud,
      const pcl::PointCloud<pcl::PointNormal>::Ptr & inputNormalCloud,
      pcl::PointCloud<pcl::PointXYZRGB>::Ptr pointsCloud,
      pcl::PointCloud<pcl::PointNormal>::Ptr normalCloud);

    template<typename T, typename U>
    void voxelizeCloud(const T & inputCloud, const float & leafSize,
      T outputCloud);

    void getBestGraspingPoints(
      const pcl::PointNormal & firstInitialPoint,
      const pcl::PointNormal & secondInitialPoint,
      const pcl::ModelCoefficients::Ptr & graspPlaneCoeff,
      const pcl::PointXYZ & centroidPoint,
      const pcl::PointCloud<pcl::PointNormal>::Ptr & firstNormalCloud,
      const pcl::PointCloud<pcl::PointNormal>::Ptr & secondNormalCloud,
      const int & numGrasps, std::vector<GraspConfiguration> & bestGrasps,
      std::vector<float> & bestRanks);
};

#endif