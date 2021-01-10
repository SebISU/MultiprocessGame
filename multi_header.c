#include "multi_header.h"


uint32_t add_trea(struct trea_node ** head, int8_t x, int8_t y , uint32_t value){
    
    if (head == NULL || value == 0){    // just for values > 0

        return 0;
    }

    struct trea_node * temp;

    if (*head == NULL){

        *head = malloc(sizeof(struct trea_node));

        if (*head == NULL){

            return 0;
        }
    }
    else{

        temp = malloc(sizeof(struct trea_node));

        if (temp == NULL){

            return 0;
        }

        temp->next = *head;
        *head = temp;
    }

    (*head)->treasure_info.position.x = x;
    (*head)->treasure_info.position.y = y;
    (*head)->treasure_info.value = value;

    return (*head)->treasure_info.value;
}

uint32_t fetch_trea(struct trea_node ** head, int8_t x, int8_t y){

    if (head == NULL){

        return 0;
    }

    struct trea_node * temp = *head;
    struct trea_node * prev_temp;
    uint32_t val;

    while(temp != NULL){

        if (temp->treasure_info.position.x == x && temp->treasure_info.position.y == y){

            val = temp->treasure_info.value;

            if (*head == temp){

                *head = temp->next;
                free(temp);
            }
            else{

                prev_temp->next = temp->next;
                free(temp);
            }

            return val;
        }

        prev_temp = temp;
        temp = temp->next;
    }

    return 0;
}

void destroy_trea(struct trea_node ** head){

    if (head != NULL){

        struct trea_node * temp;

        while(*head != NULL){

            temp = *head;
            *head = (*head)->next;
            free(temp);
        }
    }
}

void display_server(struct server_info* server){

    if (server != NULL){

        clear();

        move(0, 0);
        printw("%-54sServer`s PID: %u", *server->game_grid, server->server_pid);
        move(1, 0);
        printw("%-55sCampsite X/Y: %u/%u", *(server->game_grid + 1), server->camp_position.x,
        server->camp_position.y);
        move(2, 0);
        printw("%-55sRound number: %lu", *(server->game_grid + 2), server->round_number);
        move(3, 0);
        printw("%s\n", *(server->game_grid + 3));
        move(4, 0);
        printw("%-54sParameter:   Player1  Player2  Player3  Player4", *(server->game_grid + 4));
        move(5, 0);
        printw("%-55sPID:         %-9d%-9d%-9d%-9d", *(server->game_grid + 5),
        (server->players)->client_pid, (server->players + 1)->client_pid,
        (server->players + 2)->client_pid, (server->players + 3)->client_pid);
        move(6, 0);
        printw("%-55sType:        %-9s%-9s%-9s%-9s", *(server->game_grid + 6),
        (server->players)->client_type, (server->players + 1)->client_type,
        (server->players + 2)->client_type, (server->players + 3)->client_type);
        move(7, 0);
        printw("%-55sCurr X/Y:    %02u/%02u    %02u/%02u    %02u/%02u    %02u/%02u",
        *(server->game_grid + 7), (server->players)->curr_position.x,
        (server->players)->curr_position.y, (server->players + 1)->curr_position.x,
        (server->players + 1)->curr_position.y, (server->players + 2)->curr_position.x,
        (server->players + 2)->curr_position.y, (server->players + 3)->curr_position.x,
        (server->players + 3)->curr_position.y);
        move(8, 0);
        printw("%-55sDeaths:      %-9u%-9u%-9u%-9u", *(server->game_grid + 8),
        (server->players)->deaths, (server->players + 1)->deaths, (server->players + 2)->deaths,
        (server->players + 3)->deaths);
        move(9, 0);
        printw("%s", *(server->game_grid + 9));
        move(10, 0);
        printw("%-54sCoins:", *(server->game_grid + 10));
        move(11, 0);
        printw("%-55scarried      %-9u%-9u%-9u%-9u", *(server->game_grid + 11),
        (server->players)->coins_carried, (server->players + 1)->coins_carried,
        (server->players + 2)->coins_carried, (server->players + 3)->coins_carried);
        move(12, 0);
        printw("%-55sbrought      %-9u%-9u%-9u%-9u", *(server->game_grid + 12),
        (server->players)->coins_brought, (server->players + 1)->coins_brought,
        (server->players + 2)->coins_brought, (server->players + 3)->coins_brought);
        move(13, 0);
        printw("%s", *(server->game_grid + 13));
        move(14, 0);
        printw("%s", *(server->game_grid + 14));
        move(15, 0);
        printw("%-54sLegend:", *(server->game_grid + 15));
        move(16, 0);
        printw("%-55s1234 - players", *(server->game_grid + 16));
        move(17, 0);
        printw("%-55s#    - wall", *(server->game_grid + 17));
        move(18, 0);
        printw("%-55s~    - bushes (slow down)", *(server->game_grid + 18));
        move(19, 0);
        printw("%-55s*    - enemy", *(server->game_grid + 19));
        move(20, 0);
        printw("%-55sD    - dropped treasure", *(server->game_grid + 20));
        move(21, 0);
        printw("%-55sc    - one coin", *(server->game_grid + 21));
        move(22, 0);
        printw("%-55sC    - treasure (10 coins)", *(server->game_grid + 22));
        move(23, 0);
        printw("%-55sT    - large treasure (50 coins)", *(server->game_grid + 23));
        move(24, 0);
        printw("%-55sA    - campsite", *(server->game_grid + 24));

        move(25, 0);
        refresh();
    }
}

