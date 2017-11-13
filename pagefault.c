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
void unassignVF();
void swapvirtualframe();
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

    unassignVF(pag_del_proceso);

    // Cuenta los marcos asignados al proceso
    i=countframesassigned();
    
    printf("----------------------------- Frames assigned: %d -----------------------------\n", i);

    if(i + 1 > 3) {
      printf("GONNA SWAP NOOW\n");
      swapvirtualframe(pag_del_proceso);

      // Copiar marco
      /*FILE *swap = fopen("swap", "w");
      long int offset = lruFrame - framesbegin;
      printf("Offset: %d\n", offset);
      fseek(swap, offset, SEEK_SET);
      fclose(swap);*/
    }

    frame = getfreeframe();

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

    if(ptptr->tlastaccess < lastUsed && ptptr->framenumber != -1 && ptptr->presente == 1) {
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

void unassignVF(long pag_del_proceso) {
  int start = framesbegin+systemframetablesize;

  if((pag_del_proceso >= start) && (pag_del_proceso < start+systemframetablesize))
    systemframetable[pag_del_proceso].assigned = 0;
}

void swapvirtualframe(long pag_del_proceso) {
  int i;
  int start = framesbegin+systemframetablesize;

  int lruFrame = getLRU(); // regresa el LRU frame en hexa
  struct PROCESSPAGETABLE *ptptr = ptbr;

  printf("LRUFrame: %d\n", lruFrame);

  for(i=0;i<ptlr;i++) {
    printf("framenumber: %d\n", ptptr->framenumber);
    if(ptptr->framenumber == lruFrame) {
      printf("Hola, entré aquí");
      break;
    }
    ptptr++;
  }

  ptptr->presente = 0;

  systemframetable[ptptr->framenumber].assigned = 0;

  for(i= start; i<start+systemframetablesize; i++)
    if(!systemframetable[i].assigned) {
      systemframetable[i].assigned=1;
      ptbr[ptptr->framenumber].framenumber = i;
      break;
    }
}
