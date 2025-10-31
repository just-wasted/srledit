// sled, simple line editor

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define INPUT_BUFFER_SIZE 1024

typedef struct string_t {
	char str[INPUT_BUFFER_SIZE]; // string data
	unsigned int len; // the length of data including null terminator
	unsigned int cursor_pos; // index in str of cursor position
}string_t;

static const char *keywords[] = {"calc", "prog", "conv"};

string_t initialize_string();
int append_string(string_t *buffer, unsigned char in_char);

int main()
{
	struct termios stream_state_input;
	struct termios stram_state_input;

	string_t input_line = initialize_string(); //input buffer

	unsigned char in_char;
	int received_char;

	// get the terminal state of stdin
	tcgetattr(STDIN_FILENO, &stream_state_input);
	stram_state_input=stream_state_input;

	// disable canonical mode (buffered i/o) and local echo
	stram_state_input.c_lflag &= (~ICANON & ~ECHO);

	// apply the new settings
	tcsetattr(STDIN_FILENO, TCSANOW, &stram_state_input);

	do
	{
		received_char = getchar();
		if (received_char == EOF)
		{
			return EXIT_FAILURE;
		}
		in_char = (unsigned char) received_char;


		if (append_string(&input_line, in_char) == EXIT_SUCCESS)
		{
			fputc(in_char, stdout);
		} else {
			// do shit of buffer is full
		}

		// printf("%c", in_char);
	} while(in_char != '\n');

	// restore terminal state
	tcsetattr(STDIN_FILENO, TCSANOW, &stream_state_input);

	printf("%s\n", input_line.str);
	return EXIT_SUCCESS;
}

string_t initialize_string()
{
	string_t new;
	new.cursor_pos = 0;
	new.len = 1;
	new.str[0] = '\0';
	return new;
}

// receives buffer and char to append.
// returns 1 if buffer is full, apends null terminator
int append_string(string_t *buffer, unsigned char in_char)
{
	if (buffer->len >= INPUT_BUFFER_SIZE)
	{
		return EXIT_FAILURE;
	}
	buffer->str[buffer->cursor_pos] = in_char;
	buffer->str[buffer->cursor_pos + 1] = '\0';
	buffer->cursor_pos++;
	buffer->len++;
	return EXIT_SUCCESS;
}
