// hid_con.cpp : Defines the entry point for the console application.
//

#include "hid.h"
#include <iostream>
#include "stdafx.h"
#include < stdio.h >
#include < stdlib.h >
#include < GL/glut.h >
#include < GL/gl.h >
#include <windows.h>
#include <string>
#include <sstream>
#include <conio.h>

using namespace std;
	
unsigned long int startTime = GetTickCount();
int values [249];
int i_values [249];
int u_values [249];

char buffer_out[64];
char buffer_in[64];
int u, i;
int actual2=0;				//do mierzenia biezacej próbki
unsigned long long int mocCzynna=0;
	
	int pos=0;
	float	rquad=0;
	const int p=249,q=249,r=1;
	
	const int x=p, y=q, z=r;
	bool tab[x][y][z];
	
	void clear_tab (int table[249])
{
	for (int j=0; j<249; j++)
	{
		tab[j][values[j]][0] = 0;
	}
}

void add_value(int table[249],int data)
{
		for (int j=0; j<248; j++)
		{
			table [j] = table [j+1];
		}
		table[248]=data;
}

void refresh (int table [249],int data)
{
	clear_tab (table);
	add_value (table,data);
	for (int j =0;j<249;j++)
	{
		tab[j][values[j]][0]=1;
	}
}

void Cube(float a, float b, float c)
{
	//przod
	glColor3f(1.0f,1.0f,1.0f);
	glVertex3f( a, b, c );			// dol lewy
	glVertex3f( a+1.0, b, c);		// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,c);	// gora prawy		
	glVertex3f( a, b+1.0f, c);		// gora lewy
	
	/*//gora
	glColor3f(0.5f,0.5f,0.5f);
	glVertex3f(a, b+1.0f, -c);				// dol lewy
	glVertex3f(a+1.0f, b+1.0f, -c);			// dol prawy
	glVertex3f(a+1.0f, b+1.0f, -c-1.0f);	// gora prawy
	glVertex3f(a, b+1.0f,-c-1.0f);			// gora lewy
	
	//prawy bok
	glColor3f(0.4f,0.4f,0.4f);
	glVertex3f( a+1.0f, b, -c );			// dol lewy
	glVertex3f( a+1.0, b, -c-1.0f);			// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,-c-1.0f);		// gora prawy		
	glVertex3f( a+1.0f, b+1.0f, -c);		// gora lewy
	
	//lewy bok
	glColor3f(0.3f,0.3f,0.3f);
	glVertex3f( a, b, -c );			// dol lewy
	glVertex3f( a, b, -c-1.0f);		// dol prawy	
	glVertex3f( a, b+1.0f,-c-1.0f);	// gora prawy		
	glVertex3f( a, b+1.0f, -c);		// gora lewy
	
	//dol
	glColor3f(0.2f,0.2f,0.2f);
	glVertex3f(a, b, -c);			// dol lewy
	glVertex3f(a+1.0f, b, -c);		// dol prawy
	glVertex3f(a+1.0f, b, -c-1.0f);	// gora prawy
	glVertex3f(a, b,-c-1.0f);		// gora lewy
	
	//ty?
	glColor3f(0.1f,0.1f,0.1f);
	glVertex3f( a, b, -c-1.0f );			// dol lewy
	glVertex3f( a+1.0, b, -c-1.0f);		// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,-c-1.0f);	// gora prawy		
	glVertex3f( a, b+1.0f, -c-1.0f);		// gora lewy*/
	
}
void Cube2(float a, float b, float c)
{
	//przod
	glColor3f(0.1f,0.6f,0.6f);
	glVertex3f( a, b+1.0f, -c-1.0f);		// gora lewy
	glVertex3f( a, b, -c-1.0f );			// dol lewy
	glVertex3f( a+1.0, b, -c-1.0f);		// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,-c-1.0f);	// gora prawy		
	
	
	/*//gora
	glColor3f(0.1f,0.5f,0.5f);
	glVertex3f(a, b+1.0f, -c);				// dol lewy
	glVertex3f(a+1.0f, b+1.0f, -c);			// dol prawy
	glVertex3f(a+1.0f, b+1.0f, -c-1.0f);	// gora prawy
	glVertex3f(a, b+1.0f,-c-1.0f);			// gora lewy
	
	//prawy bok
	glColor3f(0.1f,0.4f,0.4f);
	glVertex3f( a+1.0f, b, -c );			// dol lewy
	glVertex3f( a+1.0, b, -c-1.0f);			// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,-c-1.0f);		// gora prawy		
	glVertex3f( a+1.0f, b+1.0f, -c);		// gora lewy
	
	//lewy bok
	glColor3f(0.1f,0.3f,0.3f);
	glVertex3f( a, b, -c );			// dol lewy
	glVertex3f( a, b, -c-1.0f);		// dol prawy	
	glVertex3f( a, b+1.0f,-c-1.0f);	// gora prawy		
	glVertex3f( a, b+1.0f, -c);		// gora lewy
	
	//dol
	glColor3f(0.1f,0.2f,0.2f);
	glVertex3f(a, b, -c);			// dol lewy
	glVertex3f(a+1.0f, b, -c);		// dol prawy
	glVertex3f(a+1.0f, b, -c-1.0f);	// gora prawy
	glVertex3f(a, b,-c-1.0f);		// gora lewy
	
	//ty?
	glColor3f(0.1f,0.1f,0.1f);
	
	glVertex3f( a, b, -c );			// dol lewy
	glVertex3f( a+1.0, b, -c);		// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,-c);	// gora prawy		
	glVertex3f( a, b+1.0f, -c);		// gora lewy
	*/
}
void Cube3(float a, float b, float c)
{
	glColor3f(0.5f,0.6f,0.6f);
	glVertex3f( a, b+1.0f, -c-1.0f);		// gora lewy
	glVertex3f( a, b, -c-1.0f );			// dol lewy
	glVertex3f( a+1.0, b, -c-1.0f);		// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,-c-1.0f);	// gora prawy
}
void Cube4(float a, float b, float c)
{
	glColor3f(0.9f,0.6f,0.6f);
	glVertex3f( a, b+1.0f, -c-1.0f);		// gora lewy
	glVertex3f( a, b, -c-1.0f );			// dol lewy
	glVertex3f( a+1.0, b, -c-1.0f);		// dol prawy	
	glVertex3f(a+1.0f, b+1.0f,-c-1.0f);	// gora prawy
}

