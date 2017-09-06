#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//#include <dirent.h>

#define CMD_UNKNOWN      0
#define CMD_CD           1
#define CMD_CWD          2
#define CMD_LS           3
#define CMD_EXIT         4

typedef struct piped_commands {
  char *commands[50];
} piped_commands;

char **m_environ;


char *m_strcpy(char *dest, char *src);
char *m_strncpy(char *dest, char *src, int num_bytes);
char *my_strtok_r(char *str, char *delim, char **nextp);
char *my_strstr(char *str1, char *str2);
void *my_memset(void *dest, int ch, size_t num_bytes);
void my_strcpy(char *dst, const char *src);
int  my_strlen(char *str);
int  my_strncmp(const char *f_str, const char *s_str, size_t n);
int  my_strcmp(const char *f_str, const char *s_str);

long sys_call(int syscall_number, ...);
//DIR *my_opendir(const char *name); /* TODO */
//struct dirent *my_readdir(DIR *dirp); /* TODO */
//int my_closedir(DIR *dirp); /* TODO */
int my_execvp(const char *file, char *const argv[]); /* TODO */


char *my_getenv(const char *name);
int my_setenv(char *name, char *value, int overwrite);
char *my_getcwd(char *buf, size_t size);
void *my_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
void *my_malloc(size_t size);
void my_free(void *ptr);
int  my_munmap(void *addr, size_t length);
int  my_waitpid(int pid, int *st_ptr, int options);
int  my_chdir(const char *path);
int  my_dup2(int oldfd, int newfd);
int  my_pipe(int pipefd[2]);
int  my_open(const char *pathname, int flags);
int  my_close(int fd);
int  my_fork();
int  my_read(int fd, char *c, int size);
int  my_write(int fd, char *c, int size);
int  my_putchar(int c);
int  my_puts(const char *s);
size_t my_strspn(char *s1, char *s2);
size_t my_strcspn(char *s1, char *s2);
char *my_strchr(char *s, int c);




int  get_command(char *cmd);
void handle_cd(char *path);
void handle_cwd();
void handle_ls();
void execute_non_builtin(char *cmd, char *cmd_arg);
void execute_command_line(char *cmd);
void execute_commands(char *cmd, char *cmd_arg);
void read_from_file(int num_tokens, char *cmd_tokens[]);

char ps1_variable[256] = "sbush>";

int execute_piped_commands(int num_pipes, piped_commands *cmds);

#define	PROT_READ     0x1
#define	PROT_WRITE    0x2
#define	MAP_PRIVATE   0x02
#define MAP_ANONYMOUS 0x20
#define MAP_FAILED    ((void *)-1)

#define O_RDONLY      0x0000
#define O_WRONLY      0x0001
#define O_RDWR        0x0002

#define S_IREAD       0000400
#define S_IWRITE      0000200

#define __NR_read     0
#define __NR_write    1
#define __NR_open     2
#define __NR_close    3
#define __NR_mmap     9
#define __NR_munmap   11
#define __NR_pipe     22
#define __NR_dup2     33
#define __NR_fork     57
#define __NR_wait4    61
#define __NR_getcwd   79
#define __NR_chdir    80
#define __NR_waitid   247

void *my_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
  return (void *)sys_call(__NR_mmap, addr, length, prot, flags, fd, offset);
}

int my_munmap(void *addr, size_t length) {
  return sys_call(__NR_munmap, addr, length);
}

