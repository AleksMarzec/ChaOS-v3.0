#ifndef PROCESSSCHEDULER__H
#define PROCESSSCHEDULER__H

#include "ProcessesManager.h"
#include "Process.h"
#include <iostream>
#include <list>

class ProcessScheduler {
private:
	//zmienna pomocnicza, potrzebna podczas wybierania procesu o najkr�tszym czasie, lecz przed ustawieniem ActiveProcess
	std::list<PCB*>::iterator iteratorToMinElement;
	//differenceCounter	-- licznik okre�laj�cy, ile instrukcji wykona�o si� dla ActiveProcess
	//startCounter		-- licznik okre�laj�cy, kt�ra instrukcja by�a wykonana jako pierwsza dla nowo-przypisanego ActiveProcess
	//endCounter		-- licznik okre�laj�cy, kt�ra instrukcja by�a wykonana dla ActiveProcess ostatnio (albo b�dzie wykonana..?)
	short differenceCounter, startCounter, endCounter, instructions{ 0 };
	//int helpBurstTime;
	//Procedura odpowiedzialna za ustawianie ActiveProcess metod� SRT - Shortest Remaining Time - wyw�aszczaj�c� wersj� SJF
	void SRTSchedulingAlgorithm();
public:
	//Procedura wywo�ywana przed ka�dym wykonaniem instrukcji - z jej poziomu wywo�ywany jest planista (SRTSchedulingAlgorithm) oraz metoda DoCommand z interpretera
	void RunProcess();
	void ProcessScheduler::RunProcess(int);
};
#endif PROCESSSCHEDULER__H