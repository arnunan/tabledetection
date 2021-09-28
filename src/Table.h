#pragma once
#include "ofMain.h"
#include "Cell.h"

#define INCRIMENT_JOINT_MAX_DEEP	30
#define MAX_NOT_CONCERTED_CELL		1000

class Table
{
public:
	Table(vector<Cell*>	cells, vector<Joint*> joints);
	~Table();

	void draw();

private:
	//при проходе перемещаюсь только через левый верхний узлы, 
	//это допустимо т.к. значения непройденных узлов заполнится позже, 
	//благодаря устранению несогласованности getNotConcertedCellPtr()
	void incrimentJointX(Joint * first, int deep);
	void incrimentJointY(Joint * first, int deep);

	//устранить несогласованность всех джоинтов
	void eliminateJointsInconsistency();
	//устранить несогласованность индексов джоинтов ячеки
	void eliminateCellJointInconsistency(Cell* notConcertedCellPtr);
	//назначить координаты ячейкам, согласно пронумерованным индексам
	void numerateCells();

	Cell* getNotConcertedCellPtr();

	Cell* lastNotConcertedCellPtr;

	vector<Cell*>	cells;
	vector<Joint*>	joints;
};
