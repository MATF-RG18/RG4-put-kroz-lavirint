#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <GL/glut.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 20

const static float size = 0.1;  /* Velicina kvadrata. */
float x_curr=0;
float y_curr=0;    /* Tekuce koordinate centra kvadrata. */
static float v_x, v_y;          /* Komponente vektora brzine kretanja
                                 * kvadrata. */
static int animation_ongoing;   /* Fleg koji odredjuje da li je
                                 * animacija u toku. */

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);

int main(int argc, char **argv)
{
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se funkcije za obradu dogadjaja. */
    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);


    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0.75, 0.75, 0.75, 0);
    glEnable(GL_DEPTH_TEST);
    /* Ulazi se u glavnu petlju. */
    glutMainLoop();

    return 0;
}

static void on_keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:
        /* Zavrsava se program. */
        exit(0);
        break;
    case 'w':
        y_curr+=0.1;
        printf("%c", y);
      //  print(y)
        break;
     case 's':
        y_curr-=0.1;
        break;
    case 'a':
        x_curr-=0.1;
        break;
    case 'd':
        x_curr+=0.1;
        break;
    }
    
    glutPostRedisplay();

}

static void on_reshape(int width, int height)
{
    /* Podesava se viewport. */
    glViewport(0, 0, width, height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 10);
}

static void on_display(void)
{
    /* Postavlja se boja svih piksela na zadatu boju pozadine. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Crta se kvadrat na odgovarajucoj poziciji. */
    glLineWidth(2.5); 
    glColor3f(0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-0.8, 0.08, 0.0);
    glVertex3f(0.8, 0.08, 0);
    glEnd();
    glLineWidth(2.5); 
    glColor3f(0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-0.8, -0.08, 0.0);
    glVertex3f(0.8, -0.08, 0);
    glEnd();
    glColor3f(0, 0, 1);
    glBegin(GL_POLYGON);
        glVertex3f(x_curr - size / 2, y_curr - size / 2, 0);
        glVertex3f(x_curr + size / 2, y_curr - size / 2, 0);
        glVertex3f(x_curr + size / 2, y_curr + size / 2, 0);
        glVertex3f(x_curr - size / 2, y_curr + size / 2, 0);
    glEnd();

    /* Nova slika se salje na ekran. */
    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}