// kochCurve.cpp : Defines the entry point for the console application.
//

//Martyna Łagożna 218218
//płatek śniegu Kocha

#include "stdafx.h"

float point[2];					//point's coordinates

const float pi = 3.14159;

float lengthOfEdge = 100.00;
float widthOfEdge = 2.00;
int levelGlobal = 3;					//essential for recurence

float startPoint[2] = {0 - lengthOfEdge/2 ,lengthOfEdge/3};
float currentPoint[2];

void changeStartPoint() {
	startPoint[0] = currentPoint[0];
	startPoint[1] = currentPoint[1];
}

void kochCurve(float angle, float length, int level) {
	

	float radians = (float)angle * pi / 180;		//degrees to radian

	currentPoint[0] = startPoint[0] + length * cos(radians) ;	// x = r * cos(fi)
	currentPoint[1] = startPoint[1] + length * sin(radians) ;	// y = r * sin(fi)

	if (level == 0) {
		glVertex2fv(startPoint);
		glVertex2fv(currentPoint);
	}
	else {
		level--;
		length /= 3.0;
		
		kochCurve(angle, length, level);

		changeStartPoint();
		angle += 60;
		kochCurve(angle, length, level);

		changeStartPoint();
		angle -= 120;
		kochCurve(angle, length, level);

		changeStartPoint();
		angle += 60;
		kochCurve(angle, length, level);
	}

}



void renderSonwflake() {
	glBegin(GL_LINES);

	//draw a triangle
	kochCurve(0, lengthOfEdge,levelGlobal);
	changeStartPoint();
	kochCurve(-120, lengthOfEdge,levelGlobal);
	changeStartPoint();
	kochCurve(120, lengthOfEdge,levelGlobal);
	
	glEnd();
}




//Funkcja okreslajaca, co ma byc rysowane
void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	renderSonwflake();

	glFlush();
}

// Funkcja sluzaca do kontroli zachowania proporcji rysowanych obiektow
//Niezaleznie od rozmiarow okna graficznego
void ChangeSize(GLsizei horizontal, GLsizei vertical) {
	GLfloat AspectRatio; // Deklaracja zmiennej AspectRatio okreœlaj¹cej proporcjê wymiarów okna

	if (vertical == 0) // Zabezpieczenie pzred dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical); // Ustawienie wielkoœciokna okna urz¹dzenia (Viewport)
											// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION);			// Okreœlenie uk³adu wspó³rzêdnych obserwatora
	glLoadIdentity();						// Okreœlenie przestrzeni ograniczaj¹cej
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;	// Wyznaczenie wspó³czynnika proporcji okna
															// Do okreœlenia okna obserwatora s³u¿y funkcja glOrtho(...)

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
	else
		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity(); //Okreœlenie uk³adu wspó³rzêdnych
}

//Funkcja ustalajaca stan renderowania
void MyInit(void) {
	//Kolor wnêtrza okna
	glClearColor(0.2f, 0.2f, 0.2f, 0.1f);
	glColor3f(0.7f, 0.2f, 0.3f);
	glLineWidth(widthOfEdge);
}

int main()
{

	// Ustawienie trybu wyswietlania
	// GLUT_SINGLE - pojedynczy bufor wyswietlania
	// GLUT_RGBA - model kolorów RGB
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	//Ustawienie rozmiaru okna
	glutInitWindowSize(800, 600);

	//Stworzenie okna i nadanie tytulu
	glutCreateWindow("Platek sniegu kocha");

	//Przypisanie glownej funkcji renderowania
	glutDisplayFunc(RenderScene);

	//Przypisanie funkcji wywolywanej przy zmianie rozmiaru okna
	glutReshapeFunc(ChangeSize);

	//Wywolanie funkcji ustalajacej stan renderowania
	MyInit();

	//Wywolanie funkcji uruchamiajacej szkielet biblioteki GLUT
	glutMainLoop();

    return 0;
}

