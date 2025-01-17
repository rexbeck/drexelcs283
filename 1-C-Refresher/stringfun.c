#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50
#define SPACE_CHAR ' '
#define TAB_CHAR '\t'

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void reverse_words(char *, int);
void print_words(char *, int);
void search_and_replace(char *, int, int, char *, char *);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    int buff_idx = 0, str_idx = 0, char_count = 0;
    bool wasPreviousWhitespace = false, isCurrentWhitespace = true;
    char current = *(user_str + str_idx);

    /*Iterates through both the buffer and user string, checking the current character to decide the next
    course of action.*/
    while(current != '\0'){
        if(char_count == len){
            exit(3);
        }

        isCurrentWhitespace = (current == SPACE_CHAR || current == TAB_CHAR);

        if(!isCurrentWhitespace){
            *(buff + buff_idx) = current;
            wasPreviousWhitespace = false;

            buff_idx++;
            str_idx++;
            char_count++;
        }
        else {
            //Checks for leading whitespace
            if (buff_idx == 0){
                str_idx++;
            }
            else if (wasPreviousWhitespace){
                str_idx++;
            }
            else {
                *(buff + buff_idx) = SPACE_CHAR;
                wasPreviousWhitespace = true;

                buff_idx++;
                str_idx++;
                char_count++;
            }
        }

        current = *(user_str + str_idx);
    }

    if(*(buff + buff_idx - 1) == SPACE_CHAR){
        memset(buff + buff_idx - 1, '.', (len - char_count + 1) * sizeof(char));
    }
    else {
        memset(buff + buff_idx, '.', (len - char_count) * sizeof(char));
    }
    
    return char_count;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar(']');
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int wc = 0;
    bool word_start = false;

    for(int i = 0; i < str_len; i++){
        if (word_start) {
            if (buff[i] == SPACE_CHAR){
                word_start = false;
            } else {
                continue;
            }
        }
        else {
            if (buff[i] == SPACE_CHAR){
                continue;
            } else {
                wc++;
                word_start = true;
            }
        }
    }

    return wc;
}

void reverse_words(char *buff, int str_len){
    int start_idx = 0, end_idx = str_len - 1;
    char temp;

    //Swaps the first and last character in the buffer, iterating inward to the center of the string.
    while (end_idx > start_idx){
        memcpy(&temp, buff + end_idx, 1);
        memcpy(buff + end_idx, buff + start_idx, 1);
        memcpy(buff + start_idx, &temp, 1);

        start_idx++;
        end_idx--;
    }
}

void print_words(char *buff, int str_len){
    int last_char_idx = str_len - 1;  //index of last char - strlen(str)-1;
    int wc = 1;         //counts words
    int wlen = 0;       //length of current word
    bool word_start = false;    //am I at the start of a new word
    char current;   //current character

    printf("Word Print\n----------\n");

    for(int i = 0; i < str_len; i++){
        current = *(buff + i);
        if (word_start) {
            if (current == SPACE_CHAR){
                // Just reached the space after the word ended.
                word_start = false;
                printf("(%d)\n", wlen);
                wlen = 0;
            } else {
                // In a word, continuing to output letters.
                printf("%c", current);
                wlen++;

                // Checks if current index is last and returns wordcount.
                if (i == last_char_idx){
                    printf("(%d)\n", wlen);
                }
            }
        } else {
            if (current == SPACE_CHAR){
                // Not in a word and hit another space.
                continue;
            } else {
                // Hits first letter of new word.
                printf("%d. %c", wc, current);
                wc++;
                word_start = true;
                wlen++;

                // Checks if current index is last and returns wordcount.
                if (i == last_char_idx){
                    printf("(%d)\n", wlen);
                }
            }
        }
    }

    printf("\nNumber of words returned: %d\n", wc - 1);
}

void search_and_replace(char *buff, int buffer_len, int str_len, char *old_word, char *new_word){
    int search_idx = -1, search_end_idx, old_idx = 0;
    char current;

/*iterates through the buffer looking for the the word that is getting replaced (old_word). When a 
potential word is found, the index for the first character is stored (search_idx). It then checks 
to make sure the entire word is there, storing the last index if it is.*/
    for(int buff_idx = 0; buff_idx < str_len; buff_idx++){
        current = *(buff + buff_idx);

        if(search_idx == -1){
            if(current == *(old_word + old_idx)){
                search_idx = buff_idx;
                old_idx++;
            }
            else {
                continue;
            }
        }
        else {
            if(current == old_word[old_idx]){
                old_idx++;
                if(*(old_word + old_idx) == '\0'){
                    search_end_idx = buff_idx;
                    break;
                }
            }
            else {
                old_idx = 0;
                search_idx = -1;
            }
        }
    }

    if (search_idx == -1){
        exit(1);
    }
    
/*a temporary buffer is created (temp). The start of buff to the search_idx is set to temp. Then, the new word is placed
in the spot that the old word would be. Then the rest of the original buff string is set to temp. The periods are added to the
end of temp. Finally, temp is memset to buff, overriding the original with temp.*/
    char *temp = (char *)malloc(buffer_len * sizeof(char *));

    for(int idx = 0; idx < search_idx; idx++){
        *(temp + idx) = *(buff + idx);
    }

    int replacement_idx = 0, temp_idx = search_idx;
    char replacement = *(new_word);
    while (replacement != '\0'){
        *(temp + temp_idx) = replacement;
        temp_idx++;
        replacement_idx++;
        replacement = *(new_word + replacement_idx);
    }

    for(int idx = search_end_idx + 1; idx < str_len && idx < buffer_len; idx++){
        *(temp + temp_idx) = *(buff + idx);
        temp_idx++;
    }

    memset(temp + temp_idx, '.', (buffer_len + temp_idx) * sizeof(char));
    memcpy(buff, temp, buffer_len);
    
    free(temp);
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS


int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING

    /*
    argc is the sum amount of arguments used whereas argv is an array holding each respective
    argument. So with the command ./stringfun -c "strings" the argc=3 and argv[0]="./stringfun",
    argv[1]="-c" and argv[2]="strings".
    This is safe because argc < 2 essentially checks whether argv has the correct amount of
    arguments.
    */
    
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
    The below if statement is checking whether 3 or more arguments were based with the
    command. If less than 3, it exits and gives an error to the user.
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = (char *)malloc(BUFFER_SZ * sizeof(char *));
    if (buff == NULL){
        printf("Buffer failed to initialize.");
        free(buff);
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            print_buff(buff,BUFFER_SZ);
            break;
        case 'r':
            reverse_words(buff, user_str_len);
            print_buff(buff,BUFFER_SZ);
            break;
        case 'w':
            print_words(buff, user_str_len);
            print_buff(buff,BUFFER_SZ);
            break;
        case 'x':
            if (argc == 5){
                search_and_replace(buff, BUFFER_SZ, user_str_len, argv[3], argv[4]);
                print_buff(buff,BUFFER_SZ);
                //printf("Not Implemented!\n");
            } else {
                printf("Invalid number of arguments.\n");
                usage(argv[0]);
                exit(1);
            }
            break;
        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE

/*
    It allows for the buffer size to easily be changed. Because the size is initialized once
    instead of hardcoded into every helper function, to change the size you only need to change
    a single instance.
*/
