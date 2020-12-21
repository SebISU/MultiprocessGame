#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){

    char table[128][128];
    srand(time(NULL));

    for (int x = 0; x < 1000000; ++x){

        for (int i = 0; i < 128; ++i){
            for (int j = 0; j < 128; ++j){

                if (i == 0 || j == 0 || i == 127 || j == 127){

                    table[i][j] = '%';
                }
                else if (rand() % 2 == 1){

                    table[i][j] = '#';
                }
                else{

                    table[i][j] = ' ';
                }
            }

            for (int w = 0; w < 128; ++w){

                putc(table[i][w], stdout);
            }

            putc('\n', stdout);
        }

        sleep(2);
        system("clear");
    }

    return 0;
}

/*

basic explanation of project's stucture in recording of a lecture 17.12 -> 1:00:00


*/