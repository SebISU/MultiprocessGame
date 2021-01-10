#ifndef MULTI_HEADER_H
#define MULTI_HEADER_H
#define SHD_MEM_1 "/shd_mem1"
#define SHD_MEM_2 "/shd_mem2"
#define SHD_MEM_3 "/shd_mem3"
#define SHD_MEM_4 "/shd_mem4"
#define SHD_MEM_5 "/shd_mem5"
#define SHD_MEM_CONN "/shd_mem_conn"
#define SEM_1 "/sem_1"
#define SEM_2 "/sem_2"
#define SEM_3 "/sem_3"
#define SEM_4 "/sem_4"
#define SEM_5 "/sem_5"
#define SEM_CONN_SERVER "/server_conn_players"
#define SEM_CONN_CLIENT "/client_conn_players"
#define PAGE_SIZE 4096
#define PLAYERS_NUM 4
#define GAME_WIDTH 52
#define GAME_HEIGHT 25
#define CLIENT_POS_X 2
#define CLIENT_POS_Y 2
#define BEASTS_MAX_NUM 10
#define BEAST_NUM_ROUNDS 1000

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
//#include <sys/ipc.h> tutorial yt older sysV API
#include <sys/shm.h>
#include <sys/mman.h>
//#include <sys/stat.h> // not sure if it is needed
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <ncurses.h>

    enum move_t {DOWN, UP, LEFT, RIGHT, NO_MOVE, QUIT};

    struct pos_t {

        int8_t x;
        int8_t y;
    };

    struct trea_data {  // I can use it to keep spawn and treasure sum info, dropped treasure, (treasure on map can be done on fly)

        uint32_t value;
        struct pos_t position;
    };

    struct trea_node {

        struct trea_node * next;
        struct trea_data treasure_info;
    };

    struct client_handle {

        sem_t * sem_server;
        sem_t * sem_client;
        struct api_shd_conn * api_conn;
        int32_t api_conn_id;
    };

    struct client_mv_handle {

        struct api_t * api;
        sem_t * sem_move;
        sem_t intern_sem;
        pthread_t key_bindings;
        int32_t api_id;
        int32_t key_pressed;
        int8_t player_number;
    };

    struct api_inner_t {

        struct pos_t position;
        enum move_t current_move;
        int8_t game_grid[5][5];

    };

    struct api_t {  // common api

        uint64_t round_number;
        pid_t server_pid;
        uint32_t deaths;
        uint32_t coins_carried;
        uint32_t coins_brought;
        //sem_t semaphore; // why here?
        struct api_inner_t client_data[BEASTS_MAX_NUM];
        int8_t type[6];
        int8_t player_number; // set to 0 each time on a client side so to know
                               // that client process is still running?
                               // if not set to player number on the server side after
                               // connection succeed means server wants to disconnect
                               // or killed
    };  // attribute packed? values will often change so probably it is a bad idea
