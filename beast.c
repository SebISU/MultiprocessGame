#include "multi_header.h"

int main(void){

    struct client_handle * handle = init_handle();
    struct client_mv_handle * mv_handle;

    if (handle == NULL){

        printf("\n\nProblem with info connection\n\n");
        return 1;
    }

    sem_wait(handle->sem_server);

    if (handle->api_conn->beasts_in_game == 1){

        sem_post(handle->sem_client);
        destroy_handle(handle);
        printf("\n\nBEAST ALREADY CONNECTED\n\n");
        return 2;
    }

    mv_handle = init_mv_beast_handle();

    if (mv_handle == NULL){

        sem_post(handle->sem_client);
        destroy_handle(handle);
        printf("\n\nProblem with main connection\n\n");
        return 3;
    }

    handle->api_conn->client_pid = getpid();
    strcpy((char*)handle->api_conn->client_type, "BEAST");

    sem_post(handle->sem_client);
    destroy_handle(handle);

    while(mv_handle->api->player_number != 5){      // beasts as a 5th player
        printf("BEAST\n");
        usleep(100000);
    };

    pthread_create(&mv_handle->key_bindings, NULL, bot_keybinding, (void*)mv_handle);
    pthread_t thd_id;
    srand(time(NULL));


    while(1){

        // curses.h is used, can cause a problem
        
        //printf("%d\n", mv_handle->api->player_number);

        sem_wait(mv_handle->sem_move);

        if (mv_handle->api->player_number == 0){

            printf("\n\nGAME IS OVER. SERVER CONNECTION LOST\n\n");
            break;
        }

        if (mv_handle->key_pressed == 'q' || mv_handle->key_pressed == 'Q'){

            // bot and client managed differently. Look how it is done.
            printf("\n\nTHANKS FOR THE GAME\n\n");
            break;
        }

        if (mv_handle->key_pressed != 0){

            mv_handle->key_pressed = 0;
            sem_post(&mv_handle->intern_sem);
        }

        mv_handle->api->player_number = 0;

        for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

            pthread_create(&thd_id, NULL, beast_next_move,
            (void*)(mv_handle->api->client_data + i));
        }
    }

    destroy_mv_handle(mv_handle);

    return 0;
}
