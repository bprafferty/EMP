/************************************************************
 Name: Brian Rafferty -- cssc0852 -- redID: 816988452
 Class: CS 570, Summer 2019
 Project: Program Assignment #3 -- Event Management Program
 File: emp.h
 Notes: Header file for emp.cpp, and functions by instantiating
        all methods that are used within the source code.
*************************************************************/
#ifndef EMP_H
#define EMP_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <csignal>

using namespace std;

void checkErr(int errorNo);
void signalHandler(int signalNumber);
void countdownTimer();
string getTime();
void wallClock();
void alarmClock();
void *manageThreads(void *threadPointer);

#endif

/***********************[ EOF: emp.h ]*********************/