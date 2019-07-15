Brian Rafferty -- cssc0852 -- redID: 816988452
---------------------
CS 570 Operating Systems, Summer 2019
---------------------
Program Assignment #3, EMP
---------------------
README.md
---------------------
Manifest:
    README.md
    emp.cpp
    emp.h
    makefile
----------------------
Compiling executable:
    make
----------------------
Running executable:
 * with default values:
    emp
 * with custom values:
    emp num1 num2 num3
----------------------
List/description of novel/significant design decisions:
Included error checks on all system calls used with semaphores,
and also checked for errors with possible user input.
----------------------
List/description of any extra features/algorithms/functionality 
you included which were not required:
I added extra stdout feedback on the terminal when the user
runs the executable. There will be a message with the units of
time and the amount of time for each entry.
----------------------
List/description of all known deficiencies or bugs:
There is a warning when compiling the executable, which reads:
warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]
    errorNo = pthread_create(&threadArray[i], NULL, manageThreads,(void *)i);
The executable will still run regardless of the warning message.
----------------------
Lessons Learned:
Signals a great ways to allow communication between threads,
and I learned a lot about the different kinds of signals that
exist.