#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

char *program_name = "main.alang";

char *file;
int program_size = 0;
int program_pointer = 0;
int creating_variable = 0;

char *chars_allocation;
int chars_lenght;

typedef struct Char_struct_save
{
    int n_a;
    int allocation_pos;
} Char_struct_save;

Char_struct_save *char_struct_allocation;

void ERROR_CRASH(char *msg)
{
    printf("\n");
    printf("\033[1;31m");
    printf("ERROR:\n");
    printf(msg);
    printf("\n");
    printf("\033[0;37m");
    exit(0);
}

int convert_string_hex_to_integer(char *str, int len)
{
    int res = 0;
    int str_len = len;

    for (int i = 0; i < str_len; i++)
    {
        if (str[str_len - 1 - i] >= 0x41 && str[str_len - 1 - i] <= 0x46)
        {
            int n = (char)str[str_len - 1 - i] - 55;

            if (i != 0)
            {
                for (int k = 0; k < i; k++)
                {
                    n = n * 16;
                }
            }

            res += n;
        }
        else if (str[str_len - 1 - i] >= 0x30 && str[str_len - 1 - i] <= 0x39)
        {
            int n = (char)str[str_len - 1 - i] - 48;
            if (i != 0)
            {
                for (int k = 0; k < i; k++)
                {
                    n = n * 16;
                }
            }
            res += n;
        }
    }
    return res;
}

int count_number_of_A(int name_pointer)
{
    int res = 0;
    while (file[name_pointer + res] == (char)0x41)
    {
        res++;
    }
    return res;
}

int count_number_of_chars(int name_pointer)
{
    int i = 0;
    char cur_char = file[name_pointer + 2];
    while (cur_char != 0x0A)
    {
        cur_char = file[name_pointer + 2 + i];
        i++;
    }
    return i / 3;
}

void create_and_save_chars_structures(int n_a, int n_of_chars)
{
    Char_struct_save char_struct;
    char_struct.n_a = n_a;
    char_struct.allocation_pos = chars_lenght - n_of_chars; // seta o primeiro byte da string como o ultima antigo

    printf("Number of A's = %d\n", char_struct.n_a);

    printf("aaa %d\n", char_struct.allocation_pos);

    printf("\nSTRING on byte %d = %s\n", char_struct.allocation_pos, chars_allocation + char_struct.allocation_pos);

    char_struct_allocation = realloc(char_struct_allocation, (n_a * sizeof(char_struct_allocation)));
}

void check_char()
{
    char a = 0x00;
    memcpy(&a, file, 1);
    if ((file[program_pointer - 1] == (char)0x20 && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x20) ||
        (program_pointer == 0 && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x20) ||
        (file[program_pointer - 1] == (char)0x0A && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x20))
    {
        printf("\nCreating char on byte %d\n", program_pointer);
        creating_variable = 1;
    }
    else
    {
        // printf("\n%d not a char\n", program_pointer);
        return;
    }

    // check the number of A(name)
    int name_pointer = program_pointer + 2;
    if (file[name_pointer] != (char)0x41)
    {
        ERROR_CRASH("char should have a name of 'A'");
    }

    // GOT A STRING
    int n_a = count_number_of_A(name_pointer); // number of A's variable
    name_pointer = name_pointer + (n_a - 1);

    int n_of_chars = count_number_of_chars(name_pointer);
    n_of_chars += 1; // last byte of a string
    printf("Number of chars is : %d\n", n_of_chars);

    chars_lenght += n_of_chars;
    chars_allocation = realloc(chars_allocation, chars_lenght); // allocate more memory
    chars_allocation[chars_lenght - 1] = 0x00;

    char *hex_number = malloc(3);
    hex_number[2] = 0x00;

    char cur_char = 0;
    int cur_char_n = 0;
    while (cur_char != 0x0A)
    {
        hex_number[0] = file[name_pointer + 2 + cur_char_n];
        hex_number[1] = file[name_pointer + 3 + cur_char_n];
        cur_char = file[name_pointer + 4 + cur_char_n];

        printf("%s ", hex_number);
        char c = (char)convert_string_hex_to_integer(hex_number, 2);
        memcpy((chars_allocation + chars_lenght - n_of_chars) + (cur_char_n / 3), &c, 1);

        cur_char_n += 3;
    }

    // é o name_pointer + 4 + cur_char_n... so q menos 3
    program_pointer = name_pointer + 1 + cur_char_n; // set program_pointer to last byte on line... to sart reading next byte
    printf("new ptr = %d\n", name_pointer + 1 + cur_char_n);

    creating_variable = 0;

    free(hex_number);

    create_and_save_chars_structures(n_a, n_of_chars);
}

void execute()
{

    for (program_pointer = 0; program_pointer < program_size; program_pointer++)
    {
        if (creating_variable == 0)
        {
            check_char();
            // check_int();
        }
    }
}

void open_file_and_store()
{
    int fd = open(program_name, O_RDONLY);

    struct stat st;
    stat(program_name, &st); // get file lenght
    program_size = st.st_size;

    file = (char *)malloc(program_size);

    read(fd, file, program_size);

    close(fd);
}

int main()
{

    open_file_and_store();
    printf("%s\n", file);
    printf("%d\n", program_size);
    printf("Executing\n");
    execute();
    printf("\n");

    free(file);
    return 0;
}