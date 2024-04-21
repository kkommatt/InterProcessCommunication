#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>

int f(int x) {
    return x - 1;
}

int g(int x) {
    return x * 3;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }
    // From command line value
    int x = std::stoi(argv[1]);

    // Create socket pair
    int communicationSockets[2];
    pid_t childProcessID;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, communicationSockets) < 0) {
        perror("socketpair");
        exit(1);
    }


    // Fork a child process
    childProcessID = fork();
    if (childProcessID < 0) {
        perror("fork");
        exit(1);
    } else if (childProcessID == 0) {
    
    	// Child process
        close(communicationSockets[0]);
        int result = f(x);
        write(communicationSockets[1], &result, sizeof(result));
        close(communicationSockets[1]);
    } else {
    
    	// Parent process
        close(communicationSockets[1]);
        int result = g(x);
        int childProcessResult;
        read(communicationSockets[0], &childProcessResult, sizeof(childProcessResult));
        close(communicationSockets[0]);
	
	// Calculate final result
        int finalResult = (result == 0 || childProcessResult == 0) ? 0 : result * childProcessResult;
        std::cout << "Result of f(x) * g(x): " << finalResult << std::endl;
    }

    return 0;
}
