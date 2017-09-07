#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define CMD_UNKNOWN      0
#define CMD_CD           1
#define CMD_CWD          2
#define CMD_LS           3
#define CMD_EXIT         4

#define	PROT_READ       0x1
#define	PROT_WRITE      0x2
#define	MAP_PRIVATE     0x02
#define MAP_ANONYMOUS   0x20
#define MAP_FAILED      ((void *)-1)

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002

#define S_IREAD         0000400
#define S_IWRITE        0000200

#define __NR_read       0
#define __NR_write      1
#define __NR_open       2
#define __NR_close      3
#define __NR_mmap       9
#define __NR_munmap     11
#define __NR_pipe       22
#define __NR_dup2       33
#define __NR_fork       57
#define __NR_execve     59	
#define __NR_exit       60
#define __NR_wait4      61
#define __NR_getdents   78
#define __NR_getcwd     79
#define __NR_chdir      80
#define __NR_getdents64 217
#define __NR_waitid     247

typedef struct piped_commands {
  char *commands[50];
} piped_commands;

struct linux_dirent64 {
  unsigned long  d_ino;    /* 64-bit inode number */
  unsigned long  d_off;    /* 64-bit offset to next structure */
  unsigned short d_reclen; /* Size of this dirent */
  unsigned char  d_type;   /* File type */
  char           d_name[]; /* Filename (null-terminated) */
};


char **m_environ;

long sys_call(int syscall_number, ...);
//DIR *opendir(const char *name); /* TODO */
//struct dirent *readdir(DIR *dirp); /* TODO */
//int closedir(DIR *dirp); /* TODO */

int do_execute(char *cmd, char *cmd_path[], char *env[]); 


char *getenv(const char *name);
int setenv(char *name, char *value, int overwrite);
char *getcwd(char *buf, size_t size);
void exit(int status);
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
void *malloc(size_t sz);
void free(void *mem_ptr);
int  munmap(void *addr, size_t length);
int  waitpid(int pid, int *st_ptr, int options);
int  chdir(const char *path);
int  dup2(int oldfd, int newfd);
int  pipe(int pipefd[2]);
int  open(const char *pathname, int flags);
int  close(int fd);
pid_t fork();
ssize_t read(int fd, void *c, size_t size);
int  putchar(int c);
int  puts_nonewline(const char *s);
int  puts(const char *s);

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

void print_prompt() {
  //puts_nonewline(ps1_variable);
  //puts_nonewline(" ");
  puts(ps1_variable);
}

int getdents64(int fd, struct linux_dirent64 *dirp, int count) {
  return sys_call(__NR_getdents64, fd, dirp, count);
}


//void exit(int status) {
 // sys_call(__NR_exit, status);
//}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
  return (void *)sys_call(__NR_mmap, addr, length, prot, flags, fd, offset);
}

int munmap(void *addr, size_t length) {
  return sys_call(__NR_munmap, addr, length);
}

