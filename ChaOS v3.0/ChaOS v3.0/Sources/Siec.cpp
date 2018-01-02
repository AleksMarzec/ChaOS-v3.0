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
					//dodanie wiadomo�ci do kolejki w PCB
					(*et)->addToMessages(SMS(wiad));
					//zmiana stanu procesu wysy�aj�cego na oczekuj�cy
					ActiveProcess->SetState(Waiting);
					//zmiana stanu procesu odbiorcy na gotowy
					if((*et)->GetState()==Waiting) (*et)->SetState(Ready);
					return true;
				}
			}
		}
	}
	//je�li odpowiedni proces-odbiorca nie zosta� zanleziony to zostanie zwr�cona warto�� false
	return false;
}
std::unique_ptr<SMS> Siec::odbierz()
{
	if (ActiveProcess->messagessize() == 0) return nullptr;
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
					(*et)->SetState(Ready);
				}
			}
		}
	}
	return pom;
}
void Siec::wyswietlwiad()
{
	if (ActiveProcess->messagessize() == 0) std::cout << "Brak wiadomosci w kontenerze aktywnego procesu!" << std::endl;
	else
	{
	for(int i=0;i<ActiveProcess->messagessize();i++)
	std::cout << "Wiadomosc nr "<< i << ":" << std::endl << "ID procesu wysylajacego: " << ActiveProcess->getMessage().getID() << std::endl << "ID procesu-odbiorcy: " << ActiveProcess->GetPID() << std::endl <<"ID grupy tych procesow: "<<ActiveProcess->GetGID()<<std::endl<< "Tresc wiadomosci: " << ActiveProcess->getMessage().getwiad() << std::endl;
	}
}