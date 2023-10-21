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
int chars_lenght = 0;
int last_number_of_A = 0;

typedef struct Char_struct_save
{
    int n_a;
    int allocation_pos;
} Char_struct_save;

Char_struct_save *char_struct_allocation;

char *int_allocation;
int numbers_of_int = 0;
typedef struct Int_struct_save
{
    int n_a;
    int allocation_pos;
} Int_struct_save;

Int_struct_save *int_struct_allocation;

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

int number_to_digit(char *n)
{
    int res = 0;
    for (int i = 0; n[i] != '\0'; ++i)
    {
        if (n[i] >= '0' && n[i] <= '9')
        {
            res = res * 10 + n[i] - '0';
        }
        else
        {
            return -1;
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

int count_number_of_char_array(int name_pointer)
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

int count_number_of_chars_for_integer(int name_pointer)
{
    int res = 0;
    char cur_char = file[name_pointer + 2];
    while (cur_char != 0x0A)
    {
        cur_char = file[name_pointer + 2 + res];
        res++;
    }

    return res - 1;
}

void create_and_save_chars_structures(int n_a, int n_of_chars)
{
    Char_struct_save char_struct;
    char_struct.n_a = n_a;
    char_struct.allocation_pos = chars_lenght - n_of_chars; // seta o primeiro byte da string como o ultima antigo

    char_struct_allocation = realloc(char_struct_allocation, ((last_number_of_A + 1) * sizeof(char_struct_allocation)));

    char_struct_allocation[last_number_of_A] = char_struct;

    printf("Number of A's = %d\n", char_struct_allocation[last_number_of_A].n_a);

    printf("STRING on byte %d = %s\n", char_struct_allocation[last_number_of_A].allocation_pos,
           chars_allocation + char_struct_allocation[last_number_of_A].allocation_pos);
}

int save_and_convert_char_data_from_file_to_hex(int name_pointer, int n_of_chars)
{
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

    free(hex_number);
    return cur_char_n;
}

int convert_data_from_file_to_int(int name_pointer, int n_of_chars)
{
    char *int_number = malloc(n_of_chars + 1);
    int_number[n_of_chars] = 0x00;
    for (int i = 0; i < n_of_chars; i++)
    {
        int_number[i] = file[name_pointer + 2 + i];
    }
    int res = number_to_digit(int_number);

    free(int_number);
    return res;
}

void create_and_save_int_structures(int n_a, int allocation_pos)
{
    Int_struct_save struct_save;
    struct_save.n_a = n_a;
    struct_save.allocation_pos = allocation_pos;
    int_struct_allocation = realloc(int_struct_allocation, sizeof(Int_struct_save) * numbers_of_int);

    int a = -1;
    memcpy(&a, int_allocation + allocation_pos, 4);
    printf("Number of variable is: %i\n", a);
}

void check_char()
{
    if ((file[program_pointer - 1] == (char)0x20 && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x20) ||
        (program_pointer == 0 && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x20) ||
        (file[program_pointer - 1] == (char)0x0A && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x20))
    {
        printf("\033[0;31m");
        printf("\n\nCreating char on byte %d\n", program_pointer);
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

    if (last_number_of_A != 0)
    {
        if (n_a <= last_number_of_A)
        {
            ERROR_CRASH("There is a variable with less A's then the previous");
        }
    }

    int n_of_chars = count_number_of_char_array(name_pointer);
    n_of_chars += 1; // last byte of a string
    printf("Number of chars is : %d\n", n_of_chars);

    chars_lenght += n_of_chars;
    chars_allocation = realloc(chars_allocation, chars_lenght); // allocate more memory
    chars_allocation[chars_lenght - 1] = 0x00;

    int cur_char_n = save_and_convert_char_data_from_file_to_hex(name_pointer, n_of_chars);

    // é o name_pointer + 4 + cur_char_n... so q menos 3
    printf("\nold ptr =%d", program_pointer);
    program_pointer = name_pointer + 1 + cur_char_n; // set program_pointer to last byte on line... to sart reading next byte
    printf("\nnew ptr = %d\n", program_pointer);

    creating_variable = 0;

    create_and_save_chars_structures(n_a, n_of_chars);
    last_number_of_A++;
    printf("\033[0m");
}

void check_int()
{
    if ((file[program_pointer - 1] == (char)0x20 && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x41 && file[program_pointer + 2] == (char)0x20) ||
        (program_pointer == 0 && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x41 && file[program_pointer + 2] == (char)0x20) ||
        (file[program_pointer - 1] == (char)0x0A && file[program_pointer] == (char)0x41 && file[program_pointer + 1] == (char)0x41 && file[program_pointer + 2] == (char)0x20))
    {
        printf("\033[0;36m");
        printf("\n\nCreating int on byte %d\n", program_pointer);
        creating_variable = 1;
    }
    else
    {
        // printf("\n%d not a int\n", program_pointer);
        return;
    }

    int name_pointer = program_pointer + 3;
    if (file[name_pointer] != (char)0x41)
    {
        ERROR_CRASH("int should have a name of 'A'");
    }

    // GOT A STRING
    int n_a = count_number_of_A(name_pointer); // number of A's variable
    name_pointer = name_pointer + (n_a - 1);

    printf("n of a: %d\n", n_a);

    if (last_number_of_A != 0)
    {
        if (n_a <= last_number_of_A)
        {
            ERROR_CRASH("There is a variable with less A's then the previous");
        }
    }

    int n_of_chars = count_number_of_chars_for_integer(name_pointer);
    printf("n of chars: %d\n", n_of_chars);

    int variable = convert_data_from_file_to_int(name_pointer, n_of_chars);

    printf("\nold ptr =%d", program_pointer);
    program_pointer = name_pointer + 2 + n_of_chars; // set program_pointer to last byte on line... to sart reading next byte
    printf("\nnew ptr = %d\n", program_pointer);

    numbers_of_int++;
    int_allocation = realloc(int_allocation, numbers_of_int * (sizeof(int)));
    memcpy(int_allocation + ((numbers_of_int - 1) * 4), &variable, sizeof(int));

    create_and_save_int_structures(n_a, ((numbers_of_int - 1) * 4));

    creating_variable = 0;
    last_number_of_A++;
    printf("\033[0m");
}

void execute()
{

    for (program_pointer = 0; program_pointer < program_size; program_pointer++)
    {
        if (creating_variable == 0)
        {
            check_char();
            check_int();
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