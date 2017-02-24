#include <windows.h>
#include <glut.h>
#include <cstdlib>
#include <ctime>
#include <fstream>


GLfloat pyramid[4][3] = { //policzone ze wzoru na wysokosc w trojkacei rownobocznym,   
	{ -2.0, -2.0, -2.0 },		// oraz wzorow na pyramid
	{ 2.0, -2.0, -2.0 },
	{ 0.0, -2.0, 1.732 },
	{ 0.0, 1.632, -0.846 }
};

int model = 0;

GLfloat fNormalX=0;
GLfloat fNormalY=0;
GLfloat fNormalZ=0;

static GLfloat thetaX = 0.0;   // kąt obrotu obiektu
static GLfloat thetaY = 0.0;
static GLfloat thetaZ = 0.0;

static GLfloat pix2angleX;     // przelicznik pikseli na stopnie
static GLfloat pix2angleY;

static GLint status = 0;

static float x_pos_old = 0;       // poprzednia pozycja kursora myszy
static float delta_x = 0;
static float y_pos_old = 0;       // poprzednia pozycja kursora myszy
static float delta_y = 0;


static float kolor[37];



typedef float point3[3];
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };
static GLfloat object[] = { 0.0, 0.0, 0.0 };


int level = 2;


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

void randomColor(void) 
{
	for (int i = 0; i < 36; i++) {
		float x = rand() % 100;
		kolor[i] = x / 100;
	}
}



void drawPyramid(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_TRIANGLES);

	glNormal3fv(a);
	glTexCoord2fv(a);
	glVertex3fv(a);

	glNormal3fv(b);
	glTexCoord2fv(d);
	glVertex3fv(b);

	glNormal3fv(c);
	glTexCoord2fv(b);
	glVertex3fv(c);


	glEnd();
	//------------------------------------------
	glBegin(GL_TRIANGLES);

	glNormal3fv(a);
	glTexCoord2fv(a);
	glVertex3fv(a);

	glNormal3fv(d);
	glTexCoord2fv(d);
	glVertex3fv(d);

	glNormal3fv(b);
	glTexCoord2fv(b);
	glVertex3fv(b);

	

	glEnd();
	//------------------------------------------
	glBegin(GL_TRIANGLES);

	glNormal3fv(a);
	glTexCoord2fv(a);
	glVertex3fv(a);

	glNormal3fv(c);
	glTexCoord2fv(d);
	glVertex3fv(c);

	glNormal3fv(d);
	glTexCoord2fv(b);
	glVertex3fv(d);

	glEnd();
	//------------------------------------------
	glBegin(GL_TRIANGLES);

	glNormal3fv(b);
	glTexCoord2fv(a);
	glVertex3fv(b);

	glNormal3fv(d);
	glTexCoord2fv(d);
	glVertex3fv(d);

	glNormal3fv(c);
	glTexCoord2fv(b);
	glVertex3fv(c);

	
	glEnd();
}



void calculatePyramid(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int stopien) {
	GLfloat tab[6][3];
	int j;
	if (stopien > 0) {
		for (j = 0; j < 3; j++) {
			tab[0][j] = (a[j] + b[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
			tab[1][j] = (a[j] + c[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
			tab[2][j] = (b[j] + c[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
			tab[3][j] = (a[j] + d[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
			tab[4][j] = (b[j] + d[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
			tab[5][j] = (c[j] + d[j]) / 2;
		}

		calculatePyramid(a, tab[0], tab[1], tab[3], stopien - 1);
		calculatePyramid(tab[0], b, tab[2], tab[4], stopien - 1);
		calculatePyramid(tab[1], tab[2], c, tab[5], stopien - 1);
		calculatePyramid(tab[3], tab[4], tab[5], d, stopien - 1);

	}
	else {
		drawPyramid(a, b, c, d);
	}
}

void Axes(void) {

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz?tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz?tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz?tek i koniec obrazu osi y

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
//______________________________________OBROT______________________________________

void Mouse(int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		x_pos_old = x;        // przypisanie aktualnie odczytanej pozycji kursora
		y_pos_old = y;
		// jako pozycji poprzedniej
		status = 1;          // wcięnięty został lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		y_pos_old = y;
		status = 2;
	}
	else
		status = 0;          // nie został wcięnięty żaden klawisz
}

void Motion(GLsizei x, GLsizei y) {
	delta_x = x - x_pos_old;
	x_pos_old = x;
	delta_y = y - y_pos_old;
	y_pos_old = y;

	glutPostRedisplay();     // przerysowanie obrazu sceny
}

void RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	if (status == 1) {
		thetaX += delta_x*pix2angleX;
		thetaY += delta_y*pix2angleY;
	}
	else if (status == 2) {
		viewer[2] += delta_y;
		if (viewer[2] < 0) viewer[2] = 0;
	}

	switch (model) {
	case 1:
		object[0] -= 0.1;
		model = 0;
		break;
	case 2:
		object[0] += 0.1;
		model = 0;
		break;
	case 3:
		object[1] += 0.1;
		model = 0;
		break;
	case 4:
		object[1] -= 0.1;
		model = 0;
		break;
	case 5:
		object[0] = 0.0;
		object[1] = 0.0;
		break;


	default:
		break;
	}


	gluLookAt(viewer[0], viewer[1], viewer[2], object[0], object[1], 0.0, 0.0, 1.0, 0.0);


	glRotatef(thetaX, 0.0, 1.0, 0.0);  //obrót obiektu o nowy kąt
	glRotatef(thetaY, 1.0, 0.0, 0.0);
	glRotatef(thetaZ, 0.0, 0.0, 1.0);
	//Axes();
	calculatePyramid(pyramid[0], pyramid[1], pyramid[2], pyramid[3], level);

	glPopMatrix();
	glFlush();
}

void keys(unsigned char key, int x, int y) {
	if (key == 'a') model = 1;
	if (key == 'd') model = 2;
	if (key == 'w') model = 3;
	if (key == 's') model = 4;
	if (key == 'q') model = 5;
	if (key == 'e') model = 6;

	RenderScene(); // przerysowanie obrazu sceny
}

void MyInit() {
	glClearColor(0.0, 0.0, 0.0, 1.0);

	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;


	
	glLoadIdentity();



	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess = { 20.0 };
	GLfloat light_position[] = { 0, 5 ,0, 1.0 };
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
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

	glEnable(GL_CULL_FACE);
	pBytes = LoadTGAImage("P3_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
	free(pBytes);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical) {
	pix2angleX = 360.0 / (float)horizontal;
	pix2angleY = 360.0 / (float)vertical;

	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bieżącej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkości okna okna widoku (viewport) w zależności
	// relacji pomiędzy wysokością i szerokością okna

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

}

int main() {
	randomColor();
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Trojkat Sierpińskiego - wersja 3-D");
	glutPositionWindow(100, 100);
	glutKeyboardFunc(keys);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);

	MyInit();
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutMainLoop();
}