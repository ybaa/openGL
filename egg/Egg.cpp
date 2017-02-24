/*************************************************************************************/
//	Martyna Łagożna 218218
/*************************************************************************************/
#include <windows.h>
#include <gl/gl.h>
#include <glut.h>
#include <cmath>
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

using namespace std;

typedef float point3[3];
static GLfloat thetaSpin[] = { 0.0, 0.0, 0.0 };

void spinEgg() {

	thetaSpin[0] -= 0.5;
	if (thetaSpin[0] > 360.0) thetaSpin[0] -= 360.0;

	thetaSpin[1] -= 0.5;
	if (thetaSpin[1] > 360.0) thetaSpin[1] -= 360.0;

	thetaSpin[2] -= 0.5;
	if (thetaSpin[2] > 360.0) thetaSpin[2] -= 360.0;

	glutPostRedisplay(); //odświeżenie zawartości aktualnego okna
}

const int n = 40;	//how many times square will be devided
point3 NxN[n + 1][n + 1];	//matrix which contains coordinants of points in 3D
const float PI = 3.141592;

float lengthOfEdgeOdSquare = 1.0;

float u, v;
float rearrangement = lengthOfEdgeOdSquare / n;



//------------------------------------------------------
static GLfloat viewer[] = { 0.0, 0.0, 0.0 };
float viewerR = 30.0;			//Promien sfery obserwatora

static GLfloat	fi = 0.0,		// kąty obrotu, elewacja i azymut
theta = 0.0;
static GLfloat	pix2angle_x,	// przelicznik pikseli na stopnie
pix2angle_y;

static GLint	status = 0;		//stan klawisza myszy 

static int	x_pos_old = 0,		// poprzednia pozycja kursora myszy
y_pos_old = 0;
static int	delta_x = 0,		// różnica pomiędzy pozycją bieżącą
delta_y = 0;					// i poprzednią kursora myszy


								
static GLfloat angle[] = { 0.0, 0.0, 0.0 };	//Tablica katow obrotu
//---------------------------  READ IMG FROM FILE -------------

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat) {
	/*************************************************************************************/
	// Struktura dla nagłówka pliku  TGA

#pragma pack(1)            
	typedef struct {
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte    *pbitsperpixel = NULL;

	/*************************************************************************************/
	// Wartości domyślne zwracane w przypadku błędu 

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;
	/*************************************************************************************/
	// Przeczytanie nagłówka pliku 

	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	/*************************************************************************************/
	// Odczytanie szerokości, wysokości i głębi obrazu 

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

	/*************************************************************************************/
	// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity) 

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/
	// Obliczenie rozmiaru bufora w pamięci

	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	/*************************************************************************************/
	// Alokacja pamięci dla danych obrazu

	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1) {
		free(pbitsperpixel);
		return NULL;
	}

	/*************************************************************************************/
	// Ustawienie formatu OpenGL

	switch (sDepth) {
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};

	fclose(pFile);

	return pbitsperpixel;
}




//-------------------  GENERATE NORMAL VECTORS ------------------------
float xNormal, yNormal, zNormal;
point3 normalPoints[n + 1][n + 1];
void generateNormalVector(float u, float v) {
	float xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*cos(v*PI);
	float xv = PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*sin(v*PI);
	float yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
	float yv = 0;
	float zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45)*sin(v*PI);
	float zv = -PI*(90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u)*cos(v*PI);

	xNormal = (GLfloat)(yu*zv - zu*yv);
	yNormal = (GLfloat)(zu*xv - xu*zv);
	zNormal = (GLfloat)(xu*yv - yu*xv);
}

//----------------------------------------------------------------------
point3 texture[n + 1][n + 1];
//-------------------  GENERATE MATRIX OF COORDINANTS ------------------------
void generateMatrix() {

	//fill the matrix with coordinants in 2D
	for (int i = 0; i < n+1; i++) {
		for (int j = 0; j < n+1; j++) {
			NxN[i][j][0] = rearrangement * i;
			NxN[i][j][1] = rearrangement * j;
		}
	}

	//convert 2D to 3D
	for (int i = 0; i < n+1; i++) {
		for (int j = 0; j < n+1; j++) {
			u = NxN[i][j][0];
			v = NxN[i][j][1];
			texture[i][j][0] = u;
			texture[i][j][1] = v;
			NxN[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(PI*v);
			NxN[i][j][1] = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2);
			NxN[i][j][1] -= 5;
			NxN[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(PI*v);

			generateNormalVector(u,v);

			if (i < n / 2) {
				normalPoints[i][j][0] = xNormal / (float)sqrt(pow(xNormal, 2) + pow(yNormal, 2) + pow(zNormal, 2));
				normalPoints[i][j][1] = yNormal / (float)sqrt(pow(xNormal, 2) + pow(yNormal, 2) + pow(zNormal, 2));
				normalPoints[i][j][2] = zNormal / (float)sqrt(pow(xNormal, 2) + pow(yNormal, 2) + pow(zNormal, 2));
			}
			if (i > n / 2) {
				normalPoints[i][j][0] = -1.0*xNormal / (float)sqrt(pow(xNormal, 2) + pow(yNormal, 2) + pow(zNormal, 2));
				normalPoints[i][j][1] = -1.0*yNormal / (float)sqrt(pow(xNormal, 2) + pow(yNormal, 2) + pow(zNormal, 2));
				normalPoints[i][j][2] = -1.0*zNormal / (float)sqrt(pow(xNormal, 2) + pow(yNormal, 2) + pow(zNormal, 2));
			}
			//vector on the top of egg
			if (i == n / 2) {
				normalPoints[i][j][0] = 0;
				normalPoints[i][j][1] = 1;
				normalPoints[i][j][2] = 0;
			}
			//vector at the bottom of the egg
			if (i == 0 || i == n) {
				normalPoints[i][j][0] = 0;
				normalPoints[i][j][1] = -1;
				normalPoints[i][j][2] = 0;
			}
		}
	}
}


//-------------------  PAINT EGG ------------------------
void paintLightenEgg() {

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {

			glBegin(GL_TRIANGLES);

			glNormal3fv(normalPoints[i][j + 1]);
			glTexCoord2fv(texture[i][j+1]);
			glVertex3fv(NxN[i][j + 1]);

			glNormal3fv(normalPoints[i + 1][j + 1]);
			glTexCoord2fv(texture[i + 1][j + 1]);
			glVertex3fv(NxN[i + 1][j + 1]);

			glNormal3fv(normalPoints[i + 1][j]);
			glTexCoord2fv(texture[i+1][j]);
			glVertex3fv(NxN[i + 1][j]);

			
			glEnd();
		
			glBegin(GL_TRIANGLES);

			glNormal3fv(normalPoints[i][j + 1]);
			glTexCoord2fv(texture[i][j + 1]);
			glVertex3fv(NxN[i][j + 1]);

			glNormal3fv(normalPoints[i + 1][j]);
			glTexCoord2fv(texture[i + 1][j]);
			glVertex3fv(NxN[i + 1][j]);

			glNormal3fv(normalPoints[i][j]);
			glTexCoord2fv(texture[i][j]);
			glVertex3fv(NxN[i][j]);

			glEnd();
			
			
		}
	}

	
}


