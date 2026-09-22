#include "main.h"

Status read_mp3_file(char *filename)
{
    FILE *fp;

    char title[100] = "";
    char artist[100] = "";
    char album[100] = "";
    char year[20] = "";
    char genre[100] = "";
    char composer[100] = "";

    /* Open MP3 file */
    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf(RED "ERROR: Unable to open the file\n" RESET);
        return FAILURE;
    }

    printf(GREEN "File opened successfully\n" RESET);

    /* Check ID3 tag */
    char tag[4];

    fread(tag, 1, 3, fp);
    tag[3] = '\0';

    if (strcmp(tag, "ID3") != 0)
    {
        printf(RED "ERROR: ID3 tag not found\n" RESET);
        fclose(fp);
        return FAILURE;
    }

    printf(GREEN "ID3 tag found\n" RESET);

    /* Check ID3 version */
    unsigned char version[2];

    fread(version, 1, 2, fp);

    if (version[0] != 0x03 || version[1] != 0x00)
    {
        printf(RED "ERROR: Unsupported ID3 version\n" RESET);
        fclose(fp);
        return FAILURE;
    }

    printf(GREEN "ID3v2.3 version confirmed\n" RESET);

    
    fseek(fp, 5, SEEK_CUR);

    /* Read all ID3 frames */
    while (1)
    {
        char frame_id[5];

        /* Read frame ID */
        if (fread(frame_id, 1, 4, fp) != 4)
        {
            break;
        }

        frame_id[4] = '\0';

        /* Stop at empty frame */
        if (frame_id[0] == '\0')
        {
            break;
        }

        /* Read frame size */
        unsigned char size_bytes[4];

        if (fread(size_bytes, 1, 4, fp) != 4)
        {
            break;
        }

        unsigned int frame_size =
            ((unsigned int)size_bytes[0] << 24) |
            ((unsigned int)size_bytes[1] << 16) |
            ((unsigned int)size_bytes[2] << 8) |
            (unsigned int)size_bytes[3];

        /* Read frame flags */
        unsigned char flags[2];

        if (fread(flags, 1, 2, fp) != 2)
        {
            break;
        }

        /* Avoid invalid frame sizes */
        if (frame_size == 0 || frame_size > 10000)
        {
            break;
        }

        /* Allocate memory for frame data */
        char *frame_data = malloc(frame_size + 1);

        if (frame_data == NULL)
        {
            printf(RED "Memory allocation failed\n" RESET);
            fclose(fp);
            return FAILURE;
        }

        /* Read frame data */
        if (fread(frame_data, 1, frame_size, fp) != frame_size)
        {
            free(frame_data);
            break;
        }

        frame_data[frame_size] = '\0';


        if (strcmp(frame_id, "TIT2") == 0)
        {
            strcpy(title, frame_data + 1);
        }
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            strcpy(artist, frame_data + 1);
        }
        else if (strcmp(frame_id, "TALB") == 0)
        {
            strcpy(album, frame_data + 1);
        }
        else if (strcmp(frame_id, "TYER") == 0)
        {
            strcpy(year, frame_data + 1);
        }
        else if (strcmp(frame_id, "TDRC") == 0)
        {
            strcpy(year, frame_data + 1);
        }
        else if (strcmp(frame_id, "TCON") == 0)
        {
            strcpy(genre, frame_data + 1);
        }
        else if (strcmp(frame_id, "TCOM") == 0)
        {
            strcpy(composer, frame_data + 1);
        }

        free(frame_data);
    }

    /* Display all extracted information */
    display_tag_data(title,
                     artist,
                     album,
                     year,
                     genre,
                     composer);

    fclose(fp);

    return SUCCESS;
}