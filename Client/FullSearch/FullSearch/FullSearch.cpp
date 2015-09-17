#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vector>
#include <stack>

#define	STONE(n, x, y)	stones[((n) << 6) + ((y) << 3) + (x)]
#define MAP(m, x, y) map[(x) + (y)*(width+1)]

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
				printf("%d ", value);
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

class PutPoint{
private:
	int i;
	int k;
	std::vector<int> p;
	int* neighborAry;
	bool start;

public :
	PutPoint(const int k, const int* shit, PutPoint* prevP){
		int size;
		for(size = 0; prevP->neighborAry[size] != -1; size++);

		neighborAry = new int[size+1];
		for(int i = 0; i < size; i++){
			neighborAry[i] = prevP->neighborAry[i];
		}
		neighborAry[size] = -1;

		for(int s = 0; shit[s] != -1; s++){
			for(int n = 0; neighborAry[n] != -1; n++){
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

		i = 0;
		this->k = k;
		this->start = false;
	}
	PutPoint(const int k, const int* shit, const int* neighbor, const int neighborNum){
		for(int s = 0; shit[s] != -1; s++){
			for(int n = 0; neighbor[n] != -1; n++){
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

		i = 0;
		this->k = k;
		this->start = false;
	}
	PutPoint(const int k, const int* shit, const int* startMap){
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

		i = 0;
		this->k = k;
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
		if(i < p.size()){
			*value = p[i];
			i++;
			return true;
		}

		return false;
	}
	bool isStart(){
		return start;
	}
	int GetShitNumber(){
		return k;
	}
};


void RemoveShit(int* map, const int shitNumber, const int width, const int height){
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if(MAP(map, x, y) == shitNumber+1){
				MAP(map, x, y) = 0;
			}
		}
	}
}

void PutShit(int* map, const int shitNumber, const int* putPoints, const int putAryLength){
	for(int i = 0; i < putAryLength; i++){
		map[putPoints[i]] = shitNumber+1;
	}
}

int CalcScore(int* map, int width, int height){
	int score = 0;
	for(int i = 0; i < (width+1)*height; i++){
		if(map[i] == 0){
			score++;
		}
	}
	return score;
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

bool JudgePutable(const int* map, const int* shit, const int basePoint, const int width, const int height, int* putPoints, int* putAryLength){
	int i;

	for(i = 0; shit[i] != -1; i++){
		putPoints[i] = shit[i];
	}
	*putAryLength = i;

	for(int j = 0; j < *putAryLength; j++){
		int preValue = putPoints[j];
		putPoints[j] += basePoint;

		if(putPoints[j] < 0 || putPoints[j] >= (width+1)*height){
			return false;
		}
		if(map[putPoints[j]] != 0){
			return false;			
		}
	}
	return true;
}

//後で二分探索化
bool JudgeExistValue(const int* ary, const int aryNum, const int value){
	for(int i = 0; i < aryNum; i++){
		if(ary[i] == value){
			return true;
		}
	}

	return false;
}

std::vector<int>* CalcNeighborPoint(const int* map, const int* prevNeighbor, const int* putPoints, const int putAryLength, const int width, const int height){
	std::vector<int> *neighbor = new std::vector<int>();
	for(int i = 0; prevNeighbor[i] != -1; i++){
		if(map[prevNeighbor[i]] == 0){	
			neighbor->push_back(prevNeighbor[i]);
		}
	}

	for(int i = 0; i < putAryLength; i++){
		int value = putPoints[i]+1;
		if(putPoints[i] >= 0 && putPoints[i] < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}

		value = putPoints[i]-1;
		if(putPoints[i] >= 0 && putPoints[i] < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}

		value = putPoints[i]+(width+1);
		if(putPoints[i] >= 0 && putPoints[i] < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}

		value = putPoints[i]-(width+1);
		if(putPoints[i] >= 0 && putPoints[i] < (width+1)*height && map[value] == 0 && !JudgeExistValue(neighbor->data(), neighbor->size(), value) && !JudgeExistValue(putPoints, putAryLength, value)){
			neighbor->push_back(value);
		}
	}

	neighbor->push_back(-1);

	return neighbor;
}

bool JudgeSameAry(const int* a, const int* b, const int size){
	for(int i = 0; i < size; i++){
		if(a[i] != b[i]) return false;
	}
	return true;
}

void Shit_MapToBaseAry(const int* stones, const int stoneNum, const int width, int** baseAry){
	for(int i = 0; i < 8; i++) baseAry[i] = 0;

	//普通の状態
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
		baseAry[0][bsIndex] = -1;	//-1は番兵
	}
	DEBUG_printBaseAryStone(baseAry[0]);

	//90°回転
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
		baseAry[1][bsIndex] = -1;	//-1は番兵
	}
	if(JudgeSameAry(baseAry[0], baseAry[1], 17)){
		delete baseAry[1];
		goto REVERSE;
	}
	DEBUG_printBaseAryStone(baseAry[1]);

	//180°回転
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
		baseAry[2][bsIndex] = -1;	//-1は番兵
	}
	if(JudgeSameAry(baseAry[0], baseAry[2], 17)){
		delete baseAry[2];
		goto REVERSE;
	}
	DEBUG_printBaseAryStone(baseAry[2]);

