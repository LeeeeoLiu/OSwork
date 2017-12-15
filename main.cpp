/***************************************************************/
/*         PROGRAM NAME:         PRODUCER_CONSUMER             */
/*    This program simulates two processes, producer which     */
/* continues  to produce message and  put it into a buffer     */
/* [implemented by PIPE], and consumer which continues to get  */
/* message from the buffer and use it.                         */
/*    The program also demonstrates the synchronism between    */
/* processes and uses of PIPE.                                 */
/***************************************************************/

#define PIPESIZE 8
#define PRODUCER 0
#define CONSUMER 1
#define RUN      0    /* statu of process */
#define WAIT     1    /* statu of process */
#define READY    2    /* statu of process */
#define NORMAL   0
#define SLEEP    1
#define AWAKE    2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct pcb {
    char name[20];
    int statu;
    int time;
};  /* times of execution */
struct pipetype { //buffer
    char type;
    int writeptr;
    int readptr;
    struct pcb *pointw;     /* write wait point */
    struct pcb *pointr;
}; /* read wait point  */

int PIPE[PIPESIZE]; //buffer
int PIPEEMPTY; // record the remaining number of pipe
struct pipetype pipetb;
struct pcb process[2];
struct pcb process2[2];

int runp(int out, struct pcb p[], int PIPE[], struct pipetype *tb, int t)  /* run producer */
{
    p[t].statu = RUN;
    printf("run PRODUCER. product %d     ", out);
    //if(tb->writeptr>=PIPESIZE)
    if (PIPEEMPTY <= 0) {
        p[t].statu = WAIT;
        return (SLEEP);
    }
    PIPE[tb->writeptr] = out;
    //tb->writeptr++;
    tb->writeptr = (tb->writeptr + 1) % PIPESIZE;
    PIPEEMPTY--;
    p[t].time++;
    p[t].statu = READY;
    if ((tb->pointr) != NULL) return (AWAKE);
    return (NORMAL);
}

int runc(struct pcb p[], int PIPE[], struct pipetype *tb, int t)      /* run consumer */
{
    int c;
    p[t].statu = RUN;
    printf("run CONSUMER. ");
    //if(tb->readptr>=tb->writeptr)
    if (PIPEEMPTY >= PIPESIZE) {
        p[t].statu = WAIT;
        return (SLEEP);
    } //if the readptr >= writeptr, c wait
    c = PIPE[tb->readptr];
    PIPE[tb->readptr] = -1;
    tb->readptr = (tb->readptr + 1) % PIPESIZE;
    //tb->readptr++;
    printf(" use %d      ", c);
    //if(tb->readptr>=tb->writeptr) tb->readptr=tb->writeptr=0;
    PIPEEMPTY++;
    p[t].time++;
    p[t].statu = READY;
    //if((tb->readptr)==0&&(tb->pointw)!=NULL)
    if (tb->pointw != NULL)
        return (AWAKE);
    return (NORMAL);
}

void prn(struct pcb p[], int PIPE[], struct pipetype tb) {
    int i;
    printf("\n         ");
    for (i = 0; i < PIPESIZE; i++) printf("------ ");
    printf("\n        |");
    for (i = 0; i < PIPESIZE; i++)
        if (PIPE[i] >= 0)
            printf("  %2d  |", PIPE[i]);
        else
            printf("      |");
    printf("\n         ");
    /*if((i>=tb.readptr)&&(i<tb.writeptr))
        printf("  %2d  |",PIPE[i]);
    else printf("      |");
        printf("\n         ");*/
    for (i = 0; i < PIPESIZE; i++)
        printf("------ ");
    printf("\npipempty = %d, writeptr = %d, readptr = %d,  ", PIPEEMPTY, tb.writeptr, tb.readptr);
    if (p[PRODUCER].statu == WAIT)
        printf("PRODUCER wait ");
    else
        printf("PRODUCER ready ");
    if (p[CONSUMER].statu == WAIT)
        printf("CONSUMER wait ");
    else
        printf("CONSUMER ready ");
    printf("\n");
}

char myRandom() {
    sleep(1);
    int tmp = rand() % 100;
    if (tmp < 80) {
        return 'p';
    } else {
        return 'c';
    }

}

int Wmutex, Rmutex;
int Rcount;

int P(int &tmp) {
    --tmp;
    if (tmp < 0) {
        tmp = 0;
        return 0;
    } else {
        return 1;
    }
}

int V(int &tmp) {
    ++tmp;
    if (tmp <= 0) {
        tmp = 0;
        return 0;
    } else {
        return 1;
    }
}

