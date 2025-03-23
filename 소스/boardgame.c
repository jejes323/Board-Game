#include <iostream>
#include <ctime>
#include <vector>
#include <glut.h>
#include <stdlib.h>
#include <time.h>

GLUquadric* qobj = gluNewQuadric();
GLfloat v[8][3] =
{

	{-1.0, 1.0, 1.0},

	{1.0, 1.0, 1.0},

	{1.0, -1.0, 1.0},

	{-1.0, -1.0, 1.0},



	{-1.0, 1.0, -1.0},

	{1.0, 1.0, -1.0},

	{1.0, -1.0, -1.0 },

	{-1.0, -1.0, -1.0},

};

class Vector
{
public:
	Vector(float X = 0.0, float Y = 0.0, float Z = 0.0)
		: x(X), y(Y), z(Z)
	{}

	static Vector Cross(const Vector& vec1, const Vector& vec2)
	{
		Vector vecCross;

		vecCross.x = vec1.y * vec2.z - vec1.z * vec2.y;
		vecCross.y = vec1.z * vec2.x - vec1.x * vec2.z;
		vecCross.z = vec1.x * vec2.y - vec1.y * vec2.x;

		vecCross.Normalize();
		return vecCross;
	}

	float Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	void Normalize()
	{
		float Len = Length();
		x /= Len;
		y /= Len;
		z /= Len;
	}

	float x, y, z;
};

Vector vecEye(0, 0, 0);
const float MOVEMENT_SPEED = 0.1;
Vector vecViewDirection(0, 0, -1);
Vector vecUp(0, 1, 0);

float width, height;
float SpinX = 0.0f; float SpinY = 0.0f;

static POINT LastMouse;
static POINT CurrentMouse;
static bool Mouse;

float p1alpha = 0.0; // 크기 조절
float p2alpha = 0.0; // 크기 조절
float vetaAlpha = 0.0; //크기 조절 변수 담는곳
float x1Location = 0.0; float y1Location = 0.0; // 플레이어1의 보드게임 위치
float x2Location = 0.0; float y2Location = 0.0; // 플레이어2의 보드게임 위치
int P1mark = 1; // 1(양수) = left,	-1(음수) = right [플레이어1 x 값 결정] 
int P2mark = 1; // 1(양수) = left,	-1(음수) = right [플레이어2 x 값 결정] 
int Turn = 2; // 0 = Red,	1 = Blue [누가 먼저 시작할 지 결정]
int Start = 0; // 시작을 했는지 안했는지 결정하는 변수
int BlueHunt = 0; int RedHunt; // 잡아 먹었을 때를 나타내기 위한 변수
int mode = 0; // 1 일 때 레드가 움직임, 2 일 때 블루가 움직임
int MenuResult = 8; // 메뉴 엔트리 값
int p1sizemode = 0; //크기 줄이고 키우고
int p2sizemode = 0; //크기 줄이고 키우고
double movingdiceresult = 0; // 주사위 나온 값의 2배 (블럭 한칸당 2라서)
int dicetest = 0; // 0 = 주사위를 굴리지 않았을 때, 1 = 주사위를 굴렸을 때


int SpinCount = 1; int diceResult = 0;
int T = 0;
int Count360 = 0;

float Player1Y = 0.0;
float Player2Y = 0.0;

void renderBitmapString(float x, float y, float z, void* font, const char* string)
{
	const char* c;
	glRasterPos3f(x, y, z);
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(font, *c);
}

void init()

{

	// 조명 설정

	GLfloat light_ambient[] = { .5, .5, .5, 1 };

	GLfloat light_diffuse[] = { 1, 1, 1, 1 };

	GLfloat light_specular[] = { 1, 1, 1, 1 };

	GLfloat light_pos[] = { 1, 1, 5, 1 };



	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);



	glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LINE_SMOOTH);



}


void drawBoard() {
	int q = 0;
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (q % 2 == 0) {
				if (j % 2 == 0)
					glColor3f(1, 1, 0.5);
				else
					glColor3f(0.93, 0.8, 0.48);
			}

			else {

				if (j % 2 == 0)
					glColor3f(0.93, 0.8, 0.48);
				else
					glColor3f(1, 1, 0.5);
			}

			glPushMatrix();
			glTranslatef(j * 2 * -1, 0, i * 2 * -1);
			glScalef(1, 0.2, 1);

			glutSolidCube(2);
			glPopMatrix();
		}
		q++;
	}
}