uint32_t init_shd_memories(struct api_wrap_t * api_client, struct api_wrap_conn * api_conn){

    if (api_client == NULL || api_conn == NULL){

        return 1;
    }

    shm_unlink(SHD_MEM_1);
    shm_unlink(SHD_MEM_2);
    shm_unlink(SHD_MEM_3);
    shm_unlink(SHD_MEM_4);
    shm_unlink(SHD_MEM_5);
    shm_unlink(SHD_MEM_CONN);

    int32_t status;

    api_conn->api_id = shm_open(SHD_MEM_CONN, O_CREAT | O_RDWR, 0600);

    if (api_conn->api_id == -1){

        return 1;
    }

    status = ftruncate(api_conn->api_id, sizeof(struct api_shd_conn));

    if (status == -1){

        destroy_shd_memories(api_client, api_conn);
        return 1;
    }

    api_conn->api = mmap(NULL, sizeof(struct api_shd_conn), PROT_READ | PROT_WRITE,
    MAP_SHARED, api_conn->api_id, 0);

    if (api_conn->api == NULL){

        destroy_shd_memories(api_client, api_conn);
        return 1;
    }

    for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

        if (i == 0){

            (api_client + i)->api_id = shm_open(SHD_MEM_1, O_CREAT | O_RDWR, 0600);
        }
        else if(i == 1){

            (api_client + i)->api_id = shm_open(SHD_MEM_2, O_CREAT | O_RDWR, 0600);
        }
        else if (i == 2){

            (api_client + i)->api_id = shm_open(SHD_MEM_3, O_CREAT | O_RDWR, 0600);
        }
        else if (i == 3){

            (api_client + i)->api_id = shm_open(SHD_MEM_4, O_CREAT | O_RDWR, 0600);
        }
        else if (i == 4){

            (api_client + i)->api_id = shm_open(SHD_MEM_5, O_CREAT | O_RDWR, 0600);
        }

        if ((api_client + i)->api_id == -1){

            destroy_shd_memories(api_client, api_conn);
            return 1;
        }

        status = ftruncate((api_client + i)->api_id, sizeof(struct api_t));

        if (status == -1){

            destroy_shd_memories(api_client, api_conn);
            return 1;
        }

        (api_client + i)->api = mmap(NULL, sizeof(struct api_t), PROT_READ | PROT_WRITE,
        MAP_SHARED, (api_client + i)->api_id, 0);

        if ((api_client + i)->api == NULL){

            destroy_shd_memories(api_client, api_conn);
            return 1;
        }

    }

    return 0;
}

void destroy_shd_memories(struct api_wrap_t * api_client, struct api_wrap_conn * api_conn){

    if (api_conn != NULL){

        if (api_conn->api != NULL){

            munmap(api_conn->api, sizeof(struct api_shd_conn));
        }

        close(api_conn->api_id);
        shm_unlink(SHD_MEM_CONN);
    }

    if (api_client != NULL){

        for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

            if ((api_client + i)->api != NULL){

                munmap((api_client + i)->api, sizeof(struct api_t));
            }

            if ((api_client + i)->api_id != -1){
                
                close((api_client + i)->api_id);
            }

        }

        shm_unlink(SHD_MEM_1);
        shm_unlink(SHD_MEM_2);
        shm_unlink(SHD_MEM_3);
        shm_unlink(SHD_MEM_4);
        shm_unlink(SHD_MEM_5);
    }
}

