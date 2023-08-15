#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_SIZE 1024

int main()
{
    int server_key = 12345;
    int server_shmid = -1;
    char* server_shmaddr = NULL;
    int client_key = -1;
    int client_shmid = -1;
    char* client_shmaddr = NULL;
    char client_name[20];
    
    // Connect to server shared memory
    server_shmid = shmget(server_key, SHM_SIZE, 0666);
    if (server_shmid == -1) {
        perror("Error: Failed to connect to server shared memory");
        exit(1);
    }
    server_shmaddr = (char*)shmat(server_shmid, NULL, 0);
    if (server_shmaddr == (char*)-1) {
        perror("Error: Failed to attach to server shared memory");
        exit(1);
    }
    
    // Register with the server
    while (client_key == -1) {
        std::cout << "Enter a unique client name (up to 20 characters): ";
        std::cin >> client_name;
        
        // Create client shared memory
        client_key = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
        if (client_key == -1) {
            perror("Error: Failed to create client shared memory");
            exit(1);
        }
        client_shmid = client_key;
        client_shmaddr = (char*)shmat(client_shmid, NULL, 0);
        if (client_shmaddr == (char*)-1) {
            perror("Error: Failed to attach to client shared memory");
            exit(1);
        }
        client_shmaddr[0] = '\0';
        
        // Send register request to server
        std::string request = "REGISTER " + std::string(client_name) + " " + std::to_string(client_key);
        strcpy(server_shmaddr, request.c_str());
        
        // Wait for server response
        while (strlen(client_shmaddr) == 0);
        
        // Parse server response
        std::string response(client_shmaddr);
        if (response.substr(0, 7) == "SUCCESS") {
            std::cout << response << std::endl;
        } else if (response.substr(0, 5) == "ERROR") {
            std::cout << response << std::endl;
            client_key = -1;
        } else {
            std::cout << "Error: Unexpected response from server" << std::endl;
            client_key = -1;
        }
    }
    
    // Display menu
    std::cout << std::endl;
    std::cout << "Select an action:" << std::endl;
    std::cout << "1. Arithmetic" << std::endl;
    std::cout << "2. Even or Odd" << std::endl;
    std::cout << "3. Is Prime" << std::endl;
    std::cout << "4. Is Negative" << std::endl;
    std::cout << "5. Unregister" << std::endl;
    std::cout << "6. Exit" << std::endl;
    
    // Loop until user exits
    int choice = 0;
    while (choice != 6) {
        std::cout << std::endl;
        // Get user input
    std::cout << "Enter your choice (1-6): ";
    std::cin >> choice;
    
    // Handle user choice
    std::string request, response;
    switch (choice) {
        case 1:
            int num1, num2, op;
            std::cout << "Enter first number: ";
            std::cin >> num1;
            std::cout << "Enter second number: ";
            std::cin >> num2;
            std::cout << "Select operation:" << std::endl;
            std::cout << "1. Add" << std::endl;
            std::cout << "2. Subtract" << std::endl;
            std::cout << "3. Multiply" << std::endl;
            std::cout << "4. Divide" << std::endl;
            std::cin >> op;
            request = "ARITHMETIC " + std::to_string(num1) + " " + std::to_string(num2) + " " + std::to_string(op);
            break;
        case 2:
            int num;
            std::cout << "Enter a number: ";
            std::cin >> num;
            request = "EVEN_ODD " + std::to_string(num);
            break;
        case 3:
            std::cout << "Enter a number: ";
            std::cin >> num;
            request = "IS_PRIME " + std::to_string(num);
            break;
        case 4:
            std::cout << "Enter a number: ";
            std::cin >> num;
            request = "IS_NEGATIVE " + std::to_string(num);
            break;
        case 5:
            request = "UNREGISTER";
            break;
        case 6:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Error: Invalid choice" << std::endl;
            continue;
    }
    
    // Send request to server
    strcpy(server_shmaddr, request.c_str());
    
    // Wait for server response
    while (strlen(client_shmaddr) == 0);
    
    // Parse server response
    response = client_shmaddr;
    if (response.substr(0, 7) == "SUCCESS") {
        std::cout << response << std::endl;
    } else if (response.substr(0, 5) == "ERROR") {
        std::cout << response << std::endl;
    } else {
        std::cout << "Error: Unexpected response from server" << std::endl;
    }
    
    // Clear client shared memory
    client_shmaddr[0] = '\0';
}

// Unregister with the server
std::string request = "UNREGISTER";
strcpy(server_shmaddr, request.c_str());
shmdt(server_shmaddr);
shmdt(client_shmaddr);
shmctl(client_shmid, IPC_RMID, NULL);

return 0;
}
