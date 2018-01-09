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
	if (resourceOccupied || !this->waitingProcesses.empty())
	{
		process->SetState(State::Waiting);
		process->wait = true;
		waitingProcesses.push_back(process);
		this->resourceOccupied = true;
		//std::cout << "ZMIENNA WARUNKOWA \nProces: \n";
		//process->displayProcess();
		//std::cout << "poprzez wait(PCB* process) znalaz� si� na li�cie proces�w oczekuj�cych." << std::endl;
	}
	else if (!resourceOccupied && this->waitingProcesses.empty())
	{
		process->SetState(State::Ready);
		this->resourceOccupied = true;
		//std::cout << "ZMIENNA WARUNKOWA \nProces: \n";
		//process->displayProcess();
		//std::cout << "poprzez wait(PCB* process) przeszed� od razu do stanu ready." << std::endl;
	}
	else
	{
		std::cout << "Blad z zmiennej warunkowej w metodzie wait(PCB*)." << std::endl;
	}
}


// Zmienia stan procesu na gotowy, usuwa go z listy proces�w oczekuj�cych i w��cza planist�.
// Je�li nic nie czeka pod zmienn� warunkow� to wywo�anie metody jest ignorowane.
void ConditionVariable::signal()
{
	if (resourceOccupied && !waitingProcesses.empty())
	{
		auto temp = waitingProcesses.front();
		temp->SetState(State::Ready);
		waitingProcesses.pop_front();

		if (this->waitingProcesses.empty())
		{
			this->resourceOccupied = false;
		}
		else
		{
			this->resourceOccupied = true;
		}

		//std::cout << "ZMIENNA WARUNKOWA \nProces:\n";
		//temp->displayProcess();
		//std::cout << "poprzez signal() jest w stanie ready." << std::endl;

		//delete temp;
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


//Konrad - dopisa�em �eby proces-nadawca nie przechodzi� od razu do ready
void ConditionVariable::lockmessagesender()
{
	this->resourceOccupied = true;
}