void Player1() {

	glColor3f(1, 0, 0);
	// 왼쪽면
	glBegin(GL_TRIANGLES);
	glNormal3f(-1.0f, 1.0f + Player1Y, 0.0f);
	glVertex3f(-1.0f, 0.0f + Player1Y, 1.0f);
	glVertex3f(0.0f, 2.0f + Player1Y, 0.0f);
	glVertex3f(-1.0f, 0.0f + Player1Y, -1.0f);
	glEnd();

	// 앞면
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 1.0f + Player1Y, 1.0f);
	glVertex3f(1.0f, 0.0f + Player1Y, 1.0f);
	glVertex3f(0.0f, 2.0f + Player1Y, 0.0f);
	glVertex3f(-1.0f, 0.0f + Player1Y, 1.0f);
	glEnd();

	// 오른쪽면
	glBegin(GL_TRIANGLES);
	glNormal3f(1.0f, 1.0f + Player1Y, 0.0f);
	glVertex3f(1.0f, 0.0f + Player1Y, -1.0f);
	glVertex3f(0.0f, 2.0f + Player1Y, 0.0f);
	glVertex3f(1.0f, 0.0f + Player1Y, 1.0f);
	glEnd();

	// 뒷면
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 1.0f + Player1Y, -1.0f);
	glVertex3f(-1.0f, 0.0f + Player1Y, -1.0f);
	glVertex3f(0.0f, 2.0f + Player1Y, 0.0f);
	glVertex3f(1.0, 0.0f + Player1Y, -1.0f);
	glEnd();

	glEndList();

}

void Player2() {
	glColor3f(0, 0, 1);
	// 왼쪽면
	glBegin(GL_TRIANGLES);
	glNormal3f(-1.0f, 1.0f + Player2Y, 0.0f);
	glVertex3f(-1.0f, 0.0f + Player2Y, 1.0f);
	glVertex3f(0.0f, 2.0f + Player2Y, 0.0f);
	glVertex3f(-1.0f, 0.0f + Player2Y, -1.0f);
	glEnd();

	// 앞면
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 1.0f + Player2Y, 1.0f);
	glVertex3f(1.0f, 0.0f + Player2Y, 1.0f);
	glVertex3f(0.0f, 2.0f + Player2Y, 0.0f);
	glVertex3f(-1.0f, 0.0f + Player2Y, 1.0f);
	glEnd();

	// 오른쪽면
	glBegin(GL_TRIANGLES);
	glNormal3f(1.0f, 1.0f + Player2Y, 0.0f);
	glVertex3f(1.0f, 0.0f + Player2Y, -1.0f);
	glVertex3f(0.0f, 2.0f + Player2Y, 0.0f);
	glVertex3f(1.0f, 0.0f + Player2Y, 1.0f);
	glEnd();

	// 뒷면
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 1.0f + Player2Y, -1.0f);
	glVertex3f(-1.0f, 0.0f + Player2Y, -1.0f);
	glVertex3f(0.0f, 2.0f + Player2Y, 0.0f);
	glVertex3f(1.0, 0.0f + Player2Y, -1.0f);
	glEnd();

	glEndList();
}

void Diceindex() {

	int result = T / 90;

	switch (result)
	{
	case 0:
		diceResult = 1;
		break;
	case 1:
		if (SpinCount == 1) diceResult = 3;
		else if (SpinCount == 2) diceResult = 5;
		break;
	case 2:
		diceResult = 6;
		break;
	case 3:
		if (SpinCount == 1) diceResult = 4;
		else if (SpinCount == 2) diceResult = 2;
		break;
	case 4:
		diceResult = 2; break;
	case 6:
		diceResult = 0; break;
	}

	if (MenuResult < 7) {
		switch (MenuResult) {
		case 1:
			diceResult = 1;
			MenuResult = 8;
			break;
		case 2:
			diceResult = 2;
			MenuResult = 8;
			break;
		case 3:
			diceResult = 3;
			MenuResult = 8;
			break;
		case 4:
			diceResult = 4;
			MenuResult = 8;
			break;
		case 5:
			diceResult = 5;
			MenuResult = 8;
			break;
		case 6:
			diceResult = 6;
			MenuResult = 8;
			break;
		}
	}

	if (Start == 0) {
		Start = 1;
		if (diceResult % 2 == 0) Turn = 0;
		else Turn = 1;
	}

	else {
		if (Turn == 0) {
			mode = 1;
			movingdiceresult = diceResult * 2;
			dicetest = 1;
		}
		else if (Turn == 1) {
			mode = 2;
			movingdiceresult = diceResult * 2;
			dicetest = 1;
		}
	}
}

