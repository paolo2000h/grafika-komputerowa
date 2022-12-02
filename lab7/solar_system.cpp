// ReSharper disable CommentTypo

// ReSharper disable StringLiteralTypo
// ReSharper disable CppClangTidyClangDiagnosticImplicitFloatConversion
#include<cstdio>
#include<cstdlib>
#include <GL/glut.h>
#include<cmath>
#include "iostream"
#include "cmath"
#include<Windows.h>

#define M_PI 3.141592653589793238462643383

//kąty nachylenia wzgledem eliptyki
double angleMoon = 0.0, angleEarth = 0.0, angleAsteroid = 0.0,
angleMars = 0.0,
angleMercury = 0.0,
angleVenus = 0.0,
angleJupiter = 0.0,
angleSaturn = 0.0,
angleUranus = 30.0,
angleNeptune = 60.0;
//
GLdouble sx = 0.2, sy = 0.2, sz = 0.2;
int planet1;
GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat blue[] = { 0.0f, 0.0f, 0.9f, 1.0f };
GLfloat er[] = { 0.0f, 5.0f, 0.9f, 1.0f };
GLfloat yellow[] = { 0.7f, 0.2f, 0.0f, 1.0f };
GLfloat grey[] = { 1.0f,1.0f ,1.0f };
GLfloat qAmb[] = { 0.1f, 0.1f, 0.1f, 1.0 };
GLfloat qDif[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat qSpec[] = { .50, .50, .50, 0.10f };
GLfloat qPos[] = { 0, 0, 0, 0.1f };
GLdouble sc[8] = { 0.295, 0.40, 0.50, 0.60, 0.80, 1.0, 1.05, 1.13 };
double ang = 2 * M_PI / 300;
double angular = 2 * M_PI / 50;


GLdouble azimuth[3] = { 0.0, 0.0, 0.0 };
GLdouble elevation[3] = { 0.0, 0.0, 0.0 };
GLdouble pro = 2.0;
GLdouble upp = 1.0;

static GLdouble pix2angle; // przeliczniki pikseli na stopnie
static GLdouble pix2angleY;


static GLint status = 0; // stan klawiszy myszy
// 0 - nie nacianięto żadnego klawisza
// 1 - nacianięty został lewy przycisk myszki
// 2 - nacianięty został prawy klawisz myszki

static double x_pos_old = 0; // poprzednia pozycja kursora myszy
static double delta_x = 0; // rónica pomiędzy pozycjią bieżącą
// i poprzednią kursora myszy
static double y_pos_old = 0;
static double delta_y = 0;
// pozycja obserwatora
static GLdouble viewer[] = { 0.0, 0.0, 2.0 };
static bool pause = false;

using namespace std;
//wykorzystywane do renderowania sfer
GLUquadric* quad;

//tekstury
GLuint textureSun;
GLuint textureMercury;
GLuint textureVenus;
GLuint textureEarth;
GLuint textureMoon;
GLuint textureMars;
GLuint textureJupiter;
GLuint textureSaturn;
GLuint textureUranus;
GLuint textureNeptune;

/*
 * Metoda wczytuje plik z teksturą
 * @param filename nazwa pliku z teksturą
 * @param rozmiar w piksealch tekstury
 * @return teksture
 */
GLuint LoadTextureImageFile(const char* filename, int size)
{
	GLuint texture = 0;
	FILE* file;
	// ReSharper disable once CppEntityAssignedButNoRead
	errno_t err;
	// otworzenie pliku
	if ((err = fopen_s(&file, filename, "rb") != 0))
	{
		const std::string fName = filename;
		cout << "\n\nBlad wczytania tekstury z pliku: " + fName + "!";
		return NULL;
	}


	// alokowanie pamięci
	const int width = size;
	const int height = size;

	BYTE* data = static_cast<BYTE*>(malloc(width * height * 3));

	// wczytanie tekstury 
	fread(data, width * height * 3, 1, file);
	fclose(file);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	//  texture MIP maps  
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_BGRA_EXT, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_COLOR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// zwolnienie pamięci
	free(data);

	return texture;
}

/*Funkcja zwalnia pamięć zajmowaną przez teksturę*/
void FreeCreatedTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

/*Funkcja inicjalizuje oświtlenie*/
void initLighting()
{

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);

	// set up lighting
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat matAmbience[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat matShininess[] = { 20.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbience);

	GLfloat lightAmbient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat lightDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
}

/*
 * Funkcja wczytuje tekstury, ustawia parametry rozmiaru punktu i szerokości linii, inicjalizuje
 */
void myInit()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//ustawienie rozmiaru punktu
	glPointSize(1.0);
	//ustawienie szerokosci linii
	glLineWidth(2.0);

	// start
	glEnable(GL_TEXTURE_2D);

	//inicjalizacja 
	quad = gluNewQuadric();

	//wczytanie tekstur
	textureSun = LoadTextureImageFile("sun.bmp", 1024);
	textureMercury = LoadTextureImageFile("mercury.bmp", 1024);
	textureVenus = LoadTextureImageFile("venus.bmp", 1024);
	textureEarth = LoadTextureImageFile("earth.bmp", 255);
	textureMars = LoadTextureImageFile("mars.bmp", 1024);
	textureJupiter = LoadTextureImageFile("jupiter.bmp", 1024);
	textureSaturn = LoadTextureImageFile("saturn.bmp", 1024);
	textureUranus = LoadTextureImageFile("uranus.bmp", 1024);
	textureNeptune = LoadTextureImageFile("neptune.bmp", 1024);
}