uint32_t init_semaphores(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    sem_unlink(SEM_CONN_SERVER);

    server->sem_server = sem_open(SEM_CONN_SERVER, O_CREAT | O_EXCL, 0600, 0);

    if (server->sem_server == SEM_FAILED){

        return 2;
    }

    sem_unlink(SEM_CONN_CLIENT);

    server->sem_client = sem_open(SEM_CONN_CLIENT, O_CREAT | O_EXCL, 0600, 0);

    if (server->sem_client == SEM_FAILED){

        destroy_semaphores(server);
        return 2;
    }

    sem_unlink(SEM_1);

    server->sem_client_1 = sem_open(SEM_1, O_CREAT | O_EXCL, 0600, 0);

    if (server->sem_client_1 == SEM_FAILED){

        destroy_semaphores(server);
        return 2;
    }

    sem_unlink(SEM_2);

    server->sem_client_2 = sem_open(SEM_2, O_CREAT | O_EXCL, 0600, 0);

    if (server->sem_client_2 == SEM_FAILED){

        destroy_semaphores(server);
        return 2;
    }

    sem_unlink(SEM_3);

    server->sem_client_3 = sem_open(SEM_3, O_CREAT | O_EXCL, 0600, 0);

    if (server->sem_client_3 == SEM_FAILED){

        destroy_semaphores(server);
        return 2;
    }

    sem_unlink(SEM_4);

    server->sem_client_4 = sem_open(SEM_4, O_CREAT | O_EXCL, 0600, 0);

    if (server->sem_client_4 == SEM_FAILED){

        destroy_semaphores(server);
        return 2;
    }

    sem_unlink(SEM_5);

    server->sem_client_5 = sem_open(SEM_5, O_CREAT | O_EXCL, 0600, 0);

    if (server->sem_client_5 == SEM_FAILED){

        destroy_semaphores(server);
        return 2;
    }

    if (sem_init(&server->sem_keybinding, 0, 0)){

        destroy_semaphores(server);
        return 2;
    }

    return 0;
}

void destroy_semaphores(struct server_info * server){

    if (server != NULL){

        if (server->sem_server != SEM_FAILED){

            sem_close(server->sem_server);
            sem_unlink(SEM_CONN_SERVER);
        }

        if (server->sem_client != SEM_FAILED){

            sem_close(server->sem_client);
            sem_unlink(SEM_CONN_CLIENT);
        }

        if (server->sem_client_1 != SEM_FAILED){

            sem_close(server->sem_client_1);
            sem_unlink(SEM_1);
        }

        if (server->sem_client_2 != SEM_FAILED){

            sem_close(server->sem_client_2);
            sem_unlink(SEM_2);
        }

        if (server->sem_client_3 != SEM_FAILED){

            sem_close(server->sem_client_3);
            sem_unlink(SEM_3);
        }

        if (server->sem_client_4 != SEM_FAILED){

            sem_close(server->sem_client_4);
            sem_unlink(SEM_4);
        }

        if (server->sem_client_5 != SEM_FAILED){

            sem_close(server->sem_client_5);
            sem_unlink(SEM_5);
        }

        sem_destroy(&server->sem_keybinding);
    }
}

uint32_t select_random_position(int8_t **gm_board, struct pos_t * position){

    if (gm_board == NULL || position == NULL){

        return 1;
    }

    srand(time(NULL));

    int8_t x, y;

    do{

        x = (rand() % (GAME_WIDTH - 2)) + 1;
        y = (rand() % (GAME_HEIGHT - 2)) + 1;
    }while(gm_board[y][x] != ' ');

    position->x = x;
    position->y = y;

    return 0;
}

struct server_info * init_server(int8_t ** gm_board){

    if (gm_board == NULL){

        return NULL;
    }

    struct server_info * server = (struct server_info*)malloc(sizeof(struct server_info));

    if (server == NULL){

        return NULL;
    }

    if (0 < select_random_position(gm_board, &server->camp_position)){

        free(server);
        return NULL;
    }

    server->round_number = 0;
    server->trea_list_head = NULL;
    server->server_pid = getpid();
    server->game_grid = gm_board;
    server->key_pressed = 0;

    if (0 < set_new_character_game_board(server, &server->camp_position, 'A')){

        free(server);
        return NULL;
    }

    for (int32_t i = 0; i < PLAYERS_NUM; ++i){

        if (0 < reset_player_info(server->players + i)){

            free(server);
            return NULL;
        }
    }

    if (0 < reset_beasts_info(&server->beasts)){

        free(server);
        return NULL;
    }

    if (0 < init_shd_memories(server->api_client, &server->api_conn)){

        free(server);
        return NULL;
    }

    if (reset_api_conn(server)){

        destroy_shd_memories(server->api_client, &server->api_conn);
        free(server);
        return NULL;
    }

    for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

        if (0 < reset_api_client((server->api_client + i)->api)){

            destroy_shd_memories(server->api_client, &server->api_conn);
            free(server);
            return NULL;
        }
    }

    if (init_semaphores(server)){

        destroy_shd_memories(server->api_client, &server->api_conn);
        free(server);
        return NULL;
    }

    if (pthread_mutex_init(&server->mutex, NULL)){

        destroy_shd_memories(server->api_client, &server->api_conn);
        destroy_semaphores(server);
        free(server);
        return NULL;
    }

    return server;
}

void destroy_server(struct server_info * server){

    if (server != NULL){

        endwin();
        destroy_semaphores(server);
        destroy_shd_memories(server->api_client, &server->api_conn);
        pthread_mutex_destroy(&server->mutex);
        destroy_trea(&server->trea_list_head);
        free(server);
    }
}