void DoMenu(int entryID) {
	MenuResult = entryID;
	if (MenuResult < 7) {
		Diceindex();
	}
}

void MenuFunc() {
	GLint SubMenu = glutCreateMenu(DoMenu);
	glutAddMenuEntry("Dice Result 1", 1);
	glutAddMenuEntry("Dice Result 2", 2);
	glutAddMenuEntry("Dice Result 3", 3);
	glutAddMenuEntry("Dice Result 4", 4);
	glutAddMenuEntry("Dice Result 5", 5);
	glutAddMenuEntry("Dice Result 6", 6);

	// 메인 메뉴 생성
	glutCreateMenu(DoMenu);
	glutAddMenuEntry("Information", 10);

	// 서브 메뉴를 메인 메뉴에 붙인다.
	glutAddSubMenu("Cheat", SubMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void Spin() {

	srand(time(NULL));
	if (Count360 != 50) {
		T = T + 15;
		if (T > 360) {
			Count360++;
			T = 0;
		}
	}

	else {
		int n = rand() % 5; T = n * 90;
		int DiceSide = rand() % 2;
		if (DiceSide == 0) SpinCount = 1;
		else SpinCount = 2;
		int TResult = T % 90;
		if (TResult != 0) {
			T -= TResult;
		}
		if (T >= 360) {
			T -= 360;
		}
		Diceindex();
		Count360 = 0;
	}

}

void Face(GLfloat A[], GLfloat B[], GLfloat C[], GLfloat D[]) {

	glBegin(GL_POLYGON);
	glVertex3fv(A);
	glVertex3fv(B);
	glVertex3fv(C);
	glVertex3fv(D);
	glEnd();

}

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius) {

	int i;
	int triangleAmount = 20;
	GLfloat twicePi = 2.0f * 3.15;

	glBegin(GL_TRIANGLE_FAN);

	glVertex3f(x, y, z); // center of circle

	for (i = 0; i <= triangleAmount; i++) {
		glVertex2f(
			x + (radius * cos(i * twicePi / triangleAmount)),
			y + (radius * sin(i * twicePi / triangleAmount))
		);
	}

	glEnd();

}

void drawDice(GLfloat v0[], GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], GLfloat v5[], GLfloat v6[], GLfloat v7[]) {


	glPushMatrix(); // 1
	glColor3f(1, 0, 0);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, -1.1);
	drawFilledCircle(0, 0, 0, 0.25);
	glPopMatrix();
	glColor3f(1, 1, 1);
	Face(v0, v3, v7, v4); // 앞

	glPushMatrix(); // 2
	glColor3f(0, 0, 0);
	glTranslatef(0, 0, 1.1);
	drawFilledCircle(-0.5, 0.5, 0, 0.2);
	drawFilledCircle(0.5, -0.5, 0, 0.2);
	glPopMatrix();
	glColor3f(1, 1, 1);
	Face(v0, v1, v2, v3); // 오른쪽

	glPushMatrix(); // 3
	glColor3f(0, 0, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -1.1);
	drawFilledCircle(0.5, 0.5, 0, 0.2);
	drawFilledCircle(0, 0, 0, 0.2);
	drawFilledCircle(-0.5, -0.5, 0, 0.2);
	glPopMatrix();
	glColor3f(1, 1, 1);
	Face(v0, v1, v5, v4); // 위


	glPushMatrix(); // 4
	glColor3f(0, 0, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, 1.1);
	drawFilledCircle(0.5, 0.5, 0, 0.2);
	drawFilledCircle(-0.5, 0.5, 0, 0.2);
	drawFilledCircle(-0.5, -0.5, 0, 0.2);
	drawFilledCircle(0.5, -0.5, 0, 0.2);
	glPopMatrix();
	glColor3f(1, 1, 1);
	Face(v3, v2, v6, v7); // 아래


	glPushMatrix(); // 5
	glColor3f(0, 0, 0);
	glTranslatef(0, 0, -1.1);
	drawFilledCircle(0.5, 0.5, 0, 0.2);
	drawFilledCircle(-0.5, 0.5, 0, 0.2);
	drawFilledCircle(-0.5, -0.5, 0, 0.2);
	drawFilledCircle(0.5, -0.5, 0, 0.2);
	drawFilledCircle(0, 0, 0, 0.2);
	glPopMatrix();
	glColor3f(1, 1, 1);
	Face(v4, v5, v6, v7); // 왼쪽


	glPushMatrix(); // 6
	glColor3f(0, 0, 0);
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, 1.1);
	drawFilledCircle(0.5, 0.5, 0, 0.2);
	drawFilledCircle(0.5, 0, 0, 0.2);
	drawFilledCircle(0.5, -0.5, 0, 0.2);
	drawFilledCircle(-0.5, 0.5, 0, 0.2);
	drawFilledCircle(-0.5, 0, 0, 0.2);
	drawFilledCircle(-0.5, -0.5, 0, 0.2);
	glPopMatrix();
	glColor3f(1, 1, 1);
	Face(v1, v2, v6, v5); // 뒤

}

