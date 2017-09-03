#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>

#define CMD_UNKNOWN      0
#define CMD_CD           1
#define CMD_CWD          2
#define CMD_LS           3
#define CMD_EXIT         4

char *my_strtok_r(char *str, const char *delim, char **nextp);
void handle_cd(char *path);
void handle_cwd();
void handle_ls();
int get_command(char *cmd);
void execute_non_builtin(char *cmd, char *cmd_arg);
void execute_command_line(char *cmd);
void execute_commands(char *cmd, char *cmd_arg);
void read_from_file(int num_tokens, char *cmd_tokens[]);
int _strlen(char *);
char *m_strcpy(char *, char *);
char *m_strncpy(char *, char *, int);

char ps1_variable[256] = "sbush>";

void build_argv(char *input, char *arg, char *argv[]) {
  int i = 1;
  char arr[255];
  strcpy(arr, arg);
  argv[0] = malloc(_strlen(input)+1);
  strcpy(argv[0], input);
  char *token = strtok(arr," ");
  while (token !=NULL) {
        argv[i] = malloc(_strlen(token)+1);
        strcpy(argv[i],token);
        token = strtok(NULL," ");
        i++;
  }
  argv[i] = NULL;
}
int _strlen(char *str) {
        int i;
        for (i=0; *str != '\0'; str++)
                i++;
        return i;
}
/*
int _strncmp (char *f_str, char *s_str, int n) {
  if (*f_str == 


}*/

char *my_strtok_r(char *str, const char *delim, char **nextp) {

  char *ret;
  if (str == NULL)
    str = *nextp;

  str += strspn(str, delim);
  if (*str == '\0')
    return NULL;

  ret = str;
  str += strcspn(str, delim);
  if (*str)
    *str++ = '\0';

  *nextp = str;

  return ret;
}

void handle_cd(char *path) {
  
  int ret;
  ret = chdir(path);
  if (ret == -1)
    printf("sbush: cd: %s: No such file or directory\n", path); 
}

void handle_cwd() {

  char buff[1024] = {0};
  if (getcwd(buff, sizeof(buff)) != NULL)
    printf ("%s\n", buff);
}

void handle_ls() {

  char buff[1024] = {0};
  if (getcwd(buff, sizeof(buff)) != NULL) {

    DIR *dir;
    dir = opendir(buff);
    if (dir) {

      struct dirent *d_entry;
      while ((d_entry = readdir(dir))) {
        
        /* For now, ignoring invisible files */
        if (*d_entry->d_name != '.')
          printf ("%s\n", d_entry->d_name);
      }

      closedir(dir);
    }
  }
}

int get_command(char *cmd) {

  /* built-in */
  if (strcmp(cmd, "cd") == 0)
    return CMD_CD;

  /* built-in */
  else if (strcmp(cmd, "exit") == 0)
    return CMD_EXIT;

  /*else if (strcmp(cmd, "cwd") == 0)
    return CMD_CWD;

  else if (strcmp(cmd, "ls") == 0)
    return CMD_LS;
  */

  else
    return CMD_UNKNOWN;
}
void get_path_string(char *cmd, char *path_value) {

  char *ptr = NULL;

  if ((strstr(cmd,"$PATH")) != NULL) {

    /*
     * 2 cases. eg: PATH=$PATH:/bin:/usr/bin 
     * $PATH anywhere else. beginning or somewhere else
     */ 

     char *path = strstr(cmd, "=");
     path++; 
     char *temp = path;
     int len = _strlen(path);

     ptr = strstr(path, "$PATH");

     char *sys_env = getenv("PATH");

     /* $PATH in the beginning */

     if (temp == ptr) {

       m_strncpy(path_value, sys_env, _strlen(sys_env));
       if (len - 6 > 0) { //6 len of $PATH:
	 m_strcpy(path_value+_strlen(sys_env), ptr+5);
       } 

     }
     else {
     /* $PATH anywhere else */
       m_strncpy(path_value, temp, ptr-temp); 
       m_strncpy(path_value+(ptr-temp), sys_env, _strlen(sys_env));
       m_strcpy(path_value+(ptr-temp)+_strlen(sys_env), ptr+5);
     }

//    printf("pathvalue :  [%s]\n",path_value); 
  }
  else {
    //eg: PATH=/usr/bin 
    ptr = strstr(cmd, "=");
    m_strcpy(path_value, ptr+1); 
  }
}

