#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool startsWith(char *pre, char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

char* read_one_word (FILE *f) {
    char buffer[1000];
    if(fscanf(f, " %s", buffer) == 1) {
        char* x = malloc(strlen(buffer)+1);
        strcpy(x, buffer);
        return x;   
    }
    else if(feof(f)){
        return NULL;
    }
    else{
        fprintf(stderr, "Error reading file");
        return NULL;
    } 
}

int main(int argc, char* argv[]) {
    if(argc<3){
        fprintf(stderr, "Error: Input must be ./a.out <filename.txt> <string>"); 
    } 
    else{
        printf( "argv[0] is %s\n", argv[0] );
        printf( "argv[1] is %s\n", argv[1] );
        printf( "argv[2] is %s\n", argv[2] );
    }
    
    FILE *fp = NULL;
    fp = fopen(argv[1], "r");
    if(fp==NULL){
        fprintf(stderr, "Failed to open file %s", argv[1]);
    }
    char **document = calloc(20, sizeof(char*));
    printf("Allocated initial array of 20 character pointers.\n");
    char* tmp = NULL; 
    int i = 0;
    int count = 20;
    while( (tmp = read_one_word(fp))){
        if(i>count){
            count = count * 2;
            char** new_document = realloc(document, count * sizeof(char *));
            printf("Reallocated array of %d character pointers.\n", count);
            if(new_document) document = new_document;
        }
        document[i] = tmp; 
        i = i + 1;
    }
    int xx = 0;
    for(; xx < i-1; xx++){
        if(startsWith(argv[2], document[xx])) printf("%s\n", document[xx]);
    } 
    
    int j = 0;
    while(j<i){
        free(document[j++]);
    }
    free(document);
    fclose(fp);
    return EXIT_SUCCESS;
}
