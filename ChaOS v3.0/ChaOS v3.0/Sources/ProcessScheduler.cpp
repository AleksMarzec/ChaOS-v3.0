#include "../Headers/ProcessScheduler.h"
#include "../Headers/Interpreter.h"
#include <list>

extern PCB* ActiveProcess;
extern ProcessesManager *pm;
extern Interpreter* i;

void ProcessScheduler::RunProcess(int count)
{
	for (int i = 0; i < count; i++) 
	{
		RunProcess();
	}
}

void ProcessScheduler::RunProcess()
{

	if (ActiveProcess == nullptr)
	{
		SRTSchedulingAlgorithm();
	}

	else
	{
		//Sprawdzenie, czy wyst�pi� jaki� b��d lub proces zako�czy� si� wykonywa�
		if (ActiveProcess->errorCode != 0 || ActiveProcess->GetState() == 4)
		{
			while (ActiveProcess->errorCode != 0 || ActiveProcess->GetState() == 4)
			{
				pm->killProcess(ActiveProcess->GetPID());
				ActiveProcess = nullptr;
				//Zerowanie zmiennych pomocniczych do liczenia wykonanych instrukcji ActiveProcess
				startCounter = endCounter = differenceCounter = 0;
				instructions = 0;

				//zabezpieczenie przed nullptr; zanim cos sie stanie - ustawiany jest proces bezczynnosci
				for (std::list<PCB*>::iterator p = pm->readyProcesses.begin(); p != pm->readyProcesses.end(); ++p)
				{
					if ((*p)->GetPID() == 1)
					{
						ActiveProcess = *p;
					}
				}

				SRTSchedulingAlgorithm();
			}
		}

		else
		{
			//Sprawdzenie, czy ActiveProcess jest ustawiony na jaki� proces
			if (ActiveProcess != nullptr)
			{
				if (ActiveProcess->GetPID() != 1)
				{
					++instructions;
				}
				//Sprawdzenie rozmiaru listy readyProceesses -- size() > 1 oznacza, �e ActiveProcess nie jest procesem bezczynno�ci
				if (pm->readyProcesses.size() > 1) {
					//Wyliczenie, ile instrukcji wykona�o si� dla ActiveProcess
					/*endCounter = ActiveProcess->GetInstructionCounter();
					differenceCounter = endCounter - startCounter;*/
					//ActiveProcess->SetProcesBurstTime(ActiveProcess->GetProcesBurstTime() - 1);
				}
				//Proces bezczynno�ci nie wykonuje instrukcji, wi�c nie ma powodu do ustawiania zmiennych pomocniczych
			}

			//Planista wywo�ywany za ka�dym razem - obs�uguje wszystkie mo�liwe przypadki
			SRTSchedulingAlgorithm();
		}
	}
	//Uruchomienie metody DoCommand() interpretera, gdzie mam nadzieje zmieniany jest instructionCounter wykonywanego procesu
	i->DoCommand();
}