void draw_line (float a, float b, float c,float aa, float bb, float cc, float red, float green, float blue)
{
	glColor3f(red,green,blue);
	glVertex3f( a, b, c);
	glVertex3f( aa, bb, cc);
}


void Modul( )
{
	
	char spr;
	string line;
		ReadInputReport(buffer_in);
		//DisplayInputReport(buffer_in);
		float ut=(((float)buffer_in[1])*230)/128;
		u=(int)ut;
		float it = ((float)buffer_in[2]+128)*50;
		i=(int)it/255;
		cout<<"U [V]: "<<u<<"\nI [A]: "<<i<<endl;
		int actual = u*i;

		refresh (values, (actual / 100) + 115);

		int tmp1 = (i);
		int tmp2 = (u);
		refresh (i_values,tmp1);
		refresh(u_values,tmp2);
    
		cout<<"P [W]: "<<actual<<endl;
		unsigned long int actualTime = GetTickCount();
		cout<<"T [ms]: "<<actualTime - startTime<<endl;
		cout<<"Okres [ms]:"<<actualTime - actual2<<endl;

		if (actual > 0)
		{
			mocCzynna = mocCzynna + actual*(actualTime - actual2);
		}
		cout<<"Moc czynna [Wh]: "<<mocCzynna/1000/60/60<<endl;
		actual2 = actualTime;



		Sleep(1);
		system("cls");
		
	for (int k=0; k<248; k++)
	{
		glPushMatrix();
		glTranslatef(-129.0f,0.0f,-100.0f);
		glRotatef (0,0,0,0);
		glBegin(GL_LINES);
		draw_line (k,u_values[k],0,k+1,u_values[k+1],0,0.7,0.3,0.3);
		draw_line (k,values[k],0,k+1,values[k+1],0,0.1,0.2,0.3);
		draw_line (k,i_values[k],0,k+1,i_values[k+1],0,0.5,0.5,0.5);

		glEnd();											// Done Drawing The Quad
		glPopMatrix();
	}

	
	for (int j=0; j<249;j++)
	{
		glPushMatrix();
		glTranslatef(-129.0f,0.0f,-100.0f);
		glRotatef (0,0,0,0);

		glBegin(GL_QUADS);
			Cube (249,249,0);
			Cube (j,0,0);
			Cube (0,j,0);		
			Cube (249,j,0);
			Cube (j,249,0);
			glEnd();	
		glEnd();// Done Drawing The Quad
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-129.0f,-249.0f,-100.0f);
		glRotatef (0,0,0,0);

		glBegin(GL_QUADS);
			Cube (249,249,0);
			Cube (j,0,0);
			Cube (0,j,0);		
			Cube (249,j,0);
			Cube (j,249,0);
			glEnd();	
		glEnd();// Done Drawing The Quad
		glPopMatrix();
	}
	jamp:;
}

