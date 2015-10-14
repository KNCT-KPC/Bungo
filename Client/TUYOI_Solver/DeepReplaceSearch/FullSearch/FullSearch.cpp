#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vector>
#include <stack>
#include <queue>
#include <Windows.h>

int dx1;
int dy1;
//#define ZERO_SOLVE

//#define DEBUG_CODE
//#define CHECK_CODE
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
				if(value < 30){
					printf("%c ", value+48);
				} else {
					printf("ｱ ");
				}
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

//２つの配列が同じものかをチェックする関数
bool JudgeSameAry(const int* a, const int* b, const int size){
	for(int i = 0; i < size; i++){
		if(a[i] != b[i]) return false;
	}
	return true;
}

//挿入ソート
void InsertSortArray(int num[], int n){
    int i, j, temp;

    for (i = 1; i < n; i++) {  
        temp = num[i];         
        for (j = i; j > 0 && num[j-1] > temp; j--) 
            num[j] = num[j-1];
        num[j] = temp;
	}
}

//後で二分探索化
//高速化
//配列内に値が存在するかどうかを判定するメソッド
bool JudgeExistValue(const int* ary, const int aryNum, const int value){
	for(int i = 0; i < aryNum; i++){
		if(ary[i] == value){
			return true;
		}
	}

	return false;
}

//マップ表現された糞（ズク）のサイズを測る関数
int CountShitSize(const int* a){
	int i;
	for(i = 0; a[i] != -1; i++);

	return i;
}

//マップ表現された糞（ズク）を配置点表現にする関数
void Shit_MapToBaseAry(const int* stones, const int stoneNum, const int width, int** baseAry, int* dataAry){
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
	}
	baseAry[0][bsIndex] = -1;	//-1は番兵
	dataAry[0] = min;
	//DEBUG_printBaseAryStone(baseAry[0]);

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
	}
	baseAry[1][bsIndex] = -1;	//-1は番兵
	if(JudgeSameAry(baseAry[0], baseAry[1], 17)){
		delete baseAry[1];
		baseAry[1] = 0;
		goto REVERSE;
	}
	//DEBUG_printBaseAryStone(baseAry[1]);
	dataAry[1] = min;


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
	}
	baseAry[2][bsIndex] = -1;	//-1は番兵
	if(JudgeSameAry(baseAry[0], baseAry[2], 17)){
		delete baseAry[2];
		baseAry[2] = 0;
		goto REVERSE;
	}
	//DEBUG_printBaseAryStone(baseAry[2]);
	dataAry[2] = min;

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
	}
	baseAry[3][bsIndex] = -1;	//-1は番兵
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
	baseAry[4][bsIndex] = -1;	//-1は番兵
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
	}
	baseAry[5][bsIndex] = -1;	//-1は番兵
	//DEBUG_printBaseAryStone(baseAry[5]);
	dataAry[5] = min;

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
	}
	baseAry[6][bsIndex] = -1;	//-1は番兵
	//DEBUG_printBaseAryStone(baseAry[6]);
	dataAry[6] = min;

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
	}
	baseAry[7][bsIndex] = -1;	//-1は番兵
	//DEBUG_printBaseAryStone(baseAry[7]);
	dataAry[7] = min;
}

//糞（ズク）をマップからとりのぞく関数
void RemoveAllShit(int* map, const int width, const int height){
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			MAP(map, x, y) = 0;
		}
	}
}

int RemoveShitNums(int* map, const int n, const int width, const int height){
	int num = 0;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if(MAP(map, x, y) >= n+1){
				MAP(map, x, y) = 0;
				num++;
			}
		}
	}
	return num;
}

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

