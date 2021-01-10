#include "multi_header.h"

//#include <curses.h> // -lncurses flag gcc, install package first


void display_ser(struct server_info* server);

int main()
{

int8_t game_grid[25][52] ={"###################################################",
                                "#   #       #   ~~~~~       #         #       #   #",
                                "# # ### ### ########### ### # ####### ### #####   #",
                                "# #   # # #           # # #   #     #     #   #   #",
                                "# ### # # ###~~~##### # # ##### ############# ### #",
                                "# # #   #           # # # ~~  #       #       # # #",
                                "# # ##### ### ####### # # # ### ### ### ### # # # #",
                                "# #         # #       # # #     #   #   # # #   # #",
                                "# # ####### ### ####### ##### ### ### ### # ### # #",
                                "# # #     #   #   #     #   #   #         # # # # #",
                                "# ### ### ### ### ### ### # ### ########### # # # #",
                                "# #   #       # #   #     # #   # #       # #   # #",
                                "# # ######~## # ### ### ### ### # # ##### # # ### #",
                                "# #    ~#   # #   #   #   #   #   # #     # # #   #",
                                "# # # ~~# ### ### ### ####### ### # ### ### # # ###",
                                "# # #~~ #    ~  #   # #  ~~~  #   #   #     # # # #",
                                "# # #~  ####### # # # # ##~#### ##### ####### # # #",
                                "# # #~      #   # # #   #     #   # #       ~~#   #",
                                "# ######### # ### ####### ####### # ##### # ~~### #",
                                "# #~      # #     #     #       #   #   # # ~~  # #",
                                "# # ##### # ####### # ### ##### ### # # ###~##### #",
                                "#~~~#     #         #  ~~~#~~ #     # #   #~~~~~~ #",
                                "# ### #####################~# ####### ### #~    ~ #",
                                "#   #                 ~~~~~~#~~         #    ~~   #",
                                "###################################################"};

    int8_t * game_rows[GAME_HEIGHT];

    for (int32_t i = 0; i < GAME_HEIGHT; ++i){

        game_rows[i] = game_grid[i];
    }

// try to set nodelay etc.
    initscr();

    keypad(stdscr, TRUE);
    noecho();

    struct pos_t t = {.x = 0, .y = 0};
    struct server_info server = {.round_number = 2, .server_pid = 1000, .camp_position.x = 3, .camp_position.y = 5, .game_grid = game_rows};

    int i = 0;
    int c = 10;
    while(1){

        if (i == 5){
            clear();
            printw("YABADABA\n");
            refresh();
        }
        else{

            display_ser(&server);
        }

        sleep(1);
        

        c = KEY_DOWN;

        if (c == KEY_DOWN){

            move(25, 0);
            printw("DOWN\n");
        }
    
        server.round_number *= 2;
        move(0, 0);
        i++;
    }

    return 0;
}


void display_ser(struct server_info* server){

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

        refresh();
    }
}
