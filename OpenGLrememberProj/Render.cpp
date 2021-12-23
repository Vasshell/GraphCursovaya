#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"
using namespace std;

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

double* normal(vector<double> start_coords, vector<double> middle_coords, vector<double> end_coords) {
	double vector1[3] = { start_coords[0] - middle_coords[0], start_coords[1] - middle_coords[1], start_coords[2] - middle_coords[2] };
	double vector2[3] = { end_coords[0] - middle_coords[0], end_coords[1] - middle_coords[1], end_coords[2] - middle_coords[2] };
	double normal_x = vector1[1] * vector2[2] - vector2[1] * vector1[2];
	double normal_y = -vector1[0] * vector2[2] + vector2[0] * vector1[2];
	double normal_z = vector1[0] * vector2[1] - vector2[0] * vector1[1];
	double normal[3] = { normal_x, normal_y, normal_z };
	double normal_length = sqrt(normal_x * normal_x + normal_y * normal_y + normal_z * normal_z);
	normal[0] /= normal_length;
	normal[1] /= normal_length;
	normal[2] /= normal_length;
	return normal;
}

void Prikolismus(vector<vector<double>> coords, vector<double> Center, double height) {
	vector<double> point1;
	vector<double> point2;
	vector<double> point3;
	vector<double> point4;
	vector<double> local_center = Center;
	for (int count = 0; count < coords.size() - 1; count++) {
		point1 = coords[count];
		point2 = coords[count + 1];
		point3 = coords[count];
		point4 = coords[count + 1];
		point3[2] = point3[2] - height;
		point4[2] = point4[2] - height;
		glBegin(GL_TRIANGLES);
		glNormal3dv(normal(point2, local_center, point1));
		glVertex3f(point1[0], point1[1], point1[2]);
		glVertex3f(local_center[0], local_center[1], local_center[2] );
		glVertex3f(point2[0], point2[1], point2[2]);
		glEnd();
		local_center[2] -= height;
		glBegin(GL_TRIANGLES);
		glNormal3dv(normal(point3, local_center, point4));
		glVertex3f(point3[0], point3[1], point3[2]);
		glVertex3f(local_center[0], local_center[1], local_center[2]);
		glVertex3f(point2[0], point4[1], point4[2]);
		glEnd();
		local_center[2] += height;
		glBegin(GL_QUADS);
		glNormal3dv(normal(point4, point1, point3));
		glVertex3f(point1[0], point1[1], point1[2]);
		glVertex3f(point2[0], point2[1], point2[2]);
		glVertex3f(point4[0], point4[1], point4[2]);
		glVertex3f(point3[0], point3[1], point3[2]);
		glEnd();
	}
	point1 = coords.back();
	point2 = coords.front();
	point3 = coords.back();
	point4 = coords.front();
	point3[2] = point1[2] - height;
	point4[2] = point2[2] - height;
	glBegin(GL_TRIANGLES);
	glNormal3dv(normal(point2, local_center, point1));
	glVertex3f(point1[0], point1[1], point1[2]);
	glVertex3f(local_center[0], local_center[1], local_center[2]);
	glVertex3f(point2[0], point2[1], point2[2]);
	glEnd();
	local_center[2] -= height;
	glBegin(GL_TRIANGLES);
	glNormal3dv(normal(point3, local_center, point4));
	glVertex3f(point3[0], point3[1], point3[2]);
	glVertex3f(local_center[0], local_center[1], local_center[2]);
	glVertex3f(point2[0], point4[1], point4[2]);
	glEnd();
	local_center[2] += height;
	glBegin(GL_QUADS);
	glNormal3dv(normal(point4, point1, point3));
	glVertex3f(point1[0], point1[1], point1[2]);
	glVertex3f(point2[0], point2[1], point2[2]);
	glVertex3f(point4[0], point4[1], point4[2]);
	glVertex3f(point3[0], point3[1], point3[2]);
	glEnd();
}



void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	double h = 1;
	vector<double> A = { 1, -6,0 };
	vector<double> B = { -4, -4, 0 };
	vector<double> C = { -1, 1 , 0 };
	vector<double> D = { -4, 6,0 };
	vector<double> E = { 1, 5, 0 };
	vector<double> F = { 1, 1, 0 };
	vector<double> G = { 6,0,0 };
	vector<double> H = { 1, -1 , 0 };
	vector<vector<double>> coords;
	coords.push_back(A);
	coords.push_back(B);
	coords.push_back(C);
	coords.push_back(D);
	coords.push_back(E);
	coords.push_back(F);
	coords.push_back(G);
	coords.push_back(H);
	vector<double> Center = { 0,0,0 };

	Prikolismus(coords, Center, h);

	double cx=(D[0]+E[0])/2;
	double cy= (D[1] + E[1]) / 2;
	double r = sqrt((D[0] - E[0]) * (D[0] - E[0]) + (D[1] - E[1]) * (D[1] - E[1]) + (D[2] - E[2]) * (D[2] - E[2]))/2;
	int num_segments=1000;
	
	vector<vector<double>> round_coords;
	vector<double> round_center = { cx,cy,0 };

	double angle = asin((D[1] - cy) / r);

	for (int i = 0; i < num_segments; i++)
	{
		double theta = 3.1415926 * double(i) / double(num_segments) - angle;

		double x = r * cos(theta);
		double y = r * sin(theta);

		vector<double> point = { x + cx, y + cy, 0 };
		round_coords.push_back(point);
	}
	reverse(round_coords.begin(), round_coords.end());
	Prikolismus(round_coords, round_center, h);

	//������ ��������� ���������� ��������
//	double A[2] = { -4, -4 }; 
//	double B[2] = { 4, -4 };
//	double C[2] = { 4, 4 };
//	double D[2] = { -4, 4 };
//
//glBindTexture(GL_TEXTURE_2D, texId);
//
//glColor3d(0.6, 0.6, 0.6);
//glBegin(GL_QUADS);
//
//glNormal3d(0, 0, 1);
//glTexCoord2d(0, 0);
//glVertex2dv(A);
//glTexCoord2d(1, 0);
//glVertex2dv(B);
//glTexCoord2d(1, 1);
//glVertex2dv(C);
//glTexCoord2d(0, 1);
//glVertex2dv(D);
//
//glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}