uint32_t reset_player_info(struct client_info * player){

    if (player == NULL){

        return 1;
    }

    player->client_pid = -1;
    strcpy((char*)player->client_type, "---");
    player->coins_carried = 0;
    player->coins_brought = 0;
    player->deaths = 0;
    player->into_bushes = 0;
    player->player_number = 0; // 0 means that there is no client connected
    player->curr_position.x = 0;
    player->curr_position.y = 0;
    player->prev_position.x = 0;
    player->prev_position.y = 0;
    player->spawn_position.x = 0;
    player->spawn_position.y = 0;

    return 0;
}

uint32_t reset_beasts_info(struct beasts_t * beasts){

    if (beasts == NULL){

        return 1;
    }

    beasts->beasts_pid = -1;
    beasts->client_number = 0;

    for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

        (beasts->beasts + i)->in_game = 0; // 0 means not in game
    }

    return 0;
}

uint32_t reset_api_client(struct api_t * api_client){

    if (api_client == NULL){

        return 1;
    }

    api_client->round_number = 0;
    api_client->server_pid = -1;
    api_client->deaths = 0;
    api_client->coins_carried = 0;
    api_client->coins_brought = 0;

    for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

        (api_client->client_data + i)->position.x = 0;
        (api_client->client_data + i)->position.y = 0;
        (api_client->client_data + i)->current_move = NO_MOVE;
    }

    strcpy((char*)api_client->type, "---");
    api_client->player_number = 0;


    return 0;
}

void * handle_connections(void * svr){

    if (svr == NULL){

        return NULL;
    }

    struct server_info * server = (struct server_info *)svr;

    if (update_api_conn(server)){

        return NULL;
    }

    while(1){

        sem_post(server->sem_server);

        sem_wait(server->sem_client);

        pthread_mutex_lock(&server->mutex);

        accept_new_connection(server);

        if (update_api_conn(server)){

            pthread_mutex_unlock(&server->mutex);
            return NULL;
        }

        pthread_mutex_unlock(&server->mutex);
    }

    return NULL;
}

uint32_t reset_api_conn(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    server->api_conn.api->player_number = 0;
    server->api_conn.api->client_pid = -1;
    server->api_conn.api->beasts_in_game = 0;

    return 0;
}

uint32_t update_api_conn(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    if (server->beasts.client_number > 0){

        server->api_conn.api->beasts_in_game = 1;
    }
    else{

        server->api_conn.api->beasts_in_game = 0;
    }

    if (server->api_conn.api->player_number != 0){ // accepted connection sets player_num to 0

        server->api_conn.api->client_pid = -1;
        return 0;
    }

    int32_t i;

    for (i = 0; i < PLAYERS_NUM; ++i){

        if (server->players[i].player_number == 0){
        
            break;
        }
    }

    if (i < PLAYERS_NUM){

        server->api_conn.api->player_number = i + 1;

        server->players[i].player_number = i + 1;     
    }

    server->api_conn.api->client_pid = -1;

    return 0;
}

uint32_t accept_new_connection(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    if (server->api_conn.api->client_pid < 0){

        return 3;
    }

    if (strcmp((char*)server->api_conn.api->client_type, "HUMAN") == 0 ||
    strcmp((char*)server->api_conn.api->client_type, "CPU") == 0){

        uint32_t i;

        for (i = 0; i < PLAYERS_NUM; ++i){

            if (server->players[i].player_number == server->api_conn.api->player_number){

                prepare_new_player(server);
                server->api_conn.api->player_number = 0;

                break;
            }
        }

        if (i == PLAYERS_NUM){

            return 4;
        }

    }
    else if (strcmp((char*)server->api_conn.api->client_type, "BEAST") == 0){

        if (server->beasts.client_number > 0){

            return 5;
        }
        else{

            prepare_beast(server);
        }
    }
    else {

        return 2;
    }

    return 0;
}

uint32_t prepare_new_player(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    server->players[server->api_conn.api->player_number - 1].player_number
    = server->api_conn.api->player_number;
    server->players[server->api_conn.api->player_number - 1].client_pid
    = server->api_conn.api->client_pid;
    strcpy((char*)server->players[server->api_conn.api->player_number - 1].client_type,
    (char*)server->api_conn.api->client_type);

    select_random_position(server->game_grid,
    &server->players[server->api_conn.api->player_number - 1].spawn_position);

    server->players[server->api_conn.api->player_number - 1].curr_position.x =
    server->players[server->api_conn.api->player_number - 1].spawn_position.x;
    server->players[server->api_conn.api->player_number - 1].curr_position.y =
    server->players[server->api_conn.api->player_number - 1].spawn_position.y;
    server->players[server->api_conn.api->player_number - 1].prev_position.x =
    server->players[server->api_conn.api->player_number - 1].spawn_position.x;
    server->players[server->api_conn.api->player_number - 1].prev_position.y =
    server->players[server->api_conn.api->player_number - 1].spawn_position.y;

    set_new_character_game_board(server,
    &server->players[server->api_conn.api->player_number - 1].spawn_position,
    server->players[server->api_conn.api->player_number - 1].player_number + '0');

    return 0;
}

