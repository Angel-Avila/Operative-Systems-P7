#include <stdio.h>
#include <stdlib.h>
#include "mmu.h"

#define RESIDENTSETSIZE 3

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;
extern struct PROCESSPAGETABLE *gprocesspagetable;

int getfreeframe();
void unassignVF(unsigned int frame);
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

    unassignVF((ptbr+pag_del_proceso)->framenumber);

    // Cuenta los marcos asignados al proceso
    i=countframesassigned();

    printf("Frames assigned: %d \n", i);

    //if(i + 1 > 3) {
      //printf("GONNA SWAP NOOW\n");
      swapvirtualframe();

      // Copiar marco
      /*FILE *swap = fopen("swap", "w");
      long int offset = lruFrame - framesbegin;
      printf("Offset: %d\n", offset);
      fseek(swap, offset, SEEK_SET);
      fclose(swap);*/
    //}

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
  for(i = 0; i < ptlr; i++){
    if(ptbr[i].presente){
      lastUsed = i;
      break;
    }
  }

  /* Obtener el índice del marco a liberar */
  for(i = 0; i < ptlr; i++){

    if(ptbr[i].presente){
      if(ptbr[i].tlastaccess < ptbr[lastUsed].tlastaccess){
        lastUsed = i;
      }
    }

  }

  return(lastUsed);
}

void unassignVF(unsigned int frame) {
  int start = framesbegin+systemframetablesize;

  if((frame >= start) && (frame < start+systemframetablesize))
    systemframetable[frame].assigned = 0;
}

void swapvirtualframe() {
  int start = framesbegin+systemframetablesize;

  int assigned = countframesassigned() + 1;

  //Si con este nuevo, se excede de los permitidos, hacer cambio
  if(assigned > (RESIDENTSETSIZE)) {
    printf("I'm inside\n");
    int lruFrame = getLRU(); // regresa el LRU frame en hexa

    printf("LRUFrame: %d\n", lruFrame);

    ptbr[lruFrame].presente = 0;

    systemframetable[ptbr[lruFrame].framenumber].assigned = 0;

    for(int i= start; i<start+systemframetablesize; i++)
      if(!systemframetable[i].assigned) {
        systemframetable[i].assigned=1;
        ptbr[lruFrame].framenumber = i;
        break;
      }

    }
}
