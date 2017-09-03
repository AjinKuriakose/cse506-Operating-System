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

struct command
{
  char *argv[50];
};

char *my_strtok_r(char *str, const char *delim, char **nextp);
void *my_memset(void *dest, int ch, size_t num_bytes);
void my_strcpy(char *dst, const char *src);
char *my_strstr(char *str1, char *str2);
void handle_cd(char *path);
void handle_cwd();
void handle_ls();
int get_command(char *cmd);
void execute_non_builtin(char *cmd, char *cmd_arg);
void execute_command_line(char *cmd);
void execute_commands(char *cmd, char *cmd_arg);
void read_from_file(int num_tokens, char *cmd_tokens[]);
int _strlen(char *);
int fork_pipes (int n, struct command *cmd);

int tokenize(char *arg, char *argv[], int max_tokens, char *sep) {
  int i = 0;
  char *saveptr;
  char arr[255];
  my_strcpy(arr, arg);
  char *token = my_strtok_r(arr, sep, &saveptr);
  while (token != NULL && i < max_tokens - 1) {
    argv[i] = malloc(_strlen(token) + 1);
    my_strcpy(argv[i], token);
    token = my_strtok_r(NULL, sep, &saveptr);
    i++;
  }

  argv[i] = NULL;
  return i;
}

void build_argv(char *input, char *arg, char *argv[]) {
  argv[0] = malloc(_strlen(input) + 1);
  my_strcpy(argv[0], input);
  tokenize(arg, &argv[1], 49, " ");
}

int _strlen(char *str) {
  int i;
  for (i=0; *str != '\0'; str++)
    i++;
  return i;
}

void *my_memset(void *dest, int ch, size_t num_bytes) {
  char *tmp = dest;
  while (num_bytes) {
    *tmp++ = ch;
    num_bytes--;
  }
  /*
  if (num_bytes) {
    tmp = dest;
    do {
      *tmp++ = ch;
    } while (--num_bytes);
  }
  */

  return dest;
}

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

void my_strcpy(char *dst, const char *src) {
  while (*src) {
    *dst++ = *src++; 
  }
  *dst = '\0';
}

