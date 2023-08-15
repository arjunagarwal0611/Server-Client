/*               GROUP 2   (SERVER)

    NAME                            ID

ARYAN BHARDWAJ----------------- 2020A3PS0587H
RUSHIL SRIVASTAVA---------------2020AAPS1408H
ARJUN AGARWAL-------------------2020A3PS2135H
AKSHIT ADITYA-------------------2020A1PS2509H
MANAS KUMAR KHANDWAL------------2020A4PS2272H
SHASHWAT TRIPATHI---------------2020A3PS2210H
ROHIT ARYAN --------------------2020AAPS0293H
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>
#include <stdbool.h>

#define SHM_SIZE 1024

struct client_info {
    pid_t pid;
    char name[64];
    int num_clients;
    int num_responses;
    bool flag;
};

struct request {
    pid_t pid;
    int operation;
    int arg1;
    int arg2;
};

int isPrime(int n) {
    if(n <= 1) {
        return 0;
    }

    int i;
    for(i = 2; i <= sqrt(n); i++)
     {
        if(n % i == 0) {
            return 0;
        }
    }

    return 1;
}
int isEvenOdd(int n){
    if(n%2==0){
        return 0;
    }
    return 1;
}
int isNegative(int n){
    if(n<0){
        return 1;
    }
    return 0;
}
void *client_worker(void *arg)
{
    struct client_info *client = (struct client_info *) arg;
    printf("Client %s: worker thread started\n", client->name);

    // Wait for messages from the client on the shared memory
    while (1) {
        // TODO: read from shared memory and process message
    }

    return NULL;
}

int main() {
    // Generate a shared memory key
    key_t commkey = ftok(".", 65);
    if(commkey == -1) 
    {
        perror("Error generating the key...");
        exit(1);
    }

    

    //******************************    REGISTRATION AND COMMUNICATION CHANNEL CREATION    ****************************
    // Creating the communication channel memory segment
    int shmid = shmget(commkey, SHM_SIZE, IPC_CREAT | 0666);
    if(shmid == -1) {
        perror("Error creating communication channel shared memory...");
        exit(1);
    }

    char shm = (char) shmat(shmid, NULL, 0);
    if(shm == (char*) -1) {
        perror("Error attaching shared memory segment...");
        exit(1);
    }
    printf("Shared memory created...Server is running...\n");

    // Initialize the shared memory
    struct client_info client = (struct client_info) shm;
    client->pid = 0;
    memset(client->name,'\0',sizeof(client->name));
    client->num_clients=0;
    client->num_responses=0;

    printf("Server started with shared memory key.... %d\n", commkey);
    sleep(1);
    printf("Waiting for client requests...\n");
    client->flag=false;
    // Wait for client requests
    while(1) 
    {
        struct request req = (struct request) shm;
        
        if(req->pid == 0) {
            continue;
        }
        //*******************************       SERVER RESPONSE      *******************************************
        
        if(req->pid!=0&&client->flag==false)
        {
            sleep(5);
            printf("New User registered %d and client name is %s\n",client->pid,client->name);
            printf("Registered Client Count: %d\n",++client->num_clients);
            printf("Current Total Requests : %d\n",client->num_responses);
            fflush(stdout);
            client->flag=true;
            
        }

        //**********************************************************************************************
        int result;

        switch(req->operation) 
        {
            case 1:
               
                result = req->arg1 + req->arg2;
                
                break;
            case 2:
               
                result = req->arg1 - req->arg2;
                break;
            case 3:
               
                result = req->arg1 * req->arg2;
                break;
            case 4:
                
                if(req->arg2 == 0) {
                    
                    printf("Division by zero\n");
                    
                    result = 0;
                    break;
                }

                result = req->arg1 / req->arg2;
                break;
            case 5:
                 
                result = isPrime(req->arg1);
                break;
            case 6:
                
                result = isEvenOdd(req->arg1);
                break;
            case 7:
                
                result= isNegative(req->arg1);
                break;
            default:
                // printf("Invalid Operation\n");
                result = 0;
                break;
        }




        // Send the response to the client
        req->arg1 = result;
        req->pid = 0;
    }
    //****************************************     UNREGISTER       **************************************************** 
    
    // Detach from the shared memory
    if(shmdt(shm) == -1) {
        fflush(stdout);
        perror("Error detaching memory....");
        exit(1);
    }

    // Remove the shared memory segment
    if(shmctl(shmid, IPC_RMID, NULL) == -1) 
    {
        perror("Error deleting memory segment....");
        exit(1);
    }
    printf("The registration of the client has been terminated...\n");

    return 0;
}