//マップに糞（ズク）を配置する関数
void PutShit(int* map, const int shitNumber, const int* shit, const int basePoint, const int width, const int height, int* neighborNum){
	*neighborNum = 0;
	for(int i = 0; shit[i] != -1; i++){
		int p = shit[i]+basePoint;
		map[p] = shitNumber+1;

		int value = p+1;	//左
		if(!(value >= 0 && value < width*height && (map[value] == 0 || map[value] == shitNumber+1))){
			(*neighborNum)++;
		} 
		value = p-1;	//左
		if(!(value >= 0 && value < width*height && (map[value] == 0 || map[value] == shitNumber+1))){
			(*neighborNum)++;
		} 
		value = p+width;	//上
		if(!(value >= 0 && value < width*height && (map[value] == 0 || map[value] == shitNumber+1))){
			(*neighborNum)++;
		} 
		value = p-width;	//下
		if(!(value >= 0 && value < width*height && (map[value] == 0 || map[value] == shitNumber+1))){
			(*neighborNum)++;
		} 	
	}
}

//スコアを計上する関数
int CalcScore(int* map, int width, int height){
	int score = 0;
	for(int i = 0; i < (width+1)*height; i++){
		if(map[i] == 0){
			score++;
		}
	}
	return score;
}

//糞（ズク）を配置できるかどうかをチェックする配列
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

//隣接点を計算するメソッド
std::vector<int>* CalcNeighborPoint(const int* map, const int width, const int height){
	std::vector<int> *neighbor = new std::vector<int>();

	for(int i = 0; i < width*height; i++){
		//putPoints[i] は 新しく糞（ズク）を置いた点
		//高速化 putPointsに要素が存在するかどうかをチェックする必要？
		if(map[i] == 0){
			int value = i+1;	//左
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 

			value = i-1;	//左
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 

			value = i+width;	//上
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 

			value = i-width;	//下
			if(value >= 0 && value < width*height && map[value] > 0){
				neighbor->push_back(i);
				continue;
			} 			
		}
	}

	return neighbor;
}

//部分領域を検出する関数
//高速化
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

		//空白の領域が見つかったら
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
			}	//幅優先探索おわり

			//部分領域サイズ配列に入れておく
			if(areaSizeAry != 0){
				areaSizeAry[asaIndex] = areaNum;
			}
			asaIndex++;
			
			//部分領域マップに入れておく
			if(subAreaMap != 0){
				for(int i = 0; i < area.size(); i++){
					subAreaMap[area[i]] = areaNum;
				}
			}

			//最大領域数を更新しておく
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

#ifndef REVERSE_SOLVE
void SendSolution(std::vector<Answer_t>* aStack, int stonesNum, int** shitDataAry, int width){
	// Best
	sendMsg("S");

	//解答を書く
	char buf[256];
	int line = 0;
	for(int i = 0; i < aStack->size(); i++){
		for(;line < (*aStack)[i].shitNumber; line++){
			sendMsg("");
		}

		int putX = (*aStack)[i].basePoint%width - shitDataAry[(*aStack)[i].shitNumber][(*aStack)[i].state]%8 + dx1;
		int putY = (*aStack)[i].basePoint/width - shitDataAry[(*aStack)[i].shitNumber][(*aStack)[i].state]/8 + dy1;

		char side = 'H';
		if((*aStack)[i].state >= 4){
			side = 'T';
		}

		int turn = (*aStack)[i].state%4 * 90;

		//sprintf(buf, "%c %d %d %d", side, turn, putX, putY);
		printf(buf, "%d %d %c %d", putX, putY, side, turn);
		//sendMsg(buf);
		printf(buf);

		line++;
	}

	for(;line < stonesNum; line++){
		sendMsg("");
	}
	sendMsg("E");
}
#else
void SendSolution(std::vector<Answer_t>* aStack, int stonesNum, int** shitDataAry, int width){
	// Best
	sendMsg("S");

	//解答を書く
	char buf[256];
	int line = 0;
	for(int i = 0; i < stonesNum; i++){
		for(int j = 0; j < aStack->size(); j++){
			if((*aStack)[j].shitNumber == (stonesNum-1-i)){
				int putX = (*aStack)[j].basePoint%width - shitDataAry[(*aStack)[j].shitNumber][(*aStack)[j].state]%8;
				int putY = (*aStack)[j].basePoint/width - shitDataAry[(*aStack)[j].shitNumber][(*aStack)[j].state]/8;

				char side = 'H';
				if((*aStack)[j].state >= 4){
					side = 'T';
				}

				int turn = (*aStack)[j].state%4 * 90;

				sprintf(buf, "%c %d %d %d", side, turn, putX, putY);
				//printf("%s\n", buf);
				sendMsg(buf);

				goto NEXT_LINE;
			}
		}

		sendMsg("");
		//printf("\n");
	NEXT_LINE:;
	}
	sendMsg("E");
}
#endif