void ProcessScheduler::SRTSchedulingAlgorithm()
{
	//Gdy rozmiar readyProcesses == 1 -- tym procesem jest proces bezczynno�ci
	//Gdy ActiveProcess == nullptr -- trzeba ustawi� ActiveProcess jedynym elementem w readyProcesses
	if (pm->readyProcesses.size() == 1) //==nullptr proces bzeczynnosci jeszcze nie jest ustawiony   && ActiveProcess == nullptr
	{
		ActiveProcess = *pm->readyProcesses.begin();
		//Dla procesu bezczynno�ci nie trzeba ustawia� �adnych zmiennych pomocniczych, poniewa� nie wykonuje on �adnych instrukcji
		return;
	}

	//Gdy rozmiar readyProcesses == 1 -- procesem jest ponownie proces bezczynno�ci
	//Gdy Activeprocess != nullptr -- nie trzeba ustawia� ActiveProcess
	//tu sie sypie po usunieci procesu po jego wykonaniu
	//if (pm->readyProcesses.size() == 1 && ActiveProcess->GetPID() == 1) //czyli ustawiony proces bezczynnosci
	//{
		//Nie trzeba te� niczego robi�
		//return;
	//}



	///
	//Tylko, czy to kurka dobrze, �e si� ci�gle wylicza nowy....? czy nie bedzie sie zmieniac o 1 ciagle?
	//Zgodnie z tym, co mowi dr Bartoszek, trzeba liczyc czas przed porownaniem, ale tylko dla proces�w innych niz bezczynnosci
	//tu sypie sie...
	//if (ActiveProcess != nullptr && ActiveProcess->GetPID() != 1)
	//{
	//	endCounter = ActiveProcess->GetInstructionCounter();
	//	differenceCounter = endCounter - startCounter;
	//	std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << instructions << std::endl;
	//	//std::cout << std::endl;
	//	//std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << differenceCounter << "\tinstrukcji" << std::endl;
	//	//std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\t burst time przed!" << std::endl;
	//	ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * instructions);
	//	//std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\tburst time po!" << std::endl;
	//	//std::cout << std::endl;
	//}
	///




	//Ustawienie iteratora pomocniczego na proces o najkrotszym burstTime zgodnie z algorytmem SRT
	iteratorToMinElement = std::min_element(pm->readyProcesses.begin(), pm->readyProcesses.end(), [](PCB* x, PCB* y) { return x->GetProcesBurstTime() < y->GetProcesBurstTime(); });

	//nie wiem czy to sie kiedykolwiek wywola jak nie ma nullptra ustawianego
	//Gdy ActiveProcess == nullptr -- nie trzeba liczyc czasu dla poprzednio aktywnego procesu, poniewa� proces zostal:
	//1) wykonany		2)usuniet z powodu bledu		-- jeszcze jakie� przypadki?
	if (ActiveProcess == nullptr) //czy to sie wywola jesli usunalem ustawiania ActiveProcess na nullptr
	{
		//Ustawienie ActiveProcess najlepszym procesem zgodnie z SRT
		ActiveProcess = *iteratorToMinElement;

		//Ustawienie zmiennych pomocniczych do liczenia wykonanych instrukcji ActiveProcess
		startCounter = ActiveProcess->GetInstructionCounter();
		endCounter = differenceCounter = 0;
		instructions = 0;
	}

	//ActiveProcess != nullptr -- ActiveProcess by� ustawiony przez jaki� proces
	else
	{
		///Zgodnie z tym, co mowi dr Bartoszek, trzeba liczyc czas przed porownaniem
		///ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * differenceCounter);

		//tu sie sypalo
		if (ActiveProcess != nullptr && ActiveProcess->GetPID() != 1)
		{
			endCounter = ActiveProcess->GetInstructionCounter();
			differenceCounter = endCounter - startCounter;
			std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << instructions << std::endl;
			//std::cout << std::endl;
			//std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << differenceCounter << "\tinstrukcji" << std::endl;
			//std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\t burst time przed!" << std::endl;
			ActiveProcess->SetProcesBurstTime(.5 * ActiveProcess->GetProcesBurstTime() + .5 * instructions);
			//std::cout << "[~~ Albert ~~]\t\t" << ActiveProcess->GetPID() << "\t" << ActiveProcess->GetProcesBurstTime() << "\tburst time po!" << std::endl;
			//std::cout << std::endl;
		}

		//Sprawdzenie przy pomoy PID, czy wybrany proces jest na pewno innym procesem
		if ((*iteratorToMinElement)->GetPID() != ActiveProcess->GetPID())
		{
			//Sprawdzenie, czy wybrany proces, obecnie ustawiony w iteratorze iteratorToMinElement, ma przewidywany czas kr�tszy od ActiveProcess
			if ((*iteratorToMinElement)->GetProcesBurstTime() < ActiveProcess->GetProcesBurstTime()) //nie wiem do konca, czy z tym differenceCounter, czy jescze nie /// - differenceCounter
			{
				//if and else should be together
				//if (ActiveProcess->GetPID() != 1)
				//{
					//Ustawienie ActiveProcess
					ActiveProcess = *iteratorToMinElement;
					//Ustawienie zmiennych pomocniczych do liczenia wykonywanych instrukcji ActiveProcess
					startCounter = ActiveProcess->GetInstructionCounter();
					endCounter = differenceCounter = 0;
					instructions = 0;
				//}
				//else {
					//Ustawienie ActiveProcess
							//ActiveProcess = *iteratorToMinElement;
					//Ustawienie zmiennych pomocniczych do liczenia wykonywanych instrukcji ActiveProcess
							//startCounter = ActiveProcess->GetInstructionCounter();
							//endCounter = differenceCounter = 0;
				//}
			}
			else 
			{
				return;
			}
		}
		//Sytuacja, w kt�rej zosta� wybrany ten sam proces, co ActiveProcess
		else 
		{
			return;
		}
	}

}