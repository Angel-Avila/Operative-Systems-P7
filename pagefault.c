#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mmu.h"

#define RESIDENTSETSIZE 24
#define MAX_PAGES_PER_PROCESS 3
#define K_BYTE 1024
#define PAGE_SIZE 4 * K_BYTE
#define PAGES_PER_PROCESS 6

extern char *base;
extern int framesbegin;
extern int idproc;
extern int systemframetablesize;
extern int ptlr;

extern struct SYSTEMFRAMETABLE *systemframetable;
extern struct PROCESSPAGETABLE *ptbr;
extern struct PROCESSPAGETABLE *gprocesspagetable;

int getfreeframe();
int getfreevirtualframe();
long getleastusedpage();
FILE *virtualM_f;
int virtualMemFrames[RESIDENTSETSIZE / 2] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char buffer[PAGE_SIZE];

// Rutina de fallos de página

int pagefault(char *vaddress)
{
    int i;
    int frame, virtualframe;
    long pag_a_expulsar;
    long pag_del_proceso, leastusedpage;

    // Calcula la página del proceso
    pag_del_proceso = (long)vaddress >> 12;
    // Cuenta los marcos asignados al proceso
    i = countframesassigned();

    // printf("systemframetablesize = %d\n\r", systemframetablesize);
    // printf("framesbegin = %d\n\r", framesbegin);

    if ((ptbr + pag_del_proceso)->framenumber == NINGUNO) // Si pide una pagina y no tiene un marco asignado
    {
        // printf("Proceso %d requiere un nuevo marco\n\r", idproc);
        if (i >= MAX_PAGES_PER_PROCESS) // Si tiene 3 marcos fisicos asignados, se le asigna uno virtual y se carga en memoria
        {
            virtualM_f = fopen("swap", "r+");
            leastusedpage = getleastusedpage();

            if (leastusedpage == -1)
            {
                // printf("Error, no se puede asignar marco virtual, leastusedpage = %ld\n\r", leastusedpage);
            }

            if ((ptbr + leastusedpage)->modificado == 1)
            {
                (ptbr + leastusedpage)->presente = 0;
                (ptbr + leastusedpage)->modificado = 0;

                frame = (ptbr + leastusedpage)->framenumber;

                virtualframe = getfreevirtualframe();
                // printf("Proceso %d tiene 3 marcos fisicos, asignando marco virtual %04X\n\r\n\r", idproc, virtualframe);
                if (virtualframe == -1)
                {
                    // printf("Error, no quedan marcos virtuales\n\r");
                    return (-1); // Regresar indicando error de memoria insuficiente
                }

                fseek(virtualM_f, (virtualframe - framesbegin) * PAGE_SIZE, SEEK_SET);
                fwrite(systemframetable[frame].paddress, 1, PAGE_SIZE, virtualM_f);

                (ptbr + leastusedpage)->framenumber = virtualframe;

                (ptbr + pag_del_proceso)->presente = 1;
                (ptbr + pag_del_proceso)->framenumber = frame;
            }
            else
            {
                (ptbr + leastusedpage)->presente = 0;

                virtualframe = getfreevirtualframe();
                // printf("Proceso %d tiene 3 marcos fisicos, asignando marco virtual %04X\n\r\n\r", idproc, virtualframe);
                if (virtualframe == -1)
                {
                    // printf("Error, no quedan marcos virtuales\n\r");
                    return (-1); // Regresar indicando error de memoria insuficiente
                }

                frame = (ptbr + leastusedpage)->framenumber;

                (ptbr + leastusedpage)->framenumber = virtualframe;

                (ptbr + pag_del_proceso)->presente = 1;
                (ptbr + pag_del_proceso)->framenumber = frame;
            }

            fclose(virtualM_f);
        }
        else // Si tiene menos de 3 marcos fisicos asignados
        {
            // Busca un marco libre en el sistema

            frame = getfreeframe();
            // printf("Proceso %d tiene menos de 3 marcos fisicos, asignando marco fisico %04X\n\r\n\r", idproc, frame);

            if (frame == -1)
            {
                return (-1); // Regresar indicando error de memoria insuficiente
            }

            (ptbr + pag_del_proceso)->presente = 1;
            (ptbr + pag_del_proceso)->framenumber = frame;
        }

        // printf("Saliendo\n\r");

        return (1); // Regresar todo bien
    }
    else // Si tiene un marco asignado pero no esta cargado en memoria
    {

        virtualM_f = fopen("swap", "r+");
        // printf("Proceso %d desea cargar marco virtual %lu a memoria\n\r", idproc, pag_del_proceso);
        leastusedpage = getleastusedpage();
        if (leastusedpage == -1)
        {
            // printf("Error, no se puede cargar marco virtual, leastusedpage = %ld\n\r", leastusedpage);
        }

        // if ((ptbr + leastusedpage)->modificado == 1) // Si la pagina fue escrita, se guarda
        // {
        (ptbr + leastusedpage)->presente = 0;
        (ptbr + leastusedpage)->modificado = 0;

        frame = (ptbr + leastusedpage)->framenumber;
        virtualframe = (ptbr + pag_del_proceso)->framenumber;

        memcpy(buffer, systemframetable[frame].paddress, PAGE_SIZE);

        fseek(virtualM_f, (virtualframe - framesbegin) * PAGE_SIZE, SEEK_SET);
        fread(systemframetable[frame].paddress, 1, PAGE_SIZE, virtualM_f);

        fseek(virtualM_f, (virtualframe - framesbegin) * PAGE_SIZE, SEEK_SET);
        fwrite(buffer, 1, PAGE_SIZE, virtualM_f);

        (ptbr + leastusedpage)->framenumber = virtualframe;

        (ptbr + pag_del_proceso)->presente = 1;
        (ptbr + pag_del_proceso)->framenumber = frame;
        // }
        // else // Si la pagina no esta modificada, no se guarda
        // {
        //     (ptbr + leastusedpage)->presente = 0;
        //     (ptbr + leastusedpage)->modificado = 0;

        //     frame = (ptbr + leastusedpage)->framenumber;
        //     virtualframe = (ptbr + pag_del_proceso)->framenumber;

        //     memcpy(buffer, systemframetable[frame].paddress, PAGE_SIZE);

        //     fseek(virtualM_f, (virtualframe - framesbegin) * PAGE_SIZE, SEEK_SET);
        //     fread(systemframetable[frame].paddress, 1, PAGE_SIZE, virtualM_f);

        //     fseek(virtualM_f, (virtualframe - framesbegin) * PAGE_SIZE, SEEK_SET);
        //     fwrite(buffer, 1, PAGE_SIZE, virtualM_f);

        //     (ptbr + leastusedpage)->framenumber = virtualframe;

        //     (ptbr + pag_del_proceso)->presente = 1;
        //     (ptbr + pag_del_proceso)->framenumber = frame;
        // }

        fclose(virtualM_f);

        return (1); // Regresar todo bien
    }
}

