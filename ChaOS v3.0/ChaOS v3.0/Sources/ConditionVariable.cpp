#include "../Headers/ConditionVariable.h"
#include "../Headers/Process.h"
#include <iostream>


ConditionVariable::ConditionVariable()
{
	this->resourceOccupied = false;
}


// Zmienia stan procesu na waiting, dodaje do listy proces�w oczekuj�cych 
void ConditionVariable::wait(PCB* process)
{
	if (resourceOccupied)
	{
		process->SetState(State::Waiting);
		waitingProcesses.push_back(process);
		this->resourceOccupied = false;
	}
	if (this->waitingProcesses.empty() || !resourceOccupied)
	{
		process->SetState(State::Ready);
		this->resourceOccupied = false;
		pm->AddProcessToReady(process);
	}
}


// Zmienia stan procesu na gotowy, usuwa go z listy proces�w oczekuj�cych i w��cza planist�.
// Je�li nic nie czeka pod zmienn� warunkow� to wywo�anie metody jest ignorowane.
void ConditionVariable::signal()
{
	if (!resourceOccupied && !waitingProcesses.empty())
	{
		PCB* temp = waitingProcesses.front();
		temp->SetState(State::Ready);
		pm->AddProcessToReady(temp);
		waitingProcesses.pop_front();
		this->resourceOccupied = true;
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