//-------------------  DRAW AXES ------------------------
void Axes(void) {
	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// początek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// początek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  początek i koniec obrazu osi y
	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}



void MyInit(char *tekstura) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess = { 20.0 };
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
	GLfloat light_ambient[] = { 1, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat att_constant = 1.0;
	GLfloat att_linear = 0.05;
	GLfloat att_quadratic = 0.001;


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny 
	glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora 


	pBytes = LoadTGAImage(tekstura, &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


}


//---------------------------  MENU -----------------------------
int model=2;
void menu() {

	generateMatrix();
	switch (model) {
	case 1:
		MyInit("picasso4.tga");
		glRotatef(thetaSpin[0], 1.0, 0.0, 0.0);

		glRotatef(thetaSpin[1], 0.0, 1.0, 0.0);

		glRotatef(thetaSpin[2], 0.0, 0.0, 1.0);
		paintLightenEgg();
		
		break;
	case 2:
		paintLightenEgg();
		break;
	case 3:
		MyInit("podpis.tga");
		glRotatef(thetaSpin[0], 1.0, 0.0, 0.0);

		glRotatef(thetaSpin[1], 0.0, 1.0, 0.0);

		glRotatef(thetaSpin[2], 0.0, 0.0, 1.0);
		paintLightenEgg();
		break;
	case 4:
		MyInit("podpis.tga");
		paintLightenEgg();
		break;
	case 5:
		
		break;
	case 6:
		
		break;


	default:
		break;
	}
}



void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	if (status == 1) {
		theta += delta_x*pix2angle_x;		// modyfikacja kąta obrotu o kąt proporcjonalny
		fi += delta_y*pix2angle_y;			// do różnicy położeń kursora myszy

		if (theta >= 360.0)
			theta = 0.0;
		if (fi >= 360.0)
			fi = 0.0;
	}
	else if (status == 2) {					// jeśli prawy klawisz myszy wciśnięty
		viewerR += 0.1* delta_y;			// modyfikacja polozenia obserwatora(zoom)
		if (viewerR <= 15.0)				// ograniczenie zblizenia
			viewerR = 15.0;
		if (viewerR >= 150.0)				// ograniczenie oddalenia
			viewerR = 150.0;
	}

	//Wspolrzedne obserwatora
	viewer[0] = viewerR * cos(theta) * cos(fi);
	viewer[1] = viewerR * sin(fi);
	viewer[2] = viewerR * sin(theta) * cos(fi);

	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, cos(fi), 0.0);
	// Zdefiniowanie położenia obserwatora

	Axes();

	menu();


	glFlush();
	glutSwapBuffers();

}
void keys(unsigned char key, int x, int y) {
	if (key == '1') model = 1;
	if (key == '2') model = 2;
	if (key == '3') model = 3;
	if (key == '4') model = 4;

	RenderScene(); 
}




void ChangeSize(GLsizei horizontal, GLsizei vertical) {
	pix2angle_x = 360.0*0.1 / (float)horizontal; // przeliczenie pikseli na stopnie
	pix2angle_y = 360.0*0.1 / (float)vertical;
	{
		GLfloat AspectRatio;
		if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
			vertical = 1;
		glViewport(0, 0, horizontal, vertical);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70.0, 1.0, 1.0, 30.0);// Ustawienie parametrów dla rzutu perspektywicznego
		AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
		if (horizontal <= vertical)
			glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
		else
			glOrtho(-7.5*AspectRatio, 7.5*AspectRatio, -7.5, 7.5, 10.0, -10.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}

void Mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		x_pos_old = x;
		y_pos_old = y;
		status = 1;
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		y_pos_old = y;

		status = 2;
	}
	else
		status = 0;
}

void Motion(GLsizei x, GLsizei y) {
	delta_x = x - x_pos_old;
	x_pos_old = x;

	delta_y = y - y_pos_old;
	y_pos_old = y;

	glutPostRedisplay();
}

void main(void) {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Układ współrzędnych 3-D");

	glutKeyboardFunc(keys);
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(spinEgg);
	
	MyInit("picasso4.tga");
	
	glEnable(GL_DEPTH_TEST);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	glutMainLoop();
}