char *my_strstr(char *str1, char *str2) {
  char *s1 = str1, *s2 = str2, *ret = NULL;
  int match_len = 0;

  if (!str1 || !str2 || !*str1 || !*str2)
    return ret;

  while (*s1 && *s2) {
    if (_strlen(s1) < _strlen(s2))
      break;

    if (*s1 == *s2) {
      match_len++;
      if (!ret)
        ret = s1;
      s2++;
    } else {
      match_len = 0;
      s2 = str2;
      ret = NULL;
    }
    s1++;
  }

  if (match_len != _strlen(str2))
    ret = NULL;

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

/* Not shell built-in commands, call exec */
void execute_non_builtin(char *cmd, char *cmd_arg) {
  
  /*
  char *path = getenv("PATH");
  char  pathenv[strlen(path) + sizeof("PATH=")];
  sprintf(pathenv, "PATH=%s", path);
  char *envp[] = {pathenv, NULL};
  char *tests[] = {"ls", "-lR", NULL};
  execvpe(tests[0], tests, envp);
  fprintf(stderr, "failed to execute \"%s\"\n", tests[0]);
  */
  int i;
  char *argv[50] = {0};
  build_argv(cmd, cmd_arg, argv);

  int c_status;
  pid_t pid;
  pid = fork();

  if (pid == 0) {
    if (execvp(cmd, argv) < 0) {
	    printf("exec* failed");
      exit(1);
    }
  } else {
    if (pid < 0) {
      printf("Fork failed\n");
      exit(1);
    }
    else {
      wait(&c_status);
    }
  }

  /* Freeing */
  i = 0;
  while (argv[i]) {
    free(argv[i]);
    argv[i] = NULL;
    i++;
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
  execute_command_line(code);    
}

void handle_piped_commands(char *arg) {
  char *argv[50] = {0};
  int i;
  int num_tokens = tokenize(arg, &argv[0], 50, "|");

  /*
  printf("[%s]\n", argv[0]);
  printf("[%s]\n", argv[1]);
  printf("%d\n", num_tokens);
  */

  /*
  const char *ls[] = { "ls", "-l", 0 };
  const char *awk[] = { "awk", "{print $1}", 0 };
  const char *sort[] = { "sort", 0 };
  const char *uniq[] = { "uniq", 0 };

  struct command cmd [] = { {ls}, {awk}, {sort}, {uniq} };

  fork_pipes (4, cmd);
  */

  struct command cmd[num_tokens];
  my_memset(cmd, 0, sizeof(cmd));

  for (i = 0; i < num_tokens; i++) {
    tokenize(argv[i], cmd[i].argv, 50, " ");
  }
 
  fork_pipes (num_tokens, cmd);

  /* Freeing */
  i = 0;
  while (*(cmd[i].argv)) {
    free(*(cmd[i].argv));
    *(cmd[i].argv) = NULL;
    i++;
  }
  i = 0;
  while(argv[i]) {
    free(argv[i]);
    argv[i] = NULL;
    i++;
  }

}

void read_from_stdin() {
  int cnt;
  char *str, *saveptr, *token;
  char buff[1024] = {0};
  while (fgets(buff, sizeof(buff), stdin) != NULL) {

    cnt = 1;
    size_t buff_length = _strlen(buff);
    if (buff_length != 0 && buff[buff_length - 1] == '\n') {

      buff_length--;
      buff[buff_length] = '\0';
    }

    if (my_strstr(buff, "|")) {
      handle_piped_commands(buff);
    }  else {
      str = buff;
      while (1) {
        token = my_strtok_r(str, " ", &saveptr);
        if (token == NULL)
          break;

        if (cnt == 1)
          execute_commands(token, saveptr);

        cnt++;
        str = NULL;
      }
    }
    puts("sbush> ");
    /*
    printf("sbush> ");
    fflush(stdout);
    */

    //exit(1); /* TODO : REMOVE, JUST A TEST (VALGRIND) */
  }
}

int spawn_proc (int in, int out, struct command *cmd)
{
  pid_t pid;
  if ((pid = fork ()) == 0)
  {
    if (in != 0)
    {
      dup2 (in, 0);
      close (in);
    }

    if (out != 1)
    {
      dup2 (out, 1);
      close (out);
    }

    return execvp (cmd->argv [0], (char * const *)cmd->argv);
  }

  return pid;
}

int fork_pipes (int n, struct command *cmd) {
  int i;
  int in, fd [2];

  in = 0;
  for (i = 0; i < n - 1; ++i)
  {
    if (pipe (fd) != 0)
      return 1;

    spawn_proc (in, fd [1], cmd + i);
    close (fd [1]);

    in = fd [0];
  }

  if (in != 0)
    dup2 (in, 0);

  return execvp(cmd[i].argv[0], (char * const *)cmd[i].argv);
 
  /*
  int c_status;
  pid_t pid;
  pid = fork();

  if (pid == 0) {
    if (execvp(cmd[i].argv[0], (char * const *)cmd[i].argv) < 0) {
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
  */
}

int main(int argc, char *argv[], char *envp[]) {

  puts("sbush> ");
  /*
  printf("sbush> ");
  fflush(stdout);
  */

  /*
  printf("[%s]\n", my_strstr(NULL, NULL));
  printf("[%s]\n", my_strstr("aa", NULL));
  printf("[%s]\n", my_strstr(NULL, "bb"));
  printf("[%s]\n", my_strstr("aa", ""));
  printf("[%s]\n", my_strstr("", "bb"));
  printf("[%s]\n", my_strstr("", ""));
  printf("[%s]\n", my_strstr("this is a string", "this"));
  printf("[%s]\n", my_strstr("this is a string", "is"));
  printf("[%s]\n", my_strstr("this is a string", "a"));
  printf("[%s]\n", my_strstr("this is a string", "string"));
  printf("[%s]\n", my_strstr("this is a string", "t"));
  printf("[%s]\n", my_strstr("this is a string", "s"));
  printf("[%s]\n", my_strstr("this is a string", "g"));
  exit (1);
  */

  if (argc > 1) {

    /* Case 1 : Non-interactive mode */
    read_from_file(argc, argv);

  } else {

    /* Case 2 : Interactive mode */
    read_from_stdin();
  }

  return 0;
}
