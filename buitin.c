#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <makefile.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/prctl.h>


void quit()
{
  exit(0);
}
struct jobs
{
  char name[80];
  int id;
};
void pinfo(char **args)
{
  int i=0,j;
  int k,len;
  char str[1024],stat[10000],*token,s[1024];
  while(args[i]!=NULL)
    i++;
  if(i==1)
    {
      k=getpid();
      sprintf(str,"/proc/%d/status",k);
      FILE *p=fopen(str,"r");
      if(p)
          fread(stat,1,sizeof(stat),p);
      else
      {
        fprintf(stderr,"Invalid pid\n");
        return;
      }
      token=strtok(stat,del);
      j=0;
      while(token!=NULL)
      {
        if(j==0||j==1||j==4||j==16)
          printf("%s\n",token);
        j++;
        token=strtok(NULL,del);
      }
      fclose(p);
      sprintf(str,"/proc/%d/exe",k);
      len=readlink(str, s, 1024);
      s[len]='\0';
      printf("Executable path:%s\n",s);
    }
    else
    {
      sprintf(str,"/proc/%s/status",args[1]);
      FILE *p=fopen(str,"r");
      if(p)
          fread(stat,1,sizeof(stat),p);
      else
      {
          fprintf(stderr,"Invalid pid\n");
          return;
      }
      token=strtok(stat,del);
      j=0;
      while(token!=NULL)
      {
        if(j==0||j==1||j==4||j==16)
          printf("%s\n",token);
        j++;
        token=strtok(NULL,del);
      }
      fclose(p);
      sprintf(str,"/proc/%s/exe",args[1]);
      len=readlink(str, s, 1024);
      s[len]='\0';
      printf("Executable path:%s\n",s);
    }
}

void pwd()
{
  char result[1024],*token;
  getcwd(result,sizeof(result));
	int pos=0,count=0;
  register struct passwd *pw;
  register uid_t uid;
  uid = geteuid();
  pw = getpwuid(uid);
  token=strtok(result+1,delim);
  while(token!=NULL)
  {
    if(strcmp(token,"home")==0)
    {
      token=strtok(NULL,delim);
      if(strcmp(token,pw->pw_name)==0)
         printf("~");
    }
    else
      printf("/%s",token);
    token=strtok(NULL,delim);
  }
}

void echo(char **args)
{
  int i=1,j;
  while(args[i]!=NULL)
  {
    for(j=0;j<strlen(args[i]);j++)
    {
      if((args[i][j]!='\"'&&args[i][j]!='\''))
        printf("%c",args[i][j]);
    }
    i++;
  }
  printf("\n");
}

void cd(char **tokens)
{
  char cwd[200]={0},cd[200];
  register struct passwd *pw;
  register uid_t uid;
  int i=0,err;
  while(tokens[i]!=NULL)
    i++;
  if(i==1)
  {
    strcat(cwd,"/home/");
    uid = geteuid();
    pw = getpwuid(uid);
    strcat(cwd,pw->pw_name);
    err=chdir(cwd);
  }
  else if(tokens[1][0]=='~')
  {
    strcat(cwd,"/home/");
    uid = geteuid();
    pw = getpwuid(uid);
    strcat(cwd,pw->pw_name);
    strcat(cwd,tokens[1]+1);
    err=chdir(cwd);
    if(err==-1)
      fprintf(stderr,"No directory present!\n");
  }
  else if(tokens[1][0]=='.')
  {
    if(tokens[1][1]=='.')
    {
      int i,j;
      getcwd(cwd,sizeof(cwd));
      for(i=strlen(cwd)-1;i>=0;i--)
      {
        if(cwd[i]=='/')
          break;
      }
      for(j=0;j<i;j++)
      {
        cd[j]=cwd[j];
      }
      strcat(cd,tokens[1]+2);
      err=chdir(cd);
      if(err!=0)
        fprintf(stderr,"No directory present!\n");
    }
    else
    {
      getcwd(cwd,sizeof(cwd));
      strcat(cwd,"/");
      strcat(cwd,tokens[1]);
      err=chdir(cwd);
      if(err!=0)
        fprintf(stderr,"No directory present!\n");
    }
  }
  else if(tokens[1][0] != '/')
  {
      getcwd(cwd,sizeof(cwd));
      strcat(cwd,"/");
      strcat(cwd,tokens[1]);
      err=chdir(cwd);
      if(err!=0)
        fprintf(stderr,"No directory present!\n");
  }
  else
  {
    err=chdir(tokens[1]);
    if(err!=0)
      fprintf(stderr,"No directory present!\n");
  }
}

void jobs()
{
  bg *temp=background;
  int i=1;
  while(temp!=NULL)
  {
    fprintf(stdout, "[%d]%s[%d]\n",i++,temp->name,temp->pid );
    temp=temp->next;
  }
}

void killallbg()
{
  bg *temp=background;
  int t,flag=0;
  while(temp!=NULL)
  {
    t=kill(temp->pid,9);
    if(t<0)
    {
      flag=1;
      perror("Process could  not be killed\n");
    }
    temp=temp->next;
  }
  if(flag==0)
    printf("All backgrounfd processes have been killed!\n");
  else
    printf("Unable to killallbg\n");
  background=temp;
}

void kjobs(char **tokens)
{
  bg *temp=background;
  int jn=atoi(tokens[1]);
  pid_t pid;
  int signal=atoi(tokens[2]);
  int t=jn,i;
  while(temp!=NULL && --jn)
    temp=temp->next;
  if(temp!=NULL)
    pid=temp->pid;
  else
    pid=-1;
  for(i=0;tokens[i]!=NULL;i++);

  if(i!=3)
      fprintf(stderr,"Invalid number of args\n");
  else if(pid!=-1)
      {
        if(kill(pid,signal)<0)
          perror("Signal not sent");
        else
        {
          printf("KILLED %s with pid [%d] and job number %d\n",temp->name, pid,t);
          delete(pid);
        }
      }
  else
        fprintf(stderr,"No such job number\n");
}
void fg(char **tokens)
{
  int i;
  pid_t pid,pgid,child_pid;
  //int shell;
  //pid_t shell_pgid;
  int status;
  bg *temp =background;
  int t=atoi(tokens[1]);
  for(i=0;tokens[i]!=NULL;i++);
  if(i==2)
  {
    while(temp!=NULL && --t)
      temp=temp->next;
    if(temp!=NULL)
      pid=temp->pid;
    else
      pid=-1;
    if(pid>=0)
    {
      fprintf(stderr, "%s\n", temp->name);
      pgid=getpgid(pid);
      tcsetpgrp(shell,pgid);
      //child_pid=pgid;
      if(killpg(pgid,SIGCONT)<0)
        perror("Cannot Continue");
      waitpid(pid,&status,WUNTRACED);
            //printf("\n");

      if(WIFSTOPPED(status))
      {  
        fprintf(stderr, "\n[%d]+ stopped %s\n",child_pid,temp->name );
        delete(pid);
      }
      else
      {
        delete(pid);
        //child_pid=0;
      }
      tcsetpgrp(shell,shell_pgid);
    }
    else
      fprintf(stderr, "No such job\n" );
  }
  else
      printf("Invalid number of arguments!\n" );
}
    