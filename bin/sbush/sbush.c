#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

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

  else if (strcmp(cmd, "cwd") == 0)
    return CMD_CWD;

  else if (strcmp(cmd, "ls") == 0)
    return CMD_LS;

  else
    return CMD_UNKNOWN;
}

/* TODO : Not shell built-in commands, call exec */
void execute_non_builtin(char *cmd, char *cmd_arg) {
  /*
  printf("Non-builtin : [%s]\n", cmd);

  char *path = getenv("PATH");
  char  pathenv[strlen(path) + sizeof("PATH=")];
  sprintf(pathenv, "PATH=%s", path);
  char *envp[] = {pathenv, NULL};
  char *tests[] = {"ls", "-lR", NULL};
  execvpe(tests[0], tests, envp);
  fprintf(stderr, "failed to execute \"%s\"\n", tests[0]);
  */
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

    case CMD_CWD:
      handle_cwd();
      break;

    case CMD_LS:
      handle_ls();
      break;

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

  /*
  for (i = 0; i < argc; i++)
    printf("%s\n", argv[i]);
  */

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
		  puts("sbush> ");

	  }
  }

  return 0;
}
