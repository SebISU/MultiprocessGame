#include "multi_header.h"

#include <curses.h> // -lncurses flag gcc, install package first

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

// try to set nodelay etc.
    initscr();

    keypad(stdscr, TRUE);
    noecho();

    struct pos_t t = {.x = 0, .y = 0};
    int c = 10;
    while(1){

        sleep(1);
        system("clear");
        //printf("YEAH\n");
        c = getch();

        game_grid[t.y][t.x] = '*';

        sleep(5);
        printf(" JEAA %c\n", game_grid[t.y][t.x]);
        sleep(5);

        printf("%c\n", c);

        if (c == KEY_DOWN){

            for (int x = 0; x < 25; x++){

                printf("%s\n", game_grid + x);
            }
        }
    
        if (c == KEY_DOWN){

            printf("TURURUR\n");
        }

        //printf("%d\n", c);
        if ( c == '\033') { // if the first value is esc
            c = getch(); // skip the [
            printf("%d\n", c);
            switch(getch()) { // the real value
                case 'A':
                    printf("JABA");
                    break;
                case 'B':
                    printf("JABAB");
                    break;
                case 'C':
                    printf("JABAC");
                    break;
                case 'D':
                    printf("JABAD");
                    break;
            }
    }
    }

    return 0;
}