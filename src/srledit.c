// srled, simplereadline editor

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define INPUT_BUFFER_SIZE 1024

// most terminal emulators map physical backspace key to ASCII Del
#define KEY_DELETE 127
#define KEY_BACKSPACE 8
#define KEY_INSERT

typedef struct string_t
{
    char str[INPUT_BUFFER_SIZE]; // string data
    unsigned int len; // the length of data including null terminator
    unsigned int cursor_pos; // index in str of cursor position
} string_t;

// static const char *keywords[] = {"calc", "prog", "conv"};

string_t initialize_string();
int insert_char(string_t *buffer, int in_char);
int remove_char(string_t *buffer);

int main()
{
    struct termios stream_state_default;
    struct termios stream_state_input;

    string_t input_line = initialize_string(); // input buffer

    int received_char;

    // get the terminal state of stdin
    tcgetattr(STDIN_FILENO, &stream_state_default);
    stream_state_input = stream_state_default;

    // disable canonical mode (buffered i/o) and local echo
    stream_state_input.c_lflag &= (~ICANON & ~ECHO);

    // apply the new settings
    tcsetattr(STDIN_FILENO, TCSANOW, &stream_state_input);

    printf(">>> ");

    do
    {
        received_char = getchar();
        if (received_char == EOF)
        {
            return EXIT_FAILURE;
        }

        if (received_char == KEY_DELETE)
        {
            if (remove_char(&input_line) == EXIT_FAILURE)
            {
                continue;
            }

            // NOTE: temporary echoing
            fputc(KEY_BACKSPACE, stdout);
            fputc(' ', stdout);
            fputc(KEY_BACKSPACE, stdout);

            continue;
        }

        // if the char is not printable, do nothing with it
        if (!isprint(received_char))
        {
            continue;
        }

        if (insert_char(&input_line, received_char) == EXIT_SUCCESS)
        {
            // TODO: parsing for keyword highlighting
            // NOTE: temporary echoing
            fputc(received_char, stdout);
        }
        else
        {
            // FIXME: do stuff if buffer is full
        }
    } while (received_char != '\n');

    // restore terminal state
    tcsetattr(STDIN_FILENO, TCSANOW, &stream_state_default);

    printf("%s\n", input_line.str);
    return EXIT_SUCCESS;
}

// TODO: use malloc
string_t initialize_string()
{
    string_t new;
    new.cursor_pos = 0;
    new.len = 1;
    new.str[0] = '\0';
    return new;
}

// receives buffer and char to append.
// returns 1 if buffer is full, apends null terminator.
int insert_char(string_t *buffer, int in_char)
{
    if (buffer->len >= INPUT_BUFFER_SIZE)
    {
        return EXIT_FAILURE; // do nothing if the buffer is full
    }
    buffer->str[buffer->cursor_pos + 1] = '\0';
    buffer->str[buffer->cursor_pos] = (char)in_char;
    buffer->cursor_pos++;
    buffer->len++;
    return EXIT_SUCCESS;

    // TODO: check if cursor is not at \0 and shift buffer contents accordingly
}

// receives a buffer and removes the char before current cursor position
int remove_char(string_t *buffer)
{
    if (buffer->len <= 1)
    {
        return EXIT_FAILURE; // do nothing of there is no string data to remove
    }

    // check if cursor is at the end of the string
    if (buffer->str[buffer->cursor_pos] == '\0')
    {
        buffer->str[buffer->cursor_pos - 1] = '\0';
        buffer->str[buffer->cursor_pos] = ' ';
        buffer->cursor_pos--;
        buffer->len--;
    }
    // TODO: check if cursor is not at \0 and shift buffer contents accordingly
    return EXIT_SUCCESS;
}
