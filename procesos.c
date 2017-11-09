
/* ---------------------------------------------- */
/* ---------------  PROCESOS  ------------------- */
/* ---------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#define BRINCO 4*1024
#define vaddress(X) (int) (X-base)

extern char *base;
extern int idproc;

void proc0()
{
    char *ptr=NULL;
    int i;
    char c;

    ptr=base;

    sprintf(ptr,"Escritura del proceso %d",idproc);
    for(i=0;i<3;i++)
    {
        printf("%s\n",ptr);

        sleep(1);

        ptr+=5*BRINCO;
        c=*ptr;    // Lectura

        sleep(1);

        ptr-=4*BRINCO;
        c=*ptr;   // Lectura
        sleep(1);
        c=*ptr;
             
        sleep(1);
        ptr++;
        c=*ptr;
        if(i==0)
            *ptr='A';  // Escritura
        else
            printf("Proceso 0 escribe %c, en la dirección relativa %X\n",*ptr,vaddress(ptr));          

        sleep(1);

        ptr+=2*BRINCO;
        c=*ptr;

        sleep(1);

        ptr-=BRINCO;
        c=*ptr;
        *ptr='A';             
        *ptr='B';             
        *ptr='*';
        sleep(1);
        *ptr='*';
        sleep(1);

        ptr+=BRINCO;
        c=*ptr;

        ptr=base;
    }
    return;
}


void proc1()
{
    char *ptr=NULL;
    int i;
    char c;

    ptr=base;
    sprintf(ptr,"Proceso 1, escribe mensaje de prueba");
    for(i=0;i<3;i++)
    {
        printf("%s\n",ptr);

        sleep(1);

        ptr+=3*BRINCO;
        c=*ptr;

        sleep(1);

        ptr-=2*BRINCO;
        c=*ptr;
        sleep(1);
        c=*ptr;
             
        sleep(1);
        ptr++;
        c=*ptr;
        *ptr='A';          

        sleep(1);

        ptr+=2*BRINCO;
        c=*ptr;

        sleep(1);

        ptr-=BRINCO;
        c=*ptr;
        *ptr='A';             
        *ptr='B';             
        *ptr='*';
        sleep(1);
        *ptr='*';
        sleep(1);

        ptr+=BRINCO;
        c=*ptr;
        ptr=base;
    }

    return;
}


void proc2()
{
    char *ptr=NULL;
    int i;
    char c;

    ptr=base;

    for(i=0;i<3;i++)
    {
        strcpy(ptr,"hola soy proceso 2");
        printf("copiado %s en la dirección relativa %X\n",ptr,vaddress(ptr));

        sleep(1);

        ptr+=3*BRINCO;
        c=*ptr;

        sleep(1);

        ptr+=BRINCO;
        c=*ptr;
        sleep(1);
        c=*ptr;
             
        sleep(1);
        ptr++;
        c=*ptr;
        *ptr='A';          

        sleep(1);

        ptr-=3*BRINCO;
        c=*ptr;

        sleep(1);

        ptr+=BRINCO;
        c=*ptr;
        *ptr='A';             
        *ptr='B';             
        *ptr='*';
        sleep(1);
        *ptr='*';
        sleep(1);

        ptr+=BRINCO;
        c=*ptr;
        ptr=base;
    }

    return;
}


void proc3()
{
    char *ptr=NULL;
    int i;
    char c;

    ptr=base;

    for(i=0;i<3;i++)
    {
       
	c=*ptr;
        sleep(1);

        ptr+=2*BRINCO;
	strcpy(ptr,"hola soy proceso 3");
        printf("copied %s en la dirección relativa %X\n",ptr,vaddress(ptr));
        

        sleep(1);

        ptr-=3*BRINCO;
        c=*ptr;
        sleep(1);
        c=*ptr;
             
        sleep(1);
        ptr++;
        c=*ptr;
        *ptr='A';          

        sleep(1);

        ptr+=BRINCO;
        c=*ptr;

        sleep(1);

        ptr+=2*BRINCO;
        c=*ptr;
        *ptr='A';             
        *ptr='B';             
        *ptr='*';
        sleep(1);
        *ptr='*';
        sleep(1);

        ptr-=BRINCO;
        c=*ptr;
        ptr=base;
    }
    return;
}
