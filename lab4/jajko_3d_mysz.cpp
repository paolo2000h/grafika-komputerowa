
/*************************************************************************************/

//  Modelowanie obiektów 3D - JAJO

/*************************************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include<iostream>
using namespace std;




static GLfloat observerPosition[] = { 0.0, 0.0, 15.0 };

static GLfloat thetaU = 0.0;
static GLfloat thetaV = 0.0;

static int buttonState = 0;

static int xMousePosition = 0;
static int xMousePositionDifference = 0;
static int yMousePosition = 0;
static int yMousePositionDifference = 0;

static GLfloat xAngleInPixels;
static GLfloat yAngleInPixels;

static GLfloat radiusStart = 15.0;
static GLfloat raduisCurrent = 0.0;
static GLfloat radiusDifference = 0.0;

float aspectRatio;
typedef float point3[3]; //tablica punktów 3d dla linii układu współrzędnych
const float  PI_F = 3.14159265358979f; //wartosc PI
float u, v; //punkty siatki nałożonej na dziedzinę parametryczną
float*** tab_3d; //tablica wierzchołków w 3D
float*** colors; //tablica kolorów wierzchołków w 3D
int n; //liczba przedziałów na który zostanie podzielony bok kwadratu jednostkowego dziedziny 
static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // trzy kąty obrotu
int model = 1;  // 1- punkty, 2- siatka, 3 - wypełnione trójkąty
bool axes = false;  //false - brak układu współrzędnych, true - włączony
float* color3 = new float[3]; //tablica kolorów rgb
GLfloat upp = 1.0; //zmienna do obsługi upy w funkcji gluLookAt

/*************************************************************************************/

// Funkcja rysująca osie układu współrzędnych
void Points();
void Lines();
void Triangles();
void RenderScene(void);
void RotateX(int mode);
void ColorChange();

//funkcja odpowiadająca za rysowanie linii układu współrzędnych
void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 }; //położenie max punktu linni X
	point3  x_max = { 5.0, 0.0, 0.0 };  //położenie min punktu linni X
	// początek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 }; //położenie max punktu linni Y
	point3  y_max = { 0.0,  5.0, 0.0 }; //położenie min punktu linni Y
	// początek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 }; //położenie max punktu linni Z
	point3  z_max = { 0.0, 0.0,  5.0 }; //położenie min punktu linni Z
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

/*************************************************************************************/

// Funkcja określająca co ma być rysowane (zawsze wywoływana gdy trzeba
// przerysować scenę)
void keys(unsigned char key, int x, int y)
{
	if (key == '1') model = 1; //rysowanie modelu 1 (punkty)
	if (key == '2') model = 2; //rysowanie modelu 2 (linie)
	if (key == '3') model = 3; //rysowanie modelu 3 (trójkąty)
	if (key == '4') { model = 4; thetaU = 0.0;
	thetaV = 0.0; observerPosition[0] = 0.0; observerPosition[1] = 0.0; observerPosition[2] = 15.0;
	} //czajnik
	if (key == 'w')RotateX(1); //rotacja X
	if (key == 's')RotateX(2); //rotacja X
	if (key == 'd')RotateX(3); //rotacja Y
	if (key == 'a')RotateX(4); //rotacja Y
	if (key == 'q')axes = !axes; //pokazywanie linii układu współrzędnych (ON/OFF)
	if (key == 'e')ColorChange(); //zmiana koloru rysowanego obiektu
	RenderScene(); // przerysowanie obrazu sceny
}
//funkcja odpowiedzialna za obrót w wybranym wcześniej kierunku
void RotateX(int mode) {
	if (model != 4) {
		if (mode == 1)
			thetaV += 0.05f; //zmiana kąta
		if (mode == 2)
			thetaV -= 0.05f;
		if (mode == 3)
			thetaU -= 0.05f;
		if (mode == 4)
			thetaU += 0.05f;
	}
	else if(model==4)
	{
		if (mode == 1)
			thetaV += 1.5; //zmiana kąta
		if (mode == 2)
			thetaV -= 1.5;
		if (mode == 3)
			thetaU -= 1.5;
		if (mode == 4)
			thetaU += 1.5;
	}
}

