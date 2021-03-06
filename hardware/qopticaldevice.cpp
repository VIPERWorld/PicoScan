#include "qopticaldevice.h"

#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>

QOpticalDevice::QOpticalDevice(QObject *parent) :
    QObject(parent),
    position(),
    resolution(800,600),
    cellSize(8e-6)
{
    orientation = cv::Mat::eye(3,3,CV_64F);
    intrinsicMatrix = cv::Mat::eye(3,3,CV_64F);
    distortion = cv::Mat::zeros(1,8,CV_64F);
}

void QOpticalDevice::setDistortion(cv::Mat k)
{
    distortion = k;
}

void QOpticalDevice::setIntrinsics(cv::Mat m)
{
    intrinsicMatrix = m;
}

void QOpticalDevice::setOrientation(cv::Mat o)
{
    orientation = o;
}

void QOpticalDevice::setOrientation(cv::Vec3d r)
{
    cv::Rodrigues(r,orientation);
}

void QOpticalDevice::setPosition(cv::Vec3d p)
{
    position = p;
}

void QOpticalDevice::setResolution(int u, int v)
{
    resolution = cv::Size(u,v);
    emit resolutionChanged(u,v);
}

void QOpticalDevice::setResolution(cv::Size s)
{
    this->resolution = s;
    emit resolutionChanged(s.width,
                           s.height);
}

cv::Mat QOpticalDevice::getDistortion()
{
    return distortion;
}

cv::Mat QOpticalDevice::getIntrinsics()
{
    return intrinsicMatrix;
}

double QOpticalDevice::getNormalizedU(double u)
{
    return getNormalizedCoord(u,getPrincipalU());
}

double QOpticalDevice::getNormalizedV(double v)
{
    return getNormalizedCoord(v,getPrincipalV());
}

cv::Mat QOpticalDevice::getOrientation()
{
    return orientation;
}

cv::Mat QOpticalDevice::getPixelRay(double u, double v)
{
    // normalized U and V coordinates
    double uNorm, vNorm;

    // first we compute the ray in camera coordinates
    cv::Mat camBased = getFwdVector();
    cv::Mat ray;

    // get the normalized coordinates
    uNorm = getNormalizedU(u);
    vNorm = getNormalizedV(v);

    // find the ray in camera coords
    camBased.at<double>(0) = uNorm;
    camBased.at<double>(1) = vNorm;

    // translate it based on the device's rotation
    // cv::normalize(orientation * camBased, ray);
    ray = orientation * camBased;

    return ray;

}

cv::Mat QOpticalDevice::getUpVector()
{
    // first we define what "up" is
    cv::Vec3d upVec(0.0,1.0,0.0);
    cv::Mat up;
    up = cv::Mat(upVec).reshape(1);

//    std::cout << "Up: " << up << "\n";

    // then we rotate it to match this device
    return orientation * up;
}

cv::Mat QOpticalDevice::getFwdVector()
{
    // first we define what "forward" is
    cv::Vec3d fwdVec(0.0,0.0,-1.0);
    cv::Mat fwd;
    fwd = cv::Mat(fwdVec).reshape(1);


    // then we rotate it to match this device
    fwd = orientation * fwd;
//    std::cout << "Forward: " << fwd << "\n";
    return fwd;
}

cv::Vec3d QOpticalDevice::getPosition()
{
    return position;
}

int QOpticalDevice::getResolutionU()
{
    return resolution.width;
}

int QOpticalDevice::getResolutionV()
{
    return resolution.height;
}

cv::Size QOpticalDevice::getResolution()
{
    return resolution;
}

double QOpticalDevice::getFocalLength()
{
    return intrinsicMatrix.at<double>(0,0);
}

void QOpticalDevice::setFocalLength(double f)
{
    intrinsicMatrix.at<double>(0,0) = f;
    intrinsicMatrix.at<double>(1,1) = f;
}

void QOpticalDevice::setPrincipalPoint(double u, double v)
{
    intrinsicMatrix.at<double>(0,2) = u;
    intrinsicMatrix.at<double>(1,2) = v;
}

double QOpticalDevice::getPrincipalU()
{
    return intrinsicMatrix.at<double>(0,2);
}

double QOpticalDevice::getPrincipalV()
{
    return intrinsicMatrix.at<double>(1,2);
}

double QOpticalDevice::getNormalizedCoord(double u, double c)
{
    return (u - c)/getFocalLength();
}

void QOpticalDevice::wait()
{
}
