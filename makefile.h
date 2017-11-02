#define size 1024
#define t_size 100
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define TOK_DELIM " \t\a\n"
void pwd();
void echo(char **args);
void cd(char **tokens);
char *readline();
int check(char *inp);
char **lineparser(char *com);
char **redirparser(char *com);
#define DELIM "><"
void pinfo(char **args);
#define del "\n"
#define PIPE "|"
#define delim "  \n\t/"
void redir_out(char *filename,char *command);
void redir_in(char *filename,char *command);
void redir_app(char *filename,char *command);
int check1(char *inp);
void quit();
char **pipeparser(char *com);
void jobs();
typedef struct bg
{
  char name[1024];
  pid_t pid,pgid;
  struct bg *next;
}bg;
char **ARR;
int PID;

bg * background;
int shell;
pid_t shell_pgid;
void killallbg();
void kjobs(char **tokens);
void delete(pid_t pid);
void fg(char **tokens);
