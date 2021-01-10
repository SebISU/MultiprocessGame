#include "multi_header.h"

uint32_t get_closest_player(const struct api_inner_t * beast, struct pos_t * cl_player){

    if (cl_player == NULL || beast == NULL){

        return 1;
    }

    struct pos_t position;

    for (int8_t i = 0; i < 5; ++i){
        for (int8_t j = 0; j < 5; ++j){

            if (beast->game_grid[i][j] == '1'
            || beast->game_grid[i][j] == '2'
            || beast->game_grid[i][j] == '3'
            || beast->game_grid[i][j] == '4'){

                position.x = j;
                position.y = i;

                if (can_chase(beast, &position)){

                    shortest_maze_distance(cl_player, &position);
                }
            }
        }
    }

    return 0;
}

enum move_t follow_next_move(const struct api_inner_t * beast, const struct pos_t * player){

    if (beast == NULL || player == NULL){

        return QUIT;
    }

    if (player->x >= 5 && player->y >= 5){

        return rand() % 5;
    }

    if (player->x > CLIENT_POS_X && player->y > CLIENT_POS_Y){

        if (beast->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else{

            return DOWN;
        }
    }
    else if (player->x == CLIENT_POS_X && player->y > CLIENT_POS_Y){

        return DOWN;
    }
    else if (player->x < CLIENT_POS_X && player->y > CLIENT_POS_Y){

        if (beast->game_grid[2][1] == ' '){

            return LEFT;
        }
        else{

            return DOWN;
        }
    }
    else if (player->x < CLIENT_POS_X && player->y == CLIENT_POS_Y){

        return LEFT;        
    }
    else if (player->x < CLIENT_POS_X && player->y < CLIENT_POS_Y){

        if (beast->game_grid[2][1] == ' '){

            return LEFT;
        }
        else{

            return UP;
        }
    }
    else if (player->x == CLIENT_POS_X && player->y < CLIENT_POS_Y){

        return UP;
    }
    else if (player->x > CLIENT_POS_X && player->y < CLIENT_POS_Y){

        if (beast->game_grid[2][3] == ' '){

            return RIGHT;
        }
        else{

            return UP;
        }
    }
    else{

        return RIGHT;
    }

    return NO_MOVE;
}


void * beast_next_move(void * beast){

    if (beast != NULL){

        struct api_inner_t * inner_beast = (struct api_inner_t *)beast;
        struct pos_t player = {.x = 5, .y = 5};

        if (inner_beast->position.x > 0 && inner_beast->position.y > 0){

            get_closest_player(inner_beast, &player);
            inner_beast->current_move = follow_next_move(inner_beast, &player);
        }
    }

    return NULL;
}

struct client_mv_handle * init_mv_beast_handle(){

    struct client_mv_handle * mv_handle = malloc(sizeof(struct client_mv_handle));

    if (mv_handle == NULL){

        return NULL;
    }

    mv_handle->sem_move = sem_open(SEM_5, 0);
    mv_handle->api_id = shm_open(SHD_MEM_5, O_RDWR, 0600);

    if (mv_handle->sem_move == SEM_FAILED && mv_handle->api_id == -1){

        free(mv_handle);
        return NULL;
    }
    else if (mv_handle->sem_move == SEM_FAILED && mv_handle->api_id != -1){

        close(mv_handle->api_id);
        free(mv_handle);
        return NULL;
    }
    else if (mv_handle->sem_move != SEM_FAILED && mv_handle->api_id == -1){


        free(mv_handle);
        return NULL;
    }

    mv_handle->api =  (struct api_t *)mmap(NULL, sizeof(struct api_t),
    PROT_READ | PROT_WRITE, MAP_SHARED, mv_handle->api_id, 0);

    if (mv_handle->api == NULL){

        close(mv_handle->api_id);
        sem_close(mv_handle->sem_move);
        free(mv_handle);
        return NULL;
    }

    if (sem_init(&mv_handle->intern_sem, 0, 0)){

        munmap(mv_handle->api, sizeof(struct api_t));
        close(mv_handle->api_id);
        sem_close(mv_handle->sem_move);
        free(mv_handle);
        return NULL;
    }

    mv_handle->player_number = 5;
    mv_handle->key_pressed = 0;

    return mv_handle;
}

void destroy_mv_beast_handle(struct client_mv_handle * mv_handle){

    if (mv_handle != NULL){

        if (mv_handle->sem_move != SEM_FAILED){

            sem_close(mv_handle->sem_move);
        }

        if (mv_handle->api != NULL){

            munmap(mv_handle->api, sizeof(struct api_t));
        }

        if (mv_handle->api_id != -1){

            close(mv_handle->api_id);
        }

        sem_close(&mv_handle->intern_sem);

        free(mv_handle);
    }
}