void drawPortal() {

	glColor3f(0.28, 0.9, 0.24);
	gluCylinder(qobj, 0.8, 0.8, 0.0, 20, 8);
	glColor3f(0.28, 0.78, 0.24);
	glutSolidTorus(0.3, 0.7, 20, 20);

}


void info() {
	if (Turn == 0 && RedHunt == 0 && BlueHunt == 0) {
		glColor4f(1, 0, 0, 1);
		renderBitmapString(-12 + y1Location, 2, -12 + x1Location, GLUT_BITMAP_TIMES_ROMAN_24, "Red");
	}
	else if (Turn == 1 && RedHunt == 0 && BlueHunt == 0) {
		glColor4f(0, 0, 1, 1);
		renderBitmapString(-12 + y2Location, 2, -12 + x2Location, GLUT_BITMAP_TIMES_ROMAN_24, "Blue");
	}
}

void hunt() {
	if (RedHunt == 1) {
		glColor4f(1, 1, 1, 1);
		renderBitmapString(-12 + y1Location, 3, -12 + x1Location, GLUT_BITMAP_TIMES_ROMAN_24, "Hunt!");
	}
	else if (BlueHunt == 1) {
		glColor4f(1, 1, 1, 1);
		renderBitmapString(-12 + y2Location, 3, -12 + x2Location, GLUT_BITMAP_TIMES_ROMAN_24, "Hunt!");
	}
}

void StartTurn() {
	if (Start == 0) {
		glColor4f(1, 1, 1, 1);
		renderBitmapString(-6, 10, -6, GLUT_BITMAP_TIMES_ROMAN_24, "press 'R' to start");
		renderBitmapString(-6, 9, -6, GLUT_BITMAP_TIMES_ROMAN_24, "decide who will go first");
	}
	else {
		if (Turn == 0) { // 주사위 값이 짝수 일 경우 레드
			glColor4f(1, 0, 0, 1);
			renderBitmapString(-6, 10, -6, GLUT_BITMAP_TIMES_ROMAN_24, "Red Turn");
			renderBitmapString(-6, 9, -6, GLUT_BITMAP_TIMES_ROMAN_24, "press 'R' to rolling !");
		}
		else if (Turn == 1) { // 주사위 값이 홀수 일 경우 블루
			glColor4f(0, 0, 1, 1);
			renderBitmapString(-6, 10, -6, GLUT_BITMAP_TIMES_ROMAN_24, "Blue Turn");
			renderBitmapString(-6, 9, -6, GLUT_BITMAP_TIMES_ROMAN_24, "press 'R' to rolling !");
		}
	}
}