// list of position,current move, game_grid to keep beasts, maybe a flag to know
// if beast is active, but position (0, 0) can be too
// max number of beasts == 10
// beast player number = 5


    struct api_wrap_t {

        struct api_t * api;
        int32_t api_id;
    };

    struct api_shd_conn {

        pid_t client_pid;
        int8_t client_type[6];
        int8_t player_number;
        int8_t beasts_in_game;
    };

    struct api_wrap_conn {

        struct api_shd_conn * api;
        int32_t api_id;
    };

    struct client_info {

        pid_t client_pid;
        uint32_t coins_carried;
        uint32_t coins_brought;
        uint32_t deaths;
        int8_t client_type[6];
        struct pos_t curr_position;
        struct pos_t prev_position;
        struct pos_t spawn_position;
        int8_t into_bushes;
        int8_t player_number;
    };

    struct beast_info {     // beast dies after x rounds

        struct pos_t position;
        uint64_t init_round_number;
        int8_t in_game;    // if currently running
    };

    struct beasts_t {

        pid_t beasts_pid;
        struct beast_info beasts[BEASTS_MAX_NUM];
        int8_t client_number;
    };

    struct server_info {

        pthread_mutex_t mutex;
        uint64_t round_number;
        struct trea_node * trea_list_head;
        pid_t server_pid;
        struct pos_t camp_position;
        struct api_wrap_conn api_conn;
        struct api_wrap_t api_client[PLAYERS_NUM + 1];
        int8_t ** game_grid;
        struct client_info players[PLAYERS_NUM];
        struct beasts_t beasts;
        int32_t key_pressed;
        sem_t sem_keybinding;
        sem_t * sem_server;
        sem_t * sem_client;
        sem_t * sem_client_1;
        sem_t * sem_client_2;
        sem_t * sem_client_3;
        sem_t * sem_client_4;
        sem_t * sem_client_5;

    };

    uint32_t add_trea(struct trea_node ** head, int8_t x, int8_t y , uint32_t value);
    uint32_t fetch_trea(struct trea_node ** head, int8_t x, int8_t y);
    void destroy_trea(struct trea_node ** head);
    
    void display_client(struct api_t * client);
    void display_server(struct server_info* server);
    
    uint32_t init_shd_memories(struct api_wrap_t * api_client, struct api_wrap_conn * api_conn); // server could be as a parameter
    void destroy_shd_memories(struct api_wrap_t * api_client, struct api_wrap_conn * api_conn); // -||-
    
    uint32_t init_semaphores(struct server_info * server);
    void destroy_semaphores(struct server_info * server); 
    
    struct server_info * init_server(int8_t ** gm_board);
    void destroy_server(struct server_info * server);

    void * server_keybinding(void * svr);
    uint32_t add_new_beast(struct server_info * server);
    uint32_t add_new_coin(struct server_info * server);
    uint32_t add_new_small_treasure(struct server_info * server);
    uint32_t add_new_big_treasure(struct server_info * server);

    uint32_t move_beast(struct server_info * server, int8_t beast_num);
    uint32_t move_player(struct server_info * server, int8_t player_num);
    
    uint32_t select_random_position(int8_t **gm_board, struct pos_t * position);
    uint32_t set_new_character_game_board(struct server_info * server, const struct pos_t * position, int8_t figure);
    uint32_t reset_player_info(struct client_info * player);
    uint32_t reset_beasts_info(struct beasts_t * beasts);
    uint32_t reset_api_client(struct api_t * api_client);
    uint32_t prepare_new_player(struct server_info * server);
    uint32_t prepare_beast(struct server_info * server);
    uint32_t update_api_client(struct server_info * server, int8_t client_num);
    uint32_t update_all_api_client(struct server_info * server);
    uint32_t set_client_game_board(struct server_info * server, struct api_inner_t * api_client, const struct pos_t * position);


    void * handle_connections(void * server);
    uint32_t update_api_conn(struct server_info * server);
    uint32_t reset_api_conn(struct server_info * server);
    uint32_t accept_new_connection(struct server_info * server);




    /*##########        Client funcs      ###########*/

    struct client_handle * init_handle();
    void destroy_handle(struct client_handle * handle);

    struct client_mv_handle * init_mv_handle(const int8_t player_number);
    void destroy_mv_handle(struct client_mv_handle * mv_handle);

    void * client_keybinding(void * handle);


    /*##########          Bot funcs       ###########*/

    void * bot_keybinding(void * handle);
    uint32_t get_closest_beast(const struct api_inner_t * player, struct pos_t * cl_beast);
    bool can_chase(const struct api_inner_t * client, const struct pos_t * pos_enemy);
    uint32_t shortest_maze_distance(struct pos_t * cl_enemy, struct pos_t * pos_enemy);
    enum move_t run_away_next_move(const struct api_inner_t * player, const struct pos_t * beast);

    /*##########         Beast funcs      ###########*/

    struct client_mv_handle * init_mv_beast_handle();
    void destroy_mv_beast_handle(struct client_mv_handle * mv_handle);
    uint32_t get_closest_player(const struct api_inner_t * beast, struct pos_t * cl_player);
    enum move_t follow_next_move(const struct api_inner_t * beast, const struct pos_t * player);
    void * beast_next_move(void * beast);

#endif