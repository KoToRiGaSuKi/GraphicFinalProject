#pragma once
#ifndef GLCAMERA_H
#define GLCAMERA_H
#include "eigen.h"
#include <windows.h>
//#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
//#include <GL/glut.h>

//using namespace Eigen;
class GLCamera{
public:
	GLCamera();
	GLCamera(const Eigen::Vector3d& pos, const Eigen::Vector3d& target, const Eigen::Vector3d& up);
	void setModelViewMatrix();
	void setShape(float viewAngle, float aspect, float Near, float Far);
	void slide(float du, float dv, float dn);
	void roll(float angle);
	void yaw(float angle);
	void pitch(float angle);
	float getDist();

private:
	Eigen::Vector3d m_pos;
	Eigen::Vector3d m_target;
	Eigen::Vector3d m_up;
	Eigen::Vector3d u, v, n;

};

#endif // GLCAMERA_H