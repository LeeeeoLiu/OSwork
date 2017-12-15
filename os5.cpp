#include<iostream>
#include<cstdio>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<fstream>
#include<sys/wait.h>
#include<cstring>
#include<math.h>
#include<queue>

using namespace std;

int physicsNum, loginNum;
int arrayLength; // arrayLenght means the random sequence length

int array[20]; // random sequece for memory accessing
//int array[20] = {4,3,2,1,4,3,5,4,3,2,1,5};
//int array[20] = {1,2,3,4,1,2,5,1,2,3,4,5};
void myRandom() {
    physicsNum = 3;
    loginNum = 5;
    arrayLength = 12;

    srand((unsigned int)(time(NULL)));
    cout<<"Random sequence:";
    for(int i= 0; i< arrayLength; i++) {

        array[i] = rand()%loginNum+ 1;
        cout<< array[i]<<" ";
    }
    cout<<endl;
}

void LRU() {
    int lostNum = 0; //lost page number

    int physicsArray[10]; // store content of every physics page
    int physics_num = 0; // the number of physics page which is used
    memset(physicsArray, 0, sizeof(physicsArray));  //clear

    int ror[loginNum+1];  // simulate the register for every login page
    memset(ror, 0, sizeof(ror));

    for(int i= 0; i< arrayLength; i++) {
        //1.ror
        for(int j= 1; j<= loginNum; j++) {
            ror[j] = ror[j]>>1;
        }
        //2. high order position set 1
        ror[array[i]] = ror[array[i]]+ pow(2,loginNum-1);
        //3.change out
        if(physics_num < physicsNum) {
            for(int j= 1; j<= physicsNum; j++) {
                if(physicsArray[j] == array[i]) {
                    break;
                } else if(physicsArray[j] == 0) {
                    physicsArray[j] = array[i];
                    physics_num ++;
                    lostNum ++;
                    break;
                }
            }
        } else {

            bool exist = 0;
            /* there is array[i] in physicsArray */
            for(int k=1;k<=physicsNum;k++) {
                if(physicsArray[k] == array[i]) {
                    /* restore */
                    ror[array[i]] = ror[array[i]] - pow(2,loginNum-1);
                    for(int j= 1; j<= loginNum; j++) {
                        ror[j] = ror[j]<<1;
                    }
                    for(int j=1;j<=loginNum;j++) {
                        if(ror[j] > ror[array[i]]) {
                            ror[j] = ror[j]>>1;
                        }
                    }
                    ror[array[i]] = ror[array[i]] + pow(2,loginNum-1);
                    exist = 1;
                    break;
                }
            }

            if(exist != 1) {

                bool flag = 0; // flag for whether change out
                /* there is not array[i] in physicsArray*/
                for(int j=1; j<= loginNum; j++) {
                    if(ror[j]< pow(2, loginNum-physicsNum)) { //must:loginNum>= physics;
                        ror[j] = 0;
                        for(int k= 1; k<= physicsNum; k++) {
                            if(physicsArray[k] == j) {
                                physicsArray[k] = array[i];
                                lostNum++;
                                flag = 1;
                                break;
                            }
                        }
                        if (flag == 1)
                            break;
                    }
                }
            }
        }
        for(int j= 1; j<= physicsNum; j++) {
            if(physicsArray[j] != 0)
                printf("%d ",physicsArray[j]);
        }
        printf("\n");
    }
    printf("Lost Page Number: %d\n",lostNum);
    printf("Lost Page ratio: %.2lf\n", (double)lostNum/arrayLength);
}

void FIFO() {
    int lostNum = 0; //lost page number

    queue<int>fifo;
    queue<int>Qtmp;
    for(int i= 0; i< arrayLength; i++) {
        int flag = 0; //flag means whether there is array[i] in queue
        while(!fifo.empty()) {
            int tmp = fifo.front();fifo.pop();
            if(tmp == array[i]) {
                flag = 1;
            }
            Qtmp.push(tmp);
        }
        while(!Qtmp.empty()) {
            int tmp = Qtmp.front();Qtmp.pop();
            fifo.push(tmp);
        }
        if(flag == 0) {
            lostNum++;
            if(fifo.size() < physicsNum) {
                fifo.push(array[i]);
            } else {
                fifo.pop();fifo.push(array[i]);
            }
        }
        /* show queue */
        while(!fifo.empty()) {
            int tmp = fifo.front();fifo.pop();
            printf("%d ",tmp);
            Qtmp.push(tmp);
        }
        while(!Qtmp.empty()) {
            int tmp = Qtmp.front();Qtmp.pop();
            fifo.push(tmp);
        }
        printf("\n");
    }
    printf("Lost Page Number: %d\n",lostNum);
    printf("Lost Page ratio: %.2lf\n", (double)lostNum/arrayLength);
}

int main() {
    //1. random
    myRandom();

    //2.childProcess
    int pid1, pid2;
    while((pid1 = fork())== -1);
    if(pid1 > 0) {
        while((pid2 = fork()) == -1);
    }

    if (pid1 == 0) {
        printf("--------------\npid1\n");
        LRU();
    } else if(pid2 == 0) {
        printf("--------------\npid2\n");
        FIFO();
    }
}

