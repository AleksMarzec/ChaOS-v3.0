#include "..\Headers\Siec.h"
#include"..\Headers\ProcessesManager.h"
extern PCB* ActiveProcess;
extern ProcessesManager *pm;

Siec::Siec()
{
}


Siec::~Siec()
{
}
bool Siec::wyslij(std::string wiad, int ID)
{
	std::list<std::list<PCB*>> lista = pm->getAllProcesseslist();
	//przeszukiwanie listy proces�w w poszukiwaniu grupy procesu aktywnego (procesy mog� si� komunikowa� tylko w obr�bie tej samej grupy)
	for (auto it = lista.begin(); it != lista.end(); it++)
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
						cv.rbegin()->wait(ActiveProcess);
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
	//skopiowanie listy wska�nik�w do aktywnych proces�w �eby iterowanie nie wywali�o programu w kosmos
	std::list<std::list<PCB*>> lista = pm->getAllProcesseslist();
	for (auto it = lista.begin(); it != lista.end(); it++)
	{
		//sprawdzenie czy procesu maj� to samo ID grupy
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
	std::cout << "ID aktywnego procesu: " << ActiveProcess->GetPID() << std::endl << "ID grupy tego procesu: " << ActiveProcess->GetGID() << std::endl;
	std::list<SMS> lista = ActiveProcess->getMessages();
	for(auto it=lista.begin();it!=lista.end();it++)
	std::cout << "Wiadomosc nr "<< i << ":" << std::endl << "ID procesu wysylajacego: " << it->getID() << std::endl  << std::endl << "Tresc wiadomosci: " << it->getwiad() << std::endl;
	i++;
	}
}
void Siec::wyswietlwiad()
{
	std::list<std::list<PCB*>> lista = pm->getAllProcesseslist();
	if (lista.empty()==true) std::cout << "Nie istnieje zaden proces!" << std::endl;
	else
	{
		//wy�wietlanie wiadomo�ci wszystkich proces�w grupami
		for (std::list<std::list<PCB*>>::iterator it = lista.begin(); it != lista.end(); it++)
		{
			std::cout << "Wyswietlanie wiadomosci procesow z grupy o ID " << (*it->begin())->GetGID() << std::endl;
			for (std::list<PCB*>::iterator et = it->begin(); et != it->end(); et++)
			{
				//sprawdzenie czy proces posiada jakie� nieodczytane wiadomosci
				std::list<SMS> wiadomosci = (*et)->getMessages();
				if (wiadomosci.size() == 0) std::cout << "Brak wiadomosci w kontenerze procesu o ID " << (*et)->GetPID() << std::endl;
				else
				{
					std::cout << "Wiadomosci w kontenerze procesu o ID " << (*et)->GetPID() << std::endl;
					int i = 1;
					//iteracja po li�cie wiadomo�ci procesu wy�wietlaj�c wszystkie
					for (std::list<SMS>::iterator zt = wiadomosci.begin(); zt != wiadomosci.end(); zt++)
					{
						std::cout << "Wiadomosc nr " << i << ":" << std::endl << "ID procesu wysylajacego: " << zt->getID() << std::endl << "Tresc wiadomosci: " << zt->getwiad() << std::endl;
						i++;
					}
				}
			}
		}
	}
}