int getfreeframe()
{
    int i;
    // Busca un marco libre en el sistema
    for (i = framesbegin; i < systemframetablesize + framesbegin; i++)
        if (!systemframetable[i].assigned)
        {
            systemframetable[i].assigned = 1;
            break;
        }
    if (i < systemframetablesize + framesbegin)
        systemframetable[i].assigned = 1;
    else
        i = -1;
    return (i);
}

int getfreevirtualframe()
{
    int i;
    // Busca un marco libre en el sistema
    for (i = 0; i < systemframetablesize; i++)
        if (virtualMemFrames[i] == 0)
        {
            virtualMemFrames[i] = 1;
            break;
        }
    if (i < systemframetablesize + framesbegin)
    {
        virtualMemFrames[i] = 1;
        i += systemframetablesize + framesbegin;
    }
    else
    {
        i = -1;
    }

    return (i);
}

long getleastusedpage()
{
    int i;
    long leastusedpage = -1;
    unsigned long time = 0, temptime = 0;
    // Busca un marco libre en el sistema
    for (i = 1; i < PAGES_PER_PROCESS; i++)
    {
        if ((ptbr + i)->presente == 1)
        {
            temptime = (ptbr + i)->tlastaccess - (ptbr + i)->tarrived;
            // printf("Buscando pagina menos usada del proceso %d, temptime de la pagina %d = %lu \n\r", idproc, i, temptime);
            if (time <= temptime)
            {
                leastusedpage = i;
                time = temptime;
            }
        }
    }
    // printf("Pagina menos usada del proceso %d es la %ld con temptime = %lu \n\r\n\r", idproc, leastusedpage, time);
    return leastusedpage;
}
