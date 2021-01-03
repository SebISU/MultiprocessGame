#include "multi_header.h"


uint32_t add_trea(struct trea_node ** head, uint8_t x, uint8_t y , uint32_t value){
    
    if (head == NULL){

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

uint32_t fetch_trea(struct trea_node ** head, uint8_t x, uint8_t y){

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

        printf("%-54sServer`s PID: %u\n", server->game_grid, server->server_pid);
        printf("%-55sCampsite X/Y: %u/%u\n", server->game_grid + 1, server->camp_position.x,
        server->camp_position.y);
        printf("%-55sRound number: %u\n", server->game_grid + 2, server->round_number);
        printf("%s\n", server->game_grid + 3);
        printf("%-54sParameter:   Player1  Player2  Player3  Player4\n", server->game_grid + 4);
        printf("%-55sPID:         %-9u%-9u%-9u%-9u\n", server->game_grid + 5,
        (server->players)->client_pid, (server->players + 1)->client_pid,
        (server->players + 2)->client_pid, (server->players + 3)->client_pid);
        printf("%-55sType:        %-9s%-9s%-9s%-9s\n", server->game_grid + 6,
        (server->players)->client_type, (server->players + 1)->client_type,
        (server->players + 2)->client_type, (server->players + 3)->client_type);
        printf("%-55sCurr X/Y:    %02u/%02u    %02u/%02u    %02u/%02u    %02u/%02u\n",
        server->game_grid + 7, (server->players)->curr_position.x,
        (server->players)->curr_position.y, (server->players + 1)->curr_position.x,
        (server->players + 1)->curr_position.y, (server->players + 2)->curr_position.x,
        (server->players + 2)->curr_position.y, (server->players + 3)->curr_position.x,
        (server->players + 3)->curr_position.y);
        printf("%-55sDeaths:      %-9u%-9u%-9u%-9u\n\n", server->game_grid + 8,
        (server->players)->deaths, (server->players + 1)->deaths, (server->players + 2)->deaths,
        (server->players + 3)->deaths);
        printf("%s\n", server->game_grid + 9);
        printf("%-54sCoins:\n", server->game_grid + 10);
        printf("%-55scarried      %-9u%-9u%-9u%-9u\n", server->game_grid + 11,
        (server->players)->coins_carried, (server->players + 1)->coins_carried,
        (server->players + 2)->coins_carried, (server->players + 3)->coins_carried);
        printf("%-55sbrought      %-9u%-9u%-9u%-9u\n", server->game_grid + 12,
        (server->players)->coins_brought, (server->players + 1)->coins_brought,
        (server->players + 2)->coins_brought, (server->players + 3)->coins_brought);
        printf("%s\n", server->game_grid + 13);
        printf("%s\n", server->game_grid + 14);
        printf("%-54sLegend:\n", server->game_grid + 15);
        printf("%-55s1234 - players\n", server->game_grid + 16);
        printf("%-55s#    - wall\n", server->game_grid + 17);
        printf("%-55s~    - bushes (slow down)\n", server->game_grid + 18);
        printf("%-55s*    - enemy\n", server->game_grid + 19);
        printf("%-55sD    - dropped treasure\n", server->game_grid + 20);
        printf("%-55sc    - one coin\n", server->game_grid + 21);
        printf("%-55sC    - treasure (10 coins)\n", server->game_grid + 22);
        printf("%-55sT    - large treasure (50 coins)\n", server->game_grid + 23);
        printf("%-55sA    - campsite\n", server->game_grid + 24);
    }
};

uint32_t init_shd_memories(struct api_wrap_t * api_client, struct api_wrap_conn * api_conn){

    if (api_client == NULL || api_conn == NULL){

        return 1;
    }

        // could/should be in case program failed and unlinked memos
        // shm_unlink(SHD_MEM_1);
        // shm_unlink(SHD_MEM_2);
        // shm_unlink(SHD_MEM_3);
        // shm_unlink(SHD_MEM_4);
        // shm_unlink(SHD_MEM_5);
        // shm_unlink(SHD_MEM_CONN);

    int32_t status;

    api_conn->api_id = shm_open(SHD_MEM_CONN, O_CREAT | O_RDWR, 0600); // if problem try to fiddle with chmod

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

            munmap(api_conn->api, sizeof(struct api_shd_conn)); // can check return value if not error
        }

        close(api_conn->api_id);
        shm_unlink(SHD_MEM_CONN);
    }

    if (api_client != NULL){

        for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

            if ((api_client + i)->api != NULL){

                munmap((api_client + i)->api, sizeof(struct api_t)); // can check too
            }

            if ((api_client + i)->api_id != -1){
                
                close((api_client + i)->api_id);
            }

            if (i == 0){

                shm_unlink(SHD_MEM_1);
            }
            else if(i == 1){

                shm_unlink(SHD_MEM_2);
            }
            else if (i == 2){

                shm_unlink(SHD_MEM_3);
            }
            else if (i == 3){

                shm_unlink(SHD_MEM_4);
            }
            else if (i == 4){

                shm_unlink(SHD_MEM_5);
            }
        }
        // try instead of if statements
        // shm_unlink(SHD_MEM_1);
        // shm_unlink(SHD_MEM_2);
        // shm_unlink(SHD_MEM_3);
        // shm_unlink(SHD_MEM_4);
        // shm_unlink(SHD_MEM_5);
    }
}

