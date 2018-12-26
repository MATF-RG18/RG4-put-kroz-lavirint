#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <unistd.h>
#include <math.h>
float x=1.7;
float y=1.7;
float z=1.7;
int velicina=0;
int pom=0;
static int window_width, window_height;
static int timer_active=0;
int n;
float** pozicije;
int** mat;   //matrica gde horizontalni zidovi
int** mat2;   //matrica gde vertikalni
FILE* izlazni;  //fajl za pracenje gde se kretao kvadrat
const static float size2;  /* Velicina kvadrata. */
float x_curr2=0.90;
float y_curr2=0.90;
const static float size = 0.09;  /* Velicina kvadrata. */
float omega;
float x_curr=0.95;
float y_curr=0.8;  /* Tekuce koordinate centra kvadrata. */
float korak=0.04;

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_timer(int value);

static void on_keyboard2(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_display2(void);
static void move(void);
static void set_normal_and_vertex(float a, float b,float c,float d,float e,float f);
static void kolizija1();
static void kolizija2();
static void kolizija3();
static void kolizija4();


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
       //printf("%d ", mat[i][j]);
      }

    }
       //   printf("\n");

     for(i = 0; i < n+1; i++){
      for(j = 0; j < n; j++) 
      {
       fscanf(file, "%d", &mat2[i][j]);
       //printf("%d ", mat2[i][j]);
      }
     }
   
    fclose(file);
    
}

int main(int argc, char **argv)
{
    izlazni=fopen("kretanje.txt","w");
    fajl_procitaj();
    /* Inicijalizuje se GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Kreira se prozor. */
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Registruju se callback funkcije. */
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    glutDisplayFunc(on_display);

    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0.8, 0.8, 0.8, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glLineWidth(2);

    /* Program ulazi u glavnu petlju. */
    glutMainLoop();

    return 0;
}
static void kolizija1(){
    
  
    float yrazlika=y_curr/omega;
    float xrazlika=x_curr/omega;
    int xcelo=floor(xrazlika);
    int ycelo=floor(yrazlika);
      

    if(y_curr>0 && x_curr>0){
         if(((ycelo+1)*omega-y_curr)<=korak && mat[n/2-ycelo-1][n/2+xcelo]==1)
            y_curr-=korak;
      
    }else if(y_curr<0 && x_curr<0){
        
         if(((-1)*y_curr-((-1)*ycelo-1)*omega)<=korak && mat[n/2+(-1)*ycelo-1][n/2-(-1)*xcelo]==1)
            y_curr-=korak;
    }
     if(y_curr>0 && x_curr<0){
         if(((ycelo+1)*omega-y_curr)<=korak && mat[n/2-ycelo-1][n/2-(-1)*xcelo]==1)
            y_curr-=korak;
      
    }else if(y_curr<0 && x_curr>0){
        
         if(((-1)*y_curr-((-1)*ycelo-1)*omega)<=korak && mat[n/2+(-1)*ycelo-1][n/2+xcelo]==1)
            y_curr-=korak;
    }
   
}
static void kolizija2(){
     int i, j;
     
  
    float yrazlika=y_curr/omega;
    float xrazlika=x_curr/omega;
    int xcelo=floor(xrazlika);
    int ycelo=floor(yrazlika);
     
    if(y_curr>=0 && x_curr>=0){
         if((y_curr-(ycelo)*omega)<=korak && mat[n/2-ycelo][n/2+xcelo]==1)
            y_curr+=korak;
      
    }else if(y_curr<0 && x_curr<0){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr)<=korak && mat[n/2+(-1)*ycelo][n/2-(-1)*xcelo]==1)
            y_curr+=korak;
    }
     if(y_curr>=0 && x_curr<0){
         if((y_curr-ycelo*omega)<=korak && mat[n/2-ycelo][n/2-(-1)*xcelo]==1)
            y_curr+=korak;
      
    }else if(y_curr<0 && x_curr>=0){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr)<=korak && mat[n/2+(-1)*ycelo][n/2+xcelo]==1)
            y_curr+=korak;
    }
   
}

