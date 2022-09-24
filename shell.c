#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

char path[1000] = "/bin/:/usr/bin/:";
char *remove_whitespace(char *tok)
{
    int i;

    while (isspace(*tok))
        tok++;
    for (i = strlen(tok) - 1; (isspace(tok[i])); i--)
        ;
    tok[i + 1] = '\0';
    return tok;
}
void parallel_cmd(char *tok)
{
    // printf("tok print %s", tok);
    char *tok2;
    
    int count = 0;
    int i;
    char *arr[30];
    tok2 = strtok(tok, "&");
    
    while (tok2 != NULL)
    {
        arr[count] = tok2;
        // printf("array %s", arr[count]);
        count++;
        tok2 = strtok(NULL, "&");
    }
    // printf("cnt %d", count);
    for (i = 0; i < sizeof(arr); i++)
    {
       // printf("------%d---%s", i, arr[i]);
        int fd = fork();
        if (fd == 0)
        {
            if (strstr(arr[i], "cd") != NULL)
            {
                // printf("2nd %s\n ", tok2);
               // printf("arr 1 element %s", arr[i]);
                int a = cd_builtin(arr[i]);
                // continue;
                if (a > 2 || a <= 1)
                {
                    a=0;
                    char error_message[30] = "An error has occurred for y \n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
            else if (strstr(arr[i], "path") != NULL)
            {
                int b = path_builtin(arr[i]);
                // break;
                if (b == 0)
                {   
                    strcpy(path, "");
                }
            }
            else if (strstr(arr[i], "exit") != NULL)
            {
               exit_builtin(arr[i]);
            }
            else if (strstr(arr[i], ">") != NULL)
            {
                int d = redirect_method(arr[i]);
                if(d==0){d=1;
                char error_message[30] = "An error has occurred for z \n";
                write(STDERR_FILENO, error_message, strlen(error_message));}
                
            }
            else
            {
                 //printf("---------%s", arr[i]);
                int e = exec_cmd(arr[i]);
                if (e == 0)
                {
                    char error_message[30] = "An error has occurred for x \n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
        }
        else
        {
            int wc;
            if(i==sizeof(arr)-1)    {
                wc = wait(NULL);
                break;
            }
            else    wc = waitpid(fd);
        }
    }

    // printf("array %s", arr);
}

int redirect_method(char *tok) //---redirect method
{
    int red_code = 1;
    // printf("%s", tok);
    char *red_commands[10];
    int red_count = 0;
    int i = 0;
    char *tok2;
    tok2 = strtok(tok, ">");
    while (tok2 != NULL)
    {
        red_count++;
        tok2 = remove_whitespace(tok2);
        if(strstr(tok2," ")!=NULL && strstr(tok2,"ls")==NULL){
            red_code=0;
            return red_code;
        }
        red_commands[i++] = tok2;

        tok2 = strtok(NULL, ">");
    }

    //printf("%d",red_count);
    if (red_count <= 1 || red_count > 2)
    {  //printf("terminate");
        red_code = 0;
        return red_code;
    }
    //printf("not sure");
    char *a = remove_whitespace(red_commands[1]);
    char *b = remove_whitespace(red_commands[0]);
    
    int saved_stdout = dup(1);
    int fd = open(a, O_TRUNC | O_RDWR | O_CREAT, S_IRWXU);
    dup2(fd, 1);
    red_code = exec_cmd(b);
    dup2(saved_stdout, 1);
    close(fd);
    return red_code;
}

int exit_builtin(char *tok) //------exit built-in
{
    char *tok2;
    int exit_count = 0;
    tok2 = strtok(tok, " ");
    while (tok2 != NULL)
    {
        exit_count++;
        tok2 = strtok(NULL, " ");
    }
    if (exit_count == 1)
        exit(0);
    else
    {
        char error_message[30] = "An error has occurredc\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(0);
    }
}

int path_builtin(char *tok) //---path built-in
{
    char *tok2;
    int path_count = 0;
    tok2 = strtok(tok, " ");
    while (tok2 != NULL)
    {
        if (strcmp(tok2, "path") != 0)
        {
            path_count += 1;
            if (strcmp(tok2, "reset") == 0)
                strcpy(path, "/bin/:/usr/bin/:");
            else
            {
                char *temp_path = tok2;
                // printf("%s", temp_path);
                
                strcat(path, temp_path);
                strcat(path, ":");
            }
        }
        tok2 = strtok(NULL, " ");
    }
    printf("%s", path);
    return path_count;
}

int cd_builtin(char *tok) //---cd built in
{
    char *tok2;
    tok2 = strtok(tok, " ");
    int cd_count = 0;
    while (tok2 != NULL)
    {
        cd_count++;
        if (strcmp(tok2, "cd") != 0)
        {
            char s[100];
            // printf("%s\n", getcwd(s, 100));
            if (chdir(tok2) < 0)
            {
                cd_count = 5;
                return cd_count;
            }
            // printf("%s\n", getcwd(s, 100));
        }
        tok2 = strtok(NULL, " ");
    }

    return cd_count;
}

int exec_cmd(char *tok) //---exec command
{

    int exec_code = 1;
    char *tok2;
    tok = remove_whitespace(tok);
    char *temptok = tok;
    char exec_command_path[1000];
    char *exec_command[1000] = {NULL};
    char *svptr1;
    int i = 0;
    char echo_command_path[1000];

    if (strstr(tok, "echo") != NULL)
    {
        strcpy(echo_command_path,"");
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
    //printf("----<%s", exec_command[0],exec_command[1],exec_command[2]);
    //printf("----<>%s", exec_command_path);
    char *token;
    token = strtok(path, ":");
    while (token != NULL)
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
                if(execv(temp, exec_command)<0)
                    exec_code=0;
                    
            }
            else
            {
                int wc = wait(NULL);

                break;
            }
        }
        token = strtok(NULL, ":");
        // if(token==NULL && fd!=0)  exec_code==0;
    }
    if (token == NULL)
        exec_code = 0;

    return exec_code;
}

int main(int argc, char *argv[])
{
    int c_b, p_b, e_b, r_m;
    char *buf = NULL;
    size_t size = 128;
    size_t len;

    buf = (char *)malloc(size * sizeof(char));

    if (argc == 2)
    {
        FILE *fp;
        char ch[50];
        fp = fopen(argv[1], "r");
        char commands[100];
        while (fgets(ch, 50, fp) != NULL)
        {
            strcat(commands, ch);
        }

        fclose(fp);
        // printf("%s", commands);
        char *str1, *svptr1, *svptr2, *str2, *t1, *t2;
        char dir_path[100];
        int i, count = 0;
        for (i = 1, str1 = commands;; i++, str1 = NULL)
        {
            t1 = strtok_r(str1, "\n", &svptr1);
            char commands[100];
            memset(&commands[0], 0, sizeof(commands));
            strcat(commands, t1);
            if (t1 == NULL)
                break;

            if (strstr(t1, "exit") != NULL) //-----exit builtin
            {
                exit_builtin(t1);
            }
            else if (strstr(t1, "&") != NULL)
            {
                parallel_cmd(t1);
            }
            else if (strstr(t1, ">") != NULL) //-----redirect method
            {
                r_m = redirect_method(t1);
            }
            else if (strstr(t1, "path") != NULL) //----path builtin
            {
                p_b = path_builtin(t1);
            }
            else if (strstr(t1, "cd") != NULL) //----cd builtin
            {
                c_b = cd_builtin(t1);
            }
            else
                e_b = exec_cmd(commands); //-----exec command

            for (str2 = t1;; str2 = NULL)
            {

                t2 = strtok_r(str2, " ", &svptr2);

                if (t2 == NULL)
                    break;

                count++;
            }

            if (strstr(t1, "cd") != NULL && (c_b > 2 || c_b <= 1))
            {
                c_b=1;
                char error_message[30] = "An error has occurredd\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (strstr(t1, "path") != NULL)
            {
                if (p_b == 0)
                    // memset(&path[0], 0, sizeof(path));
                    strcpy(path, "");
            }
            else if (e_b == 0)
            {
                char error_message[30] = "An error has occurrede\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            else if (r_m == 0)
            {   r_m=1;
                char error_message[30] = "An error has occurred for rm \n";
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
            // printf("%s", commands);
            // printf("Sentence %d: tokens %d\n", i, count);
            count = 0;
        }
        exit(0);
    }

    printf("dash> ");

    char *myargs[3];
    while (1)
    {
        // char *input_command = {0};

        len = getline(&buf, &size, stdin);
        char *str1, *svptr1, *svptr2, *str2, *t1, *t2;
        char dir_path[100];
        int i, count = 0;
        for (i = 1, str1 = buf;; i++, str1 = NULL)
        {
            t1 = strtok_r(str1, "\n", &svptr1);
            if (t1 == NULL)
                break;

            if (strstr(t1, "exit") != NULL) //-----exit builtin
            {
                exit_builtin(t1);
            }
            else if (strstr(t1, "&") != NULL)
            {
                parallel_cmd(t1);
            }
            else if (strstr(t1, ">") != NULL) //-----redirect method
            {
                r_m = redirect_method(t1);
            }
            else if (strstr(t1, "path") != NULL) //----path builtin
            {
                p_b = path_builtin(t1);
            }
            else if (strstr(t1, "cd") != NULL) //----cd builtin
            {
                c_b = cd_builtin(t1);
            }
            else
                e_b = exec_cmd(t1); //-----exec command



            if (strstr(t1, "cd") != NULL && (c_b > 2 || c_b <= 1))
            {
                     c_b = 0;
                char error_message[30] = "An error has occurredg\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                
            }
            else if (r_m == 0)
            {
                r_m=1;
                char error_message[30] = "An error has occurred for rm \n";
                write(STDERR_FILENO, error_message, strlen(error_message));
               
            }
            else if (strstr(t1, "path") != NULL)
            {
                if (p_b == 0)
                    // memset(&path[0], 0, sizeof(path));
                    strcpy(path, "");
            }
            else if (e_b == 0)
            {
                char error_message[30] = "An error has occurredh\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                
            }
        }
        printf("dash> ");
    }
    return 0;
}