void *my_malloc(size_t size) {
  int *plen;
  int len = size + sizeof(size);
  plen = (int *)my_mmap(0, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if (plen == MAP_FAILED)
    return NULL;

  *plen = len;
  return (void *)(&plen[1]);
}

void my_free(void *ptr) {
  if (ptr) {
    int *plen = (int *)ptr;
    int len;

    plen--;
    len = *plen;

    my_munmap((void *)plen, len);
  }
}

int my_waitpid(int pid, int *st_ptr, int options) {
  return sys_call(__NR_wait4, pid, st_ptr, options, NULL);
}

int my_chdir(const char *path) {
  return sys_call(__NR_chdir, path);
}

char *my_getcwd(char *buf, size_t size) {
  return (char *)sys_call(__NR_getcwd, buf, size);
}

int my_dup2(int oldfd, int newfd) {
  return sys_call(__NR_dup2, oldfd, newfd);
}

int my_pipe(int pipefd[2]) {
  return sys_call(__NR_pipe, pipefd);
}

int my_open(const char *pathname, int flags) {
  return sys_call(__NR_open, pathname, flags);
}

int my_close(int fd) {
  return sys_call(__NR_close, fd);
}

long sys_call(int syscall_number, ...) {
  long ret;
  __asm__(
  "mov    %%rdi,%%rax;"
  "mov    %%rsi,%%rdi;"
  "mov    %%rdx,%%rsi;"
  "mov    %%rcx,%%rdx;"
  "mov    %%r8,%%r10;"
  "mov    %%r9,%%r8;"
  "mov    0x8(%%rsp),%%r9;"
  "syscall;"
  "cmp    $0xfffffffffffff001,%%rax;"
  :"=r"(ret)
  );

  return ret;
}

int my_fork() {
  return sys_call(__NR_fork);
}

int my_read(int fd, char *c, int size) {
  return sys_call(__NR_read, fd, c, size);
}

int my_write(int fd, char *c, int size) {
  return sys_call(__NR_write, fd, c, size);
}

int my_putchar(int c) {
  char ch = c;
  if (my_write(1, &ch, 1) == 1)
    return c;

  return 0;
}

int my_puts(const char *s) {
  int ret;
  while (*s) {
    if ((ret = my_putchar(*s)) != *s)
      return EOF;
    s++;
  } 
  return (my_putchar('\n') == '\n') ? 0 : EOF;
}

int tokenize(char *arg, char *argv[], int max_tokens, char *sep) {
  int i = 0;
  char *saveptr;
  char arr[255] = {0};
  my_strcpy(arr, arg);
  char *token = my_strtok_r(arr, sep, &saveptr);
  while (token != NULL && i < max_tokens - 1) {
    argv[i] = my_malloc(my_strlen(token) + 1);
    my_strcpy(argv[i], token);
    token = my_strtok_r(NULL, sep, &saveptr);
    i++;
  }

  argv[i] = NULL;
  return i;
}

size_t my_strspn(char *s1, char *s2)
{
    size_t ret=0;
    while(*s1 && my_strchr(s2,*s1++))
        ret++;
    return ret;    
}

size_t my_strcspn(char *s1, char *s2)
{
    size_t ret=0;
    while(*s1)
        if(my_strchr(s2,*s1))
            return ret;
        else
            s1++,ret++;
    return ret;
}

char *my_strchr(char *s, int c)
{
    char* ret=0;
    do {
        if( *s == (char)c )
            ret=s;
    } while(*s++);
    return ret;
}

void build_argv(char *input, char *arg, char *argv[]) {
  argv[0] = my_malloc(my_strlen(input) + 1);
  my_strcpy(argv[0], input);
  tokenize(arg, &argv[1], 49, " ");
}

int my_strlen(char *str) {
  int i;
  for (i = 0; *str != '\0'; str++)
    i++;
  return i;
}

int my_strncmp (const char *f_str, const char *s_str, size_t n) {
  for ( ; n > 0; f_str++, s_str++, n--) { 
    if (*f_str != *s_str) { 
      return (*(unsigned char *)f_str < *(unsigned char *)s_str) ? -1 : 1;
    }
    else if(*f_str == '\0') {
      return 0;
    } 
  }
  return 0;
}

int my_strcmp (const char *f_str, const char *s_str) {

  while (*f_str && (*f_str == *s_str)) {
    f_str++; s_str++;
  }
  return (*(unsigned char *)f_str - *(unsigned char *)s_str);
}

void *my_memset(void *dest, int ch, size_t num_bytes) {
  char *tmp = dest;
  while (num_bytes) {
    *tmp++ = ch;
    num_bytes--;
  }
  return dest;
}

char *my_strtok_r(char *str, char *delim, char **nextp) {
  char *ret;
  if (str == NULL)
    str = *nextp;

  str += my_strspn(str, delim);
  if (*str == '\0')
    return NULL;

  ret = str;
  str += my_strcspn(str, delim);
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
    if (my_strlen(s1) < my_strlen(s2))
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

  if (match_len != my_strlen(str2))
    ret = NULL;

  return ret;
}

void handle_cd(char *path) {
  
  int ret;
  ret = my_chdir(path);
  if (ret == -1)
    printf("sbush: cd: %s: No such file or directory\n", path); 
}

void handle_cwd() {

  char buff[1024] = {0};
  if (my_getcwd(buff, sizeof(buff)) != NULL)
    printf ("%s\n", buff);
}

void handle_ls() {
#if 0
  char buff[1024] = {0};
  if (my_getcwd(buff, sizeof(buff)) != NULL) {

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
#endif
}

int get_command(char *cmd) {

  /* built-in */
  if (my_strcmp(cmd, "cd") == 0)
    return CMD_CD;

  /* built-in */
  else if (my_strcmp(cmd, "exit") == 0)
    return CMD_EXIT;

  else
    return CMD_UNKNOWN;
}

void get_path_string(char *cmd, char *path_value) {

  char *ptr = NULL;
  if ((my_strstr(cmd, "$PATH")) != NULL) {
    /*
     * 2 cases. eg: PATH=$PATH:/bin:/usr/bin 
     * $PATH anywhere else. beginning or somewhere else
     */ 
    char *path = my_strstr(cmd, "=");
    path++; 
    char *temp = path;
    int len = my_strlen(path);

    ptr = my_strstr(path, "$PATH");

    char *sys_env = getenv("PATH");

    /* $PATH in the beginning */
    if (temp == ptr) {
      m_strncpy(path_value, sys_env, my_strlen(sys_env));
      if (len - 6 > 0) { //6 len of $PATH:
        m_strcpy(path_value+my_strlen(sys_env), ptr+5);
      } 
    } else {
      /* $PATH anywhere else */
      m_strncpy(path_value, temp, ptr-temp); 
      m_strncpy(path_value + (ptr - temp), sys_env, my_strlen(sys_env));
      m_strcpy(path_value + (ptr - temp) + my_strlen(sys_env), ptr + 5);
    }

  } else {
    /* eg: PATH=/usr/bin */ 
    ptr = my_strstr(cmd, "=");
    m_strcpy(path_value, ptr + 1); 
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
  for (i = 0; i < len && src[i] != '\0'; i++)  {
    dest[i] = src[i];
  }
  return dest;
}

int check_if_path_cmd(char *cmd) {
  return my_strncmp(cmd, "PATH=", 5) == 0 ? 1 : 0; 
}

int check_if_ps1_cmd(char *cmd) {
  return my_strncmp(cmd, "PS1=", 4) == 0 ? 1 : 0; 
}

/*
 * if & is found, replace with '\0' and return true
 */ 
int update_if_bg_cmdarg(char *cmd_arg) {
  char *amp;
  int ret = 0;
  if ((amp = my_strstr(cmd_arg, "&")) != NULL) {
    *amp = '\0'; 
    ret = 1;
  }

  return ret; 
}

/* Not shell built-in commands, call exec */
void execute_non_builtin(char *cmd, char *cmd_arg) {
  pid_t pid;
  int i, status, bg_process = 0;
  char *argv[50] = {0};
  char path_value[1024] = {0} ; 
  
  /* PATH variable set */
  if (check_if_path_cmd(cmd)) {
    get_path_string(cmd, path_value); 
    my_setenv("PATH", path_value, 1);
    return;
  }
  /* PS1 variable set */ 
  else if (check_if_ps1_cmd(cmd)) {
    m_strcpy(ps1_variable, my_strstr(cmd, "=") + 1);
    return;
  }
  /* command & handling, true if & is found in the command */
  else if (update_if_bg_cmdarg(cmd_arg)) {
    bg_process = 1;
  }
 
  build_argv(cmd, cmd_arg, argv);

  pid = my_fork();
  if (pid == 0) {
    if (execvp(cmd, argv) < 0) {
      printf("%s: command not found\n", cmd);
      exit(1);
    }
  } else {
    if (pid < 0) {
      printf("Fork failed\n");
      exit(1);
    }
    else {
      if (!bg_process)
        my_waitpid(-1, &status, 0);
    }
  }

  /* Freeing */
  i = 0;
  while (argv[i]) {
    my_free(argv[i]);
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

    case CMD_UNKNOWN:
      execute_non_builtin(cmd, cmd_arg);
      break;

    default:
      printf("%s: command not found\n", cmd);
      break;
  }
}

/* Read from the file one line at a time and execute */
void read_from_file(int num_tokens, char *cmd_tokens[]) {

  int file = my_open(cmd_tokens[1], O_RDONLY);
  char code[1024] = {0};
  size_t n = 0;
  char c;

  if (file == -1)
    return;

  while (read(file, &c, 1) > 0)
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
  int num_cmds = tokenize(arg, &argv[0], 50, "|");
  piped_commands cmds[num_cmds];
  my_memset(cmds, 0, sizeof(cmds));

  for (i = 0; i < num_cmds; i++) {
    tokenize(argv[i], cmds[i].commands, 50, " ");
  }
 
  execute_piped_commands(num_cmds - 1, cmds);

  /* Freeing */
  i = 0;
  while (*(cmds[i].commands)) {
    my_free(*(cmds[i].commands));
    *(cmds[i].commands) = NULL;
    i++;
  }
  i = 0;
  while(argv[i]) {
    my_free(argv[i]);
    argv[i] = NULL;
    i++;
  }
}

void read_from_stdin() {
  int cnt;
  char *str, *saveptr, *token;
  char buff[1024] = {0};
  while (my_read(0, buff, sizeof(buff)) > 0) {

    cnt = 1;
    size_t buff_length = my_strlen(buff);
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

    my_puts(ps1_variable);
    /*
    printf("sbush> ");
    fflush(stdout);
    */

    //exit(1); /* TODO : REMOVE, JUST A TEST (VALGRIND) */
    my_memset(buff, 0, sizeof(buff));
  }
}

int process_start(int input_fd, int output_fd, piped_commands *cmds) {
  int status;
  pid_t pid;
  if ((pid = my_fork()) == 0) {

    if (input_fd != 0) {
      my_dup2(input_fd, 0);
      my_close(input_fd);
    }

    if (output_fd != 1) {
      my_dup2(output_fd, 1);
      my_close(output_fd);
    }

    return execvp(cmds->commands[0], cmds->commands);

  } else {
    my_waitpid(-1, &status, 0);
  }

  return pid;
}

int execute_piped_commands(int num_pipes, piped_commands *cmds) {
  int i = 0;
  int input_fd = 0; /* stdin */
  int fds[2];
  int status;
  pid_t pid;

  while (i < num_pipes) {
    if (my_pipe(fds) != 0)
      return 1;

    process_start(input_fd, fds[1], cmds + i);
    my_close(fds[1]);

    input_fd = fds[0];
    i++;
  }

  /* last command */
  pid = my_fork();
  if (pid == 0) {

    my_dup2(input_fd, 0);
    my_close(input_fd);

    if (execvp(cmds[i].commands[0], cmds[i].commands) < 0) {
      printf("failed");
      exit(1);
    }

  } else {
    my_waitpid(-1, &status, 0);
  }

  return 0;
}

char *my_getenv(const char *arg) {
  int i;
  for (i = 0; m_environ[i] !=0 ; i++) {
    if (my_strncmp(m_environ[i], "PATH=", 5) == 0) { 
      return m_environ[i];
    } 
  }
 return NULL;
}

int my_setenv(char *path_variable, char *value, int overwrite) {
   //setenv("PATH", path_value, 1);
   //overwrite variable is not used now.
  int i;
  int var_len = my_strlen(path_variable);
  int value_len = my_strlen(value);

  for (i = 0; m_environ[i] !=0 ; i++) {

    if (my_strncmp(m_environ[i], "PATH=", 5) == 0) { 
       /*
	* first free the value. Then allocate for new value
	*/
       my_free(m_environ[i]);
       m_environ[i]= my_malloc(value_len + var_len + 2);//include size of "=" as well.
	
       /*
	* copy the complete value in 3 steps. eg: PATH=/usr/bin
	*/

       m_strncpy(m_environ[i], path_variable, var_len);	
       m_strncpy(m_environ[i] + var_len, "=", 1);	
       m_strcpy(m_environ[i] + var_len + 1, value);

       return 0;
    }
  }
  return 1;
}

int main(int argc, char *argv[], char *envp[]) {

  m_environ = envp;
 // my_setenv("PATH",my_getenv("PATH"),1);
  my_puts("sbush> ");

  if (argc > 1) {

    /* Case 1 : Non-interactive mode */
    read_from_file(argc, argv);

  } else {

    /* Case 2 : Interactive mode */
    read_from_stdin();
  }

  return 0;
}

