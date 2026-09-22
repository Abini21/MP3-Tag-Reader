#include "main.h"

void display_tag_data(char *title,
                      char *artist,
                      char *album,
                      char *year,
                      char *genre,
                      char *composer)
{
    printf("\n");

    printf(CYAN "============================================================\n" RESET);
    printf(CYAN "                      MP3 TAG READER\n" RESET);
    printf(CYAN "============================================================\n" RESET);

    printf("\n");

    printf(WHITE "Title        : " RESET "%s\n", title);
    printf(WHITE "Artist       : " RESET "%s\n", artist);
    printf(WHITE "Album        : " RESET "%s\n", album);
    printf(WHITE "Year         : " RESET "%s\n", year);
    printf(WHITE "Genre        : " RESET "%s\n", genre);
    printf(WHITE "Composer     : " RESET "%s\n", composer);

    printf("\n");

    printf(GREEN "              MP3 TAG READER - SUCCESS\n" RESET);

    printf(CYAN "============================================================\n" RESET);

    printf("\n");
}