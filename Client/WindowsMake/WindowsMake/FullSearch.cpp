#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

char DEBUG_C;
#define	STONE(n, x, y)	stones[((n) << 6) + ((y) << 3) + (x)]

void DEBUG_printMap(const int* map, const int width, const int height){
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			int value = map[x + y*width];
			if(value == -1){
				printf("# ");
			} else {
				printf("%d ", value);
			}
		}
		printf("\n");
	}
	printf("\n");
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


void FullSearch(const int* Map, const int x1, const int y1, const int x2, const int y2, const int* stones, int stonesNum, char* solution){
	//strcat(solution, "H 0 2 2\n")
	//-----����������-----//�i�����j
		//�Z�ӁA���ӂ̒���
	const int width = x2-x1+1;
	const int height = y2-y1+1;

		//�킩��₷���}�b�v��`
	int* map = new int[width * height];	//�Ώۗ̈悾�����l�������}�b�v
	for(int h = 0; h < height; h++){
		for(int w = 0; w < width; w++){
			int value = Map[(w+x1) + (h+y1)*32];

			//���͂ł� �� = 1 �ɂȂ��Ă��邪�A�����ł� �� = -1 �Ƃ���B
			if(value == 0){
				map[w + h*width] = 0;
			} else {
				map[w + h*width] = -1;
			}
		}
	}

	printf("%d %d\n", width, height); //DEBUG
	DEBUG_printMap(map, width, height);

		//���i�Y�N�j�̍Ē�`
	int** shitAry = new int*[stonesNum];	//�N�_�z��\�����������i�Y�N�j�̔z��
	for(int s = 0; s < stonesNum; s++){
		shitAry[s] = new int[17];

		int min = -1;
		int bsIndex = 0;
		for(int i = 0; i < 64; i++){
			if(STONE(s, i%8, i/8) == 1){
				if(min == -1){
					min = i;
				}

					//��������₱�����������Ǒ��������Ă�
				shitAry[s][bsIndex] = (i-min) + (i/8-min/8)*(width-8);
				bsIndex++;
			}
			shitAry[s][bsIndex] = -1;	//-1�͔ԕ�
		}

		//DEBUG
		printf("\tstone %d\n", 1);
		DEBUG_printMapStone(stones, s);
		DEBUG_printBaseAryStone(shitAry[s]);
		scanf("%c", &DEBUG_C);
		//DEBUG
	}


	//-----�I������-----//
	delete map;
	delete[] shitAry;

	//DEBUG
	scanf("%c", &DEBUG_C);
}