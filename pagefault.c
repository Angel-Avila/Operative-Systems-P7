#include <stdio.h>
#include <stdlib.h>
#include "mmu.h"

#define RESIDENTSETSIZE 24

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;
extern struct PROCESSPAGETABLE *gprocesspagetable;

int getfreeframe();
int getvirtualframe();
unsigned long getLRU();
// Rutina de fallos de página

int pagefault(char *vaddress)
{
    int i;
    int frame;
    long pag_a_expulsar;
    long pag_del_proceso;

    // Calcula la página del proceso
    pag_del_proceso=(long) vaddress>>12;
    // Cuenta los marcos asignados al proceso
    i=countframesassigned();

    if(i < 3) {
      // Busca un marco libre en el sistema
      frame=getfreeframe();
      printf("Free frame: 0x%04x\n", frame);
      //printf("===================== LRU: 0x%04x =====================\n", getLRU());
    } else {
      // Busca un marco virtual libre (si virtual se cambia a frame se crea un horrible VIRUS)
      frame=getvirtualframe();
      printf("Virtual frame: 0x%04x\n", frame);

      int lruFrame = getLRU(); // regresa el LRU frame en hexa

      // Copiar marco
      FILE *swap = fopen("swap", "w");
      long int offset = lruFrame - framesbegin;
      printf("Offset: %d\n", offset);
      fseek(swap, offset, SEEK_SET);



      fclose(swap);
    }

    if(frame==-1)
    {
        return(-1); // Regresar indicando error de memoria insuficiente
    }


    (ptbr+pag_del_proceso)->presente=1;
    (ptbr+pag_del_proceso)->framenumber=frame;


    return(1); // Regresar todo bien
}

int getfreeframe()
{
    int i;
    // Busca un marco libre en el sistema
    for(i=framesbegin;i<systemframetablesize+framesbegin;i++)
        if(!systemframetable[i].assigned)
        {
            systemframetable[i].assigned=1;
            break;
        }
    if(i<systemframetablesize+framesbegin)
        systemframetable[i].assigned=1;
    else
        i=-1;
    return(i);
}

unsigned long getLRU() {
  int i;
  unsigned long lastUsed = ptbr->tlastaccess;
  struct PROCESSPAGETABLE *ptptr = ptbr;
  int lruFrame = ptbr->framenumber;

  // Busca un marco libre en el sistema
  for(i=0;i<ptlr;i++) {

    if(ptptr->tlastaccess < lastUsed && ptptr->framenumber != -1) {
      lastUsed = ptptr->tlastaccess;
      lruFrame = ptptr->framenumber;
    }

    /*printf("*********************************\n");
    printf("Last used: %d\n", lastUsed);
    printf("LRU Frame: 0x%04x\n", lruFrame);
    printf("Ptr->LastAccess %d\n", ptptr->tlastaccess);
    printf("Ptr->FrameNumber 0x%04x\n", ptptr->framenumber);*/

    ptptr++;
  }
  //printf("*********************************\n");

  return(lruFrame);
}

int getvirtualframe() {
  int i;
  int start = framesbegin+systemframetablesize;

  for(i= start; i<start+systemframetablesize; i++)
    if(!systemframetable[i].assigned) {
      systemframetable[i].assigned=1;
      break;
    }

  if(i<start+systemframetablesize)
    systemframetable[i].assigned=1;
  else
    i=-1;

  return i;
}