/*Renderowanie orbit*/
void orbit()
{
	glColor3f(0.5, 0.5, 0.5);

	for (int i = 0; i < 8; i++)
	{
		glPushMatrix();
		glDisable(GL_LIGHTING);
		if (i == 5)
		{
			glRotated(45, 1.0, 0.0, 0.0);
		}
		else
		{
			glRotated(63, 1.0, 0.0, 0.0);
		}
		glScaled(sc[i], sc[i], sc[i]);
		glBegin(GL_POINTS);
		double ang1 = 0.0;
		for (int j = 0; j < 300; j++)
		{
			glVertex2d(cos(ang1), sin(ang1));
			ang1 += ang;
		}
		glEnd();
		glPopMatrix();
	}
}

/*
 * Funkcja renderuje ciala niebieskie, teksturuje je, renderuje orbity
 */
void draw(void)
{
	// Czyszczenie okna aktualnym kolorem czyszczacym
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Czyszczenie macierzy bieżącej
	glLoadIdentity();

	// interakcja obrotu i zooma za pomoca myszki
	if (status == 1)
	{
		//obliczenie azimuth i elevation
		azimuth[0] -= delta_x * (pix2angle / 75);
		elevation[0] -= delta_y * (pix2angle / 75);
		// korekta azimuth i elevation 
		if (elevation[0] > 2 * M_PI)
			elevation[0] = 0;
		if (elevation[0] < 0)
			elevation[0] = 2 * M_PI;
	}
	else if (status == 2) //zmiana zooma
	{
		pro -= delta_y * pix2angleY;
	}

	if (status == 1 || status == 2) // zmiana polozenia obserwatora
	{
		viewer[0] = pro * cos(azimuth[0]) * cos(elevation[0]);
		viewer[1] = pro * sin(elevation[0]);
		viewer[2] = pro * sin(azimuth[0]) * cos(elevation[0]);
		if (elevation[0] > M_PI / 2)
			upp = -1.0;
		else
			upp = 1.0;
		if (elevation[0] > M_PI + (M_PI / 2))
			upp = 1.0;
	}
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, upp, 0.0);


	glPushMatrix();
	//wlaczenie bufora glebi obrazu
	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	//przeskalowanie 
	glScaled(sx, sy, sz);
	//ustawienie oswietlenia
	//glLightfv(GL_LIGHT0, GL_POSITION, qPos);
	 GLfloat lightPosition[] = { 0.0, 0.0, 0.0, 1.0 };
	//GLfloat lightPosition[] = { sx, sy, sz, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// render the solar system
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	 glDisable(GL_LIGHTING);
	//nalozenie tekstury
	glBindTexture(GL_TEXTURE_2D, textureSun);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	//renderowanie sfery
	gluSphere(quad, 1.0, 50, 50);
	//emisja swaitla bialego
	//glMaterialfv(GL_FRONT, GL_EMISSION, white);
	glPopMatrix();
	// ---------------------------  Slonce -----------------------
	glScaled(0.2, 0.2, 0.2);

	glPushMatrix();
	glEnable(GL_LIGHTING);
	glRotated(angleMercury, 0.0, 1.0, -0.5);
	glTranslated(1.5, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, textureMercury);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glScaled(0.08, 0.08, 0.08);
	gluSphere(quad, 1, 50, 50);
	glPopMatrix();
	// ---------------------------  Merkury -----------------------
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureVenus);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glRotated(angleVenus, 0.0, 1.0, -0.5);
	glTranslated(2.0, 0.0, 0.0);
	glScaled(0.1, 0.1, 0.1);
	gluSphere(quad, 1.0, 50, 50);
	glPopMatrix();
	// ---------------------------  Wenus -----------------------
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, textureEarth);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glRotated(angleEarth, 0.0, 1.0, -0.5);
	glTranslated(2.5, 0.0, 0.0);
	glScaled(0.23, 0.23, 0.23);
	gluSphere(quad, 1.0, 50, 50);
	glPushMatrix();//earth made
	// ---------------------------  Ziemia -----------------------
	glRotated(angleMoon, 0.0, 0.1, 0.05);
	glTranslated(1.3, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, textureMoon);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glScaled(0.5, 0.5, 0.5);
	gluSphere(quad, 0.5, 50, 50);
	glPopMatrix();
	glPopMatrix(); //moon made

	glScaled(5, 5, 5);
	// ---------------------------  Ksiezyc -----------------------
	orbit();
	// ---------------------------  Orbity -----------------------
	glScaled(0.2, 0.2, 0.2);
	glPushMatrix();
	glRotated(63, 1.0, 0.0, 0.0);
	int  div = 90;
	float siz = 2;
	double scl[4] = { 3.3, 3.4, 3.35, 3.2 };
	for (double j : scl)
	{
		glPushMatrix();
		siz -= 0.3f;
		glPointSize(siz);
		glScaled(j, j, j);
		glBegin(GL_POINTS);
		double ang1 = 0.0 - angleAsteroid;
		const double a = (2 * M_PI) / div;
		for (int i = 0; i < div; i++)
		{
			glColor3fv(grey);
			glVertex2d(cos(ang1), sin(ang1));
			ang1 += a;
		}
		div += 10;
		glEnd();
		glPopMatrix();
	}
	glPopMatrix();
	// ---------------------------  pas planetoid -----------------------

	glPushMatrix();
	glEnable(GL_LIGHTING);
	glRotated(angleMars, 0.0, 1.0, -0.5);
	glTranslated(-3.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, textureMars);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glScaled(0.17, 0.17, 0.17);
	gluSphere(quad, 1, 50, 50);
	glPopMatrix();

	// ---------------------------  Mars -----------------------

	glPushMatrix();
	glRotated(angleJupiter, 0.0, 1.0, -0.5);
	glTranslated(-4.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, textureJupiter);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glScaled(0.5, 0.5, 0.5);
	gluSphere(quad, 1, 50, 50);
	glPopMatrix();

	// ---------------------------  Jowisz  -----------------------
	glPushMatrix();
	glRotated(angleSaturn, 0.0, 1.0, -1.0);
	glTranslated(-5.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, textureSaturn);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glScaled(0.4, 0.4, 0.4);
	gluSphere(quad, 1, 50, 50);
	glPushMatrix();
	// ---------------------------  Saturn -----------------------
	glRotated(45, 1.0, 0.0, 0.0);
	glPointSize(3);
	glColor3f(5.0, 3.0, 1.0);
	glScaled(1.2, 1.2, 1.2);
	glBegin(GL_POINTS);
	double ang1 = 0.0;

	for (int i = 0; i < 50; i++)
	{
		glVertex2d(cos(ang1), sin(ang1));
		ang1 += angular;
	}
	glEnd();

	glPointSize(2);
	glPopMatrix();
	glPopMatrix();

	// ---------------------------  pierscienie saturna -----------------------

	glPushMatrix();
	glRotated(angleUranus, 0.0, 1.0, -0.5);
	glTranslated(5.2, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, textureUranus);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glScaled(0.23, 0.23, 0.23);
	gluSphere(quad, 1, 50, 50);
	glPopMatrix();
	// ---------------------------  Uran -----------------------


	glPushMatrix();
	glRotated(angleNeptune, 0.0, 1.0, -0.5);
	glTranslated(-5.7, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, textureNeptune);
	gluQuadricTexture(quad, 40);
	gluQuadricNormals(quad, GLU_SMOOTH);
	glScaled(0.2, 0.2, 0.2);
	gluSphere(quad, 1, 50, 50);
	glPopMatrix();
	// ---------------------------  Neptun -----------------------


	glPopMatrix();
	glFlush();
}

