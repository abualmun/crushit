#include "crushit.h"
#include "mydefs.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Helper function to check if a character is valid
bool is_valid_char(char c) {
    return (c >= 'A' && c <= 'Z');
}

void test() {
    // You can add some internal testing here if needed
    printf("Test function called.\n");
}

bool initialise(state* s, const char* str) {
    if (s == NULL || str == NULL) {
        return false;
    }

    // Attempt to open the file
    FILE* file = fopen(str, "r");
    if (file) {
        char* buffer = NULL;
        size_t buffer_size = 0;
        size_t total_length = 0;

        // Read each line and concatenate into buffer
        while (true) {
            char line[WIDTH + 2]; // +1 for newline, +1 for null terminator
            if (fgets(line, sizeof(line), file) == NULL) {
                break; // End of file
            }
            size_t line_length = strlen(line);
            if (line_length > 0 && line[line_length - 1] == '\n') {
                line[line_length - 1] = '\0'; // Remove newline
                line_length--;
            }
            total_length += line_length;

            // Resize buffer to hold the new line
            char* new_buffer = realloc(buffer, total_length + 1);
            if (new_buffer == NULL) {
                free(buffer);
                fclose(file);
                return false; // Memory allocation failed
            }
            buffer = new_buffer;

            // Copy the line to the buffer
            strcpy(buffer + (total_length - line_length), line);
        }
        fclose(file);

        // Fill the state structure
        for (int i = 0; i < total_length / WIDTH; i++) {
            for (int j = 0; j < WIDTH; j++) {
                s->board[i][j] = buffer[i * WIDTH + j];
            }
        }
        // Fill remaining rows with empty tiles
        for (int i = total_length / WIDTH; i < MAXROWS; i++) {
            memset(s->board[i], '.', WIDTH);
        }

        free(buffer);
        return true;
    } else {
        // Handle the string input directly
        int length = strlen(str);
        if (length % WIDTH != 0 || length / WIDTH > MAXROWS) {
            return false; // Malformed string
        }

        for (int i = 0; i < length / WIDTH; i++) {
            for (int j = 0; j < WIDTH; j++) {
                char tile = str[i * WIDTH + j];
                // Validate tile character
                if (tile < 'A' || tile > 'Z') {
                    return false; // Invalid character
                }
                s->board[i][j] = tile;
            }
        }
        // Fill remaining rows with empty tiles
        for (int i = length / WIDTH; i < MAXROWS; i++) {
            memset(s->board[i], '.', WIDTH);
        }
        return true;
    }
}
bool tostring(const state* s, char* str) {
    if (s == NULL || str == NULL) {
        return false;
    }

    int start_row = -1;
    int output_index = 0;

    // Find the first non-empty row
    for (int i = 0; i < MAXROWS; i++) {
        bool is_empty = true;
        for (int j = 0; j < WIDTH; j++) {
            if (s->board[i][j] != '.') {
                is_empty = false;
                break;
            }
        }
        if (!is_empty) {
            start_row = i;
            break;
        }
    }

    // If all rows are empty, return an empty string
    if (start_row == -1) {
        str[0] = '\0';
        return true;
    }

    // Collect rows from the starting point
    for (int i = start_row; i < MAXROWS; i++) {
        bool is_empty = true;
        for (int j = 0; j < WIDTH; j++) {
            if (s->board[i][j] != '.') {
                is_empty = false;
                break;
            }
        }
        
        // If the row is not empty, copy it to the output string
        if (!is_empty) {
            for (int j = 0; j < WIDTH; j++) {
                str[output_index++] = s->board[i][j];
            }
        }
    }

    // Null-terminate the output string
    str[output_index] = '\0';
    // printString(str);
    return true;
}
#include "crushit.h"
#include "mydefs.h"

bool matches(state* s) {
    if (s == NULL) {
        return false;
    }

    bool found_match = false;
    bool marked[MAXROWS][WIDTH] = { false }; // Track tiles to be removed

    // Identify the last 6 non-empty rows
    int non_empty_rows[MAXROWS] = {0}; // Keep track of non-empty rows
    int count = 0;

    // Check from the bottom up to find the last 6 non-empty rows
    for (int i = MAXROWS - 1; i >= 0; i--) {
        bool has_tile = false;
        for (int j = 0; j < WIDTH; j++) {
            if (s->board[i][j] != '.') {
                has_tile = true;
                break;
            }
        }
        if (has_tile) {
            non_empty_rows[count++] = i; // Store the index of the row
        }
    }

    // If less than 6 rows are found, adjust the count
    // count = count > 6 ? 6 : count;
    int dif = 0;
    if(count > 6){
        dif = count - 6;
    }
 // Check vertical matches
        for (int j = 0; j < WIDTH; j++) {
            for (int l = dif; l < MAXROWS - 2; l++) {
                if (s->board[l][j] != '.' && 
                    s->board[l][j] == s->board[l + 1][j] && 
                    s->board[l][j] == s->board[l + 2][j]) {
                    found_match = true;
                    marked[l][j] = true;
                    marked[l + 1][j] = true;
                    marked[l + 2][j] = true;

                    // Check for longer matches
                    for (int m = l + 3; m < MAXROWS && s->board[l][j] == s->board[m][j]; m++) {
                        marked[m][j] = true;}
                    
                }
            }
        }


    // Process only the identified rows for matches
    for (int k = dif; k < MAXROWS; k++) {
        int i = k;
        
        
        // Check horizontal matches
        // for (int n = dif; n < 6 + dif; n++) {
        for (int j = 0; j < WIDTH - 2; j++) {
            if (s->board[i][j] != '.' && 
                s->board[i][j] == s->board[i][j + 1] && 
                s->board[i][j] == s->board[i][j + 2]) {
                found_match = true;
                marked[i][j] = true;
                marked[i][j + 1] = true;
                marked[i][j + 2] = true;

                // Check for longer matches
                for (int m = j + 3; m < WIDTH && s->board[i][j] == s->board[i][m]; m++) {
                    marked[i][m] = true;
                }
            }
        }
        }


    // Replace marked tiles with '.'
    for (int i = 0; i < MAXROWS; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (marked[i][j]) {
                s->board[i][j] = '.';
            }
        }
    }

    return found_match;
}


// Drop blocks into empty spaces

bool dropblocks(state* s) {
    if (s == NULL) {
        return false;
    }

    // Iterate through each column
    for (int j = 0; j < WIDTH; j++) {
        int write_row = MAXROWS - 1; // Start writing from the bottom row

        // Iterate from the bottom of the board up to the top
        for (int i = MAXROWS - 1; i >= 0; i--) {
            if (s->board[i][j] != '.') {
                // Move the current block to the write position
                s->board[write_row][j] = s->board[i][j];
                write_row--;
            }
        }

        // Fill the remaining rows above with empty tiles
        for (int i = write_row; i >= 0; i--) {
            s->board[i][j] = '.';
        }
    }

    return true;
}

void printString(const char* str) {
    if (str != NULL) {
        printf("%s\n", str); // Print the string followed by a newline
    } else {
        printf("The string is NULL.\n");
    }
}
