#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vector>
#include <stack>
#include <queue>
#include <Windows.h>

#define	STONE(n, x, y)	stones[((n) << 6) + ((y) << 3) + (x)]
#define MAP(m, x, y) map[(x) + (y)*(width+1)]

int sendMsg(char *msg);

void DEBUG_printMap(const int* map, const int width, const int height){
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			int value = map[x + y*width];
			if(value == -1){
				printf("# ");
			} else if(value == -2){
				printf("G ");
			} else if(value == -3){
				printf("* ");
			} else {
				printf("%c ", value+48);
			}
		}
		printf("\n");
	}
}

void DEBUG_printPutShitOnMap(const int* map, const int* putPoints, const int ppNum, const int width, const int height){
	printf("\t( ");
	for(int i = 0; i < ppNum; i++){
		printf("%d ", putPoints[i]);
	}printf(")\n");

	int* DEBUG_MAP = new int[(width+1)*height];
	for(int i = 0; i < (width+1)*height; i++){
		DEBUG_MAP[i] = map[i];
	}

	for(int i = 0; i < ppNum; i++){
		if(putPoints[i] >= 0 && putPoints[i] < (width+1)*height){
			DEBUG_MAP[putPoints[i]] = -3;
		}
	}
	DEBUG_printMap(DEBUG_MAP, width+1, height);

	delete DEBUG_MAP;
}

void DEBUG_printBaseAryStone(const int* shit){
	printf("\t[ ");
	for(int i = 0; shit[i] != -1; i++){
		printf("%d ", shit[i]);
	}
	printf("]\n");
}

void DEBUG_printMapStone(const int* stones, const int n){
	for (int y=0; y<8; y++) {
		printf("\t\t");
		for (int x=0; x<8; x++) printf("%d", STONE(n, x, y));	
		printf("\n");
	}
}

char DEBUG_C;
void DEBUG_waitKey(){
	scanf("%c", &DEBUG_C);
}

bool DEBUG_JudgePutable(const int* map, const int* shit, const int basePoint, const int width, const int height, int* putPoints, int* putAryLength){
	int i;
	bool result = true;

	for(i = 0; shit[i] != -1; i++){
		putPoints[i] = shit[i];
	}
	*putAryLength = i;

	for(int j = 0; j < *putAryLength; j++){
		int preValue = putPoints[j];
		putPoints[j] += basePoint;

		if(putPoints[j] < 0 || putPoints[j] >= (width+1)*height){
			result = false;
		}
		if(map[putPoints[j]] != 0){
			result = false;
		}
	}
	return result;
}

//�z�u�_�z���ێ�����N���X
class PutPoint{
private:
	int pointIndex;
	int shitNumber;
	int shitState;
	std::vector<int> p;
	int* neighborAry;
	bool start;

public :
	PutPoint(const int shitNumber, const int shitState, const int* shit, PutPoint* prevP, const int shitSize, const int* subArea){
		int size;
		for(size = 0; prevP->neighborAry[size] != -1; size++);

		neighborAry = new int[size+1];
		for(int i = 0; i < size; i++){
			neighborAry[i] = prevP->neighborAry[i];
		}
		neighborAry[size] = -1;

		for(int s = 0; shit[s] != -1; s++){
			for(int n = 0; neighborAry[n] != -1; n++){
				if(subArea != 0 && subArea[neighborAry[n]] < shitSize){
					continue;
				}

				int value = neighborAry[n] - shit[s];
				for(int i = 0; i < p.size(); i++){
					if(value == p[i]){
						goto NEXT;
					}
				}
				p.push_back(value);

			NEXT:;
			}
		}

		this->pointIndex = 0;
		this->shitState = shitState;
		this->shitNumber = shitNumber;
		this->start = false;
	}
	PutPoint(const int shitNumber, const int shitState, const int* shit, const int* neighbor, const int neighborNum, const int shitSize, const int* subArea){
		for(int s = 0; shit[s] != -1; s++){
			for(int n = 0; neighbor[n] != -1; n++){
				if(subArea != 0 && subArea[neighbor[n]] < shitSize){
					continue;
				}
				int value = neighbor[n] - shit[s];

				for(int i = 0; i < p.size(); i++){
					if(value == p[i]){
						goto NEXT;
					}
				}
				p.push_back(value);

			NEXT:;
			}
		}

		neighborAry = new int[neighborNum+1];
		for(int i = 0; i < neighborNum; i++){
			neighborAry[i] = neighbor[i];
		}
		neighborAry[neighborNum] = -1;

		this->pointIndex = 0;
		this->shitState = shitState;
		this->shitNumber = shitNumber;
		this->start = false;
	}
	PutPoint(const int shitNumber, const int shitState, const int* shit, const int* startMap){
		for(int s = 0; shit[s] != -1; s++){
			for(int n = 0; startMap[n] != -1; n++){
				int value = startMap[n] - shit[s];
				for(int i = 0; i < p.size(); i++){
					if(value == p[i]){
						goto NEXT;
					}
				}
				p.push_back(value);

			NEXT:;
			}
		}

		neighborAry = new int[1];
		neighborAry[0] = -1;

		this->pointIndex = 0;
		this->shitState = shitState;
		this->shitNumber = shitNumber;
		this->start = true;
	}
	~PutPoint(){
		delete neighborAry;
	}

