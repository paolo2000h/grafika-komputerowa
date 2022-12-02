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
static GLfloat thetaU_2 = 0.0;
static GLfloat thetaV_2 = 0.0;

static int buttonState = 0;

static int xMousePosition = 0;
static int xMousePositionDifference = 0;
static int yMousePosition = 0;
static int yMousePositionDifference = 0;
static int x2MousePosition = 0;
static int x2MousePositionDifference = 0;
static int y2MousePosition = 0;
static int y2MousePositionDifference = 0;

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
float*** vectorN;
int n; //liczba przedziałów na który zostanie podzielony bok kwadratu jednostkowego dziedziny 
static GLfloat theta[] = { 0.0, 0.0, 0.0 }; // trzy kąty obrotu
int model = 1;  // 1- punkty, 2- siatka, 3 - wypełnione trójkąty
bool light_0 = true;
bool light_1 = true;
bool axes = false;  //false - brak układu współrzędnych, true - włączony
bool movement_0 = true;
bool movement_1 = true;
float* color3 = new float[3]; //tablica kolorów rgb
GLfloat upp = 1.0; //zmienna do obsługi upy w funkcji gluLookAt
GLfloat light0_position[] = { 30.0, 0.0, 8.0, 1.0 };
// położenie źródła
GLfloat light1_position[] = { -30.0, 0.0, 8.0, 1.0 };

/*************************************************************************************/

void Triangles();
void RenderScene(void);



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
	if (key == '1') model = 1; //rysowanie modelu 1 (jajko)
	if (key == '2') model = 2; //rysowanie modelu 2 (czajnik)
	if (key == 'z') light_0=!light_0;
	if (key == 'x') light_1 = !light_1;
	if (key == 'q')axes = !axes; //pokazywanie linii układu współrzędnych (ON/OFF)
	RenderScene(); // przerysowanie obrazu sceny
}


//funkcja przerysowująca scenę
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();
	if (!light_0) {
		glDisable(GL_LIGHT0);
		movement_0 = false;
	}
	else {
		glEnable(GL_LIGHT0);
		movement_0 = true;
	}
	if (!light_1) {
		glDisable(GL_LIGHT1);
		movement_1 = false;
	}
	else {
		glEnable(GL_LIGHT1);
		movement_1 = true;
	}
		if (buttonState == 1 && movement_0)
		{
			thetaU += xMousePositionDifference * xAngleInPixels / 30;
			thetaV += yMousePositionDifference * yAngleInPixels / 30;
			light0_position[0] = radiusStart * cos(thetaU) * cos(thetaV); // x 
			light0_position[1] = radiusStart * sin(thetaV); //y
			light0_position[2] = radiusStart * sin(thetaU) * cos(thetaV); //z
			glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
			
		}
		if (buttonState == 2 && movement_1)
		{
			thetaU_2 += x2MousePositionDifference * xAngleInPixels / 30;
			thetaV_2 += y2MousePositionDifference * yAngleInPixels / 30;
			light1_position[0] = radiusStart * cos(thetaU_2) * cos(thetaV_2); // x 
			light1_position[1] = radiusStart * sin(thetaV_2); //y
			light1_position[2] = radiusStart * sin(thetaU_2) * cos(thetaV_2); //z
			glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		}
		gluLookAt(observerPosition[0], observerPosition[1],
			observerPosition[2], 0.0, 0.0, 0.0, 0.0, upp, 0.0);

		if (axes) //pokazanie linii układu współrzędnych jeśli opcja została włączona
			Axes();
		// Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej
		
		if (model == 1) {
			glTranslatef(0, -5, 0); //obniżenie rysowanego modelu jajka o połowę wysokości linii układu współrzędnych
			Triangles();
		}
		else {
			glutSolidTeapot(3.0);
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

		x2MousePosition = x;
		y2MousePosition = y;
		buttonState = 2;
	}
	else
		buttonState = 0;
}
void MousePosition(GLsizei x, GLsizei y)
{

	xMousePositionDifference = x - xMousePosition;
	yMousePositionDifference = y - yMousePosition;
	x2MousePositionDifference = x - x2MousePosition;
	y2MousePositionDifference = y - y2MousePosition;
	//radiusDifference = y - raduisCurrent;

	xMousePosition = x;
	yMousePosition = y;
	x2MousePosition = x;
	y2MousePosition = y;
	//raduisCurrent = y;

	glutPostRedisplay();
}
/*************************************************************************************/

// Funkcja ustalająca stan renderowania i inicjalizująca tablice punktów w 3d