//funkcja odpowiadająca za zmianę koloru rysowanej figury (kolor RGB)
void ColorChange() {
	color3[0] = rand() % 100 / 100.0; //R - wartość losowa z przedziału <0;1>
	color3[1] = rand() % 100 / 100.0; //G - wartość losowa z przedziału <0;1>
	color3[2] = rand() % 100 / 100.0; //B - wartość losowa z przedziału <0;1>
}

//funkcja przerysowująca scenę
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();
	if (model!=4) {
		if (buttonState == 1)
		{
			// Aktualizacja parametrów kątowych
			thetaU += xMousePositionDifference * xAngleInPixels * 0.1;
			thetaV += yMousePositionDifference * yAngleInPixels * 0.1;

			//zapobieganie przeskokowi jajka w ruchu po osi y
			if (thetaV > 2 * PI_F)
				thetaV = 0;
			if (thetaV < 0)
				thetaV = 2 * PI_F;

			if (thetaV > PI_F / 2)
				upp = -1.0;
			else
				upp = 1.0;
			if (thetaV > PI_F + (PI_F / 2))
				upp = 1.0;
		}
		if (buttonState == 2)
		{
			// Aktualizacja promienia
			radiusStart += radiusDifference * yAngleInPixels;
		}
		// Zmiana pozycji obserwatora zgodnie z równaniami
		observerPosition[0] = radiusStart * cos(thetaU) * cos(thetaV);
		observerPosition[1] = radiusStart * sin(thetaV);
		observerPosition[2] = radiusStart * sin(thetaU) * cos(thetaV);
		gluLookAt(observerPosition[0], observerPosition[1],
			observerPosition[2], 0.0, 0.0, 0.0, 0.0, upp, 0.0);

		// Czyszczenie macierzy bieżącej
		if (axes) //pokazanie linii układu współrzędnych jeśli opcja została włączona
			Axes();
		// Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej
		glTranslatef(0, -5, 0); //obniżenie rysowanego modelu jajka o połowę wysokości linii układu współrzędnych
		//glRotatef(theta[0], 1.0, 0.0, 0.0); //rotacja modelu o kąt theta[0] wokół osi X

		//glRotatef(theta[1], 0.0, 1.0, 0.0); //rotacja modelu o kąt theta[0] wokół osi Y

		//glRotatef(theta[2], 0.0, 0.0, 1.0); //rotacja modelu o kąt theta[0] wokół osi Z
		//rysowanie modelu
		switch (model)
		{
		case 1:Points(); break; //rysowanie punktów
		case 2:Lines(); break; //rysowanie linii
		case 3:Triangles(); break; //rysowanie trójkątów
		default:
			break;
		}
	}
	else if (model == 4) {
		
		aspectRatio = glutGet(GLUT_WINDOW_WIDTH) /
			glutGet(GLUT_WINDOW_HEIGHT);
		// Czyszczenie macierzy bie??cej
		if (buttonState == 1) {
			thetaU += xMousePositionDifference * xAngleInPixels;
			thetaV += yMousePositionDifference * yAngleInPixels;
		}
		else if (buttonState == 2) {
			observerPosition[2] += yMousePositionDifference *0.05f;
			if (observerPosition[2] <= 2.0) {
				observerPosition[2] = 2.0;
			}
			if (observerPosition[2] >= 30.0) {
				observerPosition[2] = 30.0;
			}
		}
		gluLookAt(observerPosition[0], observerPosition[1], observerPosition[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		glRotatef(thetaU, 0.0, 1.0, 0.0);
		glRotatef(thetaV, 1.0, 0.0, 0.0);
		if (axes) //pokazanie linii układu współrzędnych jeśli opcja została włączona
			Axes();
		// Zdefiniowanie położenia obserwatora
		//Axes();
		// Narysowanie osi przy pomocy funkcji zdefiniowanej powyżej

		glColor3f(1.0f, 1.0f, 1.0f);
		// Ustawienie koloru rysowania na biały

		glutWireTeapot(3.0);
		// Narysowanie czajnika
	}
	glFlush();
	// Przekazanie poleceń rysujących do wykonania


	glutSwapBuffers();
	//zamiana bufora wyświetlającego obraz z buforem wykonującym obliczenia obrazu
}

void MouseButtonState(int btn, int state, int x, int y)
{
	
	// Zmiana stanu zmiennej okreslającej naciśnięcie guzika
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// Aktualizacja pozycji kursora
		xMousePosition = x;
		yMousePosition = y;
		buttonState = 1;
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		// Aktualizacja promienia

		raduisCurrent = y;
		buttonState = 2;
	}
	else
		buttonState = 0;
}
void MousePosition(GLsizei x, GLsizei y)
{

	xMousePositionDifference = x - xMousePosition;
	yMousePositionDifference = y - yMousePosition;
	radiusDifference = y - raduisCurrent;

	xMousePosition = x;
	yMousePosition = y;
	raduisCurrent = y;

	glutPostRedisplay();
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania i inicjalizująca tablice punktów w 3d

void MyInit(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //czyszczenie okna 
	//tworzenie tablic zawierających położenie punktów oraz kolorów
	tab_3d = new float** [n];
	colors = new float** [n];
	for (int x = 0; x < n; ++x) {
		tab_3d[x] = new float* [n];
		colors[x] = new float* [n];
		for (int y = 0; y < n; ++y) {
			tab_3d[x][y] = new float[3];
			colors[x][y] = new float[3];

		}
	}
	//dzielenie dziedziny parametrycznej na liczbę kwadratów n

	for (int i = 0; i < n; ++i) {
		//podział u na n równych punktów, n-1 ponieważ 0 też należy do przedziału wartości
		u = (float)i / (n - 1);

		for (int j = 0; j < n; ++j) {
			//podział v na n równych punktów, n-1 ponieważ 0 też należy do przedziału wartości
			v = (float)j / (n - 1);

			//obliczenie punktów rysowanego jajka korzystając z równań parametrycznych
			//x
			tab_3d[i][j][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(PI_F * v);
			//y
			tab_3d[i][j][1] = (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2));
			//z
			tab_3d[i][j][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(PI_F * v);

			//ustawienie kolorów dla każdego wierzchołka
			colors[i][j][0] = (rand() % 1000) / 1000.0;
			colors[i][j][1] = (rand() % 1000) / 1000.0;
			colors[i][j][2] = (rand() % 1000) / 1000.0;




		}
	}

	//zakrycie szwy (widocznej wcześniej linni polączonych trójkątów)
	for (int i = 0; i < n; i++)
	{
		colors[n - i - 1][n - 1][0] = colors[i][0][0];
		colors[n - i - 1][n - 1][1] = colors[i][0][1];
		colors[n - i - 1][n - 1][2] = colors[i][0][2];
	}

}

//rysowanie modelu za pomocą punktów
void Points() {
	glBegin(GL_POINTS);
	glColor3fv(color3);  // ustawienie koloru rysowania
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			glVertex3fv(tab_3d[i][j]);
		}
	}
	glEnd();

}