static void kolizija3(){
     int i, j;
  
  
    float yrazlika=y_curr/omega;
    float xrazlika=x_curr/omega;
    int xcelo=floor(xrazlika);
    int ycelo=floor(yrazlika);
 
    if(y_curr>=0 && x_curr>=0){
         if(((xcelo+1)*omega-x_curr)<=korak && mat2[n/2+xcelo+1][n/2-ycelo-1]==1)
            x_curr-=korak;
      
    }else if(y_curr<0 && x_curr<0){
        
         if(((-1)*x_curr-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+(-1)*ycelo-1]==1)
            x_curr-=korak;
    }
     if(y_curr>=0 && x_curr<0){
         if(((-1)*x_curr-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2-ycelo-1]==1)
            x_curr-=korak;
      
    }else if(y_curr<0 && x_curr>=0){
        
         if(((xcelo+1)*omega-x_curr)<=korak && mat2[n/2+xcelo+1][n/2+(-1)*ycelo-1]==1)
            x_curr-=korak;
    }
   
}

static void kolizija4(){
     int i, j;
  
   // printf("%f %f\n", x_curr, y_curr);
   
  
    float yrazlika=y_curr/omega;
    float xrazlika=x_curr/omega;
    int xcelo=floor(xrazlika);
    int ycelo=floor(yrazlika);
  /*    printf("%f %f\n", xrazlika, yrazlika);
      printf("%d %d\n", xcelo, ycelo);*/

    if(y_curr>=0 && x_curr>=0){
         if((x_curr-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2-ycelo-1]==1)
            x_curr+=korak;
      
    }else if(y_curr<0 && x_curr<0){
        
         if((((-1)*xcelo)*omega-(-1)*x_curr)<=korak && mat2[n/2-(-1)*xcelo][n/2+(-1)*ycelo-1]==1)
            x_curr+=korak;
    }
     if(y_curr>=0 && x_curr<0){
         if((((-1)*xcelo)*omega-(-1)*x_curr)<=korak && mat2[n/2-(-1)*xcelo][n/2-ycelo-1]==1)
            x_curr+=korak;
      
    }else if(y_curr<0 && x_curr>=0){
        
         if((x_curr-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2+(-1)*ycelo-1]==1)
            x_curr+=korak;
    }
   
}


static void on_keyboard(unsigned char key, int x, int y)
{
    //korak=0.03;
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'w':
        y_curr+=korak;
        kolizija1();
        fprintf(izlazni,"%.3f %.3f\n",x_curr, y_curr);  //dokumentujemo tacku kad promeni pravac, i znamo putanju 
        
        break;
     case 's':
        y_curr-=korak;
        kolizija2();
        fprintf(izlazni,"%.3f %.3f\n",x_curr, y_curr);
       
        break;
    case 'a':
        x_curr-=korak;
        kolizija4();
        fprintf(izlazni,"%.3f %.3f\n",x_curr, y_curr);
       
        break;
    case 'd':
        x_curr+=korak;
        kolizija3();
        fprintf(izlazni,"%.3f %.3f\n",x_curr, y_curr);
       
        break;
    case 'x':
        fclose(izlazni);
        glutDisplayFunc(on_display2);
        glutKeyboardFunc(on_keyboard2);
        break;
    }
    
    glutPostRedisplay();

}
static void on_keyboard2(unsigned char key, int x, int y)
{
    // korak=0.03;
   
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'w':
        y_curr2+=korak;
        
        break;
     case 's':
        y_curr2-=korak;
        
        break;
    case 'a':
        x_curr2-=korak;
        
        break;
    case 'd':
        x_curr2+=korak;
        
        break;
     case 't':
        /* Pokrece se simulacija. */
        move();
        if (!timer_active) {
            glutTimerFunc(100, on_timer, 0);
            timer_active = 1;
        }
        break;

    case 'y':
        /* Zaustavlja se simulacija. */
        timer_active = 0;
        break;
   
    }
    
    glutPostRedisplay();

}
static void on_timer(int value)
{
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != 0 || pom>velicina)
        return;
    if(pom==velicina){
        x_curr2=pozicije[pom][0];
        y_curr2=pozicije[pom][1];
        timer_active=0;
    }
    x_curr2=pozicije[pom][0];
    y_curr2=pozicije[pom][1];
     glLoadIdentity();

    pom++;        
    glutPostRedisplay();
        if(timer_active)
            glutTimerFunc(100, on_timer, 0);
    
   
}



void move(void){
    
    float tmpx,tmpz;
    int i;
   
    FILE* fajl=fopen("kretanje.txt","r");
    
     pozicije=(float**)malloc(1500*sizeof(float*));
    for(i=0;i<1500;i++)
        pozicije[i]=(float*)malloc(2*sizeof(float));
     i=0;
    while(2==fscanf(fajl, "%f %f", &tmpx, &tmpz)){
    
        pozicije[i][0]=tmpx;
        pozicije[i][1]=tmpz;
        i++;
        
    
    }
    velicina=i;
     for(i=0;i<velicina;i++)
        printf("%f %f\n",pozicije[i][0],pozicije[i][1]);
}