/*
 * Funkcja wykonuje ruch obiegowy dookola Slonca
 */
void update(int value)
{
	if (!pause)
	{
		angleMoon += 2;
		if (angleMoon > 360)
		{
			angleMoon -= 360;
		}
		angleEarth += 0.7;
		if (angleEarth > 360)
		{
			angleEarth -= 360;
		}
		angleMercury += 2;
		if (angleMercury > 360)
		{
			angleMercury -= 360;
		}
		angleVenus += 0.9;
		if (angleVenus > 360)
		{
			angleVenus -= 360;
		}
		angleMars += 0.5;
		if (angleMars > 360)
		{
			angleMars -= 360;
		}
		angleJupiter += 0.2;
		if (angleJupiter > 360)
		{
			angleJupiter -= 360;
		}
		angleSaturn += 0.1;
		if (angleSaturn > 360)
		{
			angleSaturn -= 360;
		}


		angleUranus += 0.05;
		if (angleUranus > 360)
		{
			angleUranus -= 360;
		}


		angleNeptune += 0.02;
		if (angleNeptune > 360)
		{
			angleNeptune -= 360;
		}


		angleAsteroid += 0.002;
		if (angleAsteroid > 360)
		{
			angleAsteroid -= 360;
		}
	}


	glutPostRedisplay();
	glutTimerFunc(20, update, 0);
}


//Funkcje obslugi muszki i klawiatury
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;
		y_pos_old = y;
		status = 1;
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		y_pos_old = y;
		status = 2;
	}
	else
		status = 0;
}

void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;
	x_pos_old = x;

	delta_y = y - y_pos_old;
	y_pos_old = y;

	glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)
{
	if (key == ' ') pause = (!pause);
	draw();
}

/*
 * Funkcja wywolywana przy zmianie rozmiaru okna
 */
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / static_cast<float>(horizontal);
	pix2angleY = 0.025f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 1.0, 0.1, 300.0);
	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


int main(int argc, char** argv)
{
	cout << "Symulacja ukladu slonecznego.\nDostepne opcje:\n";
	cout << endl;
	cout << "Spacja - wstrzymanie ruchu obiegowego wokol slonca\n";
	cout << "Lewy przycisk myszy - poruszanie kamera\n";
	cout << "Prawy przycisk myszy - przyblizanie kamery\n";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Solar System");


	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(keys);
	glutReshapeFunc(ChangeSize);


	initLighting();
	myInit();
	glutDisplayFunc(draw);
	glutTimerFunc(25, update, 0);
	glutMainLoop();
	return 0;
}