//rysowanie modelu za pomocą linii
void Lines() {
	glColor3fv(color3); //ustawienie koloru rysowania
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {

			//rysowanie linii pionowych
			if (i + 1 < n) {
				glBegin(GL_LINES);
				glVertex3fv(tab_3d[i][j]);
				glVertex3fv(tab_3d[i + 1][j]);
				glEnd();
			}
			//rysowanie linii poziomych
			if (j + 1 < n) {
				glBegin(GL_LINES);
				glVertex3fv(tab_3d[i][j]);
				glVertex3fv(tab_3d[i][j + 1]);
				glEnd();

			}
			//rysowanie linni ukośnych
			if (i + 1 < n && j + 1 < n) {
				glBegin(GL_LINES);
				glVertex3fv(tab_3d[i][j]);
				glVertex3fv(tab_3d[i + 1][j + 1]);
				glEnd();
			}
			if (i > 0 && j + 1 < n) {
				glBegin(GL_LINES);
				glVertex3fv(tab_3d[i][j]);
				glVertex3fv(tab_3d[i - 1][j + 1]);
				glEnd();
			}
		}


	}
}
//rysowanie modelu za pomocą trójkątów
void Triangles() {

	for (int i = 0; i < n - 1; i++)
	{
		for (int k = 0; k < n - 1; k++)
		{
			//goryn trojkat
			glBegin(GL_TRIANGLES);
			glColor3fv(colors[i][k]);
			glVertex3fv(tab_3d[i][k]);
			glColor3fv(colors[i + 1][k]);
			glVertex3fv(tab_3d[i + 1][k]);
			glColor3fv(colors[i][k + 1]);
			glVertex3fv(tab_3d[i][k + 1]);
			glEnd();
			//dolny trojkat
			glBegin(GL_TRIANGLES);
			glColor3fv(colors[i + 1][k]);
			glVertex3fv(tab_3d[i + 1][k]);
			glColor3fv(colors[i + 1][k + 1]);
			glVertex3fv(tab_3d[i + 1][k + 1]);
			glColor3fv(colors[i][k + 1]);
			glVertex3fv(tab_3d[i][k + 1]);
			glEnd();
		}
	}

}
/*************************************************************************************/