void MyInit(void)
{
	/*************************************************************************************/

//  Definicja materiału z jakiego zrobiony jest czajnik
//  i definicja źródła światła

/*************************************************************************************/


/*************************************************************************************/
    // Definicja materiału z jakiego zrobiony jest czajnik
	GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	// współczynniki ka =[kar,kag,kab] dla światła otoczenia
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	// współczynniki kd =[kdr,kdg,kdb] światła rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 0.0, 1.0 };
	// współczynniki ks =[ksr,ksg,ksb] dla światła odbitego               
	GLfloat mat_shininess = { 128.0 };
	// współczynnik n opisujący połysk powierzchni


/*************************************************************************************/
// Definicja źródła światła


	
	GLfloat light0_position[] = { 30.0, 0.0, 8.0, 1.0 };
	// położenie źródła
	GLfloat light1_position[] = { -30.0, 0.0, 8.0, 1.0 };

	GLfloat light0_ambient[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light1_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła otoczenia
	// Ia = [Iar,Iag,Iab]

	GLfloat light0_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light1_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie dyfuzyjne Id = [Idr,Idg,Idb]

	GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	// składowe intensywności świecenia źródła światła powodującego
	// odbicie kierunkowe Is = [Isr,Isg,Isb]

	GLfloat att_constant = { 1.0 };
	// składowa stała ds dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_linear = { 0.05 };
	// składowa liniowa dl dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

	GLfloat att_quadratic = { 0.001 };
	// składowa kwadratowa dq dla modelu zmian oświetlenia w funkcji
	// odległości od źródła

/*************************************************************************************/
// Ustawienie parametrów materiału i źródła światła

/*************************************************************************************/
// Ustawienie patrametrów materiału


	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);


	/*************************************************************************************/
	// Ustawienie parametrów źródła

	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);


	/*************************************************************************************/
	// Ustawienie opcji systemu oświetlania sceny

	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
	//glEnable(GL_LIGHT0);     // włączenie źródła o numerze 0
	//glEnable(GL_LIGHT1);     // włączenie źródła o numerze 1
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora

/*************************************************************************************/

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //czyszczenie okna 
	//tworzenie tablic zawierających położenie punktów oraz kolorów
	tab_3d = new float** [n];
	colors = new float** [n];
	vectorN = new float** [n];
	for (int x = 0; x < n; ++x) {
		tab_3d[x] = new float* [n];
		colors[x] = new float* [n];
		vectorN[x] = new float* [n];
		for (int y = 0; y < n; ++y) {
			tab_3d[x][y] = new float[3];
			colors[x][y] = new float[3];
			vectorN[x][y] = new float[3];

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

			GLfloat xu, xv, yu, yv, zu, zv;
			xu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(PI_F * v);
			xv = PI_F * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(PI_F * v);
			yu = 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
			yv = 0;
			zu = (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(PI_F * v);
			zv = -PI_F * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(PI_F * v);

			float vX, vY, vZ;
			vX = yu * zv - zu * yv;
			vY = zu * xv - xu * zv;
			vZ = xu * yv - yu * xv;

			float v_length = sqrt(pow(vX, 2) + pow(vY, 2) + pow(vZ, 2));
			if (v_length == 0) {
				v_length = 1;
			}
			vectorN[i][j][0] = vX / v_length;
			vectorN[i][j][1] = vY / v_length;
			vectorN[i][j][2] = vZ / v_length;

			if (((i + 1) > (n / 2))) { 
			vectorN[i][j][0] *= -1;       
			vectorN[i][j][1] *= -1;       
			vectorN[i][j][2] *= -1; }
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
			glNormal3fv(vectorN[i][k]);
			glVertex3fv(tab_3d[i][k]);
			glNormal3fv(vectorN[i+1][k]);
			glVertex3fv(tab_3d[i + 1][k]);
			glNormal3fv(vectorN[i][k+1]);
			glVertex3fv(tab_3d[i][k + 1]);
			glEnd();
			//dolny trojkat
			glBegin(GL_TRIANGLES);
			glNormal3fv(vectorN[i+1][k]);
			glVertex3fv(tab_3d[i + 1][k]);
			glNormal3fv(vectorN[i+1][k+1]);
			glVertex3fv(tab_3d[i + 1][k + 1]);
			glNormal3fv(vectorN[i][k+1]);
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
	cout << "Obrot swiatla 1 (kolor czerwony) - lewy przycisk myszki" << endl;
	cout << "Obrot swiatla 2 (kolor niebieski) - prawy przycisk myszki" << endl;
	cout << "1 - jajko" << endl;
	cout << "2 - czajnik" << endl;
	cout << "z - swiatlo 1 ON/OFF" << endl;
	cout << "x - swiatlo 2 ON/OFF" << endl;
	cout << "Q - osie ukladu wspolrzednych ON/OFF" << endl;
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