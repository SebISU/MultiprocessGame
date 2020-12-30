#ifndef MULTI_HEADER_H
#define MULTI_HEADER_H
#define SHD_MEM_1 "/shd_mem1"
#define SHD_MEM_2 "/shd_mem2"
#define SHD_MEM_3 "/shd_mem3"
#define SHD_MEM_4 "/shd_mem4"
#define SHD_MEM_CONN "/shd_mem_conn"
#define SEM_1 "sem_1"
#define SEM_2 "sem_2"
#define SEM_3 "sem_3"
#define SEM_4 "sem_4"
#define SEM_CONN_SERVER "server_conn_players"
#define SEM_CONN_CLIENT "client_conn_players"
#define PAGE_SIZE 4096
#define PLAYERS_NUM 4
#define GAME_WIDTH 52
#define GAME_HEIGHT 25

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
//#include <sys/ipc.h> tutorial yt older sysV API
//#include <sys/shm.h>
#include <sys/mman.h>
//#include <sys/stat.h> // not sure if it is needed
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>

    enum move_t {DOWN, UP, LEFT, RIGHT, QUIT, NO_MOVE};

    struct pos_t {

        uint8_t x;
        uint8_t y;
    };

    struct trea_data {  // I can use it to keep spawn and treasure sum info, dropped treasure, (treasure on map can be done on fly)

        uint32_t value;
        struct pos_t position;
    };

    struct trea_node {

        struct trea_node * next;
        struct trea_data treasure_info;
    };

    struct api_t {

        uint64_t round_number;
        pid_t server_pid;
        uint32_t deaths;
        uint32_t coins_carried;
        uint32_t coins_brought;
        sem_t semaphore;
        struct pos_t position;
        enum move_t current_move;
        uint8_t game_grid[5][5];
        uint8_t type[6];
        uint8_t player_number;
    };  // attribute packed? values will often change so probably it is a bad idea

    struct api_wrap_t {

        struct api_t * api;
        int32_t api_id;
    };

    struct api_shd_conn {

        pid_t client_pid;
        sem_t * sem_server;
        sem_t * sem_client;
        uint8_t client_type[6];
        uint8_t player_number;
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
        uint8_t client_type[6];
        struct pos_t curr_position;
        struct pos_t prev_position;
        struct pos_t spawn_position;
        uint8_t into_bushes;
        uint8_t player_number;
    };

    struct server_info {

        uint64_t round_number; //when and how to finish a game? Another client info type
        struct trea_node * trea_list_head;
        pid_t server_pid;
        struct pos_t camp_position;
        struct api_wrap_conn api_conn;
        struct api_wrap_t api_client[4];
        uint8_t ** game_grid;
        struct client_info players[PLAYERS_NUM];
    };

    uint32_t add_trea(struct trea_node ** head, uint8_t x, uint8_t y , uint32_t value);
    uint32_t fetch_trea(struct trea_node ** head, uint8_t x, uint8_t y);
    void destroy_trea(struct trea_node ** head);
    
    void display_client(struct api_t * client);
    void display_server(struct server_info* server);
    
    uint32_t init_shd_memories(struct api_wrap_t * api_client, struct api_wrap_conn * api_conn);
    void destroy_shd_memories(struct api_wrap_t * api_client, struct api_wrap_conn * api_conn);
    struct server_info * init_server_info(uint8_t ** gm_board);
    
    uint32_t select_random_position(uint8_t **gm_board, struct pos_t * position);
    uint32_t reset_player_info(struct client_info * player);

    void * handle_connections(void * server);
    uint32_t update_api_conn(struct server_info * api);

#endif