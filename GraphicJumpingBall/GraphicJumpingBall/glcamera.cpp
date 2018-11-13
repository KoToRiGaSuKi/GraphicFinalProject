#include "glcamera.h"

GLCamera::GLCamera()
{

}

GLCamera::GLCamera(const Eigen::Vector3d &pos, const Eigen::Vector3d &target, const Eigen::Vector3d &up)
{
	m_pos = pos;
	m_target = target;
	m_up = up;
	n = Eigen::Vector3d(pos.x() - target.x(), pos.y() - target.y(), pos.z() - target.z());
	u = Eigen::Vector3d(up.cross(n).x(), up.cross(n).y(), up.cross(n).z());
	v = Eigen::Vector3d(n.cross(u).x(), n.cross(u).y(), n.cross(u).z());


	n.normalize();
	u.normalize();
	v.normalize();

	setModelViewMatrix();
}

void GLCamera::setModelViewMatrix()
{
	double m[16];
	m[0] = u.x(); m[4] = u.y(); m[8] = u.z(); m[12] = -m_pos.dot(u);
	m[1] = v.x(); m[5] = v.y(); m[9] = v.z(); m[13] = -m_pos.dot(v);
	m[2] = n.x(); m[6] = n.y(); m[10] = n.z(); m[14] = -m_pos.dot(n);
	m[3] = 0;  m[7] = 0;  m[11] = 0;  m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m);     //用M矩阵替换原视点矩阵
}

void  GLCamera::setShape(float viewAngle, float aspect, float Near, float Far)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();                                   //设置当前矩阵模式为投影矩阵并归一化
	gluPerspective(viewAngle, aspect, Near, Far);        //对投影矩阵进行透视变换
}

void GLCamera::slide(float du, float dv, float dn)
{
	//std::cout<<"u.x:"<<u.x()<<std::endl;
	m_pos(0) = m_pos(0) + du * u.x() + dv * v.x() + dn * n.x();
	m_pos(1) = m_pos(1) + du * u.y() + dv * v.y() + dn * n.y();
	m_pos(2) = m_pos(2) + du * u.z() + dv * v.z() + dn * n.z();
	m_target(0) = m_target(0) + du * u.x() + dv * v.x() + dn * n.x();
	m_target(1) = m_target(0) + du * u.y() + dv * v.y() + dn * n.y();
	m_target(2) = m_target(0) + du * u.z() + dv * v.z() + dn * n.z();
	setModelViewMatrix();
}

void GLCamera::roll(float angle)
{
	float cs = cosf(angle*3.14159265 / 180);
	float sn = sinf(angle*3.14159265 / 180);
	Eigen::Vector3d t(u);
	Eigen::Vector3d s(v);
	u.x() = cs * t.x() - sn * s.x();
	u.y() = cs * t.y() - sn * s.y();
	u.z() = cs * t.z() - sn * s.z();

	v.x() = sn * t.x() + cs * s.x();
	v.y() = sn * t.y() + cs * s.y();
	v.z() = sn * t.z() + cs * s.z();

	setModelViewMatrix();          //每次计算完坐标轴变化后调用此函数更新视点矩阵
}

void GLCamera::pitch(float angle)
{
	float cs = cosf(angle*3.14159265 / 180);
	float sn = sinf(angle*3.14159265 / 180);
	Eigen::Vector3d t(v);
	Eigen::Vector3d s(n);

	v.x() = cs * t.x() - sn * s.x();
	v.y() = cs * t.y() - sn * s.y();
	v.z() = cs * t.z() - sn * s.z();

	n.x() = sn * t.x() + cs * s.x();
	n.y() = sn * t.y() + cs * s.y();
	n.z() = sn * t.z() + cs * s.z();


	setModelViewMatrix();
}

void GLCamera::yaw(float angle)
{
	float cs = cosf(angle*3.14159265 / 180);
	float sn = sinf(angle*3.14159265 / 180);
	Eigen::Vector3d t(n);
	Eigen::Vector3d s(u);

	n.x() = cs * t.x() - sn * s.x();
	n.y() = cs * t.y() - sn * s.y();
	n.z() = cs * t.z() - sn * s.z();

	u.x() = sn * t.x() + cs * s.x();
	u.y() = sn * t.y() + cs * s.y();
	u.z() = sn * t.z() + cs * s.z();

	setModelViewMatrix();
}

float  GLCamera::getDist()
{
	float dist = powf(m_pos.x(), 2) + powf(m_pos.y(), 2) + powf(m_pos.z(), 2);
	return powf(dist, 0.5);
}