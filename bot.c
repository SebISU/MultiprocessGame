#include "multi_header.h"


int main(void){

    struct client_handle * handle = init_handle();
    struct client_mv_handle * mv_handle;

    if (handle == NULL){

        printf("Problem with info connection");
        return 1;
    }

    sem_wait(handle->sem_server);

    if (handle->api_conn->player_number == 0){

        sem_post(handle->sem_client);
        destroy_handle(handle);
        printf("SERVER IS FULL OR DEAD\n");
        return 2;
    }

    mv_handle = init_mv_handle(handle->api_conn->player_number);

    if (mv_handle == NULL){

        sem_post(handle->sem_client);
        destroy_handle(handle);
        printf("Problem with main connection\n");
        return 3;
    }

    handle->api_conn->client_pid = getpid();
    strcpy((char*)handle->api_conn->client_type, "CPU");

    sem_post(handle->sem_client);
    destroy_handle(handle);

    while(mv_handle->api->player_number != mv_handle->player_number){

    };

    display_client(mv_handle->api);
    pthread_create(&mv_handle->key_bindings, NULL, bot_keybinding, (void*)mv_handle);
    struct pos_t beast = {.x = 5, .y = 5};
    srand(time(NULL));

    while(1){

        // here catching moves
        // curses.h is used, can cause a problem
        
        sem_wait(mv_handle->sem_move);

        system("clear");
        display_client(mv_handle->api);  // displaying after if? better or not?

        if (mv_handle->api->player_number == 0){

            printf("\n\nGAME IS OVER. SERVER CONNECTION LOST\n\n");
            break;
        }

        if (mv_handle->key_pressed == 'q' || mv_handle->key_pressed == 'Q'){

            printf("\n\nTHANKS FOR THE GAME\n\n");
            break;
        }

        if (mv_handle->key_pressed != 0){

            mv_handle->key_pressed = 0;
            sem_post(&mv_handle->intern_sem);
        }

        printf("%d\n", mv_handle->api->player_number);
        mv_handle->api->player_number = 0; // not zero means that process is dead

        // these two fuctions take much time 
        get_closest_beast(mv_handle->api->client_data, &beast);
        mv_handle->api->client_data->current_move = 
        run_away_next_move(mv_handle->api->client_data, &beast);

        beast.x = 5;
        beast.y = 5;
    }

    destroy_mv_handle(mv_handle);

    return 0;
}