void InitGL ( GLvoid )     // Przygotowanie GLa
{
	glShadeModel(GL_SMOOTH);							// Smooth wlaczony
	glClearColor(0.1f, 0.0f, 0.1f, 0.5f);				// Kolor tla = czarny ustawiony
	glClearDepth(1.0f);									// Ustawienie deptha 
	glEnable(GL_DEPTH_TEST);							// uruchomienie deptha jako depth_test
	glDepthFunc(GL_LEQUAL);								// Typ depth'a dla depth_test
	glEnable ( GL_COLOR_MATERIAL );						// wlaczenie color_material
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// ustawienie widoku
}

void Renderuj (  )   
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Czyszczenie ekranu i buffora depth
  glLoadIdentity();										// Reset aktualnego widoku
  
	Modul();											// w³¹czenie rysowania

  glutSwapBuffers ( );									// ustawienie buffora swap w glut'cie
}

//WA¯NE
void reshape ( int width , int height )   // Create The Reshape Function (the viewport)
{


	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window



	//Dostrojenie perspektywy
	gluPerspective(140.0f,(GLfloat)width/(GLfloat)height,0.0f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									
}

void klawiatura ( unsigned char key, int x, int y )  // Funkcja klawiatury dla glut'a
{
  switch ( key ) {
    case 27:        // Escape
      exit ( 0 );   // Wylaczenie programu
      break;        // 
    default:        // przygotowanie na inna obsluge
      break;
  }
}

void strzalki ( int a_keys, int x, int y )  // Obsluga przyciskow strzalek
{
  switch ( a_keys ) {
    case GLUT_KEY_UP:					// Gora
		rquad-=22.5f;
      break;
    case GLUT_KEY_DOWN:				    // Dol
      rquad+=22.5f;
      break;
    default:
      break;
  }
}




//int _tmain(int argc, _TCHAR* argv[])
int main ( int argc, char** argv )
{
	
	for(int j=0; j<249; j++)
	{
		values[j]=0;
	}
	for (int i=0;i<64;i++) {
		buffer_out[i]=0;	
		buffer_in[i]=0;
	}
	buffer_out[1]=1;// pomaranczowa dioda
	//buffer_out[1]=2;// zielona dioda
	//buffer_out[1]=4;// czerwona dioda
	//buffer_out[1]=8;// niebieska dioda

	FindTheHID();
	WriteOutputReport(buffer_out);

	


  glutInit            ( &argc, argv ); 
  glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE );		// Display Mode
  glutCreateWindow    ( "Wizualizacja" );				// Tytul okna
 // glutFullScreen      ( );								// Full Screen
  InitGL			  ( );											// przygotowanie ogl'a
  glutDisplayFunc     ( Renderuj );						// Przypisanie funkcji wyswietlania
  glutReshapeFunc     ( reshape );						// zmiana ksztaltu
  glutKeyboardFunc    ( klawiatura );					// obsluga klawiatury
  glutSpecialFunc     ( strzalki );						// obsluga strzalek
  glutIdleFunc		  ( Renderuj );						// funkcja Renderuj
  glutMainLoop        ( );								// Initialize The Main Loop

	
	return 0;
}

