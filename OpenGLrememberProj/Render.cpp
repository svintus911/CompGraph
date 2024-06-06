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



void norm(double* startpoint, double* vec1, double* vec2, double* finalvec)
{
	double veclen = 0;
	finalvec[0] = (vec1[1] - startpoint[1]) * (vec2[2] - startpoint[2]) - (vec2[1] - startpoint[1]) * (vec1[2] - startpoint[2]);
	finalvec[1] = - (vec1[0] - startpoint[0]) * (vec2[2] - startpoint[2]) + (vec2[0] - startpoint[0]) * (vec1[2] - startpoint[2]);
	finalvec[2] = (vec1[0] - startpoint[0]) * (vec2[1] - startpoint[1]) - (vec2[0] - startpoint[0]) * (vec1[1] - startpoint[1]);

	veclen = sqrt((finalvec[0] * finalvec[0]) + (finalvec[1] * finalvec[1]) + (finalvec[2] * finalvec[2]));

	for (int i = 0; i < 3; i++)
	{
		finalvec[i] = finalvec[i] / veclen;
	}
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
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/


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

	double height = 2;
	double prevpoint[3];
	double finalvec[3];

	double A[3] = { 0, 0, 0 };
	double B[3] = { 5, 2, 0 };
	double C[3] = { 4, 8, 0 };
	double D[3] = { 9, 8, 0 };
	double E[3] = { 8, 3, 0 };
	double F[3] = { 16, -1, 0 };
	double G[3] = { 7, 0, 0 };
	double H[3] = { 4, -7, 0 };

	double Ah[3] = { 0, 0, height };
	double Bh[3] = { 5, 2, height };
	double Ch[3] = { 4, 8, height };
	double Dh[3] = { 9, 8, height };
	double Eh[3] = { 8, 3, height };
	double Fh[3] = { 16, -1, height };
	double Gh[3] = { 7, 0, height };
	double Hh[3] = { 4, -7, height };


	glBegin(GL_TRIANGLES);

	glColor3d(0.4, 0.4, 0.4);
	glNormal3d(0, 0, -1);
	glVertex3dv(B);
	glVertex3dv(E);
	glVertex3dv(G);

	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(G);

	glEnd();

	glBegin(GL_QUADS);

	glColor3d(0.4, 0.4, 0.4);

	glNormal3d(0, 0, -1);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(G);
	glVertex3dv(H);

	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);

	glEnd();

	glBegin(GL_QUADS);

	glColor3d(0.1, 0.1, 0.1);
	norm(A, Ah, B, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(Bh);
	glVertex3dv(Ah);

	norm(B, Bh, C, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(Ch);
	glVertex3dv(Bh);

	norm(C, Ch, D, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(Dh);
	glVertex3dv(Ch);

	norm(D, Dh, E, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(Eh);
	glVertex3dv(Dh);

	norm(E, Eh, F, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(Fh);
	glVertex3dv(Eh);

	norm(F, Fh, G, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(Gh);
	glVertex3dv(Fh);

	norm(G, Gh, H, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(Hh);
	glVertex3dv(Gh);

	norm(H, Hh, A, finalvec);
	glNormal3d(finalvec[0], finalvec[1], finalvec[2]);
	glVertex3dv(H);
	glVertex3dv(A);
	glVertex3dv(Ah);
	glVertex3dv(Hh);

	glEnd();

	glBegin(GL_TRIANGLES);
	glColor4d(0.4, 0.4, 0.4, 0.5);

	glNormal3d(0, 0, 1);
	glVertex3dv(Bh);
	glVertex3dv(Eh);
	glVertex3dv(Gh);

	glVertex3dv(Eh);
	glVertex3dv(Fh);
	glVertex3dv(Gh);

	glEnd();

	glBegin(GL_QUADS);

	glColor4d(0.4, 0.4, 0.4, 0.5);

	glNormal3d(0, 0, 1);
	glVertex3dv(Ah);
	glVertex3dv(Bh);
	glVertex3dv(Gh);
	glVertex3dv(Hh);

	glVertex3dv(Bh);
	glVertex3dv(Ch);
	glVertex3dv(Dh);
	glVertex3dv(Eh);

	glEnd();

	/*glBegin(GL_TRIANGLES);
	double prevpoint[2] = {5, 0};

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glNormal3d(0, 0, 1);

	for (int i = 1; i < 360; i++)
	{
		double newpoint[2];
		newpoint[0] = 5 * cos(i * 0.0175);
		newpoint[1] = 5 * sin(i * 0.0175);
		glTexCoord2d(newpoint[0] / 10 + 0.5, newpoint[1] / 10 + 0.5);
		glVertex2dv(newpoint);
		glTexCoord2d(prevpoint[0] / 10 + 0.5, prevpoint[1] / 10 + 0.5);
		glVertex2dv(prevpoint);
		glTexCoord2d(0.5, 0.5);
		glVertex2d(0, 0);
		prevpoint[0] = newpoint[0];
		prevpoint[1] = newpoint[1];
	}*/


	//������ ��������� ���������� ��������
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
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