static void on_reshape(int width, int height)
{
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
}



static void on_display2(void)
{
    
    
    /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    GLfloat light_position[] = { 0.5,5,-0.5, 0 };

    /* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };

    /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };

    /* Spekularna boja svetla. */
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.8, 0.8, 0.8, 1 };

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0.2, 0.2, 0.2, 1 };

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 45;

    /* Brise se prethodni sadrzaj prozora. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
    glViewport(0, 0, window_width, window_height);

    /* Podesava se projekcija. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
            60,
            window_width/(float)window_height,
            1, 25);

    /* Podesava se tacka pogleda. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 /*    gluLookAt(
            x,y,z,
            0, 0, 0,
            0, 1, 0
        ); */
    gluLookAt(
            x_curr2+0.4,1.5,y_curr2+0.4,
            x_curr2, 0, -y_curr2,
            0, 1, 0
        );

    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    /* Podesava se viewport. */
   

   
    
    glBegin(GL_LINES);
        glColor3f(1,0,0);
        glVertex3f(0,0,0);
        glVertex3f(10,0,0);
        
        glColor3f(0,1,0);
        glVertex3f(0,0,0);
        glVertex3f(0,10,0);
        
        glColor3f(0,0,1);
        glVertex3f(0,0,0);
        glVertex3f(0,0,10);
    glEnd();
  
  
   omega=1.8/(float)(n);
    int i, j;
    
     for(i = 0; i < n+1; i++)     ///pravimo lavirint i postavljamo normale zbog svetlosti
    {
            for(j = 0; j < n; j++) 
            {
                
            if(mat[i][j]==1){
                    
                    glLineWidth(10); 
                    glColor3f(0.7, 0.0, 0.0);
                    glBegin(GL_QUADS);
                    glNormal3f(0, 0.0, (0.9-(i)*omega));
                    glVertex3f(-0.9+(j)*omega, 0.0, -(0.9-(i)*omega));  ///2-1 vektorski sa 4-1 i 3-4 sa 3-2
                    glNormal3f(0, 0.0, (0.9-(i)*omega));
                    glVertex3f(-0.9+(j+1)*omega,0.0, -(0.9-(i)*omega));
                    glNormal3f(0, omega, (0.9-(i)*omega));
                    glVertex3f(-0.9+(j+1)*omega,omega, -(0.9-(i)*omega));
                    glNormal3f(0, 0.0, (0.9-(i)*omega));
                    glVertex3f(-0.9+(j)*omega, omega, -(0.9-(i)*omega));
                
                   
                    glEnd();
                }
            }
         
    }
    glColor3f(0.7,0.0, 0.0);
  
    for(i = 0; i < n+1; i++)
    {
            for(j = 0; j < n; j++) 
            {
                
            if(mat2[i][j]==1){
                    
                    glLineWidth(5); 
                    glColor3f(0.7, 0.0, 0.0);
                    glBegin(GL_QUADS);
                    glNormal3f(0.9-(i)*omega, 0.0,0.0);
                    glVertex3f(-0.9+(i)*omega, 0.0,-(0.9-(j)*omega));
                    glNormal3f(0.9-(i)*omega, 0.0,0.0);
                    glVertex3f(-0.9+(i)*omega, 0.0, -(0.9-(j+1)*omega));
                    glNormal3f(0.9-(i)*omega, omega,0.0);
                    glVertex3f(-0.9+(i)*omega, omega, -(0.9-(j+1)*omega));
                    glNormal3f(0.9-(i)*omega, omega ,0.0);
                    glVertex3f(-0.9+(i)*omega, omega,-(0.9-(j)*omega));
                    
                    glEnd();
                }
            }
         
    }
 
    glColor3f(0.5, 0.5, 0.5);   
     glBegin(GL_QUADS);
        glNormal3f(0.0, 1,0.0);
        glVertex3f(-1,0,1);
        glNormal3f(0.0, 1,0.0);
        glVertex3f(-1,0,-1);
        glNormal3f(0.0, 1,0.0);
        glVertex3f(1,0,-1);
        glNormal3f(0.0, 1,0.0);
        glVertex3f(1,0,1);
    glEnd();
    glColor3f(0, 0, 1);
    
    glTranslatef(x_curr2, omega/6, -y_curr2);

    glutSolidCube(omega/3);    
   
 
    glutSwapBuffers();
   
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
    
   omega=1.8/(float)(n);
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
   

    /* Nova slika se salje na ekran. */
    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}