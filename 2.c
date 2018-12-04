#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <GL/glut.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 20


int n;
int** mat;   //matrica gde horizontalni zidovi
int** mat2;   //matrica gde vertikalni
FILE* izlazni;  //fajl za pracenje gde se kretao kvadrat
char preth='0';
const static float size = 0.09;  /* Velicina kvadrata. */
float x_curr=0.95;
float y_curr=0.8;    /* Tekuce koordinate centra kvadrata. */



/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);


void fajl_procitaj(void){
  
         FILE *file;
    file=fopen("lav1.txt", "r");

     int i, j;
    fscanf(file, "%d", &n);
    
     int na=n+1;

    mat=(int**)malloc(na*sizeof(int*));
    for(i=0;i<na;i++)
        mat[i]=(int*)malloc(n*sizeof(int));
      
    mat2=(int**)malloc(na*sizeof(int*));
    for(i=0;i<na;i++)
        mat2[i]=(int*)malloc(n*sizeof(int)); 


    for(i = 0; i < n+1; i++){
      for(j = 0; j < n; j++) 
      {
       fscanf(file, "%d", &mat[i][j]);
       printf("%d ", mat[i][j]);
      }

    }
          printf("\n");

     for(i = 0; i < n+1; i++){
      for(j = 0; j < n; j++) 
      {
       fscanf(file, "%d", &mat2[i][j]);
       printf("%d ", mat2[i][j]);
      }
     }
   
    fclose(file);
    
}
int main(int argc, char **argv)
{
    fajl_procitaj();
    izlazni=fopen("kretanje.txt","w");
  
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    glutKeyboardFunc(on_keyboard);
    glutDisplayFunc(on_display);
    
    glClearColor(0.75, 0.75, 0.75, 0);
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    return 0;
}


static void on_keyboard(unsigned char key, int x, int y)
{
   
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'w':
        y_curr+=0.05;
         if(preth!='w'){
            preth='w';
            fprintf(izlazni,"%.3f, %.3f\n",x_curr, y_curr);  //dokumentujemo tacku kad promeni pravac, i znamo putanju 
        }
        break;
     case 's':
        y_curr-=0.05;
         if(preth!='s'){
            preth='s';
            fprintf(izlazni,"%.3f, %.3f\n",x_curr, y_curr);
        }
        break;
    case 'a':
        x_curr-=0.05;
         if(preth!='a'){
            preth='a';
            fprintf(izlazni,"%.3f, %.3f\n",x_curr, y_curr);
        }
        break;
    case 'd':
        x_curr+=0.05;
         if(preth!='d'){
            preth='d';
            fprintf(izlazni,"%.3f, %.3f\n",x_curr, y_curr);
        }
        break;
    }
    
    glutPostRedisplay();

}

static void on_reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 10);
}

static void on_display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Crta se kvadrat na odgovarajucoj poziciji. */
    
    glColor3f(0, 0, 1);
    glBegin(GL_POLYGON);
        glVertex3f(x_curr - size / 2, y_curr - size / 2, 0);
        glVertex3f(x_curr + size / 2, y_curr - size / 2, 0);
        glVertex3f(x_curr + size / 2, y_curr + size / 2, 0);
        glVertex3f(x_curr - size / 2, y_curr + size / 2, 0);
    glEnd();
    
   
    float omega=1.8/(float)(n);
    int i, j;
    
     for(i = 0; i < n+1; i++)     ///pravimo lavirint
    {
            for(j = 0; j < n; j++) 
            {
                
            if(mat[i][j]==1){
                    
                    glLineWidth(5); 
                    glColor3f(0, 0.0, 0.0);
                    glBegin(GL_LINES);
                    glVertex3f(-0.9+(j)*omega, 0.9-(i)*omega , 0.0);
                    glVertex3f(-0.9+(j+1)*omega, 0.9-(i)*omega, 0);
                    glEnd();
                }
            }
         
    }
    
     glBegin(GL_LINE_STRIP);
        glVertex3f(-0.9+(n)*omega, 0.5, 0);
        glVertex3f(-0.9+(n)*omega, 0.9-(n)*omega, 0.0);
        glVertex3f(-0.9, 0.9-(n)*omega, 0);
    glEnd();
    for(i = 0; i < n+1; i++)
    {
            for(j = 0; j < n; j++) 
            {
                
            if(mat2[i][j]==1){
                    
                    glLineWidth(5); 
                    glColor3f(0, 0.0, 0.0);
                    glBegin(GL_LINES);
                    glVertex3f(-0.9+(i)*omega, 0.9-(j)*omega , 0.0);
                    glVertex3f(-0.9+(i)*omega, 0.9-(j+1)*omega, 0);
                    glEnd();
                }
            }
         
    }
    //gornja strana
     glColor3f(0, 0, 0);
     glBegin(GL_LINES);
        glVertex3f(-0.9, 0.9, 0.0);
        glVertex3f(-0.9+(n)*omega, 0.9, 0);
    glEnd();
     
   //leva strana
    glBegin(GL_LINES);
        glVertex3f(-0.9, -0.4, 0.0);
        glVertex3f(-0.9, 0.9, 0);
    glEnd();
   

    /* Nova slika se salje na ekran. */
    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}