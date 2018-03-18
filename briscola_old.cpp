#include <iostream>
#include <fstream>
#include <cmath>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
/////////////////								////////////////////////////////
////////////////       PARAMETRI 				////////////////////////////////
///////////////									////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int ngiocatori=200;
int ntest=20;
int ngenerazioni=1500;
int frazione=10;
double frazione2=0.1;
int nmutaz=5;

//////////////////////////////// STRUCT CARTA //////////////////////////////////
struct carta
{
    int n;
    char c;
    
    carta(){}
    
    carta(int n1, char c1)
    {
        n=n1, c=c1;
    }
    
    carta operator = (carta x)
    {
        n=x.n, c=x.c;
        return *this;
    }
    
    bool operator == (carta x)
    {
        if (n==x.n&&c==x.c) return 1;
        else return 0;
    }
    
    bool operator > (carta x) //nella briscola la carta più alta è l'asso, seguita dal 3
    {
        if (n!=1 && n!=3 && x.n!=1 && x.n!=3)
        {
            if (n>x.n) return 1;
            else return 0;
        }
        else
        {
            if ((n==1||n==3)&&(x.n!=1&&x.n!=3)) return 1;
            else
            {
                if (n==1&&x.n==3) return 1;
                else return 0;
            }
        }
    }
    
};
//////////// FINE DELLA STRUCT CARTA ///////////////////////////////


int operator + (int intero, carta x) //aggiunge ad un intero i punti della carta
{
    switch(x.n)
    {
        case (8):
            return intero+2;
        case (9):
            return intero+3;
        case (10):
            return intero+4;
        case (3):
            return intero+10;
        case (1):
            return intero+11;
        default:
            return intero;
    }
}
ostream &operator << (ostream & coutcarta, carta x) //operatore <<
{
    cout<<x.n<<' ';
    if (x.n!=10) cout<<' ';
    cout<<x.c<<' ';
    return coutcarta;
}

void scala(carta m[40], int q) //scala il mazzo di q carte
{
    for (int i=0;i<40-q;++i)
        m[i]=m[i+q];
    for (int i=40-q;i<40;++i)
        m[i]=carta(0, ' ');
}

////////////////////////////////////////////////algoritmo ordine quick sort//////////////////////////

void scambio(int *a, int uno, int due, int * a2)
{
    int box=a[uno];
    a[uno]=a[due];
    a[due]=box;
    
    box=a2[uno];
    a2[uno]=a2[due];
    a2[due]=box;
}

void quick(int *arr,int sx, int dx, int *arr2)
{
    int i=sx, j=dx-1;
    while (i<=j)
    {
        while (arr[j]>arr[sx]) j--;
        while (arr[i]<=arr[sx]&&i<=j) ++i;
        if (i<j) {scambio(arr, i, j, arr2); i++; j--;}
    }
    scambio(arr, sx, j, arr2);
    if ((j-sx)>=2)quick(arr, sx, j, arr2);
    if ((dx-j-1)>=2)quick(arr, j+1, dx, arr2);
}

///////////////fine algoritmo////////////////////////


//////////////////// FUNZIONI RELATIVE ALLA CREAZIONE DI GIOCATORI //////////////////////////////////////////////////

