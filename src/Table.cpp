#include "Table.h"

Table::Table(vector<Cell*>	cells, vector<Joint*> joints) {
	this->cells = cells;
	this->joints = joints;

	lastNotConcertedCellPtr = NULL;
	//находим первый джоинт
	Joint* firstJoint = joints[0];
	for (size_t i = 1; i < joints.size(); i++)
	{
		if (joints[i]->pos.length() < firstJoint->pos.length()) firstJoint = joints[i];
	}
	firstJoint->x = 0;
	firstJoint->y = 0;

	incrimentJointX(firstJoint, 0);
	incrimentJointY(firstJoint, 0);

	//базовые горизонталь и вертикаль с которых мы начинаем нумерацию
	vector<Joint*> baseHorizontal;
	vector<Joint*> baseVertical;

	//дл€ всех джоинтов у которых есть y координаты 
	for (size_t i = 0; i < joints.size(); i++) {
		if (joints[i]->y != -1 && joints[i] != firstJoint) baseVertical.push_back(joints[i]);
		if (joints[i]->x != -1 && joints[i] != firstJoint) baseHorizontal.push_back(joints[i]);
	}
	cout << "baseHorizontal.size() " << baseHorizontal.size() << endl;
	cout << "baseVertical.size() " << baseVertical.size() << endl;

	for (size_t i = 0; i < baseHorizontal.size(); i++)
	{
		baseHorizontal[i]->y = 0;
		incrimentJointY(baseHorizontal[i], 0);
	}
	for (size_t i = 0; i < baseVertical.size(); i++)
	{
		baseVertical[i]->x = 0;
		incrimentJointX(baseVertical[i], 0);
	}
	eliminateJointsInconsistency();
	numerateCells();
}


void Table::eliminateJointsInconsistency() {

	Cell* notConcertedCellPtr = getNotConcertedCellPtr();
	int maxDeep = MAX_NOT_CONCERTED_CELL;
	while (notConcertedCellPtr != NULL && maxDeep > 0)
	{
		eliminateCellJointInconsistency(notConcertedCellPtr);
		notConcertedCellPtr = getNotConcertedCellPtr();
		maxDeep--;
	}
	cout << "Table updateJointIndeces " << MAX_NOT_CONCERTED_CELL - maxDeep << " cells consistency" << endl;
}

void Table::eliminateCellJointInconsistency(Cell * notConcertedCellPtr)
{
	Joint* leftUp = notConcertedCellPtr->leftUp;
	Joint* leftDown = notConcertedCellPtr->leftDown;
	Joint* rightUp = notConcertedCellPtr->rightUp;
	Joint* rightDown = notConcertedCellPtr->rightDown;

	//согласуем y координату
	if (leftUp->y < rightUp->y) {
		leftUp->y = rightUp->y;
		incrimentJointY(leftUp, 0);
	}
	if (rightUp->y < leftUp->y) {
		rightUp->y = leftUp->y;
		incrimentJointY(rightUp, 0);
	}
	if (leftDown->y < rightDown->y) {
		leftDown->y = rightDown->y;
		incrimentJointY(leftDown, 0);
	}
	if (rightDown->y < leftDown->y) {
		rightDown->y = leftDown->y;
		incrimentJointY(rightDown, 0);
	}


	//согласуем x координату 
	if (leftUp->x < leftDown->x) {
		leftUp->x = leftDown->x;
		incrimentJointX(leftUp, 0);
	}
	if (leftDown->x < leftUp->x) {
		leftDown->x = leftUp->x;
		incrimentJointX(leftDown, 0);
	}
	if (rightUp->x < rightDown->x) {
		rightUp->x = rightDown->x;
		incrimentJointX(rightUp, 0);
	}
	if (rightDown->x < rightUp->x) {
		rightDown->x = rightUp->x;
		incrimentJointX(rightDown, 0);
	}

	//на случай неполносв€заной таблицы
	//если лева€ точка -1 а права€ нет
	if (leftUp->x == -1 && rightUp->x != -1) {
		if (rightUp->x < 1) {
			leftUp->x = 0;
			incrimentJointX(leftUp, 0);
		}
		else {
			leftUp->x = rightUp->x - 1;
		}
	}
	//если верхн€€ точка -1 а нижн€€ нет
	if (leftUp->y == -1 && leftDown->y != -1) {
		if (leftDown->y < 1) {
			leftUp->y = 0;
			incrimentJointY(leftUp, 0);
		}
		else {
			leftUp->y = leftDown->y - 1;
		}
	}
}

void Table::numerateCells() {
	for (int i = 0; i < cells.size(); i++) {
		cells[i]->x.clear();
		for (int j = cells[i]->leftUp->x; j < cells[i]->rightUp->x; j++)
		{
			cells[i]->x.push_back(j);
		}

		cells[i]->y.clear();
		for (int j = cells[i]->leftUp->y; j < cells[i]->leftDown->y; j++)
		{
			cells[i]->y.push_back(j);
		}
	}
}


Table::~Table()
{
	for (size_t i = 0; i < cells.size(); i++) {
		delete cells[i];
	}
	for (size_t i = 0; i < joints.size(); i++) {
		delete joints[i];
	}
}

void Table::draw() {
	ofPushMatrix();
	ofScale(1, 1);
	for (size_t i = 0; i < cells.size(); i++)
	{
		cells[i]->draw(ofColor(0, 128, 128));
	}
	if (lastNotConcertedCellPtr)lastNotConcertedCellPtr->draw(ofColor(0, 255, 255));
	ofPopMatrix();
}



void Table::incrimentJointX(Joint * first, int deep) {
	//нумеруем джоинты от него
	Joint* leftUp = first;

	//если есть €чейка дл€ которой данный нод €вл€етс€ левым верхним
	if (!leftUp->leftUpForThisCell)			return;
	Cell* cell = leftUp->leftUpForThisCell;
	if (!cell->rightUp)						return;
	if (deep > INCRIMENT_JOINT_MAX_DEEP)	return;

	Joint* rightUp = cell->rightUp;
	rightUp->x = std::max(rightUp->x, leftUp->x + 1);
	incrimentJointX(rightUp, deep + 1);
};

void Table::incrimentJointY(Joint * first, int deep) {
	//нумеруем джоинты от него
	Joint* leftUp = first;

	//если есть €чейка дл€ которой данный нод €вл€етс€ левым верхним
	if (!leftUp->leftUpForThisCell)			return;
	Cell* cell = leftUp->leftUpForThisCell;
	if (!cell->leftDown)					return;
	if (deep > INCRIMENT_JOINT_MAX_DEEP)	return;

	Joint* leftDown = cell->leftDown;
	leftDown->y = std::max(leftDown->y, leftUp->y + 1);
	incrimentJointY(leftDown, deep + 1);
};

Cell* Table::getNotConcertedCellPtr() {
	for (size_t i = 0; i < cells.size(); i++)
	{
		Cell*  cell = cells[i];
		Joint* leftUp = cells[i]->leftUp;
		Joint* leftDown = cells[i]->leftDown;
		Joint* rightUp = cells[i]->rightUp;
		Joint* rightDown = cells[i]->rightDown;

		if (leftUp->x != leftDown->x)		return cell;
		if (rightUp->x != rightDown->x)		return cell;
		if (leftUp->y != rightUp->y)		return cell;
		if (leftDown->y != rightDown->y)	return cell;
	}

	return NULL;
};



