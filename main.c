#include "main.h"

int main(int argc, char *argv[])
{
    /* Check for minimum command-line arguments */
    if (argc < 3)
    {
        printf(RED "ERROR: Insufficient command-line arguments\n" RESET);
        printf("Usage:\n");
        printf("View  : ./a.out -v sample.mp3\n");
        printf("Edit  : ./a.out -e -t new-title sample.mp3\n");
        return FAILURE;
    }

    /* View operation */
    if (strcmp(argv[1], "-v") == 0)
    {
        /* Check for correct number of arguments */
        if (argc != 3)
        {
            printf(RED "ERROR: Invalid arguments for view operation\n" RESET);
            printf("Usage: ./a.out -v sample.mp3\n");
            return FAILURE;
        }

        /* Call view function */
        return read_mp3_file(argv[2]);
    }

    /* Edit operation */
    else if (strcmp(argv[1], "-e") == 0)
    {
        /* Check for correct number of arguments */
        if (argc != 5)
        {
            printf(RED "ERROR: Invalid arguments for edit operation\n" RESET);
            printf("Usage: ./a.out -e -t new-title sample.mp3\n");
            return FAILURE;
        }

        /* Check edit option */
        if (strcmp(argv[2], "-t") != 0 &&
        strcmp(argv[2], "-a") != 0 &&
        strcmp(argv[2], "-A") != 0 &&
        strcmp(argv[2], "-y") != 0 &&
        strcmp(argv[2], "-m") != 0 &&
        strcmp(argv[2], "-c") != 0 &&
        strcmp(argv[2], "-C") != 0)
        {
            printf(RED "ERROR: Invalid edit option\n" RESET);
            printf("Valid options: -t -a -A -y -m -c -C\n");
            return FAILURE;
        }

        /* Call edit function */
        return edit_mp3_file(argv[4], argv[2], argv[3]);
    }

    /* Invalid operation */
    else
    {
        printf(RED "ERROR: Invalid operation\n" RESET);
        printf("Use -v for viewing or -e for editing\n");
        return FAILURE;
    }
}