void creo_giocatore(int**** gioc) //crea a caso un dna, inserendo per ogni combinazione di 4 carte (1 sul tavolo, 3 in mano) un numero da 0 a 2
{
    for (int i=0;i<7;++i)
        for (int j=0;j<12;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    gioc[i][j][k][l]=rand()%3;
}

void crea_nemico(int *****nemico, int *****nemico_f) //pone nemico uguale a nemico_f
{
    for (int m=0;m<ntest;++m)
        for (int i=0;i<7;++i)
            for (int j=0;j<12;++j)
                for (int k=0;k<12;++k)
                    for (int l=0;l<12;++l)
                        nemico[m][i][j][k][l]=nemico_f[m][i][j][k][l];
}

void crea_nemico_f(int *****nemico_f, int *****gioc, int *fitness) //crea nemico_f prendendo i giocatori gioc più bravi (ordinati con l'algoritmo quick sort)
{
    int *ordinati=new int [ngiocatori];
    for (int m=0;m<ngiocatori;++m)
        ordinati[m]=m;
    
    quick(fitness, 0, ngiocatori, ordinati);
    
    for (int m=0;m<ntest;++m)
        for (int i=0;i<7;++i)
            for (int j=0;j<12;++j)
                for (int k=0;k<12;++k)
                    for (int l=0;l<12;++l)
                        nemico_f[m][i][j][k][l]=gioc[ordinati[ngiocatori-m-1]][i][j][k][l];
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////											///////////////////////////
/////////////////////////// FUNZIONI E VARIABILI RELATIVE AL GIOCO ///////////////////////////
//////////////////////////											/////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


carta mazzo[40], tavolo[2], mano[2][3], briscola, vuota(0, ' ');
int punteggio[2];

void creomazzo() //mischia il mazzo
{
    int ordinati[40], acaso[40];
    for (int m=0;m<40;++m)
        ordinati[m]=m;
    
    for (int m=0;m<40;++m)
        acaso[m]=rand();
    
    quick(acaso, 0, 40, ordinati);
    
    int ncarta, seme;
    char ccarta;
    for (int m=0;m<40;++m)
    {
        
        ncarta=ordinati[m]%10+1;
        seme=ordinati[m]/10;
        switch (seme)
        {
            case (0): ccarta='d'; break;
            case (1): ccarta='c'; break;
            case (2): ccarta='s'; break;
            case (3): ccarta='b'; break;
        }
        mazzo[m]=carta(ncarta, ccarta);
    }
}

void distribuiscicarte() //distribuisce le carte come all'inizio della partita
{
    mano[0][0]=mazzo[0];
    mano[0][1]=mazzo[2];
    mano[0][2]=mazzo[4];
    mano[1][0]=mazzo[1];
    mano[1][1]=mazzo[3];
    mano[1][2]=mazzo[5];
    briscola=mazzo[6];
    scala(mazzo, 7);
    mazzo[33]=briscola;
}

void prende (int who)
{
    //aggiunge i punti delle carte prese al punteggio totale
    punteggio[who]=punteggio[who]+tavolo[0];
    punteggio[who]=punteggio[who]+tavolo[1];
    
    //pesca
    int nonwho;
    if (who==0) nonwho=1;
    else nonwho=0;
    for (int i=0;i<3;++i)
    {
        if (mano[who][i]==vuota)
        {
            mano[who][i]=mazzo[0];
        }
        if (mano[nonwho][i]==vuota)
        {
            mano[nonwho][i]=mazzo[1];
        }
    }
    
    //metto a posto il mazzo e il tavolo
    scala(mazzo, 2);
    tavolo[0]=vuota;
    tavolo[1]=vuota;
}

void calcola_primo(bool &pr) //controlla chi prende e quindi chi giocherà per primo la mano successiva
{
    if (tavolo[0].c==tavolo[1].c)
    {
        if (tavolo[0]>tavolo[1])
        {
            prende(0);
            pr=0;
        }
        else
        {
            prende(1);
            pr=1;
        }
    }
    else
    {
        if (tavolo[0].c==briscola.c)
        {
            prende(0);
            pr=0;
        }
        else
        {
            if (tavolo[1].c==briscola.c)
            {
                prende(1);
                pr=1;
            }
            else
            {
                if (pr==0)
                {
                    prende(0);
                    pr=0;
                }
                else
                {
                    prende(1);
                    pr=1;
                }
            }
        }
    }
}

int giocaIA(bool chi, int ****dna) //in base alle carte controlla il dna e sceglie la giusta carta da giocare
{
    //inizia controllando se ha meno di 3 carte
    int contvuote=0;
    for (int i=0;i<3;++i)
        if (mano[chi][i]==vuota) ++contvuote;
    
    if (contvuote==2)//se ne ha una sola la gioca
    {
        for (int i=0;i<3;++i)
        {
            if (mano[chi][i]==vuota) {}
            else return i;
        }
    }
    if (contvuote==1)//se ne ha due, gioca la prima (piccolo difetto dell'algoritmo)
    {
        for (int i=0;i<3;++i)
        {
            if (mano[chi][i]==vuota) {}
            else return i;
        }
    }
    
    //se ha 3 carte
    int nonchi;
    if (chi==0) nonchi=1;
    else nonchi=0;
    
    int a, b[3];
    
    if (tavolo[nonchi]==vuota) a=0;
    else
    {
        if (tavolo[nonchi].c==briscola.c)
        {
            if (tavolo[nonchi].n==1||tavolo[nonchi].n==3) a=6;
            else
            {
                if (tavolo[nonchi].n==8||tavolo[nonchi].n==9||tavolo[nonchi].n==10) a=5;
                else a=4;
            }
        }
        else
        {
            if (tavolo[nonchi].n==1||tavolo[nonchi].n==3) a=3;
            else
            {
                if (tavolo[nonchi].n==8||tavolo[nonchi].n==9||tavolo[nonchi].n==10) a=2;
                else a=1;
            }
            
        }
    }
    
    for (int i=0;i<3;++i)
    {
        if (a==0)
        {
            if (mano[chi][i].c==briscola.c)
            {
                if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=6;
                else
                {
                    if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=5;
                    else b[i]=4;
                }
            }
            else
            {
                if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=3;
                else
                {
                    if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=2;
                    else b[i]=1;
                }
                
            }
        }
        else
        {
            if (a<4)
            {
                if (mano[chi][i].c==briscola.c)
                {
                    if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=11;
                    else
                    {
                        if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=10;
                        else b[i]=9;
                    }
                }
                else
                {
                    if (mano[chi][i].c==tavolo[nonchi].c)
                    {
                        if (mano[chi][i]>tavolo[nonchi])
                        {
                            if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=5;
                            else
                            {
                                if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=4;
                                else b[i]=3;
                            }
                        }
                        else
                        {
                            if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=8;
                            else
                            {
                                if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=7;
                                else b[i]=6;
                            }
                        }
                    }
                    else
                    {
                        if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=2;
                        else
                        {
                            if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=1;
                            else b[i]=0;
                        }
                    }
                }
            }
            else
            {
                if (mano[chi][i].c==briscola.c)
                {
                    if (mano[chi][i]>tavolo[nonchi])
                    {
                        if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=6;
                        else
                        {
                            if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=5;
                            else b[i]=4;
                        }
                    }
                    else
                    {
                        if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=9;
                        else
                        {
                            if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=8;
                            else b[i]=7;
                        }
                    }
                }
                else
                {
                    if (mano[chi][i].n==1||mano[chi][i].n==3) b[i]=3;
                    else
                    {
                        if (mano[chi][i].n==8||mano[chi][i].n==9||mano[chi][i].n==10) b[i]=2;
                        else b[i]=1;
                    }
                }
            }
        }
    }
    
    //ordina le carte che ha in mano (in modo da considerare le mani 3 0 7 e 7 3 0 uguali)
    int b2[3], box;
    for (int i=0;i<3;++i)
        b2[i]=b[i];
    for (int i=0;i<2;++i)
        for (int j=i+1;j<3;++j)
        {
            if (b2[i]<b2[j])
            {
                box=b2[i];
                b2[i]=b2[j];
                b2[j]=box;
            }
        }
    
    //sceglie il giusto output in base al dna
    for (int i=0;i<3;++i)
        if (b[i]==b2[dna[a][b2[0]][b2[1]][b2[2]]])      return i;
    
    return -1;
}

void partita(bool primo, int ****gioc, int ****nemico) //fa svolgere la vera partita, turno per turno
{
    int uno, due;
    for (int turno=0;turno<20;++turno)
    {
        if (primo==0)
        {
            //controlla le IA, mette la carta selezionata sul tavolo e la toglie dalla mano
            uno=giocaIA(primo, gioc);
            tavolo[primo]=mano[primo][uno];
            due=giocaIA(primo+1, nemico);
            tavolo[primo+1]=mano[primo+1][due];
            
            mano[primo][uno]=vuota;
            mano[primo+1][due]=vuota;
        }
        else
        {
            uno=giocaIA(primo, nemico);
            tavolo[primo]=mano[primo][uno];
            due=giocaIA(primo-1, gioc);
            tavolo[primo-1]=mano[primo-1][due];
            
            mano[primo][uno]=vuota;
            mano[primo-1][due]=vuota;
        }
        calcola_primo(primo);
    }
}


///////////////////////////////////////////////////////////////////////////////////////
////////////////////////							///////////////////////////////////
//////////////////////// FUNZIONI DI EVOLUZIONE 	///////////////////////////////////
////////////////////////							///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////



void calcolo_fitness(int punteggio[2], int &fit)//modifica la fitness in base al risultato della partita
{
    if (punteggio[0]>100)
    {
        fit+=2;
    }
    else
    {
        if (punteggio[0]>60)
        {
            fit+=1;
        }
        else
        {
            if (punteggio[0]<20)
            {
                fit-=2;
            }
            else
            {
                if (punteggio[0]<60)
                {
                    fit-=1;
                }
                else
                {
                    //cout<<"Pareggio!\n";
                }
            }
        }
    }
    
}

void Gioca(int ****gioc, int ****nemico, int &fit) //tutte le operazioni necessarie per la partita
{
    punteggio[0]=0, punteggio[1]=0;
    for (int i=0;i<2;++i)
        tavolo[i]=vuota;
    creomazzo();
    distribuiscicarte();
    bool primo=rand()%2;
    partita(primo, gioc, nemico);
    calcolo_fitness(punteggio, fit);
}

void Gioca_test(int *****gioc, int *****test) //tutti i giocatori giocano ntest partite contro i test e vengono mostrati i risultati
{
    int v=0, p=0, s=0;
    for(int h=0; h<ngiocatori; h++)
        for (int m=0; m<ntest;m++)
        {
            punteggio[0]=0, punteggio[1]=0;
            for (int i=0;i<2;++i)
                tavolo[i]=vuota;
            creomazzo();
            distribuiscicarte();
            bool primo=rand()%2;
            partita(primo, gioc[h], test[m]);
            
            if (punteggio[0] < punteggio[1]) s++;
            if (punteggio[0] == punteggio[1]) p++;
            if (punteggio[0] > punteggio[1]) v++;
        }
    //cout << "vittorie   " << v << "         pareggi " << p << "    sconfitte    " << s << endl;
    cout<<v<<'\t'<<p<<'\t'<<s<<endl;
}

void muta(int ****genoma) //vengono mutati nmutaz geni a caso (con possibilità di rimanere uguali)
{
    for (int i=0; i<nmutaz;i++)
        genoma[rand()%7][rand()%12][rand()%12][rand()%12]=rand()%3;
}

void cross_over(int ****genoma1,int ****genoma2) //scambia una parte del genoma1 con una del genoma2
{
    int box, confine=random()%(12*12*12*7);
    for (int i=0;i<confine;++i)
    {
        int ii=(i/(144*12))%7;
        int j=(i/144)%12;
        int k=(i/12)%12;
        int l=i%12;
        box=genoma1[ii][j][k][l];
        genoma1[ii][j][k][l]=genoma2[ii][j][k][l];
        genoma2[ii][j][k][l]=box;
    }
}

void crea_new_gen(int *****gioc, int *fitness, int *****gioc_t) //primo metodo di evoluzione
{
    int *ordinati=new int [ngiocatori];
    
    for (int m=0;m<ngiocatori;++m)
        ordinati[m]=m;
    
    quick(fitness, 0, ngiocatori, ordinati);
    int h;
    for (int m=0;m<ngiocatori/frazione;++m) //copia i primi ngiocatori/frazione giocatori gioc in gioc_t fino a riempirlo
        for (int i=0;i<7;++i)
            for (int j=0;j<12;++j)
                for (int k=0;k<12;++k)
                    for (int l=0;l<12;++l)
                    {
                        h=m;
                        while(h<ngiocatori)
                        {
                            gioc_t[h][i][j][k][l]=gioc[ordinati[ngiocatori-m-1]][i][j][k][l];
                            h+=(ngiocatori/frazione);}
                    }
    
    for (int m=0;m<ngiocatori/frazione;++m) //modifica il vettore gioc mettendoci i primi ngiocatori/frazione giocatori di gioc_t
        for (int i=0;i<7;++i)
            for (int j=0;j<12;++j)
                for (int k=0;k<12;++k)
                    for (int l=0;l<12;++l)
                        gioc[m][i][j][k][l]=gioc_t[m][i][j][k][l];
    
    int *ordinati2=new int [ngiocatori- ngiocatori/frazione];
    for (int m=0;m<ngiocatori - ngiocatori/frazione;++m) ordinati2[m]=m;
    
    int *caso = new int [ngiocatori- ngiocatori/frazione];
    for (int m=0;m<ngiocatori- ngiocatori/frazione; m++) caso[m]= rand();
   	
   	quick(caso, 0, ngiocatori- ngiocatori/frazione, ordinati2);
    
    for (int m=ngiocatori/frazione ;m<ngiocatori;++m) //mescola i restanti giocatori di gioc_t e li pone in ordine in gioc
    {
        for (int i=0;i<7;++i)
            for (int j=0;j<12;++j)
                for (int k=0;k<12;++k)
                    for (int l=0;l<12;++l)
                    {
                        gioc[m][i][j][k][l]=gioc_t[ ordinati2[m - ngiocatori/frazione] ][i][j][k][l];
                    }
    }
    
    for (int i=ngiocatori/frazione; i<ngiocatori;i+=2) cross_over(gioc[i],gioc[i+1]);
    for (int i=ngiocatori/frazione; i<ngiocatori;++i) muta(gioc[i]);
}

void crea_new_gen2(int *****gioc, int *fitness) //secondo metodo di evoluzione (consigliato)
{
    int* ordinati= new int[ngiocatori];
    
    for (int m=0;m<ngiocatori;++m)
        ordinati[m]=m;
    
    quick(fitness, 0, ngiocatori, ordinati);
    
    for (int m=0;m<ngiocatori*frazione2;++m) //vengono copiati i migliori ngiocatori*frazione2 al posto dei peggiori
        for (int i=0;i<7;++i)
            for (int j=0;j<12;++j)
                for (int k=0;k<12;++k)
                    for (int l=0;l<12;++l)
                    {
                        gioc[ordinati[m]][i][j][k][l]=gioc[ordinati[ngiocatori-m-1]][i][j][k][l];
                    }
    for (int i=0; i<ngiocatori;i+=2) cross_over(gioc[i],gioc[i+1]);
    for (int i=0; i<ngiocatori;++i) muta(gioc[i]);
}

/////////////////////////// FUNZIONI DI I/O ////////////////////////////////////////////////

void scrivi_file(int *****gioc, int *fitness)// scirve su un file (in binario) il giocatore più bravo dell'ultima generazione
{
    int fitness_piubravo=-100, piubravochi=-1;
    for (int i=0;i<ngiocatori;++i)
    {
        if (fitness[i]>fitness_piubravo)
        {
            fitness_piubravo=fitness[i];
            piubravochi=i;
        }
    }
    ofstream of("briscola_prova6");
    
        for (int ii=0;ii<7;++ii)
        for (int j=0;j<12;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    of.put(gioc[piubravochi][ii][j][k][l]);
}

//////////////////////////////////// MAIN ////////////////////////////////////////////////////

int main()
{
    bool scegli_crea_new_gen;
    bool scegli_parametri;
    cout <<"Vuoi settare i parametri? [0=no, 1=sì]\n";
    cin>>scegli_parametri;
    if (scegli_parametri==1)
    {
        cout<<"Inserisci il numero dei giocatori: ";    cin>>ngiocatori;
        cout<<"Inserisci il numero dei test: ";    cin>>ntest;
        cout<<"Inserisci il numero delle generazioni: ";    cin>>ngenerazioni;
        cout<<"Inserisci il numero di geni da mutare per ogni giocatore: "; cin>>nmutaz;
        cout<<"Scegli il metodo di evoluzione [0 / 1] :"; cin>>scegli_crea_new_gen;
        if (scegli_crea_new_gen==0)
        {
            cout<<"Inserisci il denominatore della frazione di giocatori da tenere (es: 10 per tenere 1/10 dei giocatori) : "; cin>>frazione;
        }
        else
        {
            cout<<"Inserisci la percentuale di giocatori da eliminare (es: 0.1 per eliminarne il 10%) : ";   cin>> frazione2;
        }
    }
    else
    {
        cout<<"Scegli il metodo di evoluzione [0 / 1] : "; cin>>scegli_crea_new_gen;
    }
    
    //dichiaro gioc, test, nemico, nemico_f, fitness
    int *****gioc=new int****[ngiocatori];
    for (int i=0;i<ngiocatori;++i)
        gioc[i]=new int ***[7];
    for (int i=0;i<ngiocatori;++i)
        for (int j=0;j<7;++j)
            gioc[i][j]=new int **[12];
    for (int i=0;i<ngiocatori;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                gioc[i][j][k]=new int *[12];
    for (int i=0;i<ngiocatori;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    gioc[i][j][k][l]=new int [12];
    
    int *****test=new int****[ntest];
    for (int i=0;i<ntest;++i)
        test[i]=new int ***[7];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            test[i][j]=new int **[12];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                test[i][j][k]=new int *[12];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    test[i][j][k][l]=new int [12];
    
    int *****nemico=new int****[ntest];
    for (int i=0;i<ntest;++i)
        nemico[i]=new int ***[7];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            nemico[i][j]=new int **[12];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                nemico[i][j][k]=new int *[12];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    nemico[i][j][k][l]=new int [12];
    
    int *****nemico_f=new int****[ntest];
    for (int i=0;i<ntest;++i)
        nemico_f[i]=new int ***[7];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            nemico_f[i][j]=new int **[12];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                nemico_f[i][j][k]=new int *[12];
    for (int i=0;i<ntest;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    nemico_f[i][j][k][l]=new int [12];
    
    int *****gioc_t=new int****[ngiocatori];
    for (int i=0;i<ngiocatori;++i)
        gioc_t[i]=new int ***[7];
    for (int i=0;i<ngiocatori;++i)
        for (int j=0;j<7;++j)
            gioc_t[i][j]=new int **[12];
    for (int i=0;i<ngiocatori;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                gioc_t[i][j][k]=new int *[12];
    for (int i=0;i<ngiocatori;++i)
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    gioc_t[i][j][k][l]=new int [12];
    
    int *fitness=new int [ngiocatori];
    
    int contatore_nemico=0;
    
    for (int i=0;i<ngiocatori;++i)
        creo_giocatore(gioc[i]);
    for (int i=0;i<ntest;++i)
    {
        creo_giocatore(test[i]);
        for (int j=0;j<7;++j)
            for (int k=0;k<12;++k)
                for (int l=0;l<12;++l)
                    for (int m=0;m<12;++m)
                    {
                        nemico[i][j][k][l][m]=test[i][j][k][l][m];
                        nemico_f[i][j][k][l][m]=test[i][j][k][l][m];
                    }
    }
    
    for (int generazione=0;generazione<ngenerazioni;++generazione)
    {
        if(generazione %50 == 0) cout<<"Generazione "<<generazione<<endl;
        //azzera la fitness
        for (int g=0;g<ngiocatori;++g)
            fitness[g]=0;
        //calcola la fitness tramite le partite
        for (int g=0;g<ngiocatori;++g)
        {
            for (int p=0;p<ntest;++p)
            {
                Gioca(gioc[g], nemico[p], fitness[g]);
            }
        }
        
        //modifica i nemici (solo alcune generazioni prestabilite)
        if (generazione==100*pow(2, contatore_nemico-1))
        {
            cout<<"Cambio nemici!\n";
            crea_nemico(nemico, nemico_f);
            crea_nemico_f(nemico_f, gioc, fitness);
            contatore_nemico++;
        }
        
        //if (generazione %50 == 0)
        Gioca_test(gioc,test);
        if (generazione!=ngenerazioni-1) //in modo che venga scritto nel file il giocatore non evolto
        {
        if (scegli_crea_new_gen==0)
            crea_new_gen(gioc, fitness, gioc_t);//evolve il campione
        else
            crea_new_gen2(gioc, fitness);//evolve il campione
        }
    }
    
    scrivi_file(gioc, fitness);
}