uint32_t prepare_beast(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    server->beasts.beasts_pid = server->api_conn.api->client_pid;
    server->beasts.client_number = 5;

    return 0;
}

uint32_t update_api_client(struct server_info * server, int8_t client_num){

    if (server == NULL){

        return 1;
    }

    if (client_num > 0 && client_num <= PLAYERS_NUM){

        if (server->players[client_num - 1].player_number > 0
        && server->players[client_num - 1].client_pid >= 0){

            server->api_client[client_num - 1].api->round_number = server->round_number;
            server->api_client[client_num - 1].api->server_pid = server->server_pid;
            server->api_client[client_num - 1].api->deaths =
            server->players[client_num - 1].deaths;
            server->api_client[client_num - 1].api->coins_carried =
            server->players[client_num - 1].coins_carried;
            server->api_client[client_num - 1].api->coins_brought =
            server->players[client_num - 1].coins_brought;
            server->api_client[client_num - 1].api->client_data->position.x =
            server->players[client_num - 1].curr_position.x;
            server->api_client[client_num - 1].api->client_data->position.y =
            server->players[client_num - 1].curr_position.y;
            server->api_client[client_num - 1].api->client_data->current_move = NO_MOVE;
            set_client_game_board(server,
            server->api_client[client_num - 1].api->client_data,
            &server->api_client[client_num - 1].api->client_data->position);
            strcpy((char*)server->api_client[client_num - 1].api->type,
            (char*)server->players[client_num - 1].client_type);
            server->api_client[client_num - 1].api->player_number =
            server->players[client_num - 1].player_number;
        }
    }
    else if (client_num == PLAYERS_NUM + 1){

        if (server->beasts.client_number > 0 && server->beasts.beasts_pid >= 0){

            server->api_client[client_num - 1].api->round_number = server->round_number;
            server->api_client[client_num - 1].api->server_pid = server->server_pid;
            strcpy((char*)server->api_client[client_num - 1].api->type, "BEAST");
            server->api_client[client_num - 1].api->player_number =
            server->beasts.client_number;

            for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                if ((server->beasts.beasts + i)->in_game){

                    server->api_client[client_num - 1].api->client_data[i].position.x =
                    server->beasts.beasts[i].position.x;
                    server->api_client[client_num - 1].api->client_data[i].position.y =
                    server->beasts.beasts[i].position.y;
                    server->api_client[client_num - 1].api->client_data[i].current_move = NO_MOVE;
                    set_client_game_board(server,
                    server->api_client[client_num - 1].api->client_data + i,
                    &server->beasts.beasts[i].position);
                }
                else{

                    server->api_client[client_num - 1].api->client_data[i].position.x = 0;
                    server->api_client[client_num - 1].api->client_data[i].position.y = 0;
                }
            }
        }
    }
    else{

        return 2;
    }

    return 0;
}

uint32_t update_all_api_client(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    for (int8_t i = 1; i <= PLAYERS_NUM + 1; ++i){

        if (update_api_client(server, i)){

            return 2;
        }
    }

    return 0;
}

uint32_t set_new_character_game_board(struct server_info * server,
const struct pos_t * position, int8_t figure){

    if (server == NULL || position == NULL){

        return 1;
    }

    if (position->x < 1 || position->x > GAME_WIDTH - 2 || position->y < 1
    || position->y > GAME_HEIGHT - 2){

        return 2;
    }

    if (NULL == strchr("1234 A*ctTD", (int32_t)figure)){

        return 3;
    }

    server->game_grid[position->y][position->x] = figure;

    return 0;
}

uint32_t set_client_game_board(struct server_info * server, struct api_inner_t * api_client,
const struct pos_t * position){

    if (server == NULL || api_client == NULL){

        return 1;
    }

    if (position->x < 1 || position->x > GAME_WIDTH - 2 || position->y < 1
    || position->y > GAME_HEIGHT - 2){

        return 2;
    }

    int8_t x = 0;
    int8_t y = 0;

    for (int32_t i = position->y - 2; i <= position->y + 2; ++i){
        for (int32_t j = position->x - 2; j <= position->x + 2; ++j){

            if (i < 0 || i >= GAME_HEIGHT || j < 0 || j >= GAME_WIDTH){

                api_client->game_grid[y][x] = '#';
            }
            else{

                api_client->game_grid[y][x] = server->game_grid[i][j];
            }

            ++x;
        }

        ++y;
        x = 0;
    }

    return 0;
}