	const int* GetNeighborAry(){
		return neighborAry;
	}
	const int* GetPointAry(){
		return &(p[0]);
	}
	bool GetNextValue(int* value){
		if(pointIndex < p.size()){
			*value = p[pointIndex];
			pointIndex++;
			return true;
		}

		return false;
	}
	bool isStart(){
		return start;
	}
	int GetShitNumber(){
		return shitNumber;
	}
	int GetShitState(){
		return shitState;
	}
};

//�Q�̔z�񂪓������̂����`�F�b�N����֐�
bool JudgeSameAry(const int* a, const int* b, const int size){
	for(int i = 0; i < size; i++){
		if(a[i] != b[i]) return false;
	}
	return true;
}

//�}���\�[�g
void InsertSortArray(int num[], int n){
    int i, j, temp;

    for (i = 1; i < n; i++) {  
        temp = num[i];         
        for (j = i; j > 0 && num[j-1] > temp; j--) 
            num[j] = num[j-1];
        num[j] = temp;
	}
}

//��œ񕪒T����
//������
//�z����ɒl�����݂��邩�ǂ����𔻒肷�郁�\�b�h
bool JudgeExistValue(const int* ary, const int aryNum, const int value){
	for(int i = 0; i < aryNum; i++){
		if(ary[i] == value){
			return true;
		}
	}

	return false;
}

//�}�b�v�\�����ꂽ���i�Y�N�j�̃T�C�Y�𑪂�֐�
int CountShitSize(const int* a){
	int i;
	for(i = 0; a[i] != -1; i++);

	return i;
}