class wdElement{
public:
	int st;
	int basePoint;
	int fit;
	int fit2;
	int deadArea;
	int maxAreaSize;
	int needShitNum;
	int rank;
};

class WideData{
private:
	int kn;
	std::vector<wdElement*>* breadthData;
	int bdIndex;
	bool start;

public :
	WideData(int kn, std::vector<wdElement*>* breadthData, bool start){
		this->kn = kn;
		this->breadthData = breadthData;
		this->start = start;
		this->bdIndex = 0;
	}
	~WideData(){
		for(int i = 0; i < breadthData->size(); i++){
			delete breadthData->data()[i];
		}

		delete breadthData;
	}

	bool Exist(){
		if(bdIndex >= breadthData->size()){
			return false;
		}
		return true;
	}
	wdElement* GetNext(){
		bdIndex++;
		return (*breadthData)[bdIndex-1];
	}
	int GetShitNumber(){
		return kn;
	}
};


//bの方が良いならtrueを返す
bool Compare(wdElement* a, wdElement* b){
	if(a->deadArea > b->deadArea){
		return true;
	} else if(a->deadArea == b->deadArea){
		if(a->fit < b->fit){
			return true;
		} else if(a->fit == b->fit){
			if(a->fit2 < b->fit2){
				return true;
			}
		}
	}
	return false;
} 

void InsertSort(std::vector<wdElement*>* breadthData, int n){
	int i, j;
	wdElement* temp;

	for(i = 1; i < n; i++) {  
		temp = ((*breadthData)[i]);
		for(j = i; j > 0 && Compare(((*breadthData)[j-1]), temp); j--) ((*breadthData)[j]) = ((*breadthData)[j-1]);
		((*breadthData)[j]) = temp;
	}

	//ランク付け
	wdElement* nowElm = ((*breadthData)[0]);
	int nowRank = 0;
	((*breadthData)[0])->rank = nowRank;

	for(i = 0; i < n; i++){
		wdElement* elm = ((*breadthData)[i]);
		if(Compare(elm, nowElm)){
			nowElm = ((*breadthData)[i]);
			nowRank++;
		}

		((*breadthData)[i])->rank = nowRank;
	}
}

#ifdef DEBUG_CODE
	LARGE_INTEGER liFreq, start;
#endif

std::vector<Answer_t> aStack;
std::vector<Answer_t> bestAnsStack;
#ifndef ZERO_SOLVE
int minScore = 1025;
#else
int minScore = 0;
#endif
int minScore_minShitNum = 1025;
int maxRank = 0;

