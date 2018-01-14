#include "data_processing.h"

int processData(Array data){
	int TDS = 0;
	int BDS = 1;
	int UP_M = 2;
	int DOWN_M = 3;


	int states[1000][2];
	int cur_state = TDS;
	int count = 1;
	int prev_dist = data.array[0];

	states[0][0] = cur_state;
	states[0][1] = prev_dist;

	printf("Data size: %d", data.used);

	for(int i = 1; i < data.used; i++){
//		printf("%d\n", count);
		if(((i != 1 || i != data.used - 1)||(i != 2 || i != data.used - 2)) && ((data.array[i] < data.array[i+1]) && (data.array[i] > data.array[i-1])) && ((data.array[i + 1] <= data.array[i+2]) && (data.array[i - 1] >= data.array[i-2]))){
			printf(" first %d %d %d %d %d \n", data.array[i - 2],data.array[i - 1], data.array[i],data.array[i + 1], data.array[i + 2]);
			continue;
				}
		else if(((i != 1 || i != data.used - 1)||(i != 2 || i != data.used - 2)) && ((data.array[i] > data.array[i+1]) && (data.array[i] < data.array[i-1])) && ((data.array[i + 1] >= data.array[i+2]) && (data.array[i -1] <= data.array[i-2]))){
			printf("second %d %d %d %d %d\n", data.array[i - 2],data.array[i - 1], data.array[i],data.array[i + 1], data.array[i + 2]);

			continue;
		}
		else if (cur_state == TDS){
			if (prev_dist > data.array[i]) {
				cur_state = DOWN_M;
			}
		}
		else if(cur_state == BDS){
			if (prev_dist < data.array[i]){
				cur_state = UP_M;
			}
		}
		else if (cur_state == UP_M){
            if (prev_dist == data.array[i]){
            	cur_state = TDS;
            	states[count][0] = cur_state;
            	states[count][1] = prev_dist;
            	count += 1;
            }
            else if(prev_dist > data.array[i]){
            	states[count][0] = TDS;
            	states[count][1] = prev_dist;
//            	printf("TDS = %d\n", states[count][0]);
//            	printf("Distance = %d\n", states[count][1]);
            	cur_state = DOWN_M;
            	count += 1;
            }
		}
		else if(cur_state == DOWN_M){
			if (prev_dist == data.array[i]){
				cur_state = BDS;
				states[count][0] = cur_state;
				states[count][1] = prev_dist;
				count += 1;
			} else if(prev_dist < data.array[i]){
				states[count][0] = BDS;
				states[count][1] = prev_dist;
//				printf("BDS = %d\n", states[count][0]);
//				printf("Distance = %d\n", states[count][1]);
				cur_state = UP_M;
				count += 1;
			}
		}

	prev_dist = data.array[i];


	}
	printf("COUNT: %d\n", count);
	int dlength = 0;
	for(int c = 0; c < count-1; c+=2){
//    		deltas[countd] = abs(states[c][1] - states[c + 1][1]);
//    		printf("%d\n", deltas[countd]);
		dlength+=1;
	}
//	printf("%d\n", dlength);


	int deltas[dlength];

	int countd = 0;
	for(int c = 0; c < count-1; c+=2){
		deltas[countd] = abs(states[c][1] - states[c + 1][1]);
//		printf("%d\n", deltas[countd]);
		countd+=1;
	}

	int max = -1000;

	for(int d = 0; d < countd; d+=1){
		if(deltas[d] > max){
			max = deltas[d];
		}
	}
	printf("MAX: %d\n", max);

	int number = 0;
	for(int e = 0; e < countd; e+=1){
		printf("DELTA: %d\n", deltas[e]);
		if((deltas[e]>=max-5)&&(deltas[e]<=max+5)){
			number ++;
		}
	}
	return number;

}

void sendToAndroid(int numbStrikes) {
	printf("Strikes: %d\n", numbStrikes);
	//	int lst[1] = {2000};
	char temp[100] = "";
	sprintf(temp, "%u;", numbStrikes);
	USARTSendString(temp);
}
