/*  TEAM MEMBERS:
    TANMAY JAIN(txj210012)
    KEVAN MEHTA()

    PLEASE NOTE:The code works properly and compiling the code with "gcc dash.c –o dash -Wall -Werror -O"
             does not give any errors but when compiled like this, the "path function" behaves improperly.

             If compiled without the flags(although the previous method does not give any errors),
             the whole code works perfectly.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

int exec_cmd(char *tok);    //prototype for exec command function
char path[50000],path_delim[50];    //variable for path and path delimiter
 
char *remove_whitespace(char *tok)  //function to remove whitespace
{
    int i;

    while (isspace(*tok))
        tok++;
    for (i = strlen(tok) - 1; (isspace(tok[i])); i--);
    tok[i + 1] = '\0';
    return tok;
}

int redirect_method(char *tok)  //function for redirection
{
    int red_code = 1;   //error code for redirection
    char *red_commands[10]; //variable to store redirection query
    int red_count = 0;  //variable to find number of files in redirection query
    int i = 0;
    char *tok2;
    tok2 = strtok(tok, ">");
    while (tok2 != NULL)
    {
        red_count++;
        tok2 = remove_whitespace(tok2);
        if (strstr(tok2, " ") != NULL && strstr(tok2, "ls") == NULL)    // to check for this error "">output11"
        {
            red_code = 0;
            return red_code;
        }
        red_commands[i++] = tok2;

        tok2 = strtok(NULL, ">");
    }

    if (red_count <= 1 || red_count > 2)
    { 
        red_code = 0;
        return red_code;
    }
 
    char *a = remove_whitespace(red_commands[1]);   //file name in redirection query
    char *b = remove_whitespace(red_commands[0]);   // query to execute

    int saved_stdout = dup(1);
    int fd = open(a, O_TRUNC | O_RDWR | O_CREAT, S_IRWXU);
    dup2(fd, 1);    // changing file descriptor to file from stdout
    red_code = exec_cmd(b); //calling function to execute
    dup2(saved_stdout, 1);
    close(fd);
    return red_code;
}

void exit_builtin(char *tok) //function for exit built-in
{
    char *tok2;
    int exit_count = 0; //error code for function
    tok2 = strtok(tok, " ");
    while (tok2 != NULL)
    {
        exit_count++;
        tok2 = strtok(NULL, " ");
    }
    if (exit_count == 1)    // exit call
        exit(0);
    else    // to check for this error "exit bash"
    {
        char error_message[30] = "An error has occurredc\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        
    }
}

int path_builtin(char *tok) //function for path built-in
{
    char *tok2;
    char temp_path[100];
    int path_count = 0; //variable to count number of path arguements given
    tok2 = strtok(tok, " ");
    
    while (tok2 != NULL)
    {
        if (strstr(tok2, "path") == NULL)
        {
            path_count += 1;
            
            {
                
                strcat(temp_path,tok2);
                strcat(temp_path,path_delim);
                
            }
        }
        tok2 = strtok(NULL, " ");
    }
    strcpy(path,temp_path);
    return path_count;
}

int cd_builtin(char *tok) //function for cd built-in
{   
    char *tok2;
    char cd_command[1000]={0};  // variable to store cd arguement
    int cd_code = 1; //error code for the function
    int cd_count = 0; // counter for cd arguements
    tok2 = strtok(tok, " ");
    while (tok2 != NULL)
    {   
        if (strstr(tok2, "cd") == NULL)
        {   
            cd_count++;
            tok2 = remove_whitespace(tok2);
            strcat(cd_command,tok2);
            
        }
        tok2 = strtok(NULL, " ");
    }
    
    if(cd_count==1){    
        if (chdir(cd_command) < 0)
            {
                cd_code=0;  // to check for this error cd abcd(no folder abcd present)
                return cd_code;
            }
    }
    if(cd_count>=2 || cd_count<1) cd_code=0;
    return cd_code;
}

int exec_cmd(char *tok) //function to execute other than built-in commands
{   
    int exec_code = 1;  //error code for the function
    char path_path[1000]; //store path temporarily
    char *tok2;
    tok = remove_whitespace(tok);
    char *temptok = tok;
    char exec_command_path[1000];   // stores the command(uname)
    char *exec_command[1000] = {NULL};  //store the query (ls test)
    char *svptr1;
    int i = 0;
    char echo_command_path[1000];   // to store special echo arguements

    if (strstr(tok, "echo") != NULL)    // to handle echo command
    {
        strcpy(echo_command_path, "");
        char *str1 = strtok_r(tok, " ", &svptr1);
        if (str1)
        {
            str1 = remove_whitespace(str1);
            exec_command[i] = str1;
            i++;
        }
        while ((str1 = strtok_r(NULL, " ", &svptr1)) != NULL)
        {
            str1 = remove_whitespace(str1);
            strcat(echo_command_path, str1);
            strcat(echo_command_path, " ");
        }
        exec_command[i] = echo_command_path;
        
    }
    
    else    
    {
        char *str1 = strtok_r(tok, " ", &svptr1);
        if (str1)
        {
            exec_command[i] = str1;
            i++;
        }
        while ((str1 = strtok_r(NULL, " ", &svptr1)) != NULL)
        {
            exec_command[i] = str1;
        }
    }
    tok2 = strtok(temptok, " ");
    while (tok2 != NULL)
    {
        strcpy(exec_command_path, tok2);
        break;
        tok2 = strtok(temptok, " ");
    }
    char *token;
    strcpy(path_path,path);
    char* rest = path;
    while ((token = strtok_r(rest, ":", &rest)))    //looping through every paths
    {   
        char temp[500] = {0};
        strcat(temp, token);
        strcat(temp, exec_command_path);
        int fd = access(temp, X_OK);
        if (fd == 0)
        {
            int rc = fork();
            if (rc == 0)
            {
                if (execv(temp, exec_command) < 0)  //handling execv failure
                    exec_code = 0;
            }
            else
            {
                wait(NULL);
                break;
            }
        }
        token = strtok(NULL, ":");
       
    }
    strcpy(path,path_path);
    if (token == NULL)
        exec_code = 0;

    return exec_code;
}

void parallel_cmd(char *tok)    // function to execute parallel commands
{
    int i;
    char *tok2;
    for (i=0; (tok2 = strtok_r(tok,"&",&tok)) ;i++) //tokenizing parallel commmands
    {       int fd = fork();
            if (fd==0){
            if (strstr(tok2, "exit") != NULL)  //calling exit built-in
            {
                exit_builtin(tok2);
            }
            else if (strstr(tok2, ">") != NULL) //calling redirection function
            {
                int d = redirect_method(tok2);
                if (d == 0)
                {
                    d = 1;
                    char error_message[30] = "An error has occurred y\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
            else if (strstr(tok2, "path") != NULL)   //calling path built-in
            {
                int b = path_builtin(tok2);
                if (b == 0)
                {
                    strcpy(path, "");
                }
            }
            else if (strstr(tok2, "cd") != NULL)    //calling cd built-in
            {
                int a = cd_builtin(tok2);
                if (a==0)
                {
                    a = 1;
                    char error_message[30] = "An error has occurred for p \n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
            else    //calling execv function
            {
                int e = exec_cmd(tok2);
                if (e == 0)
                {
                    e = 1;
                    char error_message[30] = "An error has occurred for p1 \n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
            exit(0);
            }
        }
        if(i>1){        //waiting for all childs to complete
            
            while(i>0){
                wait(NULL);
                i = i-1;
            }
        }
    
    
}

int main(int argc, char *argv[])    
{
    
    int c_b = 1, p_b = 0, e_b = 1, r_m = 1; //error codes for all functions
    char *buf = NULL;
    size_t size = 128;
    strcpy(path_delim,"/:");    //storing default path
    strcpy(path,"/bin");
    strcat(path, path_delim);

   if (argc>2){ //if 2 files are passed
    char error_message[30] = "An error has occurredd\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(1);
   }

    if (argc == 2)  //batch mode
    {
        FILE *fp;   //reading file
        char ch[50];
        fp = fopen(argv[1], "r");
        char commands[100];
        while (fgets(ch, 50, fp) != NULL)
        {
            strcat(commands, ch);
        }
        fclose(fp);

        char *str1, *svptr1,*t1; 
        int i;
        for (i = 1, str1 = commands;; i++, str1 = NULL)
        {
            t1 = strtok_r(str1, "\n", &svptr1);
            char commands[100];
            memset(&commands[0], 0, sizeof(commands));
            strcat(commands, t1);
            if (t1 == NULL)
                break;

            if (strstr(t1, "exit") != NULL) //call to exit builtin
            {
                exit_builtin(t1);
            }
            else if (strstr(t1, "&") != NULL) //call to parallel built-in
            {
                parallel_cmd(t1);
            }
            else if (strstr(t1, ">") != NULL) //call to redirect method
            {
                r_m = redirect_method(t1);
            }
            else if (strstr(t1, "path") != NULL) //call to path builtin
            {
                p_b = path_builtin(t1);
            }
            else if (strstr(t1, "cd") != NULL) //call to cd builtin
            {
                c_b = cd_builtin(t1);
            }
            else
                e_b = exec_cmd(commands); //call to execv method

            if (strstr(t1, "cd") != NULL && c_b==0) //handling cd error
            {
                c_b = 1;
                char error_message[30] = "An error has occurredddd\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (r_m == 0)  //handling redirection error
            {
                r_m = 1;
                char error_message[30] = "An error has occurred for rm \n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (strstr(t1, "path") != NULL)    //handling path error
            {
                if (p_b == 0)
                    strcpy(path, "");
            }
            else if (e_b == 0)  //handling execv error
            {
                e_b=1;
                char error_message[30] = "An error has occurrede\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }
        exit(0);
    }

    while (1)   //batch mode
    {
        int i;
        printf("dash> ");
        getline(&buf, &size, stdin);    //reading from stdin
        char *str1, *svptr1, *t1;
        for (i = 1, str1 = buf;; i++, str1 = NULL)
        {
            t1 = strtok_r(str1, "\n", &svptr1);
            if (t1 == NULL)
                break;

            if (strstr(t1, "exit") != NULL) //call to exit built-in
            {
                exit_builtin(t1);
            }
            else if (strstr(t1, "&") != NULL) // call to parallel built-in
            {
                parallel_cmd(t1);
            }
            else if (strstr(t1, ">") != NULL) //call to redirect method
            {
                r_m = redirect_method(t1);
            }
            else if (strstr(t1, "path") != NULL) //call to path builtin
            {
                p_b = path_builtin(t1);
            }
            else if (strstr(t1, "cd") != NULL) //call to cd builtin
            {
                c_b = cd_builtin(t1);
            }
            else    //call to execv method
                e_b = exec_cmd(t1); 

            if (strstr(t1, "cd") != NULL && c_b==0) //handling cd error
            {
                c_b = 1;
                char error_message[30] = "An error has occurredg\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (r_m == 0)  //handling redirection error
            {
                r_m = 1;
                char error_message[30] = "An error has occurred for rm \n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (strstr(t1, "path") != NULL)    //handling path error
            {
                if (p_b == 0)
                    strcpy(path, "");
            }
            else if(e_b == 0)   //handling execv error
            {
                e_b = 1;
                char error_message[30] = "An error has occurredh\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }
    }
    return 0;
}