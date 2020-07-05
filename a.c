#include <sys/wait.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include <history.h>
#include"mytop.h"
#include <pwd.h>



#define MAX_CMD 100
#define MAX_DIR_NAME 100

int buildin_command(char **cmd);
char tmp[100];
void test2(char * file_name);
void outerexe(char **argv,int argc);
struct history{
	char a[100];
	struct history *next;
};

void outerexe(char **argv,int argc){
    int sign=0;
    int flag=0;
    int fd;
    pid_t pid;
    int statu;
    char* temp[MAX_CMD];
    char* file;
    char *temp1[MAX_CMD];
    for(int i=0;i<argc;i++){
        temp[i]=(char *)argv[i];}
    temp[argc]=NULL;
    int x;
    for(int i=0;i<argc;i++){
        if(strcmp(temp[i],">")==0){
            file=temp[i+1];
            flag=1;
            x=i;
        }
        if(strcmp(temp[i],"<")==0)
		        {
	    		file = temp[i+1];
		    	flag = 2;
		      	x= i;
		        }
        if(strcmp(temp[i],"|")==0){
           int a=0;
           for(int j=i+1;j<argc;j++,a++){
               temp1[a]=temp[j];

           }
           temp1[a]=NULL;
           flag=3;
           x=i;
        }
        if(strcmp(temp[i],"&")==0){
            temp[i]=NULL;
            sign=1;
           flag=4;
            x=i;
        }

    }
    if(flag!=0){
        temp[x]=NULL;
    }

    pid=fork();
    if(pid==0){
        switch (flag){
            case 0:{
                execvp(temp[0],temp);
                exit(0);
                break;
            }
            case 1:{
                fd=open(file,O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
                dup2(fd,1);
                execvp(temp[0],temp);
                exit(0);
                break;
            }
            case 2:
				{
					if((fd = open(file,O_RDONLY,0644)) < 0)
						printf("openfile error\n");
					close(0);//关闭标准输入
					dup2(fd,0);//将该文件对应到标准输入
					execvp(temp[0],temp);
					exit(0);
					break;
				}

           
            case 3:
            {
                int pid2;
                int satu;
                int fd2;
                if((pid2 = fork()) < 0)
                    perror("fork error");
                if(pid2 == 0)
                {
                    if((fd2 = open("/tmp/1.txt",O_WRONLY|O_CREAT|O_TRUNC,0644)) < 0)
                        perror("open");
                    dup2(fd2,1);
                    execvp(temp[0],temp);
                    exit(0);
                }
                waitpid(pid2,&satu,0);
                close(fd2);
                fd2 = open("/tmp/1.txt",O_RDONLY);
                 dup2(fd2,0);
                execvp(temp1[0],temp1);
                if(remove("/tmp/1.txt") < 0)
                    perror("remove error");
                exit(0);
                break;
            }
            case 4:{
                execvp(temp[0],temp);
                int devnullfd;
                devnullfd = open("/dev/null", 0);
                dup2(devnullfd, 0);
                dup2(devnullfd, 1);
                dup2(devnullfd, 2);
               // 处理SIGCHLD信号
                signal(SIGCHLD,SIG_IGN);

                return;
            }
            default:printf("wrong input!\n");
               



        }
       
        //if(sign == 0)
           
    }
if(sign==0)
    waitpid(pid,&status,0);


}

void test1(char *buf){
    char cmds[MAX_CMD];
    char *argv[MAX_CMD];
    //memset(argv,0, sizeof(argv));
    strcpy(cmds,buf);
    //printf("%s",cmds);
    while(*buf==' '){
        buf++;
    }
    int i = 0;

    int argc = 0;
    while(*buf!='\n'){

        while(buf[i]!='\n'&&buf[i]!=' '){
           i++;
        }

        if(buf[i] == '\n'){
            buf[i] = '\0';
            argv[argc++] = buf;
            break;
        }
        buf[i] = '\0';
        argv[argc++] = buf;

        buf+=i+1;
       
        i = 0;
        while(*buf==' '){
            buf++;
        }
    }
    
    argv[argc] = NULL;
    
    if(buildin_command(argv)){return ;}
   
    outerexe(argv,argc);
    
    //pid_t pid = fork();
    //if(pid == 0){
           //   execvp(argv[0],argv); 
             // exit(1);
         // }

//wait(&pid);    
    }
    
    


char history[MAX_CMD][MAX_CMD];
int main(int argc,char *argv[])
{char cmdstring[MAX_CMD];

 
    int n;
    //存储历史
    int his_count=0;      //历史数量
    
    int argcout = 0;        //命令计数器
    while(1) {
    
      
        memset(cmdstring,0,sizeof(cmdstring));
        
	
        printf("your cmd>>");
        fflush(stdout);
        
        /*read*/
        if((n=read(0,cmdstring,MAX_CMD))<0){
            printf("read error");
        }
      strcpy(history[his_count],cmdstring);
       
       
       
        test1(cmdstring);
        his_count++;
        
}
}





int buildin_command(char **argv){   
    if(strcmp(argv[0],"exit")==0){
        exit(0);
    }
    if(strcmp(argv[0],"history")==0){
        for(int i=atoi(argv[1]);i>=0;i--){
        printf("%s",history[i]);
        }
        return 1;
    }
  if(strcmp(argv[0],"mytop")==0){
        mytop();
    }
        
    
    if(strcmp(argv[0],"cd")==0){
        if(chdir(argv[1]))
            printf("myselous:cd:%s:no such directory\n",argv[1]);
        
        return 1;
    }
    if(strcmp(argv[0],"pwd")==0){
        char buf[MAX_DIR_NAME];
        printf("%s\n",getcwd(buf,sizeof(buf)));
        return 1;
    }
    return 0;
}