void PlayerMoving() {
	if (mode == 0 && dicetest == 1) {
		if (Turn == 0) {
			Turn = 1;
			dicetest = 0;
			if ((int)x1Location == (int)x2Location && (int)y1Location == (int)y2Location) {
				RedHunt = 1; // 레드가 잡아 먹었을 때 변수
				x2Location = 0;
				y2Location = 0;
				P2mark = 1;
			}
		}
		else if (Turn == 1) {
			Turn = 0;
			dicetest = 0;
			if ((int)x1Location == (int)x2Location && (int)y1Location == (int)y2Location) {
				BlueHunt = 1; // 블루가 잡아먹었을 때 변수
				x1Location = 0;
				y1Location = 0;
				P1mark = 1;
			}
		}
	}
	if (mode == 1) {
		movingdiceresult -= (0.05);
		x1Location += (0.05 * P1mark);
		printf("%lf\n", movingdiceresult);
		if (movingdiceresult <= 0) {
			x1Location = (int)x1Location;
			if ((int)x1Location % 2 == 1) {
				x1Location += 1;
			}
			printf("movingdiceresult = %lf\n", movingdiceresult);
			printf("x1 = %lf\n", x1Location);
			mode = 0;
		}
		if (y1Location == 0.0 && x1Location > 12.1) {
			y1Location = 2.0;
			movingdiceresult -= 2.0;
			P1mark = -1;
		}
		else if (y1Location == 2.0 && x1Location < 0.1) {
			if (movingdiceresult > 0.15) {
				y1Location = 4.0;
				movingdiceresult -= 2.0;
				P1mark = 1;
			}
		}
		else if (y1Location == 4.0 && x1Location > 12.1) {
			y1Location = 6.0;
			movingdiceresult -= 2.0;
			P1mark = -1;
		}
		else if (y1Location == 6.0 && x1Location < 0.1) {
			if (movingdiceresult > 0.15) {
				y1Location = 8.0;
				movingdiceresult -= 2.0;
				P1mark = 1;
			}
		}
		else if (y1Location == 8.0 && x1Location > 12.1) {
			y1Location = 10.0;
			movingdiceresult -= 2.0;
			P1mark = -1;
		}
		else if (y1Location == 10.0 && x1Location < 0.1) {
			if (movingdiceresult > 0.15) {
				y1Location = 12.0;
				movingdiceresult -= 2.0;
				P1mark = 1;
			}
		}
		else if (y1Location == 12.0 && x1Location > 12.1) {
			printf("승리\n");
			P1mark = 1;
			exit(1);
		}
	}
	else if (mode == 2) {
		movingdiceresult -= (0.05);
		x2Location += (0.05 * P2mark);
		printf("%lf\n", movingdiceresult);
		if (movingdiceresult <= 0) {
			x2Location = (int)x2Location;
			if ((int)x2Location % 2 == 1) {
				x2Location += 1;
			}
			printf("movingdiceresult = %lf\n", movingdiceresult);
			printf("x2 = %lf\n", x2Location);
			mode = 0;
		}
		if (y2Location == 0.0 && x2Location > 12.1) {
			y2Location = 2.0;
			movingdiceresult -= 2.0;
			P2mark = -1;
		}
		else if (y2Location == 2.0 && x2Location < 0.1) {
			if (movingdiceresult > 0.15) {
				y2Location = 4.0;
				movingdiceresult -= 2.0;
				P2mark = 1;
			}
		}
		else if (y2Location == 4.0 && x2Location > 12.1) {
			y2Location = 6.0;
			movingdiceresult -= 2.0;
			P2mark = -1;
		}
		else if (y2Location == 6.0 && x2Location < 0.1) {
			if (movingdiceresult > 0.15) {
				y2Location = 8.0;
				movingdiceresult -= 2.0;
				P2mark = 1;
			}
		}
		else if (y2Location == 8.0 && x2Location > 12.1) {
			y2Location = 10.0;
			movingdiceresult -= 2.0;
			P2mark = -1;
		}
		else if (y2Location == 10.0 && x2Location < 0.1) {
			if (movingdiceresult > 0.15) {
				y2Location = 12.0;
				movingdiceresult -= 2.0;
				P2mark = 1;
			}
		}
		else if (y2Location == 12.0 && x2Location > 12.1) {
			printf("승리\n");
			P2mark = 1;
			exit(1);
		}
	}
}

