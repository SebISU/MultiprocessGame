#include "multi_header.h"


int main(void){

    struct client_handle * handle = init_handle();
    struct client_mv_handle * mv_handle;

    if (handle == NULL){

        printf("Problem with validation connection");
        return 1;
    }

    sem_wait(handle->sem_server);

    if (handle->api_conn->player_number == 0){

        sem_post(handle->sem_client);
        destroy_handle(handle);
        printf("SERVER IS FULL\n");
        return 2;
    }

    mv_handle = init_mv_handle(handle->api_conn->player_number);

    if (mv_handle == NULL){

        sem_post(handle->sem_client);
        destroy_handle(handle);
        printf("Problem with move connection\n");
        return 3;
    }

    handle->api_conn->client_pid = getpid();
    strcpy(handle->api_conn->client_type, "HUMAN");

    sem_post(handle->sem_client);
    destroy_handle(handle);

    while(mv_handle->api->player_number != mv_handle->player_number){

    };

    display_client(mv_handle->api);
    pthread_create(&mv_handle->key_bindings, NULL, client_keybinding, (void*)mv_handle);

    while(1){

        // here catching moves
        // curses.h is used, can cause a problem
        
        sem_wait(mv_handle->sem_move);

        display_client(mv_handle->api);  // displaying after if? better or not?

        if (mv_handle->api->player_number == 0){

            printf("\n\nGAME IS OVER. SERVER CONNECTION LOST\n\n");
            break;
        }

        if (mv_handle->key_pressed != 0){

            if (mv_handle->key_pressed == 'q' || mv_handle->key_pressed == 'Q'){

                mv_handle->api->client_data->current_move = QUIT; // does not required
                printf("\n\nTHANKS FOR THE GAME\n\n");
                break;
            }
            else if (mv_handle->key_pressed == KEY_UP){

                mv_handle->api->client_data->current_move = UP;
            }
            else if (mv_handle->key_pressed == KEY_DOWN){

                mv_handle->api->client_data->current_move = DOWN;
            }
            else if (mv_handle->key_pressed == KEY_LEFT){

                mv_handle->api->client_data->current_move = LEFT;
            }
            else if (mv_handle->key_pressed == KEY_RIGHT){

                mv_handle->api->client_data->current_move = RIGHT;
            }

            mv_handle->key_pressed = 0;
            sem_post(&mv_handle->intern_sem);
        }

        mv_handle->api->player_number = 0;

    }

    destroy_mv_handle(mv_handle);

    return 0;
}