char *m_strcpy(char *dest, char *src) {
  int i;
  for (i=0; src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  dest[i] = '\0';
  return dest;
}

char *m_strncpy(char *dest, char *src, int len) {
  int i;
  for (i=0; i<len && src[i] != '\0'; i++)  {
    dest[i] = src[i];
  }
  return dest;
}

int check_if_path_cmd(char *cmd) {
  return strncmp(cmd,"PATH=",5) == 0 ? 1 : 0; 
}

int check_if_ps1_cmd(char *cmd) {
  return strncmp(cmd,"PS1=",4) == 0 ? 1 : 0; 
}

/* Not shell built-in commands, call exec */
void execute_non_builtin(char *cmd, char *cmd_arg) {
  char *envp[] = {"TERM=xterm",0};
  char path_value[1024] = {0} ; 
  if (check_if_path_cmd(cmd)) {
     get_path_string(cmd, path_value); 
//     printf("setting env var : [%s]\n",path_value);
     setenv("PATH", path_value ,1);
     return;
  }
  else if(check_if_ps1_cmd(cmd)) {
    m_strcpy(ps1_variable, strstr(cmd, "=") + 1);
    return;
  }
  char *argv[10];
  build_argv(cmd, cmd_arg, argv);

  int c_status;
  pid_t pid;
  pid = fork();

  if (pid == 0) {
    if (execvpe(cmd, argv,envp) < 0) {
	printf("exec* failed");
        exit(1);
    }
  }
  else {
    if (pid < 0) {
      printf("Fork failed\n");
      exit(1);
    }
    else {
      wait(&c_status);
    }

  }

}

void execute_command_line(char *cmd) {
 
  int i;
  char *str, *token, *saveptr;
  for (i = 1, str = cmd; ; i++, str = NULL) {

    token = my_strtok_r(str, " ", &saveptr);
    if (token == NULL)
      break;

    if (i == 1 && token[0] != '#')
      execute_commands(token, saveptr);
  }
}

void execute_commands(char *cmd, char *cmd_arg) {

  int cmd_id = get_command(cmd);
  switch (cmd_id) {

    case CMD_CD:
      handle_cd(cmd_arg);
      break;

    case CMD_EXIT:
      exit(0);

    /*case CMD_CWD:
      handle_cwd();
      break;
    */
    /*case CMD_LS:
      handle_ls();
      break;
    */
    case CMD_UNKNOWN:
      execute_non_builtin(cmd, cmd_arg);
      break;

    default:
      printf("%s : command not found\n", cmd);
      break;
  }
}

/* Read from the file one line at a time and execute */
void read_from_file(int num_tokens, char *cmd_tokens[]) {

  FILE *file = fopen(cmd_tokens[1], "r");
  char code[1024] = {0};
  size_t n = 0;
  int c;

  if (file == NULL)
    return;

  while ((c = fgetc(file)) != EOF)
  {
    code[n++] = (char) c;
    if (c == '\n') {
      code[n - 1] = '\0';
      execute_command_line(code);    
      n = 0;
    }
  }

  code[n] = '\0'; 
}

int main(int argc, char *argv[], char *envp[]) {

  int i;
  char *str, *saveptr, *token;
  char buff[1024] = {0};

  /*
  printf("test\n");
  const char* s = getenv("PATH");
  printf("PATH :%s\n",(s!=NULL)? s : "getenv returned NULL");
  printf("end test\n");
  */

  puts("sbush> ");


  if (argc > 1) {

    /* Case 1 : Non-interactive mode */
    read_from_file(argc, argv);

  } else {

    /* Case 2 : Interactive mode */
	  while (fgets(buff, sizeof(buff), stdin) != NULL) {

		  size_t buff_length = strlen(buff);
		  if (buff_length != 0 && buff[buff_length - 1] == '\n') {

			  buff_length--;
			  buff[buff_length] = '\0';
		  }

		  for (i = 1, str = buff; ; i++, str = NULL) {

			  token = my_strtok_r(str, " ", &saveptr);
			  if (token == NULL)
				  break;

			  if (i == 1)
          execute_commands(token, saveptr);
		  }
		  puts(ps1_variable);

	  }
  }

  return 0;
}
