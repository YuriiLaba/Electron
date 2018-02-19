#include "data_processing.h"

int processData(int dataTest[], int size){
	int TDS = 0;
	int BDS = 1;
	int UP_M = 2;
	int DOWN_M = 3;





//	printf("Data size: %d",sizeof(data));

	//start

//	int tempList[102];
//	int resultList[100];
//
//	tempList[0] = 0;
//
//	int padding = 1;
//
//	for(int i = 0; i < 100; i++){
//		tempList[i + padding] = dataTest[i];
//	}
//
//	tempList[101] = 1;
//
//
//
//	int u = 0;
//	while (u != 100){
//		int tempSum = 0;
//		///(tempList[u]+tempList[u+1]+tempList[u+2])
//		tempSum = ((tempList[u]*1.0/3.0) + (tempList[u+1]*1.0/3.0) + (tempList[u+2]*1.0/3.0));
//		resultList[u] = tempSum;
//		u += 1;
//	}
//	printf("%d\n", 10000000000000000000);
//	for (int i = 0; i < 100; i++){
//		printf("%d\n", resultList[i]);
//	}
//	printf("%d\n", 10000000000000000000);

	//finish


	int states[1000][2];
	int cur_state = TDS;
	int count = 1;
	int prev_dist = dataTest[0];

//	printf("%d\n", prev_dist);


	states[0][0] = cur_state;
	states[0][1] = prev_dist;
//	printf("%d\n", states[0][0]);
//	printf("%d\n", states[0][1]);

	for(int i = 1; i < size; i++){
//		printf("%d\n", count);
		if (cur_state == TDS){
//			printf("%d\n", prev_dist);
			if (prev_dist < dataTest[i]) {
				cur_state = DOWN_M;
			}
		}
		else if(cur_state == BDS){
			if (prev_dist > dataTest[i]){
				cur_state = UP_M;
			}
		}
		else if (cur_state == UP_M){
            if (prev_dist == dataTest[i]){
            	cur_state = TDS;
            	states[count][0] = cur_state;
            	states[count][1] = prev_dist;
            	count += 1;
            }
            else if(prev_dist < dataTest[i]){
            	states[count][0] = TDS;
            	states[count][1] = prev_dist;
//            	printf("TDS = %d\n", states[count][0]);
//            	printf("Distance = %d\n", states[count][1]);
            	cur_state = DOWN_M;
            	count += 1;
            }
//            else{
//            	cur_state = UP_M;
//            }
		}
		else if(cur_state == DOWN_M){
			if (prev_dist == dataTest[i]){
				cur_state = BDS;
				states[count][0] = cur_state;
				states[count][1] = prev_dist;
//				printf("BDS = %d\n", states[count][0]);
				printf("Distance = %d\n", states[count][1]);

				count += 1;
//				printf("%d\n", count);
			} else if(prev_dist > dataTest[i]){
				states[count][0] = BDS;
				states[count][1] = prev_dist;
//				printf("BDS = %d\n");
				printf("Distance = %d\n", states[count][1]);
				cur_state = UP_M;
				count += 1;
//				printf("%d\n", count);
			}
//			else{
//				cur_state = DOWN_M;
//			}
		}

	prev_dist = dataTest[i];


	}
	int max = -1000;
//	printf("COUNT=%d\n", count);
	for(int c = 0; c < count; c+=1){
		if(c%2 != 0){
//			printf("%d\n", c);
//			printf("Distance__1 = %d\n", states[c][1]);
			if(states[c][1] > max){
				max=states[c][1];

			}

		}
//		printf("Distance__1 = %d\n", states[c][1]);
	}
//	printf("%d\n", dlength);


//	int deltas[dlength];

//	int max = -1000;
//
//	for(int c = 1; c < count-1; c+=2){
////		if(states[c][1] > max){
//		printf("%d\n",states[c][1]);
////		}
//	}

	printf("MAX: ");
	printf("%d\n", max);

	int number = 0;
	int delta = 3;
	for(int c = 0; c < count; c+=1){
		if(c%2 != 0){
			if((states[c][1] >= max-delta)||(states[c][1] >= max+delta)){
//				printf("%d\n",states[c][1]);
				number++;
			}
		}
	}
	printf("Strikes: %d\n", number);
	return max;

}

void sendToAndroid(int numbStrikes) {
	printf("Number of real strikes: %d\n", numbStrikes);
	char temp[100] = "";
	sprintf(temp, "%u;", numbStrikes);
	USARTSendString(temp);
}
