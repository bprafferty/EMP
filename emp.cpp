/************************************************************
 Name: Brian Rafferty -- cssc0852 -- redID: 816988452
 Class: CS 570, Summer 2019
 Project: Program Assignment #3 -- Event Management Program
 File: emp.cpp
 Notes: This is the source code for a program that creates 4
        threads that run concurrently. Each thread plays a 
        different role: thread 0 is the signal handler which
        checks for any interrupts from the alarm thread and 
        countdown timer, thread 1 is the countdown timer watching 
        total time elapsed, thread 2 is the wall clock printing 
        the current local time in hours:minutes:seconds format 
        based upon the disclosed interval, and thread 3 is the 
        alarm clock that waits until the set amount of time has 
        elapsed before sending its interrupt signal. Once all 
        time has elapsed, the signal handler closes all of the 
        threads and a clean exit occurs.
*************************************************************/
#include "emp.h"

using namespace std;

#define THREAD_COUNT 4

//create array of 4 threads
pthread_t threadArray[THREAD_COUNT];

//semaphore to make threads run one at a time
sem_t FLAG;
int errorNo;

//variable to communicate between threads
volatile sig_atomic_t signalStatus;

//default values if nothing entered
int totalTime = 32;
int printInterval = 1;
int alarmTime = 17;

//booleans to check for times
bool alarmReached = false;
bool timeReached = false;

//variable to determine time step
int step;

//variables to find local time
time_t currentTime;
struct tm timeConvert;
char outputTime[10];

/************************************************************
 FUNCTION: checkErr
 DESCRIPTION: Receives the error number from sections of code
                that need to make sure that all system calls
                were successful. 
 I/O:   Input: int
        Output: void
*************************************************************/
void checkErr(int errorNo) {
    if (errorNo != 0) {
        cout << "ErrorNo other than zero: " << errorNo << endl;
        exit(0);
    }
}

/************************************************************
 FUNCTION: signalHandler
 DESCRIPTION: If the thread id is 0, the signal handler thread
                is created which is built to receive any signals
                from other threads. It will always receive SIGINT
                signals which are of value 2, so differentiation
                is determined using booleans that check to see
                if the alarm has been reached or if the total time
                has elapsed. The appropriate message is printed or
                the threads are closed depending upon the booleans.
 I/O:   Input: int
        Output: void
*************************************************************/
void signalHandler(int signalNumber) {
    signalStatus = signalNumber;

    if (signalStatus == 2 && alarmReached == true) {
        cout << "Alarm signal received. " << alarmTime << " seconds have elapsed." << endl;
        alarmReached = false;
    }
    else if (signalStatus == 2 && timeReached == true) {
        usleep(2000);
        cout << "Termination signal received. " << totalTime << " seconds have elapsed." << endl;
        pthread_exit(NULL);
    }
}

/************************************************************
 FUNCTION: countdownTimer
 DESCRIPTION: If the thread id is 1, the countdown timer thread
                is created. It operates by iterating until the
                total time is reached 1 second at a time. After 
                the total time is reached, a signal is sent to the 
                handler that indicates that the program is done 
                running and that the threads must be closed before 
                the program terminates.
 I/O:   Input: int
        Output: void
*************************************************************/
void countdownTimer() {
    
    for (int i = 0; i < totalTime; i++){
        //tick every second
        usleep(1000000);
    }
    timeReached = true;
    raise(SIGINT);
}

/************************************************************
 FUNCTION: getTime
 DESCRIPTION: Returns the current time in the format as follows:
                Hours:Minutes:Seconds
 I/O:   Input: void
        Output: string
*************************************************************/
string getTime(){
    currentTime = time(0);
    timeConvert = *localtime(&currentTime);
    //grab just the hour:minute:second from the local time conversion
    strftime(outputTime, sizeof(outputTime), "%X", &timeConvert);
    return outputTime;
}

/************************************************************
 FUNCTION: wallClock
 DESCRIPTION: If the thread id is 2, then the wall clock thread
                is created. It runs by iterating through the
                total time 1 second at a time. It compares the 
                interval time, and if it is 1, then it prints 
                the local time to the console each second. If 
                the interval time is 60, then it prints the local
                time to the console every 60 seconds.
 I/O:   Input: int
        Output: void
*************************************************************/
void wallClock() {
    for (int i = 0; i < totalTime; i++){
        //tick every second
        usleep(1000000);

        step = totalTime - i;
        if (printInterval == 1) { 
            cout << "Current time: " << getTime() << endl;
        }
        else if (printInterval == 60 && (step % printInterval == 0)) {
            cout << "Current time: " << getTime() << endl;
        }
    } 
}