uint32_t select_random_position(uint8_t **gm_board, struct pos_t * position){

    if (gm_board == NULL || position == NULL){

        return 1;
    }

    srand(time(NULL));

    uint8_t x, y;

    // should add a condition in case of no free positions available

    do{

        x = (rand() % (GAME_WIDTH - 2)) + 1;
        y = (rand() % (GAME_HEIGHT - 2)) + 1;
    }while(gm_board[x][y] != ' ');

    position->x = x;
    position->y = y;

    return 0;
}

struct server_info * init_server_info(uint8_t ** gm_board){

    if (gm_board == NULL){

        return NULL;
    }

    struct server_info * server = (struct server_info*)malloc(sizeof(struct server_info));

    if (server == NULL){

        return NULL;
    }

    if ( 0 < select_random_position(gm_board, &server->camp_position)){

        free(server);
        return NULL;
    }

    server->round_number = 0;
    server->trea_list_head = NULL;
    server->server_pid = getpid();
    server->game_grid = gm_board;

    if (0 < set_new_character_game_board(server, &server->camp_position, 'A')){

        free(server);
        return NULL;
    }

    for (int32_t i = 0; i < PLAYERS_NUM + 1; ++i){

        if (1 == reset_player_info(server->players + i)){

            free(server);
            return NULL;
        }
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

    return server;
}

uint32_t reset_player_info(struct client_info * player){

    if (player == NULL){

        return 1;
    }

    player->client_pid = 0;
    strcpy(player->client_type, "---");
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
};

uint32_t set_api_client(struct server_info * server){



}

void * handle_connections(void * svr){

    //what if server == NULL, cannot stop server because you don't have any info, data

    // think how to stop this thread properly. Easy without semaphors ...

    struct server_info * server = (struct server_info *)svr;

    sem_unlink(SEM_CONN_SERVER); // for sure, maybe does not required
    sem_unlink(SEM_CONN_CLIENT); // if sem is opened somewhere it still runs until it will be closed

    server->api_conn.api->sem_server = sem_open(SEM_CONN_SERVER, O_CREAT | O_EXCL, 0600, 1);
    
    if (server->api_conn.api->sem_server == SEM_FAILED){

        return NULL;
    }

    server->api_conn.api->sem_client = sem_open(SEM_CONN_CLIENT, O_CREAT | O_EXCL, 0600, 0);
    
    if (server->api_conn.api->sem_client == SEM_FAILED){

        sem_close(server->api_conn.api->sem_server);
        sem_unlink(SEM_CONN_SERVER);
        return NULL;
    }

    while(1){

        if (update_api_conn(server)){   // think it over

            sem_close(server->api_conn.api->sem_server);
            sem_unlink(SEM_CONN_SERVER);
            sem_close(server->api_conn.api->sem_client);
            sem_unlink(SEM_CONN_CLIENT);

            return NULL;
        }

        sem_wait(server->api_conn.api->sem_client);

        accept_new_connection(server);  // what happen if connecting failed

        sem_post(server->api_conn.api->sem_server);

    }

    return NULL;
}

uint32_t reset_api_conn(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    server->api_conn.api->player_number = 0;
    server->api_conn.api->client_pid = 0;
    server->api_conn.api->beasts_in_game = 0;

    return 0;
}

uint32_t update_api_conn(struct server_info * server){  // add statements for beasts_in_game

    if (server == NULL){

        return 1;
    }

    if (server->api_conn.api->player_number != 0){

        server->api_conn.api->client_pid = 0;
        return 0;
    }

    int32_t i;

    for (i = 0; i < PLAYERS_NUM; ++i){

        if (server->players[i].player_number == 0){
        
            break;
        }
    }

    if (i < PLAYERS_NUM){
        
        reset_player_info(server->players + i);

        server->api_conn.api->player_number = i + 1;
        server->players[i].player_number = i + 1; // mutex probably does not required
    }

    server->api_conn.api->client_pid = 0;

    return 0;
}

uint32_t accept_new_connection(struct server_info * server){

    // can add a possibility to many attempts like in the web requests

    if (server == NULL){

        return 1;
    }

    if (strcmp(server->api_conn.api->client_type, "HUMAN") != 0 &&
    strcmp(server->api_conn.api->client_type, "CPU") != 0 &&
    strcmp(server->api_conn.api->client_type, "BEAST") != 0){

        return 2;
    }

    if (server->api_conn.api->client_pid == 0){

        return 3;
    }

    int32_t i;

    for (i = 0; i < PLAYERS_NUM; ++i){

        if (server->players[i].player_number == server->api_conn.api->player_number){

            prepare_new_player(server);
            reset_api_conn(server);

            break;
        }
    }

    if (i == PLAYERS_NUM){

        return 4;
    }

    return 0;
}

uint32_t prepare_new_player(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    server->players[server->api_conn.api->player_number - 1].client_pid = server->api_conn.api->client_pid;
    strcpy(server->players[server->api_conn.api->player_number - 1].client_type,
    server->api_conn.api->client_type);

    select_random_position(server->game_grid,
    &server->players[server->api_conn.api->player_number - 1].spawn_position);

    // where are if conditions in case of problems with these funcs?

    set_new_character_game_board(server,
    &server->players[server->api_conn.api->player_number - 1].spawn_position,
    &server->players[server->api_conn.api->player_number - 1].player_number + '0');

    update_api_client(server, server->api_conn.api->player_number);

// while loop on the client side until player number in the shared memory will be set

    return 0;
}

uint32_t set_new_character_game_board(struct server_info * server,
const struct pos_t * position, uint8_t figure){

    if (server == NULL || position == NULL){

        return 1;
    }

    if (position->x < 1 || position->x > GAME_WIDTH - 2 || position->y < 1
    || position->y > GAME_HEIGHT - 2){

        return 2;
    }

    if (NULL == strchr("1234A*ctTD", (int32_t)figure)){

        return 3;
    }

    server->game_grid[position->x][position->y] = figure;

    return 0;
}

uint32_t update_api_client(struct server_info * server, uint8_t client_num){

    if (server == NULL){

        return 1;
    }

    if (client_num < 1 || client_num > PLAYERS_NUM){

        return 2;
    }

    for (int32_t i = 1; i < PLAYERS_NUM + 1; ++i){

        if (i == client_num){

            server->api_client[i - 1].api->coins_brought = server->players[i - 1].coins_brought;
            server->api_client[i - 1].api->coins_carried = server->players[i - 1].coins_carried;
            server->api_client[i - 1].api->deaths = server->players[i - 1].deaths;
            server->api_client[i - 1].api->position = server->players[i - 1].curr_position;
            strcpy(server->api_client[i - 1].api->type, server->players[i - 1].client_type);
            server->api_client[i - 1].api->server_pid = server->server_pid;
            server->api_client[i - 1].api->round_number = server->round_number;
            server->api_client[i - 1].api->current_move = NO_MOVE;

            set_client_game_board(server->game_grid, server->api_client[i - 1].api->game_grid,
            &server->players[i - 1].curr_position);

// what if set_client will go wrong? Here mutex can/should be used to be sure that
// the same value is set to api and passed as a func parameter
// clash with game cycle thread
// except you set a proper condition in that thread

            server->api_client[i - 1].api->player_number = server->players[i - 1].player_number;
            server->api_conn.api->player_number = 0;
            break;
        }
    }

    return 0;
}

uint32_t set_client_game_board(uint8_t ** gm_board, uint8_t ** gm_board_cl,
const struct pos_t * position){

    if (gm_board == NULL || gm_board_cl == NULL){

        return 1;
    }

    if (position->x < 1 || position->x > GAME_WIDTH - 2 || position->y < 1
    || position->y > GAME_HEIGHT - 2){

        return 2;
    }

    uint8_t x = 0;
    uint8_t y = 0;

    for (int32_t i = position->x - 2; i <= position->x + 2; ++i){
        for (int32_t j = position->y - 2; j <= position->y + 2; ++j){

            if (i < 0 || i >= GAME_WIDTH || j < 0 || j >= GAME_HEIGHT){

                gm_board_cl[x][y] = '#';
            }
            else{

                gm_board_cl[x][y] = gm_board[i][j];
            }

            ++y;
        }

        ++x;
        y = 0;
    }

    return 0;
}