// Funkcja ma za zadanie utrzymanie stałych proporcji rysowanych
// w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal (wysokość i szerokość okna) są
// przekazywane do funkcji za każdym razem gdy zmieni się rozmiar okna.



void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	xAngleInPixels = 360.0 / (float)horizontal;
	yAngleInPixels = 360.0 / (float)vertical;
	//GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  określającej proporcję
	// wymiarów okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkościokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji 
	glLoadIdentity();
	// Czyszcznie macierzy bieżącej            
	aspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	gluPerspective(70, aspectRatio, 1.0, 30.0);
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH),
		glutGet(GLUT_WINDOW_HEIGHT));
	// Wyznaczenie współczynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej
	// przestrzeni ograniczającej pozwalającej zachować właściwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczjącej służy funkcja
	// glOrtho(...)            
	if (horizontal <= vertical)
	{
		glViewport(0, (vertical - horizontal) / 2, horizontal,
			horizontal);
	}
	else
	{
		glViewport((horizontal - vertical) / 2, 0, vertical,
			vertical);
	}
	// Przełączenie macierzy bieżącej na macierz widoku modelu                                   
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Czyszcenie macierzy bieżącej
}

/*************************************************************************************/


void main(void)
{

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(600, 600); //ustawienie wielkości wyświetlanego okna

	glutCreateWindow("Jajko_3D"); //ustawienie nazwy okna
	//obsługa konsoli
	cout << "Podaj N: (liczba przedzialow na ktory zostanie podzielony bok kwadratu jednostkowego dziedziny parametrycznej)" << endl;
	do {

		cin >> n; //pobranie od użytkownika liczby n określającej na ile kwadratów podzielić dziedzinę parametryczną
		if (n < 1) { cout << "Podaj poprawna wartosc, N>0, N jest liczba calkowita" << endl; }
	} while (n < 1);
	system("cls"); //czyszczenie ekranu
	//wyświetlenie menu z opisem klawiszy
	cout << "Obrot obiektu - mysz" << endl;
	cout << "Lewy przycisk myszy - obrot" << endl;
	cout << "Prawy przycisk myszy - przyblizenie" << endl;
	cout << "1 - punkty" << endl;
	cout << "2 - linie" << endl;
	cout << "3 - trojkaty" << endl;
	cout << "4 - czajnik" << endl;
	cout << "W,S - rotacja X" << endl;
	cout << "A,D - rotacja Y" << endl;
	cout << "Q - osie ukladu wspolrzednych ON/OFF" << endl;
	cout << "E - zmiana koloru jajka dla trybu 1,2" << endl;
	//ustawienie koloru początowego
	color3[0] = 0.0;
	color3[1] = 0.0;
	color3[2] = 1.0;
	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Bedzie ona wywoływana za każdym razem
	// gdy zajdzie potrzba przeryswania okna 
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// zazmiany rozmiaru okna      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	glutKeyboardFunc(keys);
	glutMouseFunc(MouseButtonState);
	glutMotionFunc(MousePosition);

	// Włączenie mechanizmu usuwania powierzchni niewidocznych

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}

/*************************************************************************************/
