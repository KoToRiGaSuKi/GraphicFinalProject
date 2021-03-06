#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <SOIL.h>
#include "glcamera.h"

#define G 9.8
#define DELT_T 0.002
#define GLUT_WHEEL_UP 3
#define GLUT_WHEEL_DOWN 4
#define LENGTH 40
#define HEIGHT 40
#define WIDTH 40

void display(void);

int GroundTexture;
int SphereTexture;
int WallTexture;
GLUquadricObj *quadPlanet;

typedef struct b{ //定义储存球体的结构
	GLdouble y;
	GLdouble vy;
}Ball;
Ball ball;

typedef struct { //定义储存光标位置的结构
	GLint x;
	GLint y;
}Cursor;

Cursor myCursor;
GLCamera *myCamera;
static bool Pause = false;

void init(void) {
	ball.y = 10;
	ball.vy = 0; //初始化球的位置 高度10 速度0
	glClearColor(0.0, 0.0, 0.0, 0.0); //背景颜色 纯黑
	glEnable(GL_DEPTH_TEST); //开启深度测试
	glShadeModel(GL_SMOOTH); //颜色过渡模式

	glEnable(GL_LIGHTING); //开启光照
	glEnable(GL_LIGHT0); //开启光源0
	glEnable(GL_COLOR_MATERIAL); //开启颜色材质 即可以通过GLColor给模型着色
	glEnable(GL_TEXTURE_2D); //开启2D材质

	myCursor.x = 242; 
	myCursor.y = 219; //这个数字是固定光标的位置
	SetCursorPos(350, 350); //设置光标位置  坐标是相对于屏幕左上角 不是窗口
	glutSetCursor(GLUT_CURSOR_NONE); //设置光标类型为无 即看不到光标
	Eigen::Vector3d pos(0.0, 0.0, 39.0);
	Eigen::Vector3d target(0.0, 0.0, 0.0);
	Eigen::Vector3d up(0.0, 1.0, 0.0);
	myCamera = new GLCamera(pos, target, up); //创建相机控制类

	//加载材质
	GroundTexture = SOIL_load_OGL_texture("木材1.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glBindTexture(GL_TEXTURE_2D, GroundTexture); //绑定材质

	//加载材质
	SphereTexture = SOIL_load_OGL_texture("地球.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glBindTexture(GL_TEXTURE_2D, SphereTexture); //绑定材质

	//加载材质
	WallTexture = SOIL_load_OGL_texture("墙壁.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	glBindTexture(GL_TEXTURE_2D, WallTexture); //绑定材质

	//设置材质的参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //线性插值过滤，获取坐标点附近4个像素的加权平均值。
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //设置纹理坐标为重复
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	quadPlanet = gluNewQuadric();
}

void drawMovingBall(bool flag) {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix(); //模型矩阵压栈
	glTranslated(0, ball.y, 0); //移动绘图中心到球心
	//glColor3f(1.0f, 1.0f, 1.0f); //设置颜色
	glBindTexture(GL_TEXTURE_2D, SphereTexture); //绑定材质
	//glGenerateMipmap(GL_TEXTURE_2D);
	gluQuadricTexture(quadPlanet, GLU_TRUE);
	//glutSolidSphere(2, 50, 50); //绘制实心球体
	gluSphere(quadPlanet, 2, 50, 50);
	if (flag) { //flag控制去不去修改球的坐标 如果不修改则球可以保持静止
		if (ball.y <= -10) {
			ball.vy = -ball.vy + G * DELT_T;
		}
		else {
			ball.vy = ball.vy + G * DELT_T;
		}
		ball.y -= ball.vy * DELT_T;
	}
	glPopMatrix(); //模型矩阵出栈
}

void idle(void){
	display(); //闲时操作设置为显示函数
}

void drawGround() { //绘制地面
	glPushMatrix(); //模型矩阵压栈
	glBindTexture(GL_TEXTURE_2D, GroundTexture); //绑定材质
	glBegin(GL_TRIANGLES); //开始绘制三角形 这里使用两个三角形拼成一个方形的地面
	glTexCoord2f(0.0f, 0.0f); //材质坐标绑定 如果不再次调用此函数 则后续定点全部绑定该点
	glVertex3f(-40.0f, -12.0f, 40.0f); //左上角 顶点坐标
	glTexCoord2f(1.0f, 0.0f); //右上角
	glVertex3f(40.0f, -12.0f, 40.0f);
	glTexCoord2f(0.0f, 1.0f); //左下角
	glVertex3f(-40.0f, -12.0f, -40.0f);
	glVertex3f(-40.0f, -12.0f, -40.0f);
	glTexCoord2f(1.0f, 0.0f); //右上角
	glVertex3f(40.0f, -12.0f, 40.0f);
	glTexCoord2f(1.0f, 1.0f); //右下角
	glVertex3f(40.0f, -12.0f, -40.0f);
	glEnd(); //结束绘图
	glPopMatrix(); //模型矩阵出栈
}

//0 前侧的
//1 后侧的
//2 左侧的
//3 右侧的
void drawWall(int direction) {

	float WallPos[4][4][3] = {
		{
			-40.0f, 28.0f, -40.0f,//左上
			40.0f, 28.0f, -40.0f,//右上
			-40.0f, -12.0f, -40.0f,//左下
			40.0f, -12.0f, -40.0f//右下
		},
		{
			-40.0f, 28.0f, 40.0f,
			40.0f, 28.0f, 40.0f,
			-40.0f, -12.0f, 40.0f,
			40.0f, -12.0f, 40.0f
		},
		{
			40.0f, 28.0f, -40.0f,
			40.0f, 28.0f, 40.0f,
			40.0f, -12.0f, -40.0f,
			40.0f, -12.0f, 40.0f
		},
		{
			-40.0f, 28.0f, -40.0f,
			-40.0f, 28.0f, 40.0f,
			-40.0f, -12.0f, -40.0f,
			-40.0f, -12.0f, 40.0f
		}
	};

	int repeat = 1;

	glPushMatrix(); //模型矩阵压栈
	glBindTexture(GL_TEXTURE_2D, WallTexture); //绑定材质
	glBegin(GL_TRIANGLES); //开始绘制三角形 这里使用两个三角形拼成一个方形的地面
	glTexCoord2f(0.0f, 0.0f); //材质坐标绑定 如果不再次调用此函数 则后续定点全部绑定该点
	glVertex3f(WallPos[direction][0][0], WallPos[direction][0][1], WallPos[direction][0][2]); //左上角 顶点坐标
	glTexCoord2f(1.0f * repeat, 0.0f); //右上角
	glVertex3f(WallPos[direction][1][0], WallPos[direction][1][1], WallPos[direction][1][2]);
	glTexCoord2f(0.0f, 1.0f * repeat); //左下角
	glVertex3f(WallPos[direction][2][0], WallPos[direction][2][1], WallPos[direction][2][2]);
	glVertex3f(WallPos[direction][2][0], WallPos[direction][2][1], WallPos[direction][2][2]);
	glTexCoord2f(1.0f * repeat, 0.0f); //右上角
	glVertex3f(WallPos[direction][1][0], WallPos[direction][1][1], WallPos[direction][1][2]);
	glTexCoord2f(1.0f * repeat, 1.0f * repeat); //右下角
	glVertex3f(WallPos[direction][3][0], WallPos[direction][3][1], WallPos[direction][3][2]);
	glEnd(); //结束绘图
	glPopMatrix(); //模型矩阵出栈

}

void display(void) {
	glLoadIdentity(); //复位
	myCamera->setModelViewMatrix(); //设定相机位置
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除缓冲区
	if(Pause)
		drawMovingBall(0);
	else
		drawMovingBall(1);
	drawGround();
	drawWall(0);
	drawWall(1);
	drawWall(2);
	drawWall(3);
	glFlush(); //刷新缓存
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h); //设置视窗大小
	myCamera->setShape(45.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0); //设定相机能看到的大小
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); //复位
}

void mouse(int button, int state, int x, int y) { //鼠标处理函数 用不到了
	switch (button) {
	case GLUT_LEFT_BUTTON:
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	case GLUT_WHEEL_UP:
		glScalef(1.1f, 1.1f, 1.1f);
		glutPostRedisplay();
		break;
	case GLUT_WHEEL_DOWN:
		glScalef(0.9f, 0.9f, 0.9f);
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void RotateX(float angle){ //相机水平旋转 参数为弧度
	float d = myCamera->getDist();
	int cnt = 100;
	float theta = angle / cnt;
	float slide_d = -2 * d*sin(theta*3.14159265 / 360);
	myCamera->yaw(theta / 2);
	for (; cnt != 0; --cnt){
		myCamera->slide(slide_d, 0, 0);
		myCamera->yaw(theta);
	}
	myCamera->yaw(-theta / 2);
}

void RotateY(float angle){ //相机纵向旋转 参数为弧度
	float d = myCamera->getDist();
	int cnt = 100;
	float theta = angle / cnt;
	float slide_d = 2 * d*sin(theta*3.14159265 / 360);
	myCamera->pitch(theta / 2);
	for (; cnt != 0; --cnt){
		myCamera->slide(0, slide_d, 0);
		myCamera->pitch(theta);
	}
	myCamera->pitch(-theta / 2);
}

void mouseMove(int x, int y) {
	if (abs(x - myCursor.x) > 30 || abs(y - myCursor.y) > 30) {//如果移动幅度过大 则忽略
		SetCursorPos(350, 350);
		return;
	}
	if (abs(x - myCursor.x) > 5 || abs(y - myCursor.y) > 5) { //移动幅度过小也会被忽略 因为这个函数会随时触发 没动也会进入这个函数处理
		int dx = x - myCursor.x;
		int dy = y - myCursor.y;
		RotateX(dx / 3); //水平旋转
		RotateY(dy / 3); //竖直旋转 将鼠标的位移作为弧度传入函数 除以三是为了变换的更平滑
		SetCursorPos(350, 350); //光标位置复位
		glutPostRedisplay(); //设置重新绘图标识
	}
}

void KeyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
	case ' ': //空格切换暂停状态
		Pause = !Pause;
		break;

	case 27://ESC
		exit(0);
		break;
	}

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); //单缓冲区 RGB模式 深度缓存
	glutInitWindowSize(600, 600); //设置窗口大小
	glutInitWindowPosition(100, 100); //设置窗口位置 相对于屏幕左上角
	glutCreateWindow(argv[0]); //创建窗口
	init(); //初始化
	glutDisplayFunc(display); //绑定显示函数
	glutReshapeFunc(reshape); //绑定reshape函数
	glutKeyboardFunc(KeyboardFunc); //绑定键盘事件处理函数
	glutPassiveMotionFunc(mouseMove); //绑定鼠标移动事件处理函数
	glutIdleFunc(idle); //绑定闲时函数
	glutMainLoop(); //开始主循环
	return 0;
}