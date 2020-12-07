#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int []);

int  main(int  argc, char *argv[])
{
    int    ShmID;
    int    *ShmPTR;
    pid_t  pid;
    int    status;

    

    ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }

    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    ShmPTR[0] = 0; // Bank account
    ShmPTR[1] = 0; // Turn
    
    pid = fork();
    if (pid > 0) {
        int i;
        for (i = 0; i<25; i++){
            sleep(rand()%6);
            while (ShmPTR[1] == 1);
            if (ShmPTR[0] <= 100){
                int amount = rand()%101;
                if (amount%2 == 0){
                    ShmPTR[0]+=amount;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", amount, ShmPTR[0]);
                }else{
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
            }else{
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", ShmPTR[0]);
            }
            ShmPTR[1] = 1;
        }
        
        exit(1);
    }
    else if (pid == 0) {
        ClientProcess(ShmPTR);
        exit(0);
    }

    wait(&status);
    printf("Server has detected the completion of its child...\n");
    shmdt((void *) ShmPTR);
    printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0);
}

void  ClientProcess(int  ShmPTR1[])
{
    printf("   Client process started\n");
    int i;
    for (i = 0; i<25; i++){
        sleep(rand()%6);
        while (ShmPTR1[1] == 0);
        int amount = rand()%51;
        if (amount<=ShmPTR1[0]){
            ShmPTR1[0]-=amount;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", amount, ShmPTR1[0]);
        }else{
            printf("Poor Student: Not Enough Cash ($%d)\n", ShmPTR1[0]);
        }
        ShmPTR1[1] = 0;
    }
    printf("   Client is about to exit\n");
}