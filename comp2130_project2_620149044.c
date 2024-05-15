#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PLANES 1000

int planes[MAX_PLANES] = {0};
int planeCount = 0;

void sigHandler(int signum) {
    if (signum == SIGUSR1) {
        printf("Bomber %d to base, bombs away!\n", getpid());
    } else if (signum == SIGUSR2) {
        printf("Plane %d has been refueled\n", getpid());
    } else if (signum == SIGCHLD) {
        // Child process exited
        planeCount--;
    }
}

void launchPlane() {
    pid_t pid = fork();

    if (pid == -1) {

        perror("fork");
        exit(EXIT_FAILURE);

    } else if (pid == 0) {

        // Child process
        signal(SIGUSR1, sigHandler);
        signal(SIGUSR2, sigHandler);
        signal(SIGCHLD, sigHandler);

        int fuel = 100;
        for (int seconds = 0; fuel > 0; ++seconds) {
            sleep(1);
            fuel -= 5;

            if (seconds % 3 == 2) {
                printf("Bomber %d to base, %d%% of fuel left\n", getpid(), fuel);
            }

            if (fuel <= 0) {
                printf("SOS! Plane %d is going to crash\n", getpid());
                exit(EXIT_SUCCESS);
            }
        }

        exit(EXIT_SUCCESS);

    } else {

        // Parent process
        int i;
        for (i = 0; i < MAX_PLANES; ++i) {
            
            if (planes[i] == 0) {

                planes[i] = pid;
                planeCount++;
                printf("Plane %d has been launched\n", pid);
                break;

            }
        }

        if (i == MAX_PLANES) {
            fprintf(stderr, "Error: Maximum number of planes reached.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void dropBomb(int planeID) {
    if (planeCount > 0) {

        if (planes[planeID - 1] != 0) {

            printf("Bomber %d to base, bombs away!\n", planes[planeID - 1]);
            kill(planes[planeID - 1], SIGUSR1);

        } else {

            printf("Plane does not exist\n");

        }
    } else {

        printf("No planes are flying, can't drop a bomb!\n");

    }
}

void refuelPlane(int planeID) {
    if (planeCount > 0 && planeID > 0 && planeID <= planeCount) {

        printf("Plane %d has been refueled\n", planes[planeID - 1]);
        kill(planes[planeID - 1], SIGUSR2);

    } else {

        printf("Invalid plane ID or no planes are flying\n");

    }
}

void quitSimulation() {
    for (int i = 0; i < MAX_PLANES; ++i) {
        if (planes[i] != 0) {

            kill(planes[i], SIGTERM);
            waitpid(planes[i], NULL, 0);

        }
    }

    printf("Simulation ending....\n");
    printf("Stimulation exited.\n");
    exit(EXIT_SUCCESS);

}

int main() {

    // Signals
    signal(SIGUSR1, sigHandler);
    signal(SIGUSR2, sigHandler);

    while (1) {
        char command[10];
        int planeID = 0;

        printf("Enter command l (for launch), b (for bomb), r (for refuel), and q (for quit): ");
        scanf("%s", command);

        if (strcmp(command, "q") == 0)
        {
            quitSimulation();
        }
        if (strcmp(command, "l") == 0) {

            launchPlane();

        } else if (strcmp(command, "b") == 0) {

            printf("Which plane should drop a bomb?\n");
            scanf("%d", &planeID);
            dropBomb(planeID);

        } else if (strcmp(command, "r") == 0) {
            
            printf("Which plane should be refueled?\n");
            scanf("%d", &planeID);
            refuelPlane(planeID);

        } else {

            printf("Invalid command. Valid commands: launch, bomb, refuel, quit\n");

        }
    }

    return 0;
}