//�}�b�v�\�����ꂽ���i�Y�N�j��z�u�_�\���ɂ���֐�
void Shit_MapToBaseAry(const int* stones, const int stoneNum, const int width, int** baseAry, int* dataAry){
	for(int i = 0; i < 8; i++) baseAry[i] = 0;

	//���ʂ̏��
	int min = -1;
	int bsIndex = 0;
	baseAry[0] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, i%8, i/8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[0][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[0][bsIndex] = -1;	//-1�͔ԕ�
	dataAry[0] = min;
	//DEBUG_printBaseAryStone(baseAry[0]);

	//90����]
	min = -1;
	bsIndex = 0;
	baseAry[1] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, i/8, 7-i%8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[1][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[1][bsIndex] = -1;	//-1�͔ԕ�
	if(JudgeSameAry(baseAry[0], baseAry[1], 17)){
		delete baseAry[1];
		baseAry[1] = 0;
		goto REVERSE;
	}
	//DEBUG_printBaseAryStone(baseAry[1]);
	dataAry[1] = min;


	//180����]
	min = -1;
	bsIndex = 0;
	baseAry[2] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, 7-i%8, 7-i/8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[2][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[2][bsIndex] = -1;	//-1�͔ԕ�
	if(JudgeSameAry(baseAry[0], baseAry[2], 17)){
		delete baseAry[2];
		baseAry[2] = 0;
		goto REVERSE;
	}
	//DEBUG_printBaseAryStone(baseAry[2]);
	dataAry[2] = min;

	//270����]
	min = -1;
	bsIndex = 0;
	baseAry[3] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, 7-i/8, i%8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[3][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[3][bsIndex] = -1;	//-1�͔ԕ�
	//DEBUG_printBaseAryStone(baseAry[3]);
	dataAry[3] = min;


REVERSE :;
	min = -1;
	bsIndex = 0;
	baseAry[4] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, 7-i%8, i/8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[4][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[4][bsIndex] = -1;	//-1�͔ԕ�
	for(int i = 0; i < 4; i++){
		if(baseAry[i] != 0){
			if(JudgeSameAry(baseAry[i], baseAry[4], 17)){
				delete baseAry[4];
				baseAry[4] = 0;
				return;
			}
		}
	}
	//DEBUG_printBaseAryStone(baseAry[4]);
	dataAry[4] = min;

	//90����]
	if(baseAry[1] == 0) return;
	min = -1;
	bsIndex = 0;
	baseAry[5] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, 7-i/8, 7-i%8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[5][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[5][bsIndex] = -1;	//-1�͔ԕ�
	//DEBUG_printBaseAryStone(baseAry[5]);
	dataAry[5] = min;

	//180����]
	if(baseAry[2] == 0) return;
	min = -1;
	bsIndex = 0;
	baseAry[6] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, i%8, 7-i/8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[6][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[6][bsIndex] = -1;	//-1�͔ԕ�
	//DEBUG_printBaseAryStone(baseAry[6]);
	dataAry[6] = min;

	//270����]
	if(baseAry[2] == 0) return;
	min = -1;
	bsIndex = 0;
	baseAry[7] = new int[17];
	for(int i = 0; i < 64; i++){
		if(STONE(stoneNum, i/8, i%8) == 1){
			if(min == -1){
				min = i;
			}

			baseAry[7][bsIndex] = (i-min) + (i/8-min/8)*((width+1)-8);
			bsIndex++;
		}
	}
	baseAry[7][bsIndex] = -1;	//-1�͔ԕ�
	//DEBUG_printBaseAryStone(baseAry[7]);
	dataAry[7] = min;
}

//���i�Y�N�j���}�b�v����Ƃ�̂����֐�
int RemoveShit(int* map, const int shitNumber, const int shitSize, const int width, const int height){
	int num = 0;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if(MAP(map, x, y) == shitNumber+1){
				MAP(map, x, y) = 0;
				num++;

				if(shitSize == num){
					return num;
				}
			}
		}
	}
	return 0;
}

//�}�b�v�ɕ��i�Y�N�j��z�u����֐�
void PutShit(int* map, const int shitNumber, const int* shit, const int basePoint){
	for(int i = 0; shit[i] != -1; i++){
		map[shit[i]+basePoint] = shitNumber+1;
	}
}

//�X�R�A���v�シ��֐�
int CalcScore(int* map, int width, int height){
	int score = 0;
	for(int i = 0; i < (width+1)*height; i++){
		if(map[i] == 0){
			score++;
		}
	}
	return score;
}

//���i�Y�N�j��z�u�ł��邩�ǂ������`�F�b�N����z��
bool JudgePutable(const int* map, const int* shit, const int basePoint, const int width, const int height){
	for(int j = 0; shit[j] != -1; j++){
		int value = shit[j]+basePoint;
		if(value < 0 || value >= (width+1)*height){
			return false;
		}
		if(map[value] != 0){
			return false;			
		}
	}
	return true;
}

//�אړ_���v�Z���郁�\�b�h
std::vector<int>* CalcNeighborPoint(const int* map, const int width, const int height){
	std::vector<int> *neighbor = new std::vector<int>();

	for(int i = 0; i < width*height; i++){
		//putPoints[i] �� �V�������i�Y�N�j��u�����_
		//������ putPoints�ɗv�f�����݂��邩�ǂ������`�F�b�N����K�v�H
		if(map[i] == 0){
			int value = i+1;	//��
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 

			value = i-1;	//��
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 

			value = i+width;	//��
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 

			value = i-width;	//��
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 			
		}
	}

	return neighbor;
	/*
	std::vector<int> *neighbor = new std::vector<int>();

	//�O��܂ł̗אړ_�������p��
	for(int i = 0; prevNeighbor[i] != -1; i++){
		if(map[prevNeighbor[i]] == 0){	
			neighbor->push_back(prevNeighbor[i]);
		}
	}

	//�V�������i�Y�N�j��u�����_�̎l�ߖT���`�F�b�N
	for(int i = 0; i < putAryLength; i++){
		//putPoints[i] �� �V�������i�Y�N�j��u�����_
		//������ putPoints�ɗv�f�����݂��邩�ǂ������`�F�b�N����K�v�H

		int value = putPoints[i]+1;	//��
		if(value >= 0 && value < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}

		value = putPoints[i]-1;	//�E
		if(value >= 0 && value < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}

		value = putPoints[i]+(width+1);	//��
		if(value >= 0 && value < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}

		value = putPoints[i]-(width+1);	//��
		if(value >= 0 && value < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}
	}

	neighbor->push_back(-1);	//�ԕ�
	return neighbor;
	*/
}

//�����̈�����o����֐�
//������
int CheckSubArea(const int* map, const int width, const int height, const int freeSize, int* areaSizeAry, int* subAreaMap, int* minAreaNum){
	std::queue<int> queue;
	std::vector<int> area;

	int free = freeSize;
	int* check = new int[width*height];
	for(int i = 0; i < width*height; i++) check[i] = 0;

	int asaIndex = 0;
	int areaNum;
	int maxAreaNum = -1;
	*minAreaNum = INT_MAX;
	for(int i = 0; i < width*height; i++){
		areaNum = 0;

		//�󔒂̗̈悪����������
		if(map[i] == 0 && check[i] == 0){
			area.clear();

			area.push_back(i);
			check[i] = 1;
			areaNum++;

			queue.push(i);
			while(queue.size() != 0){
				int p = queue.front();
				queue.pop();

				if(p-width >= 0 && map[p-width] == 0 && check[p-width] == 0){
					queue.push(p-width);
					check[p-width] = 1;
					areaNum++;
					area.push_back(p-width);
				}
				if(p+width < width*height && map[p+width] == 0 && check[p+width] == 0){
					queue.push(p+width);
					check[p+width] = 1;
					areaNum++;
					area.push_back(p+width);
				}
				if(p-1 >= 0 && map[p-1] == 0 && check[p-1] == 0){
					queue.push(p-1);
					check[p-1] = 1;
					areaNum++;
					area.push_back(p-1);
				}
				if(p+1 < width*height && map[p+1] == 0 && check[p+1] == 0){
					queue.push(p+1);
					check[p+1] = 1;
					areaNum++;
					area.push_back(p+1);
				}
			}	//���D��T�������

			//�����̈�T�C�Y�z��ɓ���Ă���
			if(areaSizeAry != 0){
				areaSizeAry[asaIndex] = areaNum;
			}
			asaIndex++;
			
			//�����̈�}�b�v�ɓ���Ă���
			if(subAreaMap != 0){
				for(int i = 0; i < area.size(); i++){
					subAreaMap[area[i]] = areaNum;
				}
			}

			//�ő�̈搔���X�V���Ă���
			if(areaNum > maxAreaNum){
				maxAreaNum = areaNum;
			}
			if(areaNum < *minAreaNum){
				*minAreaNum = areaNum;
			}

			free -= areaNum;
			if(free == 0){
				break;
			}
		}
	}

	if(areaSizeAry != 0){
		InsertSortArray(areaSizeAry, asaIndex);
		areaSizeAry[asaIndex] = -1;
	}

	delete check;
	return maxAreaNum;
}

static int areaAry[1025];	//���̕��ł����܂킵�Ă���
static int subAreaMap[1025];
int CalcDeadArea(const int* map, const int width, const int height, const int freeSize, const int* shitAry, const int shitNum, int* areaAry, int* needShitNum){
//int CalcDeadArea(const int* map, const int width, const int height, const int freeSize, const int* shitAry, const int shitNum, int* areaAry){
	*needShitNum = 0;
	int* tempShit = new int[shitNum];
	for(int i = 0; i < shitNum; i++){
		tempShit[i] = shitAry[i];
	}

	for(int a = 0; areaAry[a] != -1; a++){
		for(int i = 0; i < shitNum; i++){
			if(tempShit[i] == -1) continue;

			if(tempShit[i] <= areaAry[a]){
				areaAry[a] = 0;
				tempShit[i] = -1;
				(*needShitNum)++;
				break;
			}
		}
	}

	int result = 0;
	for(int a = 0; areaAry[a] != -1; a++){
		result += areaAry[a];
	}

	delete tempShit;
	return result;
}

typedef struct {
	int shitNumber;
	int state;
	int basePoint;
} Answer_t;

void SendSolution(std::vector<Answer_t>* aStack, int stonesNum, int** shitDataAry, int width){
	// Best
	sendMsg("S");

	//�𓚂�����
	char buf[256];
	int line = 0;
	for(int i = 0; i < aStack->size(); i++){
		for(;line < (*aStack)[i].shitNumber; line++){
			sendMsg("");
		}

		int putX = (*aStack)[i].basePoint%width - shitDataAry[(*aStack)[i].shitNumber][(*aStack)[i].state]%8;
		int putY = (*aStack)[i].basePoint/width - shitDataAry[(*aStack)[i].shitNumber][(*aStack)[i].state]/8;

		char side = 'H';
		if((*aStack)[i].state >= 4){
			side = 'T';
		}

		int turn = (*aStack)[i].state%4 * 90;

		sprintf(buf, "%c %d %d %d", side, turn, putX, putY);
		sendMsg(buf);

		line++;
	}

	for(;line < stonesNum; line++){
		sendMsg("");
	}
	sendMsg("E");
}

void InsertSort(std::vector<int*>* breadthData, int n){
	int i, j;
	int* temp;

	for (i = 1; i < n; i++) {  
		temp = ((*breadthData)[i]);
		for (j = i; j > 0 && ((*breadthData)[j-1])[2] < temp[2]; j--) ((*breadthData)[j]) = ((*breadthData)[j-1]);
		((*breadthData)[j]) = temp;
	}
}

class WideData{
private:
	int kn;
	std::vector<int*>* breadthData;
	int bdIndex;
	bool start;

public :
	WideData(int kn, std::vector<int*>* breadthData, bool start){
		this->kn = kn;
		this->breadthData = breadthData;
		this->start = start;
		this->bdIndex = 0;
	}
	~WideData(){
		breadthData->clear();
		delete breadthData;
	}

	bool Exist(){
		if(bdIndex >= breadthData->size()){
			return false;
		}
		return true;
	}
	int* GetNext(){
		bdIndex++;
		return (*breadthData)[bdIndex-1];
	}
	int GetShitNumber(){
		return kn;
	}
};

#define DEBUG_CODE

#define CUTTING_THRESHOLD -1
//#define CUTTING_THRESHOLD stonesNum/2
void FullSearch(const int* Map, const int x1, const int y1, const int x2, const int y2, const int* stones, const int stonesNum, char* solution){
	//-----����������-----//�i�����j
		//�Z�ӁA���ӂ̒���
	const int width = x2-x1+1;
	const int height = y2-y1+1;

		//�킩��₷���}�b�v��`
	int* map = new int[(width+1) * height];	//�Ώۗ̈悾�����l�������}�b�v
	std::vector<int> startNeighbor;

	int freeSize = width*height;
	const int maxFreeSize = freeSize;
	for(int h = 0; h < height; h++){
		for(int w = 0; w < width; w++){
			int value = Map[(w+x1) + (h+y1)*32];

			//���͂ł� �� = 1 �ɂȂ��Ă��邪�A�����ł� �� = -1 �Ƃ���B
			if(value == 0){
				MAP(m, w, h) = 0;
			} else {
				MAP(m, w, h) = -1;
				freeSize--;
			}
			startNeighbor.push_back(w+h*(width+1));	//�u����n�_�Ƃ��ċL��
		}
	}
//	startNeighbor.push_back(-1);	//-1�͔ԕ�
	for(int h = 0; h < height; h++){
		MAP(m, width, h) = -2;
	}

#ifdef DEBUG_CODE
	printf("%d %d\n", width, height); //DEBUG
	DEBUG_printMap(map, width+1, height);

	printf("free : %d\n", freeSize);
	DEBUG_waitKey();
#endif

		//���i�Y�N�j�̍Ē�`
	int*** shitAry = new int**[stonesNum];	//�N�_�z��\�����������i�Y�N�j�̔z��
	int** shitDataAry = new int*[stonesNum];
	int* shitSizeAry = new int[stonesNum];
	int totalSize = 0;
	for(int s = 0; s < stonesNum; s++){
#ifdef DEBUG_CODE
		printf("\t%c\n", s+1+48);
#endif
		shitAry[s] = new int*[8];
		shitDataAry[s] = new int[8];

		DEBUG_printMapStone(stones, s);
		Shit_MapToBaseAry(stones, s, width, shitAry[s], shitDataAry[s]);

		shitSizeAry[s] = CountShitSize(shitAry[s][0]);	//�T�C�Y���v��
		totalSize += shitSizeAry[s];
#ifdef DEBUG_CODE
		printf("\t size : %d\n", shitSizeAry[s]);
#endif
		/*
		for(int i = 0; i < 8; i++){
			printf("\t\t %d\n", shitDataAry[s][i]);
		}
		*/
	}

#ifdef DEBUG_CODE
	printf("total size : %d\n", totalSize);	//DEBUG
	printf("free :%d\n", freeSize);
	DEBUG_waitKey();
	printf("\nStart : \n");	//DEBUG
#endif

	//-----�T��-----//�i�����j
	std::vector<Answer_t> aStack;
	std::vector<Answer_t> bestAnsStack;

	int putPoints[17];
	int putAryLength;
	int minScore = 1025;
	int minScore_minShitNum = 1025;
	int putShitNum = 0;

#ifdef DEBUG_CODE
	LARGE_INTEGER liFreq, start;
	QueryPerformanceFrequency( &liFreq );
	QueryPerformanceCounter( &start );
#endif

	int startKn = 0;
	std::stack<WideData*> bdStack;

	int DEBUG_COUNT = 0;
	int kn = 0;
	while(1){
		//���D��T���p�̃R�[�h
		bool startFlag = false;

		if(bdStack.size() == 0){
			kn = startKn;
			startFlag = true;
			startKn++;
		} else {
			kn = bdStack.top()->GetShitNumber();
			int remove = RemoveShit(map, kn, shitSizeAry[kn], width, height);
			freeSize += remove;
			if(remove != 0) {
				putShitNum--;
				aStack.pop_back();
			}

			if(!bdStack.top()->Exist()) {
				delete bdStack.top();
				bdStack.pop();
				continue;
			}

			//���z�u����
			int* data = bdStack.top()->GetNext();
			int st = data[0];
			int placePoint = data[1];

			PutShit(map, kn, shitAry[kn][st], placePoint);
			putShitNum++;
			freeSize -= shitSizeAry[kn];
			Answer_t ans; ans.basePoint=placePoint; ans.state=st; ans.shitNumber=kn;
			aStack.push_back(ans);

			if(kn == stonesNum-1){
				goto TERMINAL;
			}

			/*l
			printf("\tshit : %d(%c)\n", kn, kn+49);
			DEBUG_printMap(map, width+1, height);
			printf("free :%d\n", freeSize);
			printf("ans : %d == %d : shitNum\n", aStack.size(), putShitNum);
			DEBUG_waitKey();
			*/
			kn++;	//�R�R
		}
			
		int fit, needShitNum;
		for(int i = 0; i < 1025; i++) subAreaMap[i] = -1;
		int checkArea = CheckSubArea(map, width+1, height, freeSize, areaAry, subAreaMap, &fit);

		//�T�C�Y�I�ɒu���Ȃ����i�Y�N�j���X�L�b�v
		while(kn < stonesNum && shitSizeAry[kn] > checkArea){
			kn++;
		}
		if(kn == stonesNum) {
			goto TERMINAL;
		}

		int deadArea = CalcDeadArea(map, width+1, height, freeSize, &(shitSizeAry[kn]), stonesNum-kn, areaAry, &needShitNum);
		if(minScore < deadArea || (minScore == deadArea && minScore_minShitNum <= needShitNum+putShitNum)){
			continue;
		}

	BREADTH_MAKE:
		std::vector<int*>* breadthData = new std::vector<int*>();

		for(int st = 0; st < 8; st++){
			const int* shit = shitAry[kn][st];
			if(shitAry[kn][st] == 0) continue;

			//�x�[�X�|�C���g�z��쐬
			std::vector<int> baseAry;
			if(startFlag){
				baseAry = startNeighbor;
			} else {
				std::vector<int>* neighbor = CalcNeighborPoint(map, width+1, height); //�R�R

				for(int s = 0; shit[s] != -1; s++){
					for(int n = 0; n < neighbor->size(); n++){
						int value = (*neighbor)[n] - shit[s];

						for(int i = 0; i < baseAry.size(); i++){
							if(value == baseAry[i]){
								goto NEXT;
							}
						}
						baseAry.push_back(value);

					NEXT:;
					}
				}

				delete neighbor;
			}

			//�S�Ẵx�[�X�|�C���g��T��
			for(int i = 0; i < baseAry.size(); i++){
				int basePoint = baseAry[i];

				/*
				printf("%d\n", basePoint);
				bool a = DEBUG_JudgePutable(map, shit, basePoint, width, height, putPoints, &putAryLength);
				DEBUG_printPutShitOnMap(map, putPoints, putAryLength, width, height);
				DEBUG_waitKey();
				*/

				if(JudgePutable(map, shit, basePoint, width, height)){
					PutShit(map, kn, shitAry[kn][st], basePoint);
					freeSize -= shitSizeAry[kn];
					int needShitNum = 0;

					int* temp = new int[3];
					temp[0] = st;
					temp[1] = basePoint;
					temp[2] = fit;
					breadthData->push_back(temp);

					RemoveShit(map, kn, shitSizeAry[kn], width, height);
					freeSize += shitSizeAry[kn];
				}
			}
		}

		if(breadthData->size() == 0){	//�u���Ȃ��Ȃ�
			delete breadthData;
			kn++;
			if(kn == stonesNum) goto TERMINAL;
			goto BREADTH_MAKE;
		} else {
			InsertSort(breadthData, breadthData->size());
			bdStack.push(new WideData(kn, breadthData, startFlag));
		}
		continue;

	TERMINAL:
		int score = CalcScore(map, width, height);

		bool update = false;
		if(minScore > score){
			update = true;
		} else if(minScore == score){
			if(minScore_minShitNum > putShitNum){
				update = true;
			}
		}

		if(update){
			minScore = score;
			minScore_minShitNum = putShitNum;
		
			DEBUG_printMap(map, width+1, height);
			printf("score(%d,%d)\n", minScore, minScore_minShitNum);
			LARGE_INTEGER end;
			QueryPerformanceCounter( &end );
			printf("time : %d\n", (end.QuadPart - start.QuadPart)/liFreq.QuadPart);

			bestAnsStack = aStack;
			//SendSolution(&bestAnsStack, stonesNum, shitDataAry, width+1);
		}

		if(bdStack.top()->GetShitNumber() == stonesNum-1){	//�u�������i�Y�N�j���Ō�Ȃ�
			delete bdStack.top();
			bdStack.pop();

			int remove = RemoveShit(map, kn, shitSizeAry[kn], width, height);
			freeSize += remove;
			if(remove != 0) putShitNum--;

			aStack.pop_back();
		}
	}
	
	/*
	//��������J��Ԃ�
	while(1){
		PutPoint* p = pStack.top();
		kn = p->GetShitNumber();
		st = p->GetShitState();
		const int* shit = shitAry[kn][st];

		if(kn == 0){	//DEBUG
			printf("koko\n");
			DEBUG_FLAG++;
		}

		int nextBasePoint;
		if(!p->GetNextValue(&nextBasePoint)){
			if(pStack.size() == 1){
				break;	//the end
			} else {
				//�d�����Ă�������ɂ�null�������Ă���̂ŁA�X�L�b�v����B
				do {
					st++;
				} while(st < 8 && shitAry[kn][st] == 0);

				pStack.pop();	//���݂̔z�u�_�z����|�b�v�i���~�����j

				if(st >= 8){	//���ׂĂ̌������I��
					st = 0;	//���������Z�b�g����

					int checkArea = freeSize;
					bool cutting = CUTTING_THRESHOLD < kn;
					if(cutting){
						for(int i = 0; i < 1025; i++) subAreaMap[i] = -1;
						checkArea = CheckSubArea(map, width+1, height, freeSize, 0, subAreaMap);
					}

					//�c��̋󂫗̈悩��A�u���Ȃ����i�Y�N�j���X�L�b�v����B
					int temp_kn = kn;
					do{
						kn++;
					} while(kn < stonesNum && shitSizeAry[kn] > checkArea);

					//�����R���ȏ�u���Ȃ��Ȃ�A�I�[�Ƃ���
					if(kn >= stonesNum){
						kn = temp_kn;
						delete p;
						goto TERMINAL;
					}

					if(p->isStart()){
						pStack.push(new PutPoint(kn, st, shitAry[kn][st], &(startNeighbor[0])));
					} else {
						if(cutting){
							pStack.push(new PutPoint(kn, st, shitAry[kn][st], p, shitSizeAry[kn], subAreaMap));
						} else {
							pStack.push(new PutPoint(kn, st, shitAry[kn][st], p, -1, 0));
						}
					}
				} else {
					if(p->isStart()){
						pStack.push(new PutPoint(kn, st, shitAry[kn][st], &(startNeighbor[0])));
					} else {
						pStack.push(new PutPoint(kn, st, shitAry[kn][st], p, -1, 0));
					}
				}

				delete p;
				continue;
			}
		}

		bool result = JudgePutable(map, shit, nextBasePoint, width, height, putPoints, &putAryLength);

		if(result){	//������Ȃ�
			//�z�u����
			PutShit(map, kn, putPoints, putAryLength);
			freeSize -= shitSizeAry[kn];
			putShitNum++;

			Answer_t ans;
			ans.basePoint = nextBasePoint;
			ans.shitNumber = kn;
			ans.state = st;
			aStack.push_back(ans);
				//�𓚍쐬

			int checkArea = freeSize;
			bool cutting = CUTTING_THRESHOLD < kn;
			if(cutting){
				for(int i = 0; i < 1025; i++) subAreaMap[i] = -1;
				checkArea = CheckSubArea(map, width+1, height, freeSize, areaAry, subAreaMap);
					//�߂�l�͕����̈�̂����ő�ʐ�
					//static��`���ꂽareaAry�ɂ͕����̈��񂪓����Ă���
			}

			//���ɔz�u�\��̕��i�Y�N�j���󂫗̈�����傫���Ȃ�X�L�b�v����
			int temp_kn = kn;
			do{
				kn++;
			} while(kn < stonesNum && shitSizeAry[kn] > checkArea);
			
			//���i�Y�N�j�����������Ȃ�
			if(kn >= stonesNum){
				kn = temp_kn;
				goto TERMINAL;	//�I�[�Ƃ���
			}

			if(cutting){
				int needShitNum = 0;
				int deadArea = CalcDeadArea(map, width+1, height, freeSize, &(shitSizeAry[kn]), stonesNum-kn, areaAry, &needShitNum);

				//if(DEBUG_FLAG >= 2){
				if(false){
					printf("now : %d\n", kn);
					DEBUG_printMap(map, width+1, height);
					printf("shit size : ");
					for(int i = kn; i < stonesNum; i++){
						printf("%d ", shitSizeAry[i]);
					} printf("\n");
					printf("area size : ");
					for(int i = 0; areaAry[i] != -1; i++){
						printf("%d ", areaAry[i]);
					} printf("\n");
					printf("%d < %d\n", minScore, deadArea);
					printf("\n");
					DEBUG_waitKey();
				}

				if(minScore < deadArea || (minScore == deadArea && minScore_minShitNum <= needShitNum+putShitNum)){
					//�ŗǃX�R�A�𒴂����Ȃ��Ȃ�A�p��
					int removeShitNumber = p->GetShitNumber();
					RemoveShit(map, removeShitNumber, shitSizeAry[removeShitNumber], width, height);
					freeSize += shitSizeAry[removeShitNumber];
					aStack.pop_back();
					putShitNum--;
					continue;
				}
			}
			std::vector<int>* neighbor = CalcNeighborPoint(map, p->GetNeighborAry(), putPoints, putAryLength, width, height); //�R�R

			if(cutting){
				pStack.push(new PutPoint(kn, 0, shitAry[kn][0], neighbor->data(), neighbor->size(), shitSizeAry[kn], subAreaMap));
			} else {
				pStack.push(new PutPoint(kn, 0, shitAry[kn][0], neighbor->data(), neighbor->size(), -1, 0));
			}
			delete neighbor;
		}
		continue;

	TERMINAL:
		//�|�C���g�W�v�A�𓚍쐬
		int score = CalcScore(map, width, height);

		bool update = false;
		if(minScore > score){
			update = true;
		} else if(minScore == score){
			if(minScore_minShitNum > putShitNum){
				update = true;
			}
		}

		if(update){
			minScore = score;
			minScore_minShitNum = putShitNum;
		
			DEBUG_printMap(map, width+1, height);
			printf("score(%d,%d)\n", minScore, minScore_minShitNum);
			LARGE_INTEGER end;
			QueryPerformanceCounter( &end );
			printf("time : %d\n", (end.QuadPart - start.QuadPart)/liFreq.QuadPart);

			bestAnsStack = aStack;
			SendSolution(&bestAnsStack, stonesNum, shitDataAry, width+1);
		}

		int removeShitNumber = pStack.top()->GetShitNumber();
		RemoveShit(map, removeShitNumber, shitSizeAry[removeShitNumber], width, height);
		freeSize += shitSizeAry[removeShitNumber];
		aStack.pop_back();
		putShitNum--;

		if(pStack.top()->GetShitNumber() == stonesNum-1){	//�u�������i�Y�N�j���Ō�Ȃ�
			delete pStack.top();
			pStack.pop();	//����ǂ��u���Ă��X�R�A�͕ς��Ȃ��̂Ń|�b�v
	
			removeShitNumber = pStack.top()->GetShitNumber();
			RemoveShit(map, removeShitNumber, shitSizeAry[removeShitNumber], width, height);
			freeSize += shitSizeAry[removeShitNumber];
			putShitNum--;
			aStack.pop_back();
		}

		continue;
	}
	*/
	//�����܂ŌJ��Ԃ�
#ifdef DEBUG_CODE
	printf("\nEND : score(%d,%d)\n", minScore, minScore_minShitNum);
	printf("\n");
	for(int i = 0; i < bestAnsStack.size(); i++){
		printf("\t%d %d %d\n", bestAnsStack[i].shitNumber, bestAnsStack[i].basePoint, bestAnsStack[i].state);
	}
#endif


	//-----�I������-----//
	delete shitSizeAry;
	delete map;
	delete[] shitAry;

	//DEBUG
	DEBUG_waitKey();
}

//�f�o�b�O�p�̃e���v���[�g��
/*
	if(stonesNum/2 < kn){
		DEBUG_printMap(subAreaMap, width+1, height);printf("\n\n");
		printf("%d\n", kn);
		DEBUG_printMap(map, width+1, height);
		printf("free size : %d\n", freeSize);

		for(int i = 0; areaAry[i] != -1; i++){
			printf("%d ", areaAry[i]);
		}
		printf("\n");
		DEBUG_waitKey();
	}
*/