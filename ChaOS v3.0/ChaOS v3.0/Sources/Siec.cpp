#include "..\Headers\Siec.h"
#include"..\Headers\ProcessesManager.h"
#include"..\Headers\ConditionVariable.h"
#include<vector>
extern PCB* ActiveProcess;
extern ProcessesManager *pm;
extern std::vector<ConditionVariable> cv;
Siec::Siec()
{
}


Siec::~Siec()
{
}
bool Siec::wyslij(std::string wiad, int ID)
{
	//przeszukiwanie listy proces�w w poszukiwaniu grupy procesu aktywnego (procesy mog� si� komunikowa� tylko w obr�bie tej samej grupy)
	for (auto it = pm->getAllProcesseslist().begin(); it != pm->getAllProcesseslist().end(); it++)
	{
		if (ActiveProcess->GetGID() == (*it->begin())->GetGID())
		{
			//je�li GID si� zgadza to szukamy procesu o wskazanym ID
			for (auto et = it->begin(); et != it->end(); et++)
			{
				if (ID == (*et)->GetPID())
				{
					bool wolnazmienna = false;
					for (int i = 0; i < cv.size(); i++)
					{
						//sprawdzenie czy nie ma nieu�ywanej zmiennej odpowiedzialnej za komunikacj� w kontenerze
						if (!cv[i].getResourceOccupied())
						{
							wolnazmienna = true;
							cv[i].lockmessagesender();
							(*et)->addToMessages(SMS(wiad,i));
							cv[i].wait(ActiveProcess);
							break;
						}
					}
					if (!wolnazmienna)
					{
						//stworzenie zmiennej warunkowej odpowiadaj�cej za synchroniczn� komunikacj�
						ConditionVariable x;
						x.lockmessagesender();
						cv.push_back(x);
						//dodanie wiadomo�ci do kolejki w PCB
						(*et)->addToMessages(SMS(wiad,cv.size()-1));
						cv.end()->wait(ActiveProcess);
					}
					return true;
				}
			}
		}
	}
	//je�li odpowiedni proces-odbiorca nie zosta� znaleziony to zostanie zwr�cona warto�� false
	return false;
}
std::unique_ptr<SMS> Siec::odbierz()
{
	if (ActiveProcess->getMessages().size() == 0) return nullptr;
	std::unique_ptr<SMS> pom = std::make_unique<SMS>(ActiveProcess->getMessage());
	ActiveProcess->deleteMessage();
	for (auto it = pm->getAllProcesseslist().begin(); it != pm->getAllProcesseslist().end(); it++)
	{
		if (ActiveProcess->GetGID() == (*it->begin())->GetGID())
		{
			for (auto et = it->begin(); et != it->end(); et++)
			{
				if (pom->getID() == (*et)->GetPID())
				{
					//zmiana procesu nadawcy na gotowy (czeka� na odebranie wiadomo�ci)
					cv[pom->getCVindex()].signal();
				}
			}
		}
	}
	return pom;
}
void Siec::wyswietlwiadaktywnego()
{
	if (ActiveProcess->getMessages().size() == 0) std::cout << "Brak wiadomosci w kontenerze aktywnego procesu!" << std::endl;
	else
	{
	int i = 1;
	std::cout << "ID aktywnego procesu: " << ActiveProcess->GetPID() << "ID grupy tego procesu: " << ActiveProcess->GetGID() << std::endl;
	for(auto it=ActiveProcess->getMessages().begin();it!=ActiveProcess->getMessages().end();it++)
	std::cout << "Wiadomosc nr "<< i << ":" << std::endl << "ID procesu wysylajacego: " << it->getID() << std::endl  << std::endl << "Tresc wiadomosci: " << it->getwiad() << std::endl;
	i++;
	}
}
void Siec::wyswietlwiad()
{
	if (pm->getAllProcesseslist().size() == 0) std::cout << "Nie istnieje zaden proces!" << std::endl;
	else
	{
		for (auto it = pm->getAllProcesseslist().begin(); it != pm->getAllProcesseslist().end(); it++)
		{
			std::cout << "Wyswietlanie wiadomosci procesow z grupy o ID " << (*it->begin())->GetGID();
			for (auto et = (*it).begin(); et != (*it).end(); et++)
			{
				if ((*et)->getMessages().size() == 0) std::cout << "Brak wiadomosci w kontenerze procesu o ID " << (*et)->GetPID() << std::endl;
				else
				{
					std::cout << "Wiadomosci w kontenerze procesu o ID " << (*et)->GetPID() << std::endl;
					int i = 1;
					for (auto zt = (*et)->getMessages().begin();zt != (*et)->getMessages().end(); zt++)
					{
						std::cout << "Wiadomosc nr " << i << ":" << std::endl << "ID procesu wysylajacego: " << zt->getID() << std::endl << "Tresc wiadomosci: " << zt->getwiad() << std::endl;
						i++;
					}
				}
			}
		}
	}
}