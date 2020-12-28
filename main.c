/* JPEG Encoder Library
 * Copyright (c) 2006-2016 SIProp.org Noritsuna Imamura(noritsuna@siprop.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "jpegencoder.h"

#define DEBUG_ON_PC
#include "flash_emu.h"


typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

str2int_errno str2int(int *out, char *s, int base) {
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}


int main(int argc, char** argv) {
    
    if (argc < 7) {
        fprintf(stderr, "Error: Not enough arguments, got %d \n", argc);
        return -1;
    }

    char *fname_original = argv[1];
    char *fname_jpeg = argv[2];

    int width = 0;
    int height = 0;
    char *width_str = argv[3];
    char *height_str = argv[4];

    str2int(&width, width_str, 10);
    str2int(&height, height_str, 10);
    char *format = argv[5];
    char *quality = argv[6];

    open_files(fname_original, fname_jpeg);


    // Setup
    jpeg_data data = {0};

    if (width == 0 || height == 0) {
        fprintf(stderr, "Error: Invalid width '%s' or height '%s' specified \n", width_str, height_str);
        return -1;
    }

    data.width = width;
    data.height = height;
    if (strcmp(format, "y") == 0) {
        data.c_info = color_info_gray_scale;
    } else if (strcmp(format, "yuv411") == 0) {
        data.c_info = color_info_yuv411;
    } else {
        fprintf(stderr, "Error: Unknown format '%s' \n", format);
        return -1;
    }

    if (strcmp(quality, "normal") == 0) {
        data.quality = normal;
    } else if (strcmp(quality, "highest") == 0) {
        data.quality = highest;
    } else if (strcmp(quality, "higher") == 0) {
        data.quality = higher;
    } else {
        fprintf(stderr, "Error: Unknown format '%s' \n", format);
        return -1;
    }


    encode_image(&data, read_data, write_data);

    close_files();

    printf("Wrote JPEG to %s\n", fname_jpeg);

    return 0;
}