/************************************************************
 FUNCTION: alarmClock
 DESCRIPTION: If the thread id is 3, then create the thread
                that manages the alarm clock. It does so by
                iterating through the total time, 1 second
                at a time. It lowers the semaphore into the
                critical section and compares the current 
                time step to the set alarm time, and if it
                is a match, a signal is sent to the handler
                that the alarm must go off. This can only happen
                once due to the boolean alarmReached variable.
                After the signal is sent, the semaphore is raised
                back out of the critical section and the other
                threads can have their turn to run.
 I/O:   Input: int
        Output: void
*************************************************************/
void alarmClock() {
    for (int i = 0; i < totalTime; i++){
        //tick every second
        usleep(1000000);

        errorNo = sem_wait(&FLAG);
        checkErr(errorNo);

        step = i+1;
        if (step == alarmTime && alarmReached == false){
            usleep(2000);
            alarmReached = true;
            raise(SIGINT);
        }

        errorNo = sem_post(&FLAG);
        checkErr(errorNo);
    }
}

/************************************************************
 FUNCTION: manageThreads
 DESCRIPTION: Converts the thread's address into its id by
                casting it as a long. The id is then used
                to create each thread one at a time.
 I/O:   Input: void*
        Output: void
*************************************************************/
void *manageThreads(void *threadPointer) {
    long id;
    id = (long)threadPointer;

    if (id == 0) {
        signal(SIGINT, signalHandler);
    }
    else if (id == 1) {
        countdownTimer();
    }
    else if (id == 2) {
        wallClock();
    }
    else {
        alarmClock();
    }
    
    return NULL;
}


/************************************************************
 FUNCTION: main
 DESCRIPTION: Starts by checking to see if the user entered any
                input of their own. If they did error checking
                takes place to make sure that 3 arguments are 
                entered, and that the second argument is either
                1 or 60. If no arguments are entered, then the 
                program will use 32, 1, and 17 as the default 
                parameters. A semaphore is then created which 
                will maintain that threads cannot enter critical
                sections simultaneously. Each thread is then created 
                using a loop, and each error number is checked
                to insure valid system calls. Each thread is
                ran concurrently, but in a fashion that does
                not allow for corruption of their data using
                join calls on each thread. Once all threads
                have terminated, the semaphore will be destroyed
                and a message indicating success will print to 
                the console.
 I/O:   Input: int, char*
        Output: int
*************************************************************/
int main(int argc,char *argv[]){
    //if user enters custom values
    if (argc == 4){
        totalTime = atoi(argv[1]);
        printInterval = atoi(argv[2]);
        alarmTime = atoi(argv[3]);
    }
    //check for incorrect number of args entered
    else if(argc > 1 && argc != 4) {
        cout << "Error: incorrect number of arguments." << endl;
        exit(0);
    }
    //check for incorrect interval entered 
    if(printInterval != 1 && printInterval != 60) {
        cout << "Error: incorrect interval. (must be 1 or 60)" << endl;
        exit(0);
    }

    //if successful, print times to console to show user what to expect
    cout << "Times listed below in seconds..." << endl;
    cout << "Total Time: " << totalTime << endl;
    cout << "Print Interval: " << printInterval << endl;
    cout << "Alarm Time: " << alarmTime << endl;

    //initialize the semaphore
    errorNo = sem_init(&FLAG, 0, 1);
    checkErr(errorNo);
    
    //create 4 threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        errorNo = pthread_create(&threadArray[i], NULL, manageThreads,(void *)i);
        checkErr(errorNo); 
    }

    //join the 4 threads 
    for (int i = 0; i < THREAD_COUNT; i++) {
        errorNo = pthread_join(threadArray[i], NULL);
        checkErr(errorNo); 
    }

    //destroy the semaphore after use
    errorNo = sem_destroy(&FLAG);
    checkErr(errorNo);

    cout << "Threads closed and program terminated." << endl;
}

/***********************[ EOF: emp.cpp ]*********************/