#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <unistd.h>
#include <math.h>
#include "image.h"
#define FILENAME0 "door.bmp"
#define FILENAME1 "wall.bmp"

float x=1.5;  //x, y, z sluze za poziciju kamere u on_display2
float y=1.6;
float z=1.5;
int velicina=0; //kaze nam velicinu matrice pozicije, tj broj vrsta
int pom=0;    ///pomocna promenljiva kojom se setamo kroz matricu pozicije
static int window_width, window_height;
static int timer_active=0;
int n;              //na koliko polja delimo lavirint
float** pozicije;  //matrica u kojoj cuvamo izabrano kretanje
int** mat;   //matrica gde horizontalni zidovi
int** mat2;   //matrica gde vertikalni
FILE* izlazni;  //fajl za pracenje gde se kretao kvadrat
const static float size2;  /* Velicina kvadrata. */
float x_curr2=0.90;          //tekuce koordinate kocke ovo y je ustvari z u prostoru
float y_curr2=0.90;
const static float size = 0.09;  /* Velicina kvadrata. */
float omega;
float x_curr=0.95;
float y_curr=0.8;  /* Tekuce koordinate centra kvadrata. */
float korak=0.04;    //korak za koji pomeramo kvadrat(kockicu)
int pritisnut[]={0, 0, 0, 0};  //indikatori za tastere koji kaze dal pritisnuti
int drzise[]={0, 0, 0, 0};   //indikatori za tastere koji kazu dal se drze tasteri
static GLuint names[2];  //za teksture
static float poz_X = 0.95*20, poz_Z = -0.6*20;  ///pozicije u on_display3

static float ugao = 132;  //ugao za koji smo se okrenuli oko y ose, tj oko kocke

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y); //2d kretanje u pocetnom prikazu
static void on_timer(int value); ///timer za simulaciju izabranog puta
static void on_timer2(int value); //timer za dijagonalno kretanje
static void on_timer3(int value); //timer za simulaciju u on_sdisplay3
static void on_keyboard2(unsigned char key, int x, int y); ///za on_display2
static void on_keyboard3(unsigned char key, int x, int y); ///za on_display3
static void on_reshape(int width, int height);
static void on_display(void); //2d
static void on_display2(void); //3d
static void on_display3(void); //3d 

static void move(void);     //vadimo pozicije kretanja iz fajla u koji su prethodno upisane
static void kolizija1();   //kolizije u 2d
static void kolizija2();    //kolizije u 3d, razlika samo u promenljivoj
static void kolizija3();    //kolizije u 3d za on_display3, jedva ih uradih, ne preporucujem pregledanje. 

static void initialize(void); //neke fje i teksture se ovde inicijalizuju


void on_keyboard_up(unsigned char key, int x, int y) {  ///registruje kad smo pustili taster

    switch (key) {
    case 27:
        exit(0);
        break;
    case 'w':
        pritisnut[0]=1;  ///pritisli smo
        drzise[0]=0;     ///dal drzali ili ne, ovde se cuva indukator da je sad pusteno
        break;
     case 's':
          pritisnut[1]=1;
          drzise[1]=0;
        break;
    case 'a':
        pritisnut[2]=1;
         drzise[2]=0;
        break;
    case 'd':
        pritisnut[3]=1;
        drzise[3]=0;
        break;
   
    }
    

}


