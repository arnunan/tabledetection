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
	//��� ������� ����������� ������ ����� ����� ������� ����, 
	//��� ��������� �.�. �������� ������������ ����� ���������� �����, 
	//��������� ���������� ����������������� getNotConcertedCellPtr()
	void incrimentJointX(Joint * first, int deep);
	void incrimentJointY(Joint * first, int deep);

	//��������� ����������������� ���� ��������
	void eliminateJointsInconsistency();
	//��������� ����������������� �������� �������� �����
	void eliminateCellJointInconsistency(Cell* notConcertedCellPtr);
	//��������� ���������� �������, �������� ��������������� ��������
	void numerateCells();

	Cell* getNotConcertedCellPtr();

	Cell* lastNotConcertedCellPtr;

	vector<Cell*>	cells;
	vector<Joint*>	joints;
};
