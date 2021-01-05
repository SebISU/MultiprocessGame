#include "multi_header.h"


uint32_t add_trea(struct trea_node ** head, uint8_t x, uint8_t y , uint32_t value){
    
    if (head == NULL || value == 0){    // just for > 0 values

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

    shm_unlink(SHD_MEM_1);
    shm_unlink(SHD_MEM_2);
    shm_unlink(SHD_MEM_3);
    shm_unlink(SHD_MEM_4);
    shm_unlink(SHD_MEM_5);
    shm_unlink(SHD_MEM_CONN);

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

uint32_t select_random_position(uint8_t **gm_board, struct pos_t * position){

    if (gm_board == NULL || position == NULL){

        return 1;
    }

    srand(time(NULL));

    uint8_t x, y;

    // should add a condition in case of no free positions available
    // 1300 fields in the board. 10000 loops as a limit?

    do{

        x = (rand() % (GAME_WIDTH - 2)) + 1;
        y = (rand() % (GAME_HEIGHT - 2)) + 1;
    }while(gm_board[y][x] != ' ');

    position->x = x;
    position->y = y;

    return 0;
}

struct server_info * init_server(uint8_t ** gm_board){

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

    return server;
}

void destroy_server(struct server_info * server){

    if (server != NULL){

        destroy_semaphores(server);
        destroy_shd_memories(server->api_client, &server->api_conn);
        free(server);
    }
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

uint32_t reset_beasts_info(struct beasts_t * beasts){

    if (beasts == NULL){

        return 1;
    }

    beasts->beasts_pid = 0;
    beasts->client_number = 0;

    for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

        (beasts->beasts + i)->in_game = 0; // 0  means not in game
    }

    return 0;
}

uint32_t reset_api_client(struct api_t * api_client){

    if (api_client == NULL){

        return 1;
    }

    api_client->round_number = 0;
    api_client->server_pid = 0;
    api_client->deaths = 0;
    api_client->coins_carried = 0;
    api_client->coins_brought = 0;

    for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

        (api_client->client_data + i)->position.x = 0;
        (api_client->client_data + i)->position.y = 0;
        (api_client->client_data + i)->current_move = NO_MOVE;
    }

    strcpy(api_client->type, "---");
    api_client->player_number = 0;


    return 0;
}

void * handle_connections(void * svr){

    if (svr == NULL){   // for sure, too detailed?

        return NULL;
    }

    // think how to stop this thread properly. I don't have to stop this process.
    // It will be killed when I stop the main thread.

    struct server_info * server = (struct server_info *)svr;

    while(1){

        if (update_api_conn(server)){

            return NULL;
        }

        sem_post(server->sem_server);

        sem_wait(server->sem_client);

        accept_new_connection(server);  // what will happen if connecting fail
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

        server->api_conn.api->player_number = i + 1;
        server->players[i].player_number = i + 1; // mutex probably does not required
    }

    server->api_conn.api->client_pid = 0;

    return 0;
}

uint32_t accept_new_connection(struct server_info * server){

    // can add a possibility for many attempts like in the web requests

    if (server == NULL){

        return 1;
    }

    if (server->api_conn.api->client_pid == 0){

        return 3;
    }

    if (strcmp(server->api_conn.api->client_type, "HUMAN") == 0 ||
    strcmp(server->api_conn.api->client_type, "CPU") == 0){

        uint32_t i;

        for (i = 0; i < PLAYERS_NUM; ++i){

            if (server->players[i].player_number == server->api_conn.api->player_number){

                reset_player_info(server->players + i);
                prepare_new_player(server);
                update_api_client(server, i + 1);
                server->api_conn.api->player_number = 0;

                break;
            }
        }

        if (i == PLAYERS_NUM){

            return 4;
        }

    }
    else if (strcmp(server->api_conn.api->client_type, "BEAST") == 0){

        if (server->beasts.client_number > 0){

            return 5;
        }
        else{

            reset_beasts_info(&server->beasts);
            prepare_beast(server);
            update_api_client(server, PLAYERS_NUM + 1u);
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
    strcpy(server->players[server->api_conn.api->player_number - 1].client_type,
    server->api_conn.api->client_type);

    select_random_position(server->game_grid,
    &server->players[server->api_conn.api->player_number - 1].spawn_position);

    server->players[server->api_conn.api->player_number - 1].curr_position.x =
    server->players[server->api_conn.api->player_number - 1].spawn_position.x;
    server->players[server->api_conn.api->player_number - 1].curr_position.y =
    server->players[server->api_conn.api->player_number - 1].spawn_position.y;

    // where are if conditions in case of problems with these funcs?

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

// killing the beast has to be managed on the server side
uint32_t update_api_client(struct server_info * server, uint8_t client_num){

    if (server == NULL){

        return 1;
    }

    if (client_num > 0 && client_num <= PLAYERS_NUM){

        if (server->players[client_num - 1].player_number > 0){ // shouldn't cause a problem

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
            set_client_game_board(server->game_grid,
            server->api_client[client_num - 1].api->client_data->game_grid,
            &server->api_client[client_num - 1].api->client_data->position);
            strcpy(server->api_client[client_num - 1].api->type,
            server->players[client_num - 1].client_type);
            server->api_client[client_num - 1].api->player_number =
            server->players[client_num - 1].player_number;
        }
    }
    else if (client_num == PLAYERS_NUM + 1){

        if (server->beasts.client_number > 0){

            server->api_client[client_num - 1].api->round_number = server->round_number;
            server->api_client[client_num - 1].api->server_pid = server->server_pid;
            strcpy(server->api_client[client_num - 1].api->type, "BEAST");
            server->api_client[client_num - 1].api->player_number =
            server->beasts.client_number;

            for (int32_t i = 0; i < BEASTS_MAX_NUM; ++i){

                if ((server->beasts.beasts + i)->in_game){

                    server->api_client[client_num - 1].api->client_data[i].position.x =
                    server->beasts.beasts[i].position.x;
                    server->api_client[client_num - 1].api->client_data->position.y =
                    server->beasts.beasts[i].position.y;
                    server->api_client[client_num - 1].api->client_data[i].current_move = NO_MOVE;
                    set_client_game_board(server->game_grid,
                    server->api_client[client_num - 1].api->client_data[i].game_grid,
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

// what if set_client will go wrong? Here mutex can/should be used to be sure that
// the same value is set to api and passed as a func parameter
// clash with game cycle thread
// except you set a proper condition in that thread

    return 0;
}

uint32_t update_all_api_client(struct server_info * server){

    if (server == NULL){

        return 1;
    }

    for (uint8_t i = 1; i <= PLAYERS_NUM; ++i){

        if (update_api_client(server, i)){

            return 2;
        }
    }

    if (update_api_client(server, PLAYERS_NUM + 1)){

        return 2;
    }

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

    if (NULL == strchr("1234A*ctTD", (int32_t)figure)){ // casted because strchr takes int, cherry on top...

        return 3;
    }

    server->game_grid[position->y][position->x] = figure;

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

    for (int32_t i = position->y - 2; i <= position->y + 2; ++i){
        for (int32_t j = position->x - 2; j <= position->x + 2; ++j){

            if (i < 0 || i >= GAME_HEIGHT || j < 0 || j >= GAME_WIDTH){

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

// server_keybinding manages keybindings completely. Works on server struct.
// Uses mutexes when sth new is added to the game. When q/Q pressed thread terminates
// with keypressed = q set in server struct. Main thread checks if q is set. If so
// terminates the game.
// Maybe kill the game in this thread?
// Does not required, but intern semaphore should be posted() in main thread just after
// checking the condition so to thread has ASAP time to catch the key and perform operations

void * server_keybinding(void * svr){

/* can print info in console when sth went wrong with setting new figures */

    struct server_info * server = (struct server_info *)svr;

    while(1){

        //improve this, now it does not work
        server->key_pressed = getch();

        if (server->key_pressed == 'q' || server->key_pressed == 'Q'){

            return NULL;
        }
        else if (server->key_pressed == 'b' || server->key_pressed == 'B'){
//mutex in each elif
            add_new_beast(server);
        }
        else if (server->key_pressed == 'c'){

            add_new_coin(server);
        }
        else if (server->key_pressed == 't'){

            add_new_small_treasure(server);
        }
        else if (server->key_pressed == 'T'){

            add_new_big_treasure(server);
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
    set_new_character_game_board(server, &new_beast, "*");

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

    if (set_new_character_game_board(server, &new_coin, "c")){

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

    if (set_new_character_game_board(server, &new_sm_trea, "t")){

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

    if (set_new_character_game_board(server, &new_bg_trea, "T")){

        fetch_trea(&server->trea_list_head, new_bg_trea.x, new_bg_trea.y);
        return 4;
    }

    return 0;
}

uint32_t move_beast(struct server_info * server, uint8_t beast_num){

    if (server == NULL || server->game_grid == NULL || beast_num > BEASTS_MAX_NUM){

        return 1;
    }

    struct pos_t temp_pos;
    uint8_t tmp_num = 0;

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
    // better or not?
    // else if (server->game_grid[temp_pos.y][temp_pos.x] == 'c' 
    // || server->game_grid[temp_pos.y][temp_pos.x] == 't' 
    // || server->game_grid[temp_pos.y][temp_pos.x] == 'T'){

    //     fetch_trea(&server->trea_list_head, temp_pos.x, temp_pos.y);
    //     set_new_character_game_board(server, 
    //     &server->beasts.beasts[beast_num - 1].position, ' ');
    //     set_new_character_game_board(server, 
    //     &temp_pos, '*');
    //     server->beasts.beasts[beast_num - 1].position.x = temp_pos.x;
    //     server->beasts.beasts[beast_num - 1].position.y = temp_pos.y;
    // }
    // can rewrite using value - '0' - 1, but now code is clearer
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

uint32_t move_player(struct server_info * server, uint8_t player_num){

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
    else {

        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].into_bushes = 0; // consider this, works both ways

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

        add_trea(&server->trea_list_head, temp_pos.x, temp_pos.y,
        server->players[player_num - 1].coins_carried +
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].coins_carried);
        server->players[player_num - 1].coins_carried = 0;
        server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].coins_carried = 0;
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
        &server->players[server->game_grid[temp_pos.y][temp_pos.x] - '0' - 1].curr_position,
        player_num + '0');
        set_new_character_game_board(server, &temp_pos, 'D');
    }
    else if (server->game_grid[temp_pos.y][temp_pos.x] == '#'){ // else could be enough

        server->players[player_num - 1].prev_position.x =
        server->players[player_num - 1].curr_position.x;
        server->players[player_num - 1].prev_position.y =
        server->players[player_num - 1].curr_position.y;
        server->players[player_num - 1].into_bushes = 0;
    }

    return 0;
}