void Search(std::vector<int> startNeighbor, int*** srcShitAry, int** srcShitDataAry, int* srcShitSizeAry, const int width, const int height, const int stonesNum, const int* srcMap, const int maxFreeSize){
RESTART :
	aStack.clear();
	int freeSize = maxFreeSize;

	int* map = new int[(width+1) * height];	//対象領域だけを考慮したマップ
	for(int i = 0; i < (width+1)*height; i++) map[i] = srcMap[i];

	int*** shitAry = srcShitAry;	//起点配列表現をした糞（ズク）の配列
	int** shitDataAry = srcShitDataAry;
	int* shitSizeAry = srcShitSizeAry;
	/*
	int*** shitAry = new int**[stonesNum];	//起点配列表現をした糞（ズク）の配列
	int** shitDataAry = new int*[stonesNum];
	int* shitSizeAry = new int[stonesNum];
	for(int s = 0; s < stonesNum; s++){
		//糞（ズク）配列のコピー
		shitAry[s] = new int*[8];
		for(int i = 0; i < 8; i++){
			if(srcShitAry[s][i] == 0) {
				shitAry[s][i] = 0;
				continue;
			}

			shitAry[s][i] = new int[17];
			for(int j = 0; j < 17; j++){
				shitAry[s][i][j] = srcShitAry[s][i][j];
			}
		}

		//糞（ズク）データ配列のコピー
		shitDataAry[s] = new int[8];
		for(int i = 0; i < 8; i++){
			shitDataAry[s] = srcShitDataAry[s];
		}

		//サイズ配列のコピー
		shitSizeAry[s] = srcShitSizeAry[s];
	}*/

	int areaAry[1025];	//下の方でつかいまわしている
	int subAreaMap[2048];
	int putPoints[17];
	int putAryLength;
	int putShitNum = 0;

	int startKn = 0;
	std::stack<WideData*> bdStack;

	int DEBUG_COUNT = 0;
	int kn = 0;
	int tmlCount = 0;
	while(1){
		//幅優先探索用のコード
		bool startFlag = false;

		if(bdStack.size() == 0){
			kn = startKn;
			startFlag = true;
			if(startKn == maxRank+1){
				break;
			}
			startKn++;
		} else {
		PUT_PROCESS:
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

			//実配置処理
			wdElement* data = bdStack.top()->GetNext();
			while(data->deadArea > minScore || (data->deadArea == minScore && data->needShitNum+putShitNum >= minScore_minShitNum)){
				if(!bdStack.top()->Exist()){
					delete bdStack.top();
					bdStack.pop();
					goto PUT_PROCESS;
				}
				data = bdStack.top()->GetNext();
			}

			if(data->rank > maxRank) {
				delete bdStack.top();
				bdStack.pop();
				continue;
			}

			int st = data->st;
			int placePoint = data->basePoint;

			int DUMMY;
			PutShit(map, kn, shitAry[kn][st], placePoint, width+1, height, &DUMMY);
			putShitNum++;
			freeSize -= shitSizeAry[kn];
			Answer_t ans; ans.basePoint=placePoint; ans.state=st; ans.shitNumber=kn;
			aStack.push_back(ans);

			if(kn == stonesNum-1){
				goto TERMINAL;
			}

#ifdef DEBUG_CODE
#ifdef CHECK_CODE
			printf("\tshit : %d(%c)\n", kn, kn+49);
			DEBUG_printMap(map, width+1, height);
			printf("free : %d\n", freeSize);
			printf("dead : %d\n", data->deadArea);
			DEBUG_waitKey();
#endif
#endif
			kn++;
			while(kn < stonesNum && shitSizeAry[kn] > data->maxAreaSize){
				//サイズ的に置けない糞（ズク）をスキップ
				kn++;
#ifdef DEBUG_CODE
#ifdef CHECK_CODE
				printf("%d skipped\n\n", kn);
#endif
#endif
			}
			if(kn == stonesNum) {
				goto TERMINAL;
			}

			//int deadArea = CalcDeadArea(map, width+1, height, freeSize, &(shitSizeAry[kn]), stonesNum-kn, areaAry, &needShitNum);
			/*
			if(minScore < data->deadArea || (minScore == data->deadArea && minScore_minShitNum <= data->needShitNum+putShitNum)){
				continue;
			}*/
		}
		//配置処理ここまで



	BREADTH_MAKE:
		std::vector<wdElement*>* breadthData = new std::vector<wdElement*>();
		for(int st = 0; st < 8; st++){
			const int* shit = shitAry[kn][st];
			if(shitAry[kn][st] == 0) continue;

			//ベースポイント配列作成
			std::vector<int> baseAry;
			if(startFlag){
				baseAry = startNeighbor;
			} else {
				std::vector<int>* neighbor = CalcNeighborPoint(map, width+1, height); //ココ

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

			//全てのベースポイントを探索
			for(int i = 0; i < baseAry.size(); i++){
				int basePoint = baseAry[i];

				/*
				printf("%d\n", basePoint);
				bool a = DEBUG_JudgePutable(map, shit, basePoint, width, height, putPoints, &putAryLength);
				DEBUG_printPutShitOnMap(map, putPoints, putAryLength, width, height);
				DEBUG_waitKey();
				*/

				if(JudgePutable(map, shit, basePoint, width, height)){
					int neighborNum;
					PutShit(map, kn, shitAry[kn][st], basePoint, width+1, height, &neighborNum);
					freeSize -= shitSizeAry[kn];

					int needShitNum = 0;
					int fit;
					int checkArea = CheckSubArea(map, width+1, height, freeSize, areaAry, subAreaMap, &fit);
					int deadArea = CalcDeadArea(map, width+1, height, freeSize, &(shitSizeAry[kn]), stonesNum-kn, areaAry, &needShitNum);

					//doko
					RemoveShit(map, kn, shitSizeAry[kn], width, height);
					freeSize += shitSizeAry[kn];

					//ここでやれるだろ
					if(minScore < deadArea || (minScore == deadArea && minScore_minShitNum <= needShitNum+putShitNum)){
						continue;
					}

					wdElement* temp = new wdElement();
					temp->st = st;
					temp->basePoint = basePoint;
					temp->fit = fit;//1024-areaNum;
					temp->fit2 = neighborNum;
					temp->deadArea = deadArea;
					temp->maxAreaSize = checkArea;
					temp->needShitNum = needShitNum;
					breadthData->push_back(temp);
				}
			}
		}

		if(breadthData->size() == 0){	//置けないなら
#ifdef DEBUG_CODE
#ifdef CHECK_CODE
			printf("%d cannot place\n\n", kn);
#endif
#endif
			delete breadthData;
			kn++;
			if(kn == stonesNum) goto TERMINAL;
			goto BREADTH_MAKE;
		} else {
			InsertSort(breadthData, breadthData->size());


		//	for(int i = 0; i < breadthData->size(); i++){
#ifdef DEBUG_CODE
#ifdef CHECK_CODE
			printf("shit %d(%c) place pattern\n", kn, kn+49); 
#endif
#endif
			for(int i = 0; i < breadthData->size() && (breadthData->data())[i]->rank == 0; i++){
				wdElement* elm = breadthData->data()[i];
#ifdef DEBUG_CODE
#ifdef CHECK_CODE
				printf("\t%d | %d-(%d, %d) : %d [%d %d]", elm->rank, elm->st, elm->basePoint%(width+1), elm->basePoint/(width+1), elm->deadArea, elm->fit, elm->fit2);
				DEBUG_waitKey();
#endif
#endif
			}

			bdStack.push(new WideData(kn, breadthData, startFlag));
		}
		continue;

TERMINAL:
#ifdef DEBUG_CODE
#ifdef CHECK_CODE
		printf("\t<<<terminal>>>\n\n");
#endif
#endif
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
#ifdef DEBUG_CODE
			DEBUG_printMap(map, width+1, height);
			LARGE_INTEGER end;
			QueryPerformanceCounter( &end );
			printf("score(%d,%d)\n", minScore, minScore_minShitNum);
			printf("time : %d\n", (end.QuadPart - start.QuadPart)/liFreq.QuadPart);
#endif
			bestAnsStack = aStack;
			tmlCount = 0;
			SendSolution(&bestAnsStack, stonesNum, shitDataAry, width+1);
		} else {
			tmlCount++;
#ifdef DEBUG_CODE
#ifdef CHECK_CODE
			if(tmlCount%100 == 0){
				printf("%d : %d\n", tmlCount/100, kn);
			}
#endif
#endif
#define MAX_TERMINAL_COUNT 100
			int maxCount = MAX_TERMINAL_COUNT;
			if(minScore == 0) maxCount = 100;
			if(tmlCount == maxCount){	//十万回終端を探索しても解が改善されないなら
				//koko
				while(bdStack.size() > 1){
					delete bdStack.top();
					bdStack.pop();
				}
				while(aStack.size() > 1){
					aStack.pop_back();
				}
				freeSize += RemoveShitNums(map, startKn, width, height);
				//状態をクリア
				putShitNum = 1;
				tmlCount = 0;
				//
#ifdef DEBUG_CODE
#ifdef CHECK_CODE
				printf("replace\n");
				DEBUG_printMap(map, width+1, height);
				printf("free : %d\n", freeSize);
				DEBUG_waitKey();
#endif
#endif
				continue;
			}
		}

		if(bdStack.size() == 0) break;
		if(bdStack.top()->GetShitNumber() == stonesNum-1){	//置いた糞（ズク）が最後なら
			delete bdStack.top();
			bdStack.pop();

			int remove = RemoveShit(map, kn, shitSizeAry[kn], width, height);
			freeSize += remove;
			if(remove != 0) putShitNum--;

			aStack.pop_back();
		}
	}


	maxRank++;
	delete map;
	goto RESTART;
}


