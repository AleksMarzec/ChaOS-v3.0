#include "../Headers/ConditionVariable.h"
//#include "ProcessScheduling.h"		Czekam na Alberta XD
#include "../Headers/Process.h"
#include <iostream>

ConditionVariable::ConditionVariable()
{
	this->resourceOccupied = false;
}

// Zmienia stan procesu na waiting, dodaje do listy proces�w oczekuj�cych 
void ConditionVariable::wait(PCB* process)
{
	// Tych if�w nie jestem pewna - zobaczymy podczas test�w jak to b�dzie dzia�a�
	//if (resourceOccupied)
	//{
		process->SetState(State::Waiting);
		waitingProcesses.push_back(process);
		// SRTSchedulingAlgorithm();
	//}
	//else
	//{
	//	process->SetState(State::Ready);
		// Dodanie do listy proces�w oczekuj�cych na li�cie planisty
		// SRTSchedulingAlgorithm();
	//}
}

// Zmienia stan procesu na gotowy, usuwa go z listy proces�w oczekuj�cych i w��cza planist�.
// Je�li nic nie czeka pod zmienn� warunkow� to wywo�anie metody jest ignorowane.
void ConditionVariable::signal()
{
	if (!resourceOccupied && !waitingProcesses.empty())
	{
		PCB* temp = waitingProcesses.front();
		temp->SetState(State::Ready);
		// Dodanie do listy proces�w oczekuj�cych na li�cie planisty
		waitingProcesses.pop_front();
		//this->resourceOccupied = true;
		//SRTSchedulingAlgorithm();
		delete temp;
	}
}


// Wypisuje jakie� pierdo�y o PCB do sprawdzenia co tam siedzi w li�cie.
void ConditionVariable::displayWaitingProcesses()
{
	for (std::list<PCB*>::iterator it = waitingProcesses.begin(); it != waitingProcesses.end(); it++)
	{
		(*it)->displayProcess();
	}
}


bool ConditionVariable::getResourceOccupied()
{
	return resourceOccupied;
}