void fajl_procitaj(void){  //alociramo memoriju za matrice u kojima oznaceno dal imam ili nemam zid.
  
    FILE *file;
    file=fopen("lav1.txt", "r");

     int i, j;
    fscanf(file, "%d", &n);
    
     int na=n+1;

    mat=(int**)malloc(na*sizeof(int*));   //2 matrice imam, jedna za horizontalne, jedna za vertikalne zidove
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
    glutKeyboardUpFunc(on_keyboard_up);
  //  glutIgnoreKeyRepeat(1);
    /* Obavlja se OpenGL inicijalizacija. */
    glClearColor(0.8, 0.8, 0.8, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glLineWidth(2);
    initialize();
    /* Program ulazi u glavnu petlju. */
    glutMainLoop();

    return 0;
}
static void kolizija1(){
    
   //xrazlika i yrazlika nam kazu koliko ima polja ispod i levo, kad to zaokruzimo na ceo broj, dobijemo u kom polju se nalazimo u svakom kvadrantu(tj u neku ruku koordinate dsamog polja u kom smo), pritom ovo mogu biti i negativne vrednosti, sto mora da se prilagodi kodu, i u odnosu na ta polja izrazena preko kooordinata, moramo da posetimo odgovarajuca polja matrice u kojoj su zidovi, da bi videli da li tu kocimo ili ne
    float yrazlika=y_curr/omega;
    float xrazlika=x_curr/omega;
    int xcelo=floor(xrazlika);
    int ycelo=floor(yrazlika);
    
    if(y_curr>1)     ///sprecavamo da ispadne sa postolja
       y_curr-=korak;
    else if(y_curr<-1)
       y_curr+=korak;
    else if(x_curr<-1)
       x_curr+=korak;
    else if(x_curr>1)
       x_curr-=korak;
    ///kad se krececmo na gore
    if(y_curr>0 && x_curr>0){  //mozemo u onaj gore da udarimo, pitamo da li onaj sledec i zid ima trivijalnu razliku u odnosu na nasu kockicu i posto smo u prvom kvadrantu, horizontalnim zidovima odgovaraju gornja polovina matrice i desna polovina matrice, sto izrazimo preko toga u kom smo polju. ako smo na primer odmah iznad y ose, u matrici je to 4-0-1=3indeks, tj cetvrta vrsta u matrici kaze dal imamo zid iznad, x ravnomerno se manja u odnosu na matricu. Slicno je za sve dole.  
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
    
    ///kad se krecemo na dole
    
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
    ///kad se krecemo na desno
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
    
    ///kad se krecemo na levo
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
static void kolizija2(){ ///isto kao kolizija1, samo sto su ovde x_curr2 i y_curr2 umesto x_curr i y_curr
    
   float yrazlika=y_curr2/omega;
    float xrazlika=x_curr2/omega;
    int xcelo=floor(xrazlika);
    int ycelo=floor(yrazlika);
       if(y_curr2>1)
       y_curr2-=korak;
    else if(y_curr2<-1)
       y_curr2+=korak;
    else if(x_curr2<-1)
       x_curr2+=korak;
    else if(x_curr2>1)
       x_curr2-=korak;
    ///kad se krececmo na gore
    if(y_curr2>0 && x_curr2>0){
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2-ycelo-1][n/2+xcelo]==1)
            y_curr2-=korak;
      
    }else if(y_curr2<0 && x_curr2<0){
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+(-1)*ycelo-1][n/2-(-1)*xcelo]==1)
            y_curr2-=korak;
    }
     if(y_curr2>0 && x_curr2<0){
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2-ycelo-1][n/2-(-1)*xcelo]==1)
            y_curr2-=korak;
      
    }else if(y_curr2<0 && x_curr2>0){
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+(-1)*ycelo-1][n/2+xcelo]==1)
            y_curr2-=korak;
    }
    
    ///kad se krecemo na dole
    
     if(y_curr2>=0 && x_curr2>=0){
         if((y_curr2-(ycelo)*omega)<=korak && mat[n/2-ycelo][n/2+xcelo]==1)
            y_curr2+=korak;
      
    }else if(y_curr2<0 && x_curr2<0){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+(-1)*ycelo][n/2-(-1)*xcelo]==1)
            y_curr2+=korak;
    }
     if(y_curr2>=0 && x_curr2<0){
         if((y_curr2-ycelo*omega)<=korak && mat[n/2-ycelo][n/2-(-1)*xcelo]==1)
            y_curr2+=korak;
      
    }else if(y_curr2<0 && x_curr2>=0){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+(-1)*ycelo][n/2+xcelo]==1)
            y_curr2+=korak;
    }
    ///kad se krecemo na desno
    if(y_curr2>=0 && x_curr2>=0){
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2-ycelo-1]==1)
            x_curr2-=korak;
      
    }else if(y_curr2<0 && x_curr2<0){
        
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+(-1)*ycelo-1]==1)
            x_curr2-=korak;
    }
     if(y_curr2>=0 && x_curr2<0){
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2-ycelo-1]==1)
            x_curr2-=korak;
      
    }else if(y_curr2<0 && x_curr2>=0){
        
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2+(-1)*ycelo-1]==1)
            x_curr2-=korak;
    }
    
    ///kad se krecemo na levo
    if(y_curr2>=0 && x_curr2>=0){
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2-ycelo-1]==1)
            x_curr2+=korak;
      
    }else if(y_curr2<0 && x_curr2<0){
        
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2+(-1)*ycelo-1]==1)
            x_curr2+=korak;
    }
     if(y_curr2>=0 && x_curr2<0){
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2-ycelo-1]==1)
            x_curr2+=korak;
      
    }else if(y_curr2<0 && x_curr2>=0){
        
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2+(-1)*ycelo-1]==1)
            x_curr2+=korak;
    }
   
}
static void kolizija3(){
    
    /*ok, ovo je stvarno nehumano pregledati, izvinjavam se, nije prekopirano ono gore, jer od spina zavisi sta za nas znaci pravo i unazad, a opet u matrici gde su zidovi je bitno u kom smo kvadrantu da bi videli jel ima zid ili ne ,pa mora na ovoliko slucajeva da se izdvoji. Jedino sto je dobro, je sto ima ponavljanja jer su neke rotacije za pozitivan ugao jednake drugima za negativan. Nisam detaljno komentarisala, jer je slicna ideja kao sa kolizijama1 samo sto je jos komplikovanije jer je obrnuto z ovde i na pocetnom ekranu....**/
    
   x_curr2=-poz_X/20;    ///ovako je zbog transliranja u on_display3
    y_curr2=-poz_Z/20;
    float yrazlika=y_curr2/omega;
    float xrazlika=x_curr2/omega;
    int xcelo=floor(xrazlika);
    int ycelo=floor(yrazlika);
    printf("%f %f %d %d\n", xrazlika, yrazlika,   xcelo, ycelo); 
       if(y_curr2>1.1)
       y_curr2-=korak;
    else if(y_curr2<-1.1)
       y_curr2+=korak;
    else if(x_curr2<-1.1)
       x_curr2+=korak;
    else if(y_curr2>1.1)
       x_curr2+=korak;
    ///kad se krececmo na gore
    
    ///kad je ugao izmedju 270  i 360
     if(y_curr2>0 && x_curr2>0 && ugao>=270 && ugao<360){ 
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2<0 && ugao>=270 && ugao<360){  
        
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=270 && ugao<360){  
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2>0 && ugao>=270 && ugao<360){  
        
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2-(-1)*ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
     ///kad je ugao izmedju -85  i 0
     if(y_curr2>0 && x_curr2>0 && ugao>=-85 && ugao<0){ 
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2<0 && ugao>=-85 && ugao<0){  
        
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=-85 && ugao<0){  //ovaj kao treci
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2>0 && ugao>=-85 && ugao<0){  //prvo ovaj
        
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2-(-1)*ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
    ///kad je ugao izmedju 85 i 160
     if(y_curr2>=0 && x_curr2>=0 && ugao>=85 && ugao<160){  
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;           
      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=85 && ugao<160){
        
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2+xcelo+1][n/2+ycelo]==1)
           poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
     if(y_curr2>=0 && x_curr2<0 && ugao>=85 && ugao<160){                          
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+ycelo]==1)
           poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
      
    }else if(y_curr2<0 && x_curr2>=0 && ugao>=85 && ugao<160){
        
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2+(-1)*ycelo]==1)
           poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
      ///kad je ugao izmedju -160 i -270
     if(y_curr2>=0 && x_curr2>=0 && ugao>=-270 && ugao<-160){  
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2+ycelo]==1)
            poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;           
      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=-270 && ugao<-160){
        
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2+xcelo+1][n/2+ycelo]==1)
           poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
     if(y_curr2>=0 && x_curr2<0 && ugao>=-270&& ugao<-160){                         
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+ycelo]==1)
           poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
      
    }else if(y_curr2<0 && x_curr2>=0 && ugao>=-270 && ugao<-160){
        
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2+(-1)*ycelo]==1)
           poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
   
    if(y_curr2>=0 && x_curr2>=0 && ugao<-270 && ugao>=-360){
         if((y_curr2-(ycelo)*omega)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;            

    }else if(y_curr2<0 && x_curr2<0 && ugao<-270 && ugao>=-360){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }
     if(y_curr2>0 && x_curr2<0 && ugao<-270 && ugao>=-360){ 
         if((y_curr2-ycelo*omega)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao<-270 && ugao>=-360){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }///ova dva kretanja, za ovaj pozitivan i ovaj negativan ugao, su ista
    //kad je ugao izmedju 0 i 85
      if(y_curr2>=0 && x_curr2>=0 && ugao<85 && ugao>=0){
         if((y_curr2-(ycelo)*omega)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;            

    }else if(y_curr2<0 && x_curr2<0 && ugao<85 && ugao>=0){ 
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }
     if(y_curr2>0 && x_curr2<0 && ugao<85 && ugao>=0){  
         if((y_curr2-ycelo*omega)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao<85 && ugao>=0){ 
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }
   
      ///kad je ugao izmedju 160  i 270
     if(y_curr2>0 && x_curr2>0 && ugao>=160 && ugao<270){ 
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
             poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;            
      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=160 && ugao<270){
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+ycelo+1][n/2-(-1)*xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=160 && ugao<270){
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2-(-1)*xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao>=160 && ugao<270){ 
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
    }
      ///kad je ugao izmedju -160 i -85 
     if(y_curr2>0 && x_curr2>0 && ugao>=-160 && ugao<-85){ 
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
             poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;            
      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=-160 && ugao<-85){
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+ycelo+1][n/2-(-1)*xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=-160 && ugao<-85){
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2-(-1)*xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao>=-160 && ugao<-85){ 
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
    }
    
    
    ///kad se krecemo na dole
     ///kad je ugao izmedju 160  i 270
     if(y_curr2>0 && x_curr2>0 && ugao>=160 && ugao<270){ 
         if((y_curr2-(ycelo)*omega)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=160 && ugao<270){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
        
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=160 && ugao<270){
         if(y_curr2-((ycelo)*omega)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao>=160 && ugao<270){ 
        
         if(((-1)*ycelo)*omega-((-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
    }
      ///kad je ugao izmedju -85 i -160
     if(y_curr2>0 && x_curr2>0 && ugao>=-160 && ugao<-85){ 
         if((y_curr2-(ycelo)*omega)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=-160 && ugao<-85){
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
        
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=-160 && ugao<-85){
         if(y_curr2-((ycelo)*omega)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao>=-160 && ugao<-85){ 
        
         if(((-1)*ycelo)*omega-((-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;  
    }
  ///kad je ugao izmedju 0 i 85
    if(y_curr2>=0 && x_curr2>=0 && ugao<85 && ugao>=0){
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;            //y_curr2-=korak;

    }else if(y_curr2<0 && x_curr2<0 && ugao<85 && ugao>=0){ //
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }
     if(y_curr2>0 && x_curr2<0 && ugao<85 && ugao>=0){  
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao<85 && ugao>=0){ 
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
         
    }else if(y_curr2>0 && x_curr2<0 && ugao<85 && ugao>=0){ 
        
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }
     ///kad je ugao izmedju -270 i -360
    if(y_curr2>=0 && x_curr2>=0 && ugao<-270 && ugao>=-360){
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;            

    }else if(y_curr2<0 && x_curr2<0 && ugao<-270 && ugao>=-360){ 
        
         if((((-1)*ycelo)*omega-(-1)*y_curr2)<=korak && mat[n/2+ycelo][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }
     if(y_curr2>0 && x_curr2<0 && ugao<-270 && ugao>=-360){  
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2-(-1)*xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
      
    }else if(y_curr2<0 && x_curr2>0 && ugao<-270 && ugao>=-360){ 
        
         if(((-1)*y_curr2-((-1)*ycelo-1)*omega)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
         
    }else if(y_curr2>0 && x_curr2<0 && ugao<-270 && ugao>=-360){ 
        
         if(((ycelo+1)*omega-y_curr2)<=korak && mat[n/2+ycelo+1][n/2+xcelo]==1)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;  
    }
    
  //kad je ugao izmedju 270 i 360
   if(y_curr2>0 && x_curr2>0 && ugao>=270 && ugao<360){ 
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2<0 && ugao>=270 && ugao<360){  
        
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=270 && ugao<360){  
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2>0 && ugao>=270 && ugao<360){  
        
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2-(-1)*ycelo]==1)
             poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
     //kad je ugao izmedju 0 i -85
     if(y_curr2>0 && x_curr2>0 && ugao>=-85 && ugao<0){ 
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2<0 && ugao>=-85 && ugao<0){  
        
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
     if(y_curr2>0 && x_curr2<0 && ugao>=-85 && ugao<0){  
         if(((-1)*x_curr2-((-1)*xcelo-1)*omega)<=korak && mat2[n/2-(-1)*xcelo+1][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         
    }else if(y_curr2<0 && x_curr2>0 && ugao>=-85 && ugao<0){  
        
         if(((xcelo+1)*omega-x_curr2)<=korak && mat2[n/2+xcelo+1][n/2-(-1)*ycelo]==1)
             poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        
    }
   ///kad je ugao izmedju 85 i 160
     if(y_curr2>=0 && x_curr2>=0 && ugao>=85 && ugao<160){  
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;           
      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=85 && ugao<160){
        
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2+xcelo][n/2+ycelo]==1)
           poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
     if(y_curr2>=0 && x_curr2<0 && ugao>=85 && ugao<160){                        
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2+ycelo]==1)
           poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
      
    }else if(y_curr2<0 && x_curr2>=0 && ugao>=85 && ugao<160){
        
         if((x_curr2-(xcelo)*omega)<=korak && mat2[n/2+xcelo][n/2+(-1)*ycelo]==1)
           poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
    ///kad je ugao izmedju -270 i -160
     if(y_curr2>=0 && x_curr2>=0 && ugao>=-270 && ugao<-160){  
         if((x_curr2-xcelo*omega)<=korak && mat2[n/2+xcelo][n/2+ycelo]==1)
            poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;           
      
    }else if(y_curr2<0 && x_curr2<0 && ugao>=-270 && ugao<-160){
        
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2+xcelo][n/2+ycelo]==1)
           poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
     if(y_curr2>=0 && x_curr2<0 && ugao>=-270 && ugao<-160){                           
         if((((-1)*xcelo)*omega-(-1)*x_curr2)<=korak && mat2[n/2-(-1)*xcelo][n/2+ycelo]==1)
           poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
      
    }else if(y_curr2<0 && x_curr2>=0 && ugao>=-270 && ugao<-160){
        
         if((x_curr2-(xcelo)*omega)<=korak && mat2[n/2+xcelo][n/2+(-1)*ycelo]==1)
           poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
    }
}
static void on_keyboard(unsigned char key, int x, int y)
{
    //korak=0.03;

    switch (key) {  ///postavljamo da drzimo taster, ako ga zapravo ne drzimo, tj pustimo ga, ovo ce se azurirati, postaviti
    case 27:        //na 0 u on_keyboard_up funkciji,radimo po principu, drzi se dok se ne dokaze suprotno. Prilikom pomeranja, proveravamo da li udaramo u zid
        exit(0);
        break;
    case 'w':
        drzise[0]=1;
        y_curr+=korak/2;
        kolizija1();
        glutTimerFunc(100, on_timer2, 1);
        break;
     case 's':
        drzise[1]=1;
        y_curr-=korak/2;
        kolizija1();
       glutTimerFunc(100, on_timer2, 1);
        break;
    case 'a':
        drzise[2]=1;
        x_curr-=korak/2;
        kolizija1();
        glutTimerFunc(100, on_timer2, 1);
        break;
    case 'd':
        drzise[3]=1;
        x_curr+=korak/2;
        kolizija1();
        glutTimerFunc(100, on_timer2, 1);
        break;
    case 'x':
        fclose(izlazni);
        glutDisplayFunc(on_display2);
        glutKeyboardFunc(on_keyboard2);
        break;
    case 'c':
        fclose(izlazni);
        glutDisplayFunc(on_display3);
        glutKeyboardFunc(on_keyboard3);
    }
  
  

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
        kolizija2();
        break;
     case 's':
        y_curr2-=korak;
        kolizija2();
        break;
    case 'a':
        x_curr2-=korak;
        kolizija2();
        break;
    case 'd':
        x_curr2+=korak;
        kolizija2();
        break;
     case 't':
        /* Pokrece se simulacija. */
        move();
        if (!timer_active) {
            pom=0;
            glutTimerFunc(100, on_timer, 0);
            timer_active = 1;
        }
        break;
    case 'y':
        /* Zaustavlja se simulacija. */
        timer_active = 0;
        break;
    case 'c':
        glutDisplayFunc(on_display3);
        glutKeyboardFunc(on_keyboard3);
   
    }
    
    glutPostRedisplay();

}
static void on_keyboard3(unsigned char key, int x, int y)
{
    // korak=0.03;
   
    switch (key) {
    case 27:
        exit(0);
        break;
    case 'w':  //pomeramo se u zavisnosti od spina
        if(ugao>=85 && ugao<160)
            poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
        else if(ugao<85 && ugao>=0)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        else  if(ugao>=160 && ugao<270)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
        else  if(ugao>=270 && ugao<360)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        else if(ugao<=-85 && ugao>-160)
            poz_Z += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
        else if(ugao>-85)
            poz_X += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        else  if(ugao<=-160 && ugao>-270)
            poz_X += cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
        else  if(ugao<=-270 && ugao>-360)
            poz_Z += cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        kolizija3();
        break;
     case 's':
          if(ugao>85 && ugao<160)
            poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
          else if(ugao<=90 && ugao>=0)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
          else if(ugao>=160 && ugao<270)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
        else  if(ugao>=270 && ugao<360)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
         else if(ugao<=-85 && ugao>-160)
            poz_Z -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
        else if(ugao>-85)
            poz_X -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        else  if(ugao<=-160 && ugao>-270)
            poz_X -= cos(ugao * (M_PI/180.0)) - sin(ugao * (M_PI/180.0))*0.1;
        else  if(ugao<=-270 && ugao>-360)
            poz_Z -= cos(ugao * (M_PI/180.0)) + sin(ugao * (M_PI/180.0))*0.1;
        kolizija3();
        break;
    case 'a':
       // x_curr2-=korak;
        ugao -=	2;
        ugao = (int)ugao % 360;
        printf("%f\n", ugao);
        break;
    case 'd':
       // x_curr2+=korak;
         ugao += 2;
        ugao = (int)ugao % 360;
        printf("%f\n", ugao);
        break;
     case 't':
        /* Pokrece se simulacija. */
        move();
        if (!timer_active) {
            pom=0;
            glutTimerFunc(100, on_timer3, 0);
            timer_active = 1;
        }
        break;
    case 'y':
        /* Zaustavlja se simulacija. */
        timer_active = 0;
        break;
     case 'x':
        glutDisplayFunc(on_display2);
        glutKeyboardFunc(on_keyboard2);
    }
    
    glutPostRedisplay();

}


static void on_timer2(int value)
{
    if(value!=1)
        return;
   
    //ako nismo pustili taster, znaci da hocemo da nam se kockica pomera i u tom smeru
        if(drzise[0]==1)
            y_curr+=korak/2;
         if(drzise[1]==1)
            y_curr-=korak/2;
        if(drzise[2]==1)
            x_curr-=korak/2;
        if(drzise[3]==1)
            x_curr+=korak/2;
        fprintf(izlazni,"%.3f %.3f\n",x_curr, y_curr); //upisujemo u fajl
        glutPostRedisplay();
 
   
}

static void on_timer(int value)
{
    //azuriramo pozicije kocke iz matrice
    if (value != 0 || pom>velicina)
        return;
    if(pom==velicina){
        x_curr2=pozicije[pom-1][0];
        y_curr2=pozicije[pom-1][1];
        timer_active=0;
    }
    
    x_curr2=pozicije[pom][0];
    y_curr2=pozicije[pom][1];
 
    pom++;        
    glutPostRedisplay();
        if(timer_active)
            glutTimerFunc(100, on_timer, 0);
    
   
}


static void on_timer3(int value){
    //azuriramo pozicije kocke iz matrice
    if (value != 0 || pom>velicina)
        return;
    if(pom==velicina){
        x_curr2=pozicije[pom-1][0];
        y_curr2=pozicije[pom-1][1];
        timer_active=0;
    }
    
   // x_curr2=pozicije[pom][0];
    ///y_curr2=pozicije[pom][1];
    
    poz_X=-pozicije[pom][0]*20;
    poz_Z=pozicije[pom][1]*20;
     glLoadIdentity();


    pom++;        
    glutPostRedisplay();
        if(timer_active)
            glutTimerFunc(100, on_timer3, 0);
    
   
}



void move(void){ //sluzi za stavljanje svih pozicija kojima smo se kretali u matricu brpoz*2
    
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
   //  for(i=0;i<velicina;i++)
     //   printf("%f %f\n",pozicije[i][0],pozicije[i][1]);
}

static void on_reshape(int width, int height)
{
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
}

int provera(){
    if(poz_X<-17 && poz_Z>=13 && poz_Z<=17)
        return 1;
    
    return 0;
}



static void on_display3(void)
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
            0.1, 25);

    /* Podesava se tacka pogleda. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  
        gluLookAt(
            0.5-poz_X/20, 0.7 , 0.5-poz_Z/20,-poz_X/20, 0,- poz_Z/20, 0, 1, 0
        );

    glTranslatef(-poz_X/20, 0, -poz_Z/20);   ////translacija oko kockice
    glRotatef(ugao, 0, 1, 0);
    glTranslatef(poz_X/20, 0, poz_Z/20);
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
   

   /*
    
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
  */
  
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
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glColor3f(0.5, 0.5, 0.5);   
     glBegin(GL_QUADS);                    ///pravimo kvadar koji je postolje
       // glNormal3f(0.0, 1,0.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1,0,1);
        //glNormal3f(0.0, 1,0.0);
        glTexCoord2f(12, 0);
        glVertex3f(-1,0,-1);
        //glNormal3f(0.0, 1,0.0);
        glTexCoord2f(12, 6);
        glVertex3f(1,0,-1);
        //glNormal3f(0.0, 1,0.0);
        glTexCoord2f(0, 6);
        glVertex3f(1,0,1);
         // glNormal3f(0.0, 1,0.0);
        
        //gornji
        glTexCoord2f(0, 0);
        glVertex3f(-1,0,-1);
        glTexCoord2f(12, 0);
        glVertex3f(-1,-0.2,-1);
        glTexCoord2f(12, 6);
        glVertex3f(1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(1,0,-1);
        
         //desni
        glTexCoord2f(0, 0);
        glVertex3f(1,0,1);
        glTexCoord2f(12, 0);
        glVertex3f(1,-0.2,1);
        glTexCoord2f(12, 6);
        glVertex3f(1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(1,0,-1);
        //levi
         glTexCoord2f(0, 0);
        glVertex3f(-1,0,1);
        glTexCoord2f(12, 0);
        glVertex3f(-1,-0.2,1);
        glTexCoord2f(12, 6);
        glVertex3f(-1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(-1,0,-1);
        
        //donji
        glTexCoord2f(0, 0);
        glVertex3f(1,0,1);
        glTexCoord2f(12, 0);
        glVertex3f(1,-0.2,1);
        glTexCoord2f(12, 6);
        glVertex3f(-1,-0.2,1);
        glTexCoord2f(0, 6);
        glVertex3f(-1,0,1);
        
        //ispod
         glTexCoord2f(0, 0);
        glVertex3f(-1,-0.2,1);
        glTexCoord2f(12, 0);
        glVertex3f(-1,-0.2,-1);
        glTexCoord2f(12, 6);
        glVertex3f(1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(1,-0.2,1);
        
    glEnd();
    glColor3f(0, 0, 1);
     glBindTexture(GL_TEXTURE_2D, 0);
     printf("%f %f\n", -poz_X/20, -poz_Z/20);
    glTranslatef(-poz_X/20, omega/6, 0-poz_Z/20);   ///translacija kockice na apdejtovanu poziciju

    glutSolidCube(omega/3);    
   if(provera()){
       printf("Uspeh :)\n");
       
   }
    glutSwapBuffers();
   
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
     gluLookAt(
            x,y,z,
            x_curr2, 0, -y_curr2,
            0, 1, 0
        ); 
  /* gluLookAt(
            x_curr2-0.4,1.5,y_curr2-0.4,
            x_curr2, 0, -y_curr2,
            0, 1, 0
        );*/

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
   

   /*
    
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
  */
  
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
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, names[1]);
    glColor3f(0.5, 0.5, 0.5);   
     glBegin(GL_QUADS);
       // glNormal3f(0.0, 1,0.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1,0,1);
        //glNormal3f(0.0, 1,0.0);
        glTexCoord2f(12, 0);
        glVertex3f(-1,0,-1);
        //glNormal3f(0.0, 1,0.0);
        glTexCoord2f(12, 6);
        glVertex3f(1,0,-1);
        //glNormal3f(0.0, 1,0.0);
        glTexCoord2f(0, 6);
        glVertex3f(1,0,1);
         // glNormal3f(0.0, 1,0.0);
        
        //gornji
        glTexCoord2f(0, 0);
        glVertex3f(-1,0,-1);
        glTexCoord2f(12, 0);
        glVertex3f(-1,-0.2,-1);
        glTexCoord2f(12, 6);
        glVertex3f(1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(1,0,-1);
        
         //desni
        glTexCoord2f(0, 0);
        glVertex3f(1,0,1);
        glTexCoord2f(12, 0);
        glVertex3f(1,-0.2,1);
        glTexCoord2f(12, 6);
        glVertex3f(1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(1,0,-1);
        //levi
         glTexCoord2f(0, 0);
        glVertex3f(-1,0,1);
        glTexCoord2f(12, 0);
        glVertex3f(-1,-0.2,1);
        glTexCoord2f(12, 6);
        glVertex3f(-1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(-1,0,-1);
        
        //donji
        glTexCoord2f(0, 0);
        glVertex3f(1,0,1);
        glTexCoord2f(12, 0);
        glVertex3f(1,-0.2,1);
        glTexCoord2f(12, 6);
        glVertex3f(-1,-0.2,1);
        glTexCoord2f(0, 6);
        glVertex3f(-1,0,1);
        
        //ispod
         glTexCoord2f(0, 0);
        glVertex3f(-1,-0.2,1);
        glTexCoord2f(12, 0);
        glVertex3f(-1,-0.2,-1);
        glTexCoord2f(12, 6);
        glVertex3f(1,-0.2,-1);
        glTexCoord2f(0, 6);
        glVertex3f(1,-0.2,1);
        
    glEnd();
    glColor3f(0, 0, 1);
     glBindTexture(GL_TEXTURE_2D, 0);
    glTranslatef(x_curr2, omega/6, -y_curr2);

    glutSolidCube(omega/3);    
   
 
    glutSwapBuffers();
   
}

static void initialize(void)
{
    //Ovaj kod i image.c i image.h je sa sedmog casa preuzet !!!!
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    image_read(image, FILENAME0);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(2, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Kreira se druga tekstura. */
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

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
    //glutPostRedisplay();
    glutSwapBuffers();
}