#define CUTTING_THRESHOLD -1
//#define CUTTING_THRESHOLD stonesNum/2
void FullSearch(const int* Map, const int x1, const int y1, const int x2, const int y2, const int* stones, const int stonesNum, char* solution){
	//-----初期化処理-----//（長い）
		//短辺、長辺の調査
	const int width = x2-x1+1;
	const int height = y2-y1+1;

	dx1 = x1;
	dy1 = y1;

		//わかりやすくマップ定義
	int* map = new int[(width+1) * height];	//対象領域だけを考慮したマップ
	std::vector<int> startNeighbor;

	int freeSize = width*height;
	const int maxFreeSize = freeSize;
	for(int h = 0; h < height; h++){
		for(int w = 0; w < width; w++){
			int value = Map[(w+x1) + (h+y1)*32];

			//入力では 壁 = 1 になっているが、ここでは 壁 = -1 とする。
			if(value == 0){
				MAP(m, w, h) = 0;
			} else {
				MAP(m, w, h) = -1;
				freeSize--;
			}
			startNeighbor.push_back(w+h*(width+1));	//置ける地点として記憶
		}
	}
	for(int h = 0; h < height; h++){
		MAP(m, width, h) = -2;
	}

#ifdef DEBUG_CODE
	printf("%d %d\n", width, height); //DEBUG
	DEBUG_printMap(map, width+1, height);

	printf("free : %d\n", freeSize);
	DEBUG_waitKey();
#endif

		//糞（ズク）の再定義
	int*** shitAry = new int**[stonesNum];	//起点配列表現をした糞（ズク）の配列
	int** shitDataAry = new int*[stonesNum];
	int* shitSizeAry = new int[stonesNum];
	int totalSize = 0;
	for(int rs = 0; rs < stonesNum; rs++){
#ifndef REVERSE_SOLVE
		int s = rs;
#else
		int s = (stonesNum - 1) - rs;
#endif
#ifdef DEBUG_CODE
		printf("\t%c\n", s+1+48);
#endif
		shitAry[rs] = new int*[8];
		shitDataAry[rs] = new int[8];

//		DEBUG_printMapStone(stones, s);
		Shit_MapToBaseAry(stones, s, width, shitAry[rs], shitDataAry[rs]);

		shitSizeAry[rs] = CountShitSize(shitAry[rs][0]);	//サイズを計る
		totalSize += shitSizeAry[rs];
#ifdef DEBUG_CODE
		printf("\t size : %d\n", shitSizeAry[rs]);
#endif
	}

#ifdef DEBUG_CODE
	printf("total size : %d\n", totalSize);	//DEBUG
	printf("free :%d\n", freeSize);
	DEBUG_waitKey();
	printf("\nStart : \n");	//DEBUG

	QueryPerformanceFrequency( &liFreq );
	QueryPerformanceCounter( &start );
#endif

	Search(startNeighbor, shitAry, shitDataAry, shitSizeAry, width, height, stonesNum, map, freeSize);
	//-----探索-----//（長い）


	//ここまで繰り返し
#ifdef DEBUG_CODE
	printf("\nEND : score(%d,%d)\n", minScore, minScore_minShitNum);
	printf("\n");
	for(int i = 0; i < bestAnsStack.size(); i++){
		printf("\t%d %d %d\n", bestAnsStack[i].shitNumber, bestAnsStack[i].basePoint, bestAnsStack[i].state);
	}
#endif


	//-----終了処理-----//
	delete shitSizeAry;
	delete map;
	delete[] shitAry;
}

//デバッグ用のテンプレート↓
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