void * server_keybinding(void * svr){

    struct server_info * server = (struct server_info *)svr;

    WINDOW * key_win = newwin(1, 1, 25, 0);
    keypad(key_win, TRUE);

    while(1){

        server->key_pressed = wgetch(key_win);

        flushinp();

        if (server->key_pressed == 'q' || server->key_pressed == 'Q'){

            return NULL;
        }
        else if (server->key_pressed == 'b' || server->key_pressed == 'B'){

            pthread_mutex_lock(&server->mutex);

            add_new_beast(server);
        
            pthread_mutex_unlock(&server->mutex);
        }
        else if (server->key_pressed == 'c'){

            pthread_mutex_lock(&server->mutex);

            add_new_coin(server);

            pthread_mutex_unlock(&server->mutex);
        }
        else if (server->key_pressed == 't'){

            pthread_mutex_lock(&server->mutex);

            add_new_small_treasure(server);

            pthread_mutex_unlock(&server->mutex);
        }
        else if (server->key_pressed == 'T'){

            pthread_mutex_lock(&server->mutex);

            add_new_big_treasure(server);

            pthread_mutex_unlock(&server->mutex);
        }

        sem_wait(&server->sem_keybinding);
    }

    return NULL;
}

uint32_t add_new_beast(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    struct pos_t new_beast;

    if (server->beasts.client_number == 0){

        return 2;
    }

    int32_t i;

    for (i = 0; i < BEASTS_MAX_NUM; ++i){

        if (server->beasts.beasts[i].in_game == 0){

            break;
        }
    }

    if (i == BEASTS_MAX_NUM){

        return 3;
    }

    if (select_random_position(server->game_grid, &new_beast)){

        return 4;
    }

    server->beasts.beasts[i].position.x = new_beast.x;
    server->beasts.beasts[i].position.y = new_beast.y;
    server->beasts.beasts[i].in_game = 1;
    server->beasts.beasts[i].init_round_number = server->round_number;
    set_new_character_game_board(server, &new_beast, '*');

    return 0;
}

uint32_t add_new_coin(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    struct pos_t new_coin;

    if (select_random_position(server->game_grid, &new_coin)){

        return 2;
    }

    if (!add_trea(&server->trea_list_head, new_coin.x, new_coin.y, 1)){

        return 3;
    }

    if (set_new_character_game_board(server, &new_coin, 'c')){

        fetch_trea(&server->trea_list_head, new_coin.x, new_coin.y);
        return 4;
    }

    return 0;
}

uint32_t add_new_small_treasure(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    struct pos_t new_sm_trea;

    if (select_random_position(server->game_grid, &new_sm_trea)){

        return 2;
    }

    if (!add_trea(&server->trea_list_head, new_sm_trea.x, new_sm_trea.y, 10)){

        return 3;
    }

    if (set_new_character_game_board(server, &new_sm_trea, 't')){

        fetch_trea(&server->trea_list_head, new_sm_trea.x, new_sm_trea.y);
        return 4;
    }

    return 0;
}

uint32_t add_new_big_treasure(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    struct pos_t new_bg_trea;

    if (select_random_position(server->game_grid, &new_bg_trea)){

        return 2;
    }

    if (!add_trea(&server->trea_list_head, new_bg_trea.x, new_bg_trea.y, 50)){

        return 3;
    }

    if (set_new_character_game_board(server, &new_bg_trea, 'T')){

        fetch_trea(&server->trea_list_head, new_bg_trea.x, new_bg_trea.y);
        return 4;
    }

    return 0;
}