	//270°回転
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
		baseAry[3][bsIndex] = -1;	//-1は番兵
	}
	DEBUG_printBaseAryStone(baseAry[3]);


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
		baseAry[4][bsIndex] = -1;	//-1は番兵
	}
	for(int i = 0; i < 4; i++){
		if(JudgeSameAry(baseAry[i], baseAry[4], 17)){
			delete baseAry[4];
			return;
		}
	}
	DEBUG_printBaseAryStone(baseAry[4]);


	//90°回転
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
		baseAry[5][bsIndex] = -1;	//-1は番兵
	}
	DEBUG_printBaseAryStone(baseAry[5]);

	//180°回転
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
		baseAry[6][bsIndex] = -1;	//-1は番兵
	}
	DEBUG_printBaseAryStone(baseAry[6]);

	//270°回転
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
		baseAry[7][bsIndex] = -1;	//-1は番兵
	}
	DEBUG_printBaseAryStone(baseAry[7]);
}

void FullSearch(const int* Map, const int x1, const int y1, const int x2, const int y2, const int* stones, const int stonesNum, char* solution){
	//strcat(solution, "H 0 2 2\n")
	//-----初期化処理-----//（長い）
		//短辺、長辺の調査
	const int width = x2-x1+1;
	const int height = y2-y1+1;

		//わかりやすくマップ定義
	int* map = new int[(width+1) * height];	//対象領域だけを考慮したマップ
	std::vector<int> startNeighbor;

	for(int h = 0; h < height; h++){
		for(int w = 0; w < width; w++){
			int value = Map[(w+x1) + (h+y1)*32];

			//入力では 壁 = 1 になっているが、ここでは 壁 = -1 とする。
			if(value == 0){
				MAP(m, w, h) = 0;
				startNeighbor.push_back(w+h*width);
			} else {
				MAP(m, w, h) = -1;
			}
		}
	}
	for(int h = 0; h < height; h++){
		MAP(m, width, h) = -2;
	}

	startNeighbor.push_back(-1);	//-1は番兵

	printf("%d %d\n", width, height); //DEBUG
	DEBUG_printMap(map, width+1, height);

		//糞（ズク）の再定義

	int** shitAry;
	int*** shitAry_D = new int**[stonesNum];	//起点配列表現をした糞（ズク）の配列
	for(int s = 0; s < stonesNum; s++){
		shitAry_D[s] = new int*[8];
		//shitAry[s] = new int[17];

		DEBUG_printMapStone(stones, s);
		Shit_MapToBaseAry(stones, s, width, shitAry_D[s]);

		DEBUG_waitKey();

		//DEBUG
//		printf("\tstone %d\n", s);
//		DEBUG_printBaseAryStone(shitAry[s]);
		//DEBUG
	}
	printf("\n");	//DEBUG

	DEBUG_waitKey();
	printf("\nStart : \n");	//DEBUG

	//-----探索-----//（長い）
	int kn = 0;
	std::stack<PutPoint*> pStack;

	pStack.push(new PutPoint(kn, shitAry[kn], &(startNeighbor[0])));
	int putPoints[17];
	int putAryLength;

	int minScore = 1025;
	int putShitNum = 0;
	int minScore_minShitNum = 1025;

	//ここから繰り返し
	while(1){
		//DEBUG_printMap(map, width+1, height); printf("\n");

		PutPoint* p = pStack.top();
		kn = p->GetShitNumber();
		const int* shit = shitAry[kn];

		RemoveShit(map, kn, width, height);

		int nextBasePoint;
		if(!p->GetNextValue(&nextBasePoint)){
			if(pStack.size() == 1){
				break;
			} else {
				//printf("\n＝＝＝＝＝ cannot place No.%d ＝＝＝＝＝\n", kn);

				pStack.pop();
				if(kn+1 >= stonesNum){
					goto TERMINAL;
				}

				if(p->isStart()){
					pStack.push(new PutPoint(kn+1, shitAry[kn+1], &(startNeighbor[0])));
				} else {
					pStack.push(new PutPoint(kn+1, shitAry[kn+1], p));
				}
				/*
				printf("↓↓↓↓↓ next shit ↓↓↓↓↓\n");
				DEBUG_waitKey();
				*/
				continue;
			}
		}

		/*
		printf("\tput point : %d\n", nextBasePoint);
		DEBUG_printBaseAryStone(shit);
		*/

		bool result = JudgePutable(map, shit, nextBasePoint, width, height, putPoints, &putAryLength);
		/*
		bool result = DEBUG_JudgePutable(map, shit, nextBasePoint, width, height, putPoints, &putAryLength);
		DEBUG_printPutShitOnMap(map, putPoints, putAryLength, width, height);
		*/
		
		if(result){
			//printf("\nplace > ");
			
			PutShit(map, kn, putPoints, putAryLength);
			putShitNum++;

			if(kn+1 >= stonesNum){
				goto TERMINAL;
			}

			std::vector<int>* neighbor = CalcNeighborPoint(map, p->GetNeighborAry(), putPoints, putAryLength, width, height); 

			/* DEBUG
			printf("neighbor[ ");
			for(int i = 0; i < neighbor->size()-1; i++){
				printf("%d ", neighbor->at(i));
			}
			printf("]\n\n");

			DEBUG_printMap(map, width+1, height);
			*/

			pStack.push(new PutPoint(kn+1, shitAry[kn+1], neighbor->data(), neighbor->size()));
			delete neighbor;

			/* DEBUG
			printf("\n↓↓↓↓↓ next shit ↓↓↓↓↓\n");
			DEBUG_waitKey();
			*/
		} else {
			//printf("\ncannot place\n");
		}

		continue;
	TERMINAL:
		//ポイント集計、解答作成
		int score = CalcScore(map, width, height);
		if(minScore > score){
			minScore = score;
			minScore_minShitNum = putShitNum;
		} else if(minScore == score){
			if(minScore_minShitNum > putShitNum){
				minScore_minShitNum = putShitNum;
			}
		}

		if(score == 0){
			DEBUG_printMap(map, width+1, height);
			printf("\n");
		}

		/*
		printf("\n\t !!!!! terminal !!!!!\n\n");
		DEBUG_printMap(map, width+1, height);
		printf("\n\t   score : %d\n", score);
		printf("\t   shitNum : %d\n", putShitNum);

		printf("↑↑↑↑↑ preview shit ↑↑↑↑↑\n");
		DEBUG_waitKey();
		*/
		putShitNum--;

		continue;
	}

	//ここまで繰り返し


	//-----終了処理-----//
	delete map;
	delete[] shitAry;

	printf("\nEND : score(%d,%d)\n", minScore, minScore_minShitNum);

	//DEBUG
	DEBUG_waitKey();
}