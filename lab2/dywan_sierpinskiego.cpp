#include <windows.h>

#include <gl/gl.h>

#include <gl/glut.h>

/*************************************************************************************/


// Funkcaja określająca, co ma być rysowane
// (zawsze wywoływana, gdy trzeba przerysować scenę)

int level = 4; //poziom rekurencji dywanu
float szerokosc = 200; //dlugosc boku kwadratu
int stop_def = 0;  //stopien deformacji dywanu
int color = 0;  //0 - color OFF, 1 - color ON


//Glowna funkcja wykorzystujaca rekurencje do rysowania kwadratow
//funkcja rysujDywan podana dla 8 roznych polozen w ukladzie odniesienia
void rysujDywan(float x, float y, float szerokosc, int level) {

	if (level > 0) {
		szerokosc /= 3; 
		rysujDywan(x, y, szerokosc, level - 1);
		rysujDywan(x - szerokosc, y, szerokosc, level - 1);
		rysujDywan(x - 2 * szerokosc, y, szerokosc, level - 1);
		rysujDywan(x, y - szerokosc, szerokosc, level - 1);
		rysujDywan(x, y - 2 * szerokosc, szerokosc, level - 1);
		rysujDywan(x - 2 * szerokosc, y - szerokosc, szerokosc, level - 1);
		rysujDywan(x - 2 * szerokosc, y - 2 * szerokosc, szerokosc, level - 1);
		rysujDywan(x - szerokosc, y - 2 * szerokosc, szerokosc, level - 1);

	}
	else {

		glBegin(GL_POLYGON); //rysowanie kwadrarow przy pomocy biblioteki OpenGL


		//okreslenie wartosci przesuniecia scian kwadratow w zaleznosci od
		//wybranego stopnia deformacji (stop_def)
		float def_dyw = ((rand() % 20) * stop_def * 0.01) / (10 * szerokosc); 

		glColor3f((rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color);
		glVertex2f(x + def_dyw, y + def_dyw);
		glColor3f((rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color);
		glVertex2f(x + def_dyw, y - szerokosc + def_dyw);
		glColor3f((rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color);
		glVertex2f(x - szerokosc + def_dyw, y - szerokosc + def_dyw);
		glColor3f((rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color, (rand() % 100) * 0.01 * color);
		glVertex2f(x - szerokosc + def_dyw, y + def_dyw);

		glEnd();

	}

}


void RenderScene(void)

{

	glClear(GL_COLOR_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	rysujDywan(szerokosc / 2, szerokosc / 2, szerokosc, level);

	glFlush();
	// Przekazanie poleceń rysujących do wykonania

}

/*************************************************************************************/

// Funkcja ustalająca stan renderowania



void MyInit(void)

{

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Kolor okna wnętrza okna - ustawiono na szary

}

/*************************************************************************************/

// Funkcja służąca do kontroli zachowania proporcji rysowanych obiektów
// niezależnie od rozmiarów okna graficznego



void ChangeSize(GLsizei horizontal, GLsizei vertical)

// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna

{

	GLfloat AspectRatio;

	// Deklaracja zmiennej AspectRatio określającej proporcję wymiarów okna



	if (vertical == 0)
		// Zabezpieczenie pzred dzieleniem przez 0

		vertical = 1;


	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkościokna okna urządzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)


	glMatrixMode(GL_PROJECTION);
	// Określenie układu współrzędnych obserwatora

	glLoadIdentity();
	// Określenie przestrzeni ograniczającej

	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie współczynnika proporcji okna

	// Gdy okno na ekranie nie jest kwadratem wymagane jest
	// określenie okna obserwatora.
	// Pozwala to zachować właściwe proporcje rysowanego obiektu
	// Do określenia okna obserwatora służy funkcja glOrtho(...)



	if (horizontal <= vertical)

		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

	else

		glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Określenie układu współrzędnych    

	glLoadIdentity();

}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli



void main(void)

{

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wyświetlania
	// GLUT_SINGLE - pojedynczy bufor wyświetlania
	// GLUT_RGBA - model kolorów RGB



	glutCreateWindow("Dywan sierpinskiego");
	// Utworzenie okna i określenie treści napisu w nagłówku okna


	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną (callback)
	// Biblioteka GLUT będzie wywoływała tą funkcję za każdym razem, gdy
	// trzeba będzie przerysować okno


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powyżej) wykonuje wszelkie 
	// inicjalizacje konieczneprzed przystąpieniem do renderowania


	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

}

/*************************************************************************************/


