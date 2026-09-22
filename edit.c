#include "main.h"

/* Get the corresponding ID3 frame ID */
char *get_frame_id(char *option)
{
    if (strcmp(option, "-t") == 0)
    {
        return "TIT2";
    }
    else if (strcmp(option, "-a") == 0)
    {
        return "TPE1";
    }
    else if (strcmp(option, "-A") == 0)
    {
        return "TALB";
    }
    else if (strcmp(option, "-y") == 0)
    {
        return "TYER";
    }
    else if (strcmp(option, "-m") == 0)
    {
        return "TCON";
    }
    else if (strcmp(option, "-c") == 0)
    {
        return "COMM";
    }
    else if (strcmp(option, "-C") == 0)
    {
        return "TCOM";
    }

    return NULL;
}

Status edit_mp3_file(char *filename,
                     char *option,
                     char *new_value)
{
    FILE *fp;

    char *target_frame;

    /* Get frame ID */
    target_frame = get_frame_id(option);

    if (target_frame == NULL)
    {
        printf(RED "ERROR: Invalid edit option\n" RESET);
        return FAILURE;
    }

    /* Open MP3 file in read/write binary mode */
    fp = fopen(filename, "rb+");

    if (fp == NULL)
    {
        printf(RED "ERROR: Unable to open the file\n" RESET);
        return FAILURE;
    }

    printf(GREEN "File opened successfully\n" RESET);

    /* Check ID3 tag */
    char tag[4];

    if (fread(tag, 1, 3, fp) != 3)
    {
        printf(RED "ERROR: Unable to read ID3 tag\n" RESET);
        fclose(fp);
        return FAILURE;
    }

    tag[3] = '\0';

    if (strcmp(tag, "ID3") != 0)
    {
        printf(RED "ERROR: ID3 tag not found\n" RESET);
        fclose(fp);
        return FAILURE;
    }

    /* Check ID3 version */
    unsigned char version[2];

    if (fread(version, 1, 2, fp) != 2)
    {
        printf(RED "ERROR: Unable to read ID3 version\n" RESET);
        fclose(fp);
        return FAILURE;
    }

    if (version[0] != 0x03 || version[1] != 0x00)
    {
        printf(RED "ERROR: Unsupported ID3 version\n" RESET);
        fclose(fp);
        return FAILURE;
    }

    printf(GREEN "ID3v2.3 version confirmed\n" RESET);

    /*
     * Skip:
     * 1 byte -> Flags
     * 4 bytes -> Tag size
     */
    fseek(fp, 5, SEEK_CUR);

    /* Search for required frame */
    while (1)
    {
        char frame_id[5];

        if (fread(frame_id, 1, 4, fp) != 4)
        {
            break;
        }

        frame_id[4] = '\0';

        /* Empty frame means end of ID3 frames */
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

        /* Check frame size */
        if (frame_size == 0 || frame_size > 100000)
        {
            break;
        }

        /* Allocate memory for frame data */
        unsigned char *frame_data =
            malloc(frame_size);

        if (frame_data == NULL)
        {
            printf(RED "ERROR: Memory allocation failed\n" RESET);
            fclose(fp);
            return FAILURE;
        }

        /* Read frame data */
        if (fread(frame_data, 1, frame_size, fp) != frame_size)
        {
            free(frame_data);
            break;
        }

        /*
         * Check whether this is the required frame.
         *
         * For year, our MP3 uses TDRC instead of TYER.
         */
        int frame_found = 0;

        if (strcmp(option, "-y") == 0)
        {
            if (strcmp(frame_id, "TYER") == 0 ||
                strcmp(frame_id, "TDRC") == 0)
            {
                frame_found = 1;
            }
        }
        else
        {
            if (strcmp(frame_id, target_frame) == 0)
            {
                frame_found = 1;
            }
        }

        if (frame_found)
        {
            /*
             * Text frames:
             * First byte = text encoding
             * Remaining bytes = actual text
             */
            if (strcmp(frame_id, "COMM") != 0)
            {
                unsigned int new_length =
                    strlen(new_value) + 1;

                
                if (new_length > frame_size)
                {
                    printf(RED
                           "ERROR: New value is too long for this frame\n"
                           RESET);

                    printf("Maximum text length: %u characters\n",
                           frame_size - 1);

                    free(frame_data);
                    fclose(fp);
                    return FAILURE;
                }

                /*
                 * Keep ISO-8859-1 encoding.
                 */
                frame_data[0] = 0x00;

                /* Clear old text */
                memset(frame_data + 1,
                       0,
                       frame_size - 1);

                /* Copy new text */
                strcpy((char *)(frame_data + 1),
                       new_value);

                /*
                 * Move back to beginning of frame data.
                 */
                fseek(fp, -(long)frame_size, SEEK_CUR);

                /* Write modified frame */
                fwrite(frame_data,
                       1,
                       frame_size,
                       fp);

                printf(GREEN
                       "Tag updated successfully\n"
                       RESET);

                free(frame_data);
                fclose(fp);

                return SUCCESS;
            }

            /*
             * COMMENT frame
             *
             * ID3v2.3 COMM structure:
             *
             * 1 byte  -> Encoding
             * 3 bytes -> Language
             * N bytes -> Description
             * 1 byte  -> NULL separator
             */
            else
            {
                unsigned int comment_start = 4;

                /*
                 * Current file uses encoding 00
                 * (ISO-8859-1).
                 */
                if (frame_data[0] != 0x00)
                {
                    printf(RED
                           "ERROR: Only ISO-8859-1 comment "
                           "editing is supported\n"
                           RESET);

                    free(frame_data);
                    fclose(fp);
                    return FAILURE;
                }

                /* Find description terminator */
                while (comment_start < frame_size &&
                       frame_data[comment_start] != '\0')
                {
                    comment_start++;
                }

                if (comment_start >= frame_size)
                {
                    printf(RED
                           "ERROR: Invalid comment frame\n"
                           RESET);

                    free(frame_data);
                    fclose(fp);
                    return FAILURE;
                }

                /* Move past NULL separator */
                comment_start++;

                unsigned int available =
                    frame_size - comment_start;

                if (strlen(new_value) >= available)
                {
                    printf(RED
                           "ERROR: New comment is too long\n"
                           RESET);

                    free(frame_data);
                    fclose(fp);
                    return FAILURE;
                }

                /* Clear old comment */
                memset(frame_data + comment_start,
                       0,
                       available);

                /* Copy new comment */
                strcpy((char *)(frame_data + comment_start),
                       new_value);

                /* Move back to frame data */
                fseek(fp, -(long)frame_size, SEEK_CUR);

                /* Write modified comment */
                fwrite(frame_data,
                       1,
                       frame_size,
                       fp);

                printf(GREEN
                       "Comment updated successfully\n"
                       RESET);

                free(frame_data);
                fclose(fp);

                return SUCCESS;
            }
        }

        free(frame_data);
    }

    printf(RED "ERROR: Required tag frame not found\n" RESET);

    fclose(fp);

    return FAILURE;
}