void p1size() {
	if (p1sizemode == 1) {
		if (p1alpha <= 1.0) {
			p1alpha += 0.1;
			vetaAlpha += 0.1;
			if (vetaAlpha >= 1.0) {
				p1sizemode = 0;
				vetaAlpha = 0;
			}
		}
	}
}

void p2size() {
	if (p2sizemode == 1) {
		if (p2alpha <= 1.0) {
			p2alpha += 0.1;
			vetaAlpha += 0.1;
			if (vetaAlpha >= 1.0) {
				p2sizemode = 0;
				vetaAlpha = 0;
			}
		}
	}
}

void display()

{
	PlayerMoving();

	gluLookAt(vecEye.x, vecEye.y, vecEye.z, vecEye.x + vecViewDirection.x, vecEye.y + vecViewDirection.y, vecEye.z + vecViewDirection.z, vecUp.x, vecUp.y + 2, vecUp.z);

	if (SpinCount == 0)	Spin(); // 주사위 돌리기

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glTranslatef(-2.0f, 1.0f, -24);

	glRotatef(-SpinY, 1.0f, 0.0f, 0.0f);
	glRotatef(-SpinX, 0.0f, 1.0f, 0.0f);

	glRotatef(90, 0, 1, 0);
	glTranslatef(6, -2, 4 * 2);

	drawBoard(); // 보드판 그리기

	glPushMatrix(); // 주사위 그리기
	glTranslatef(-6, 7, -6);

	if (SpinCount == 0) {
		glRotatef(T, 1, 1, 0);
	}
	else if (SpinCount == 1) {
		glRotatef(T, 0, 0, 1);
	}
	else if (SpinCount == 2) {
		glRotatef(T, 0, 1, 0);
	}

	glPushMatrix();
	drawDice(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
	glPopMatrix();

	glPushMatrix(); // 플레이어 1
	glTranslatef(-12 + y1Location, 0, -12 + x1Location); // 말 위치
	glRotatef(270, 0, 1, 0); // 머리 위치
	if (x1Location == 8.0 && y1Location == 2.0 || x1Location == 4.0 && y1Location == 6.0) {
		glScalef(0.0 + p1alpha, 0.0 + p1alpha, 0.0 + p1alpha);
		Player1Y = 1.8;
		p1sizemode = 1;
		p1size();
		x1Location = 4.0;
		y1Location = 6.0;

	}
	else if (x1Location == 2.0 && y1Location == 2.0 || x1Location == 6.0 && y1Location == 10.0) {
		glScalef(0.0 + p1alpha, 0.0 + p1alpha, 0.0 + p1alpha);
		Player1Y = 1.8;
		p1sizemode = 1;
		p1size();
		x1Location = 6.0;
		y1Location = 10.0;
	}
	else {
		p1alpha = 0.0;
		Player1Y = 0.0;
	}
	Player1();
	glPopMatrix();

	glPushMatrix(); // 플레이어2
	glTranslatef(-12 + y2Location, 0, -12 + x2Location); // 말 위치
	glRotatef(270, 0, 1, 0); // 머리 위치
	if (x2Location == 8.0 && y2Location == 2.0 || x2Location == 4.0 && y2Location == 6.0) {
		glScalef(0.0 + p2alpha, 0.0 + p2alpha, 0.0 + p2alpha);
		Player2Y = 1.8;
		p2sizemode = 1;
		p2size();
		x2Location = 4.0;
		y2Location = 6.0;
	}
	else if (x2Location == 2.0 && y2Location == 2.0 || x2Location == 6.0 && y2Location == 10.0) {
		glScalef(0.0 + p2alpha, 0.0 + p2alpha, 0.0 + p2alpha);
		Player2Y = 1.8;
		p2sizemode = 1;
		p2size();
		x2Location = 6.0;
		y2Location = 10.0;
	}
	else {
		p2alpha = 0.0;
		Player2Y = 0.0;
	}
	Player2();
	glPopMatrix();

	glPushMatrix(); // 1번 포탈
	glTranslatef(-10, 0.0, -4);
	glRotatef(90, 90, 1, 0);
	drawPortal();
	if (x1Location == 8.0 && y1Location == 2.0 || x1Location == 4.0 && y1Location == 6.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else if (x2Location == 8.0 && y2Location == 2.0 || x2Location == 4.0 && y2Location == 6.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 0.0, 20, 8);
	}
	glPopMatrix();

	glPushMatrix(); // 2번 포탈
	glTranslatef(-6, 0.0, -8);
	glRotatef(90, 90, 1, 0);
	drawPortal();
	if (x1Location == 8.0 && y1Location == 2.0 || x1Location == 4.0 && y1Location == 6.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else if (x2Location == 8.0 && y2Location == 2.0 || x2Location == 4.0 && y2Location == 6.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 0.0, 20, 8);
	}
	glPopMatrix();

	glPushMatrix(); // 3번 포탈
	glTranslatef(-2, 0.0, -6);
	glRotatef(90, 90, 1, 0);
	drawPortal();
	if (x1Location == 2.0 && y1Location == 2.0 || x1Location == 6.0 && y1Location == 10.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else if (x2Location == 2.0 && y2Location == 2.0 || x2Location == 6.0 && y2Location == 10.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 0.0, 20, 8);
	}
	glPopMatrix();

	glPushMatrix(); // 4번 포탈
	glTranslatef(-10, 0.0, -10);
	glRotatef(90, 90, 1, 0);
	drawPortal();
	if (x1Location == 2.0 && y1Location == 2.0 || x1Location == 6.0 && y1Location == 10.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else if (x2Location == 2.0 && y2Location == 2.0 || x2Location == 6.0 && y2Location == 10.0) {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 1.5, 20, 8);
		glutSolidTorus(0.3, 0.7, 20, 20);
	}
	else {
		glTranslatef(0, 0, -1.5);
		gluCylinder(qobj, 0.8, 0.8, 0.0, 20, 8);
	}
	glPopMatrix();

	StartTurn(); // 시작, 선공, 후공 메세지
	hunt(); // 잡아먹었을 때 나오는 메세지
	info();

	glLoadIdentity();
	glutSwapBuffers();
	glFlush();
}

void reshape(int width, int height)
{
	::width = width;
	::height = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, width / (1.0 * height), 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void MyMotion(int x, int y) {

	CurrentMouse.x = x;
	CurrentMouse.y = y;

	if (Mouse)
	{
		SpinX -= (CurrentMouse.x - LastMouse.x);
		SpinY -= (CurrentMouse.y - LastMouse.y);
	}

	LastMouse.x = CurrentMouse.x;
	LastMouse.y = CurrentMouse.y;

	glutPostRedisplay();
}

void MyMouse(int button, int state, int x, int y) {

	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			LastMouse.x = CurrentMouse.x = x;
			LastMouse.y = CurrentMouse.y = y;
			Mouse = true;
		}

		else
			Mouse = false;
		break;
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON:
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void processKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'R':case 'r': // 주사위 굴리기
		SpinCount = 0;
		RedHunt = 0; BlueHunt = 0; // 잡아먹은 값 초기화
		printf("주사위 눈의 값 : %d\n", diceResult);
		break;
	case 'C':case 'c':
		x1Location = 8;
		y1Location = 2;
		break;

	case '1':
		SpinX = 0.0f;
		SpinY = 0.0f;
		vecEye.x = 1.6;
		vecEye.y = 0;
		vecEye.z = -14.6;
		break;
	case '2':
		SpinX = 0.0f;
		SpinY = 0.0f;
		vecEye.x = -1.9;
		vecEye.y = 0;
		vecEye.z = -19.3;
		break;
	case '3':
		SpinX = 0.0f;
		SpinY = 0.0f;
		vecEye.x = 0;
		vecEye.y = 0;
		vecEye.z = -25.3;
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(400, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("20212992 서영우, 20213001 박동희");
	init();
	glutKeyboardFunc(processKeyboard);
	glutReshapeFunc(reshape);
	MenuFunc();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutMainLoop();
}