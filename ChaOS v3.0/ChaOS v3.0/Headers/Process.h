#ifndef PROCESS__H
#define PROCESS__H
#include"..\Headers\SMS.h"
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
enum State { New, Ready, Waiting, Running, Terminated };


class PCB
{
private:
	int PID;
	int GID;
	State state;
	int instructionCounter;

	int burstTime;
	std::string programName;
	std::list<SMS>messages;
	std::pair<int, bool>* pages;
	int pagesSize;


public:
	int registers[4]; 
	static int processesCounter;
	PCB(std::string fileName, int GID);
	void SetState(State newState);
	void SetProcesBurstTime(int newBurstTime);
	int GetProcesBurstTime();
	int GetPID();
	int GetGID();
	std::string GetFileName();
	State GetState();
	void displayProcess();
	void SetInstructionCounter(int counter);
	int GetInstructionCounter();
	void setPages(std::pair<int, bool>* newPages);
	std::pair<int, bool>* getPages();
	void setPagesSize(int num);
	int getPagesSize();
	void addToMessages(SMS message);
	SMS getMessage();
	//Konrad: doda�em usuwanie bo potrzebuj� te� geta do wy�wietlania wiadomo�ci
	void deleteMessage();
	//Konrad: d�ugo�ci te� b�d� potrzebowa�
	int messagessize();
};
#endif PROCESS__H