void *malloc(size_t sz) {
  int *mem_ptr;
  mem_ptr = (int *)mmap(0, sz + sizeof(sz), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if (mem_ptr == MAP_FAILED)
    return NULL;

  *mem_ptr = sz + sizeof(sz);
  return (void *)(mem_ptr + 1);
}

void free(void *mem_ptr) {
  if (mem_ptr) {
    munmap((void *)mem_ptr, *((int *)mem_ptr - 1));
  }
}

int waitpid(int pid, int *st_ptr, int options) {
  return sys_call(__NR_wait4, pid, st_ptr, options, NULL);
}

int chdir(const char *path) {
  return sys_call(__NR_chdir, path);
}

char *getcwd(char *buf, size_t size) {
  return (char *)sys_call(__NR_getcwd, buf, size);
}

int dup2(int oldfd, int newfd) {
  return sys_call(__NR_dup2, oldfd, newfd);
}

int pipe(int pipefd[2]) {
  return sys_call(__NR_pipe, pipefd);
}

int open(const char *pathname, int flags) {
  return sys_call(__NR_open, pathname, flags);
}

int close(int fd) {
  return sys_call(__NR_close, fd);
}

int execve(const char *filename, char *const argv[], char *const envp[]) {
  return sys_call(__NR_execve, filename, argv, envp);
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

pid_t fork() {
  return sys_call(__NR_fork);
}

ssize_t read(int fd, void *c, size_t size) {
  return sys_call(__NR_read, fd, c, size);
}

ssize_t write(int fd, const void *c, size_t size) {
  return sys_call(__NR_write, fd, c, size);
}


int puts_nonewline(const char *s) {
  int ret;
  while (*s) {
    if ((ret = putchar(*s)) != *s)
      return EOF;
    s++;
  } 
  return 0;
}

int puts(const char *s) {
  if (puts_nonewline(s) < 0)
    return EOF;

  return (putchar('\n') == '\n') ? 0 : EOF;
}

/*manu*/
int tokenize(char *arg, char *argv[], int max_tokens, char *sep); 

int find_path_and_exe(char *cmd, char *argv[], char *env[]) {

  char *slash = strstr(cmd, "/");
  /* commands like ls */
  if (slash == 0) {

    return do_execute(cmd, argv, env);

  }
  /*complete path: eg: /bin/ls */

  if(execve(cmd, argv, env) < 0) {
   return -1;
  }
  return -1;
}

int do_execute(char *cmd, char *argv[], char *env[]) {

  char *path_env = getenv("PATH");
  char *paths[50] = {0};
  char exe_path[255] = {0};

  int i, len, ret = 1;
  int num_paths  = tokenize(path_env + 5, paths, 50, ":");
  /* 
   * iterate and find out the path of cmd
   */
  for (i = 0; i < num_paths; i++) {
    len = strlen(paths[i]);

    strncpy(exe_path, paths[i], len);    
    strncpy(exe_path + len, "/", 1);
    strcpy(exe_path + len + 1, cmd);

    if(execve(exe_path, argv, env) < 0) {
    //  printf("command to try : %s\n", exe_path);
      continue;
    }	
  }
    if (i == num_paths)
	ret = -1; 
  return ret;
}

/*manu*/
int tokenize(char *arg, char *argv[], int max_tokens, char *sep) {
  int i = 0;
  char *saveptr;
  char arr[255] = {0};
  strcpy(arr, arg);
  char *token = strtok_r(arr, sep, &saveptr);
  while (token != NULL && i < max_tokens - 1) {
    argv[i] = malloc(strlen(token) + 1);
    strcpy(argv[i], token);
    token = strtok_r(NULL, sep, &saveptr);
    i++;
  }

  argv[i] = NULL;
  return i;
}

void build_argv(char *input, char *arg, char *argv[]) {
  argv[0] = malloc(strlen(input) + 1);
  strcpy(argv[0], input);
  tokenize(arg, &argv[1], 49, " ");
}


void handle_cd(char *path) {
  
  int ret;
  ret = chdir(path);
  if (ret != 0) {
    puts_nonewline("sbush: cd: ");
    puts_nonewline(path); 
    puts(": No such file or directory"); 
  }
}

void handle_cwd() {
  char buff[1024] = {0};
  if (getcwd(buff, sizeof(buff)) != NULL)
    puts(buff);
}

void handle_ls() {

  char buff[1024] = {0};
  if (getcwd(buff, sizeof(buff)) == NULL)
    return;

  int fd;
  int ret;
	int i = 0;
	char buf[1024];
	struct linux_dirent64 *d_ent;
	fd = open(buff, O_RDONLY);
  ret = getdents64(fd, (struct linux_dirent64 *)buf, 1024);

	while (i < ret) {
		d_ent = (struct linux_dirent64 *) (buf + i);
    if ((d_ent->d_name)[0] != '.')
      puts(d_ent->d_name);

		i += d_ent->d_reclen;
	}

#if 0
  char buff[1024] = {0};
  if (getcwd(buff, sizeof(buff)) != NULL) {

    DIR *dir;
    dir = opendir(buff);
    if (dir) {

      struct dirent *d_entry;
      while ((d_entry = readdir(dir))) {
        
        /* For now, ignoring invisible files */
        if (*d_entry->d_name != '.')
          puts(d_entry->d_name);
      }

      closedir(dir);
    }
  }
#endif
}

int get_command(char *cmd) {

  /* built-in */
  if (strcmp(cmd, "cd") == 0)
    return CMD_CD;

  /* built-in */
  else if (strcmp(cmd, "exit") == 0)
    return CMD_EXIT;

  else
    return CMD_UNKNOWN;
}

void get_path_string(char *cmd, char *path_value) {

  char *ptr = NULL;
  if ((strstr(cmd, "$PATH")) != NULL) {
    /*
     * 2 cases. eg: PATH=$PATH:/bin:/usr/bin 
     * $PATH anywhere else. beginning or somewhere else
     */ 
    char *path = strstr(cmd, "=");
    path++; 
    char *temp = path;
    int len = strlen(path);

    ptr = strstr(path, "$PATH");

    char *sys_env = getenv("PATH");

    /* $PATH in the beginning */
    if (temp == ptr) {
      strncpy(path_value, sys_env, strlen(sys_env));
      if (len - 6 > 0) { //6 len of $PATH:
        strcpy(path_value+strlen(sys_env), ptr+5);
      } 
    } else {
      /* $PATH anywhere else */
      strncpy(path_value, temp, ptr-temp); 
      strncpy(path_value + (ptr - temp), sys_env, strlen(sys_env));
      strcpy(path_value + (ptr - temp) + strlen(sys_env), ptr + 5);
    }

  } else {
    /* eg: PATH=/usr/bin */ 
    ptr = strstr(cmd, "=");
    strcpy(path_value, ptr + 1); 
  }
}


int check_if_path_cmd(char *cmd) {
  return strncmp(cmd, "PATH=", 5) == 0 ? 1 : 0; 
}

int check_if_ps1_cmd(char *cmd) {
  return strncmp(cmd, "PS1=", 4) == 0 ? 1 : 0; 
}

/*
 * if & is found, replace with '\0' and return true
 */ 
int update_if_bg_cmdarg(char *cmd_arg) {
  char *amp;
  int ret = 0;
  if ((amp = strstr(cmd_arg, "&")) != NULL) {
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
    setenv("PATH", path_value, 1);
    return;
  }
  /* PS1 variable set */ 
  else if (check_if_ps1_cmd(cmd)) {
    strcpy(ps1_variable, strstr(cmd, "=") + 1);
    return;
  }
  /* command & handling, true if & is found in the command */
  else if (update_if_bg_cmdarg(cmd_arg)) {
    bg_process = 1;
  }
 
  build_argv(cmd, cmd_arg, argv);

  pid = fork();
  if (pid == 0) {
     if (find_path_and_exe(cmd, argv, m_environ) < 0) {
  //  if (execvp(cmd, argv) < 0) {
      puts_nonewline(cmd);
      puts(": command not found");
      exit(1);
    }
  } else {
    if (pid < 0) {
      exit(1);
    }
    else {
      if (!bg_process)
        waitpid(-1, &status, 0);
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

    token = strtok_r(str, " ", &saveptr);
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
      puts_nonewline(cmd);
      puts(": command not found");
      break;
  }
}

/* Read from the file one line at a time and execute */
void read_from_file(int num_tokens, char *cmd_tokens[]) {

  int file = open(cmd_tokens[1], O_RDONLY);
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
  int i = 0;
  int num_cmds  = tokenize(arg, &argv[0], 50, "|");
  int num_pipes = num_cmds - 1;
  piped_commands cmds[num_cmds];
  memset(cmds, 0, sizeof(cmds));

  while (i < num_cmds) {
    tokenize(argv[i], cmds[i].commands, 50, " ");
    i++;
  }
 
  execute_piped_commands(num_pipes, cmds);

  /* Freeing */
  i = 0;
  while (i < num_cmds) {
    int j = 0;
    while(cmds[i].commands[j]) {
      free(cmds[i].commands[j]);
      cmds[i].commands[j] = NULL;
      j++;
    }
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
  while (read(0, buff, sizeof(buff)) > 0) {

    cnt = 1;
    size_t buff_length = strlen(buff);
    if (buff_length != 0 && buff[buff_length - 1] == '\n') {

      buff_length--;
      buff[buff_length] = '\0';
    }

    if (strstr(buff, "|")) {
      handle_piped_commands(buff);
    }  else {
      str = buff;
      while (1) {
        token = strtok_r(str, " ", &saveptr);
        if (token == NULL)
          break;

        if (cnt == 1)
          execute_commands(token, saveptr);

        cnt++;
        str = NULL;
      }
    }

    print_prompt();

    memset(buff, 0, sizeof(buff));
  }
}

int process_start(int input_fd, int output_fd, piped_commands *cmds) {
  int status;
  pid_t pid = fork();
  if (pid == 0) {

    if (input_fd != 0) {
      dup2(input_fd, 0);
      close(input_fd);
    }

    if (output_fd != 1) {
      dup2(output_fd, 1);
      close(output_fd);
    }
    return find_path_and_exe(cmds->commands[0], cmds->commands, m_environ);
    //return execvp(cmds->commands[0], cmds->commands);

  } else {
    waitpid(-1, &status, 0);
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
    if (pipe(fds) != 0)
      return 1;

    process_start(input_fd, fds[1], cmds + i);
    close(fds[1]);

    input_fd = fds[0];
    i++;
  }

  /* last command */
  pid = fork();
  if (pid == 0) {

    dup2(input_fd, 0);
    close(input_fd);

    if (find_path_and_exe(cmds[i].commands[0], cmds[i].commands, m_environ)) {
      exit(1);
    }

  } else {
    waitpid(-1, &status, 0);
  }

  return 0;
}

char *getenv(const char *arg) {
  int i;
  for (i = 0; m_environ[i] !=0 ; i++) {
    if (strncmp(m_environ[i], "PATH=", 5) == 0) { 
      return m_environ[i];
    } 
  }
 return NULL;
}

int setenv(char *path_variable, char *value, int overwrite) {
  //setenv("PATH", path_value, 1);
  //overwrite variable is not used now.
  int i;
  int var_len = strlen(path_variable);
  int value_len = strlen(value);

  for (i = 0; m_environ[i] !=0 ; i++) {

    if (strncmp(m_environ[i], "PATH=", 5) == 0) { 
      /*
       * first free the value. Then allocate for new value
       */
      free(m_environ[i]);
      m_environ[i]= malloc(value_len + var_len + 2);//include size of "=" as well.

      /*
       * copy the complete value in 3 steps. eg: PATH=/usr/bin
       */
      strncpy(m_environ[i], path_variable, var_len);	
      strncpy(m_environ[i] + var_len, "=", 1);	
      strcpy(m_environ[i] + var_len + 1, value);

      return 0;
    }
  }
  return 1;
}

int main(int argc, char *argv[], char *envp[]) {

  m_environ = envp + argc + 1;
  //find_path_and_exe("/home/manmathew/main/path/a.sh",argv,envp);
 // setenv("PATH",getenv("PATH"),1);

  if (argc > 1) {

    /* Case 1 : Non-interactive mode */
    read_from_file(argc, argv);

  } else {

    /* Case 2 : Interactive mode */
    print_prompt();
    read_from_stdin();
  }

  return 0;
}