uint32_t move_beast(struct server_info * server, int8_t beast_num){

    if (server == NULL || server->game_grid == NULL || beast_num > BEASTS_MAX_NUM){

        return 1;
    }

    struct pos_t temp_pos;
    int8_t tmp_num = 0;

    if (server->api_client[PLAYERS_NUM].api->client_data[beast_num - 1].current_move
    == LEFT){

        temp_pos.x = server->beasts.beasts[beast_num - 1].position.x - 1;
        temp_pos.y = server->beasts.beasts[beast_num - 1].position.y;
    }
    else if (server->api_client[PLAYERS_NUM].api->client_data[beast_num - 1].current_move
    == UP){

        temp_pos.x = server->beasts.beasts[beast_num - 1].position.x;
        temp_pos.y = server->beasts.beasts[beast_num - 1].position.y - 1;
    }
    else if (server->api_client[PLAYERS_NUM].api->client_data[beast_num - 1].current_move
    == RIGHT){

        temp_pos.x = server->beasts.beasts[beast_num - 1].position.x + 1;
        temp_pos.y = server->beasts.beasts[beast_num - 1].position.y;
    }
    else if (server->api_client[PLAYERS_NUM].api->client_data[beast_num - 1].current_move
    == DOWN){

        temp_pos.x = server->beasts.beasts[beast_num - 1].position.x;
        temp_pos.y = server->beasts.beasts[beast_num - 1].position.y + 1;
    }
    else {

        return 0;
    }

    if (server->game_grid[temp_pos.y][temp_pos.x] == ' '){

        set_new_character_game_board(server, 
        &server->beasts.beasts[beast_num - 1].position, ' ');
        set_new_character_game_board(server, 
        &temp_pos, '*');
        server->beasts.beasts[beast_num - 1].position.x = temp_pos.x;
        server->beasts.beasts[beast_num - 1].position.y = temp_pos.y;
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '1'){

        tmp_num = 1;
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '2'){

        tmp_num = 2;
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '3'){

        tmp_num = 3;
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '4'){

        tmp_num = 4;
    }
    
    if (tmp_num > 0){

        if (server->players[tmp_num - 1].coins_carried > 0){

            set_new_character_game_board(server, 
            &temp_pos, 'D');
            add_trea(&server->trea_list_head, temp_pos.x, temp_pos.y,
            server->players[tmp_num - 1].coins_carried);
            server->players[tmp_num - 1].coins_carried = 0;
        }
        else{

            set_new_character_game_board(server, 
            &server->beasts.beasts[beast_num - 1].position, ' ');
            set_new_character_game_board(server, 
            &temp_pos, '*');
            server->beasts.beasts[beast_num - 1].position.x = temp_pos.x;
            server->beasts.beasts[beast_num - 1].position.y = temp_pos.y;
        }

        temp_pos.x = server->players[tmp_num - 1].spawn_position.x;
        temp_pos.y = server->players[tmp_num - 1].spawn_position.y;
        server->players[tmp_num - 1].deaths++;
        server->players[tmp_num - 1].prev_position.x =
        server->players[tmp_num - 1].curr_position.x;
        server->players[tmp_num - 1].prev_position.y =
        server->players[tmp_num - 1].curr_position.y;
        server->players[tmp_num - 1].curr_position.x = temp_pos.x;
        server->players[tmp_num - 1].curr_position.y = temp_pos.y;
        server->players[tmp_num - 1].into_bushes = 0;

        if (server->game_grid[temp_pos.y][temp_pos.x] == 'D'
        || server->game_grid[temp_pos.y][temp_pos.x] == 'c'
        || server->game_grid[temp_pos.y][temp_pos.x] == 't'
        || server->game_grid[temp_pos.y][temp_pos.x] == 'T'){

            server->players[tmp_num - 1].coins_carried +=
            fetch_trea(&server->trea_list_head, temp_pos.x, temp_pos.y);
        }
        else if (server->game_grid[temp_pos.y][temp_pos.x] == '*'){

            for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                if (server->beasts.beasts[i].position.x == temp_pos.x
                && server->beasts.beasts[i].position.y == temp_pos.y){

                    server->beasts.beasts[i].in_game = 0;
                    break;
                }
            }
        }

        set_new_character_game_board(server, &temp_pos, tmp_num + '0');

    }

    return 0;
}

