#include "multi_header.h"

void display_client(struct api_t * client){

    if (client != NULL){

        clear();

        move(0, 0);
        printw("%.*s%49sServer`s PID: %u", 5, *(client->client_data->game_grid), "", client->server_pid);
        move(1, 0);
        printw("%.*s%50sCampsite X/Y: unknown", 5, *(client->client_data->game_grid + 1), "");
        move(2, 0);
        printw("%.*s%50sRound number: %lu", 5, *(client->client_data->game_grid + 2), "", client->round_number);
        move(3, 0);
        printw("%.*s", 5, *(client->client_data->game_grid + 3));
        move(4, 0);
        printw("%.*s%49sPlayer:", 5, *(client->client_data->game_grid + 4), "");
        move(5, 0);
        printw("%-55sNumber:     %u", "", client->player_number);
        move(6, 0);
        printw("%-55sType:       %5s", "", client->type);
        move(7, 0);
        printw("%-55sCurr X/Y:   %02u/%02u", "", client->client_data->position.x,
        client->client_data->position.y);
        move(8, 0);
        printw("%-55sDeaths:     %u", "", client->deaths);
        move(9, 0);
        printw("%-55sCoins carried:   %u", "", client->coins_carried);
        move(10, 0);
        printw("%-55sCoins brought: %u", "", client->coins_brought);
        move(11, 0);
        printw("%-54sLegend:", "");
        move(12, 0);
        printw("%-55s1234 - players", "");
        move(13, 0);
        printw("%-55s#    - wall", "");
        move(14, 0);
        printw("%-55s~    - bushes (slow down)", "");
        move(15, 0);
        printw("%-55s*    - enemy", "");
        move(16, 0);
        printw("%-55sD    - dropped treasure", "");
        move(17, 0);
        printw("%-55sc    - one coin", "");
        move(18, 0);
        printw("%-55sC    - treasure (10 coins)", "");
        move(19, 0);
        printw("%-55sT    - large treasure (50 coins)", "");
        move(20, 0);
        printw("%-55sA    - campsite", "");

        refresh();
    }
}

struct client_handle * init_handle(){

    struct client_handle * handle = (struct client_handle *)malloc(sizeof(struct client_handle));

    if (handle == NULL){

        return NULL;
    }

    handle->sem_client = sem_open(SEM_CONN_CLIENT, 0);

    if ((void*)handle->sem_client == SEM_FAILED){

        free(handle);
        return NULL;
    }

    handle->sem_server = sem_open(SEM_CONN_SERVER, 0);

    if ((void*)handle->sem_server == SEM_FAILED){

        sem_close(handle->sem_client);
        free(handle);
        return NULL;
    }

    handle->api_conn_id = shm_open(SHD_MEM_CONN, O_RDWR, 0600);

    if (handle->api_conn_id == -1){

        sem_close(handle->sem_client);
        sem_close(handle->sem_server);
        free(handle);
        return NULL;
    }

    handle->api_conn =  (struct api_shd_conn *)mmap(NULL, sizeof(struct api_shd_conn),
    PROT_READ | PROT_WRITE, MAP_SHARED, handle->api_conn_id, 0);

    if (handle->api_conn == NULL){

        close(handle->api_conn_id);
        sem_close(handle->sem_client);
        sem_close(handle->sem_server);
        free(handle);
        return NULL;
    }

    return handle;
}

void destroy_handle(struct client_handle * handle){

    if (handle != NULL){

        if (handle->sem_server != SEM_FAILED){

            sem_close(handle->sem_server);
        }

        if (handle->sem_client != SEM_FAILED){

            sem_close(handle->sem_client);
        }

        if (handle->api_conn != NULL){

            munmap(handle->api_conn, sizeof(struct api_shd_conn));
        }

        if (handle->api_conn_id != -1){

            close(handle->api_conn_id);
        }

        free(handle);
    }
}

struct client_mv_handle * init_mv_handle(const int8_t player_number){

    if (player_number == 0 || player_number > PLAYERS_NUM){

        return NULL;
    }

    struct client_mv_handle * mv_handle = malloc(sizeof(struct client_mv_handle));

    if (mv_handle == NULL){

        return NULL;
    }

    if (player_number == 1){

        mv_handle->sem_move = sem_open(SEM_1, 0);
        mv_handle->api_id = shm_open(SHD_MEM_1, O_RDWR, 0600);

    }
    else if (player_number == 2){
        
        mv_handle->sem_move = sem_open(SEM_2, 0);
        mv_handle->api_id = shm_open(SHD_MEM_2, O_RDWR, 0600);

    }
    else if (player_number == 3){

        mv_handle->sem_move = sem_open(SEM_3, 0);
        mv_handle->api_id = shm_open(SHD_MEM_3, O_RDWR, 0600);
    }
    else{

        mv_handle->sem_move = sem_open(SEM_4, 0);
        mv_handle->api_id = shm_open(SHD_MEM_4, O_RDWR, 0600);
    }

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

    mv_handle->player_number = player_number;
    mv_handle->key_pressed = 0;

    return mv_handle;
}

void destroy_mv_handle(struct client_mv_handle * mv_handle){

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

        sem_destroy(&mv_handle->intern_sem);

        free(mv_handle);
    }
}

void * client_keybinding(void * handle){

    struct client_mv_handle * mv_handle = (struct client_mv_handle *)handle;

    while(1){

        mv_handle->key_pressed = getch();

        flushinp();

        if (mv_handle->key_pressed == 'q' || mv_handle->key_pressed == 'Q'){

            return NULL;
        }

        sem_wait(&mv_handle->intern_sem);
    }

    return NULL;
}