int main() {
    Wmutex = Rmutex = 1;
    Rcount = 0;
    PIPEEMPTY = PIPESIZE;
    memset(PIPE, -1, sizeof(PIPE));

    int output, ret; //output mean NO. of resource
    char in[2]; // selector
    /* init pipetype */
    pipetb.type = 'c';
    pipetb.writeptr = 0;
    pipetb.readptr = 0;
    pipetb.pointw = pipetb.pointr = NULL;
    /* init two process */
    strcpy(process[PRODUCER].name, "Producer");
    strcpy(process[CONSUMER].name, "Consumer");
    process[PRODUCER].statu = process[CONSUMER].statu = READY;
    process[PRODUCER].time = process[CONSUMER].time = 0;
    strcpy(process2[PRODUCER].name, "Producer2");
    strcpy(process2[CONSUMER].name, "Consumer2");
    process2[PRODUCER].statu = process2[CONSUMER].statu = READY;
    process2[PRODUCER].time = process2[CONSUMER].time = 0;
    /* init NO. of resource */
    output = 0;
    printf("Now starting the program!\n");
    printf("Press 'p' to run PRODUCER, press 'c' to run CONSUMER.\n");
    printf("Press 'e' to exit from the program.\n");
    /* infinite circulation */
    while (1) {
        in[0] = 'N';
        while (in[0] == 'N') {
            scanf("%s", in);
            //in[0] = myRandom();
            if (in[0] != 'e' && in[0] != 'p' && in[0] != 'c') in[0] = 'N';
        }
        if (in[0] == 'e') {
            printf("Program completed!\n");
            exit(0);
        }
        /* operator is 'p' and producer is ready
        * if it doesn't allow to write, wait and pointw=p
        * else if there is c waiting, awake
        */
        if (in[0] == 'p') {
            if (P(Wmutex)) {
                if (in[0] == 'p' && process[PRODUCER].statu == READY) {
                    output = output % 99 + 1; //0-99
                    if ((ret = runp(output, process, PIPE, &pipetb, PRODUCER)) == SLEEP)
                        pipetb.pointw = &process[PRODUCER]; //
                    if (ret == AWAKE) {
                        (pipetb.pointr)->statu = READY;
                        pipetb.pointr = NULL;
                        runc(process, PIPE, &pipetb, CONSUMER);

                    }
                }
                printf("\nStart producing product!");
                if (P(Wmutex)) {
                    if (in[0] == 'p' && process2[PRODUCER].statu == READY) {
                        output = output % 99 + 1; //0-99
                        if ((ret = runp(output, process2, PIPE, &pipetb, PRODUCER)) == SLEEP)
                            pipetb.pointw = &process2[PRODUCER]; //
                        if (ret == AWAKE) {
                            (pipetb.pointr)->statu = READY;
                            pipetb.pointr = NULL;
                            runc(process2, PIPE, &pipetb, CONSUMER);
                        }
                    }
                    V(Wmutex);
                } else {
                    printf("\nThe write resource is  mutex");
                }
                V(Wmutex);
                printf("\nProduce product finished!\n");
            }
        }
        if (in[0] == 'c') {
            if (P(Rmutex)) {
                ++Rcount;
                V(Rmutex);
                if (in[0] == 'c' && process[CONSUMER].statu == READY) {
                    if ((ret = runc(process, PIPE, &pipetb, CONSUMER)) == SLEEP)
                        pipetb.pointr = &process[CONSUMER];
                    if (ret == AWAKE) {
                        (pipetb.pointw)->statu = READY;
                        pipetb.pointw = NULL;
                        runp(output, process, PIPE, &pipetb, PRODUCER);
                    }
                }
                P(Rmutex);
                --Rcount;
                V(Rmutex);
                if (rand() % 100 <= 50) {
                    if (P(Rmutex)) {
                        ++Rcount;
                        V(Rmutex);
                        if (in[0] == 'c' && process[CONSUMER].statu == READY) {
                            if ((ret = runc(process, PIPE, &pipetb, CONSUMER)) == SLEEP)
                                pipetb.pointr = &process[CONSUMER];
                            if (ret == AWAKE) {
                                (pipetb.pointw)->statu = READY;
                                pipetb.pointw = NULL;
                                runp(output, process, PIPE, &pipetb, PRODUCER);
                            }
                        }
                        P(Rmutex);
                        --Rcount;
                        V(Rmutex);
                    }
                }
            }
        }


        if (in[0] == 'p' && process[PRODUCER].statu == WAIT)
            printf("PRODUCER is waiting, can't be scheduled.\n");
        if (in[0] == 'c' && process[CONSUMER].statu == WAIT)
            printf("CONSUMER is waiting, can't be scheduled.\n");
        prn(process, PIPE, pipetb);
    }
}