uint32_t move_player(struct server_info * server, int8_t player_num){

    if (server == NULL || server->game_grid == NULL || player_num > PLAYERS_NUM){

        return 1;
    }

    struct pos_t temp_pos;

    if (server->api_client[player_num - 1].api->client_data->current_move == LEFT){

        temp_pos.x = server->players[player_num - 1].curr_position.x - 1;
        temp_pos.y = server->players[player_num - 1].curr_position.y;
    }
    else if (server->api_client[player_num - 1].api->client_data->current_move == UP){

        temp_pos.x = server->players[player_num - 1].curr_position.x;
        temp_pos.y = server->players[player_num - 1].curr_position.y - 1;
    }
    else if (server->api_client[player_num - 1].api->client_data->current_move == RIGHT){

        temp_pos.x = server->players[player_num - 1].curr_position.x + 1;
        temp_pos.y = server->players[player_num - 1].curr_position.y;
    }
    else if (server->api_client[player_num - 1].api->client_data->current_move == DOWN){

        temp_pos.x = server->players[player_num - 1].curr_position.x;
        temp_pos.y = server->players[player_num - 1].curr_position.y + 1;
    }
    else if (server->api_client[player_num - 1].api->client_data->current_move == QUIT){

        set_new_character_game_board(server, &server->players[player_num - 1].curr_position, ' ');
        reset_player_info(server->players + player_num - 1);
        server->api_client[player_num - 1].api->client_data->current_move = NO_MOVE;
        update_api_conn(server);

        return 0;
    }
    else {

        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].into_bushes = 0;

        return 0;
    }

    if (server->game_grid[temp_pos.y][temp_pos.x] == ' '){

        set_new_character_game_board(server, 
        &server->players[player_num - 1].curr_position, ' ');
        set_new_character_game_board(server, &temp_pos, player_num + '0');
        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].curr_position.x = 
        temp_pos.x;
        server->players[player_num - 1].curr_position.y = 
        temp_pos.y;
        server->players[player_num - 1].into_bushes = 0;
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == 'A'){

        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].coins_brought +=
        server->players[player_num - 1].coins_carried;
        server->players[player_num - 1].coins_carried = 0;
        server->players[player_num - 1].into_bushes = 0;
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == 'D'
    || server->game_grid[temp_pos.y][temp_pos.x] == 'c'
    || server->game_grid[temp_pos.y][temp_pos.x] == 't'
    || server->game_grid[temp_pos.y][temp_pos.x] == 'T'){

        set_new_character_game_board(server, 
        &server->players[player_num - 1].curr_position, ' ');
        set_new_character_game_board(server, &temp_pos, player_num + '0');
        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].curr_position.x = 
        temp_pos.x;
        server->players[player_num - 1].curr_position.y = 
        temp_pos.y;
        server->players[player_num - 1].into_bushes = 0;

        server->players[player_num - 1].coins_carried +=
        fetch_trea(&server->trea_list_head, temp_pos.x, temp_pos.y);
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '~'){

        if (server->players[player_num - 1].prev_position.x ==
        server->players[player_num - 1].curr_position.x
        && server->players[player_num - 1].prev_position.y ==
        server->players[player_num - 1].curr_position.y
        && server->players[player_num - 1].into_bushes == 1){

            set_new_character_game_board(server, 
            &server->players[player_num - 1].curr_position, ' ');
            set_new_character_game_board(server, &temp_pos, player_num + '0');
            server->players[player_num - 1].curr_position.x = 
            temp_pos.x;
            server->players[player_num - 1].curr_position.y = 
            temp_pos.y;
            server->players[player_num - 1].into_bushes = 0;
        }
        else{
            server->players[player_num - 1].prev_position.x =
            server->players[player_num - 1].curr_position.x;
            server->players[player_num - 1].prev_position.y =
            server->players[player_num - 1].curr_position.y;
            server->players[player_num - 1].into_bushes = 1;
        }
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '*'){

        if (server->players[player_num - 1].coins_carried > 0){

            set_new_character_game_board(server, 
            &server->players[player_num - 1].curr_position, 'D');
            add_trea(&server->trea_list_head,
            server->players[player_num - 1].curr_position.x,
            server->players[player_num - 1].curr_position.y,
            server->players[player_num - 1].coins_carried);
            server->players[player_num - 1].coins_carried = 0;
        }
        else{

            set_new_character_game_board(server, 
            &server->players[player_num - 1].curr_position, ' ');
        }

        temp_pos.x = server->players[player_num - 1].spawn_position.x;
        temp_pos.y = server->players[player_num - 1].spawn_position.y;
        server->players[player_num - 1].deaths++;
        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].curr_position.x = temp_pos.x;
        server->players[player_num - 1].curr_position.y = temp_pos.y;
        server->players[player_num - 1].into_bushes = 0;

        if (server->game_grid[temp_pos.y][temp_pos.x] == 'D'
        || server->game_grid[temp_pos.y][temp_pos.x] == 'c'
        || server->game_grid[temp_pos.y][temp_pos.x] == 't'
        || server->game_grid[temp_pos.y][temp_pos.x] == 'T'){

            server->players[player_num - 1].coins_carried +=
            fetch_trea(&server->trea_list_head, temp_pos.x, temp_pos.y);
        }
        else if (server->game_grid[temp_pos.y][temp_pos.x] == '*'){

            for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                if (server->beasts.beasts[i].position.x == temp_pos.x
                && server->beasts.beasts[i].position.y == temp_pos.y){

                    server->beasts.beasts[i].in_game = 0;
                    break;
                }
            }
        }

        set_new_character_game_board(server, &temp_pos, player_num + '0');
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '1'
    || server->game_grid[temp_pos.y][temp_pos.x] == '2'
    || server->game_grid[temp_pos.y][temp_pos.x] == '3'
    || server->game_grid[temp_pos.y][temp_pos.x] == '4'){

        server->players[player_num - 1].deaths++;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].deaths++;
        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].prev_position.x =
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position.x;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].prev_position.y =
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position.y;
        server->players[player_num - 1].curr_position.x =
        server->players[player_num - 1].spawn_position.x;
        server->players[player_num - 1].curr_position.y =
        server->players[player_num - 1].spawn_position.y;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position.x =
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].spawn_position.x;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position.y =
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].spawn_position.y;
        server->players[player_num - 1].into_bushes = 0;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].into_bushes = 0;

        set_new_character_game_board(server, 
        &server->players[player_num - 1].curr_position, player_num + '0');
        set_new_character_game_board(server, 
        &server->players[player_num - 1].prev_position, ' ');
        set_new_character_game_board(server, 
        &server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position,
        server->game_grid[temp_pos.y][temp_pos.x]);
        set_new_character_game_board(server, 
        &server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].prev_position, ' ');

        if (server->players[player_num - 1].coins_carried > 0
        || server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].coins_carried > 0){
        
            add_trea(&server->trea_list_head, temp_pos.x, temp_pos.y,
            server->players[player_num - 1].coins_carried +
            server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].coins_carried);
            
            if (server->players[player_num - 1].curr_position.x != temp_pos.x
            && server->players[player_num - 1].curr_position.y != temp_pos.y
            && server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position.x
            != temp_pos.x
            && server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position.y
            != temp_pos.y){
                
                set_new_character_game_board(server, &temp_pos, 'D');
            }
        }
        
        server->players[player_num - 1].coins_carried = 0;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].coins_carried = 0;
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '#'){

        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].into_bushes = 0;
    }

    return 0;
}
