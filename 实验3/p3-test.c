#include <stdio.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>


#define Concurrency 17 //并发数
#define writetime 12000
#define readtime 8000
#define Blocksize 4096
#define filesize (50 * 1024 *1024 )//文件大小
#define maxline (100 * 1024+26 ) //大于等于块的大小 
#define readbuff (10 * 1024 * 1024)


char examtext[maxline] = "abcdefghijklmnopqrstuvwxyz";//用来写的
struct timeval starttime, endtime, spendtimeSpec;

char buff[maxline];

/*写文件:打开文件，判断返回值，如果正常打开文件就判断是否随机写，进行写操作*/
void write_file(int blocksize, bool isrand, char* filepath)
{
    int fp=open(filepath,O_RDWR | O_CREAT | O_SYNC  , 0755);
    int i=0;

    if(fp >0){
        for(;i<writetime;i++){
           int x=write(fp,examtext,blocksize);
           if(x<0){
             printf("write error!");
             break;
          }
            if(!isrand)
                lseek(fp,rand()%filesize,SEEK_SET);
        }
        
        
        
    }
    else {printf("open error!");}
    lseek(fp,0,SEEK_SET);

}

/*读文件:打开文件，判断返回值，如果正常打开就判断是否随机读，进行读操作*/
void read_file(int blocksize, bool isrand, char *filepath) {
    //to do....
    int fp=open(filepath,O_RDONLY);
    
    int i = 0;
      if(fp>0){
        for (; i < readtime; i++) {
        
            int x=read(fp, buff, blocksize);
            if(x<0){
             printf("read error!\n");
             break;
          }
            if (!isrand)//如果是随机读
                lseek(fp, rand() % filesize, SEEK_SET);//文件偏移量设为offset
        
        }
        }
        else {printf("open error!");}
        lseek(fp, 0, SEEK_SET);//重置指针
    
}
//计算时间差，在读或写操作前后分别取系统时间，然后计算差值即为时间差。
long get_time_left(struct timeval starttime, struct timeval endtime)
{
 //to do....
 long spendtime = 1000 * (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_usec - starttime.tv_usec)/1000; /* ms */
  return spendtime;
}

/*主函数：首先创建和命名文件，通过循环执行read_file和write_file函数测试读写差异。
测试blocksize和concurrency对测试读写速度的影响，最后输出结果。*/
int main()
{   srand((unsigned)time(NULL));
    int i=0;
    
  
   char * filepathram[19]={"/home/myram/ram1.txt","/home/myram/ram2.txt","/home/myram/ram3.txt","/home/myram/ram4.txt","/home/myram/ram5.txt","/home/myram/ram6.txt","/home/myram/ram7.txt","/home/myram/ram8.txt","/home/myram/ram9.txt","/home/myram/ram10.txt","/home/myram/ram11.txt","/home/myram/ram12.txt","/home/myram/ram13.txt","/home/myram/ram14.txt","/home/myram/ram15.txt","/home/myram/ram16.txt","/home/myram/ram17.txt","/home/myram/ram18.txt","/home/myram/ram19.txt"};
   
   
   char * filepathdisk[19]={"/usr/disk1.txt","/usr/disk2.txt","/usr/disk3.txt","/usr/disk4.txt","/usr/disk5.txt","/usr/disk6.txt","/usr/disk7.txt","/usr/disk8.txt","/usr/disk9.txt","/usr/disk10.txt","/usr/disk11.txt","/usr/disk12.txt","/usr/disk13.txt","/usr/disk14.txt","/usr/disk15.txt","/usr/disk16.txt","/usr/disk17.txt","/usr/disk18.txt","/usr/disk19.txt"};
   
  
   
    for(int j=0;j<maxline;){
      strncat(examtext,"hello",5);//字符串接在后面
      j=j+5;
    
    }
    
  
    
    for (int block = 64; block <= Blocksize;)
    {
        for ( int concurrency= 1; concurrency <= Concurrency;)
        {
            gettimeofday(&starttime, NULL);
            for (i = 1; i <=concurrency; i++)
            {
                if (fork() == 0)
                {
                
               //  write_file(block, true, filepathram[i-1]);
                    //ram顺序写
                    
                //write_file(block, false, filepathram[i-1]);
                    //ram随机写
                   
                    read_file(block, true,filepathram[i-1]);
                    //ram顺序读
                   // read_file(block, false, filepathram[i-1]);
                    //ram随机读

                //  write_file(block,true,filepathdisk[i-1]);
                    //磁盘顺序写
                 // write_file(block,false,filepathdisk[i-1]);
                    //磁盘随机写
                  
                    
                   // read_file(block, true,filepathdisk[i-1]);
                    //磁盘顺序读
                  //  read_file(block, false, filepathdisk[i-1]);
                    //磁盘随机读

                    exit(1);
                }
            }
            while (wait(NULL) != -1)
            {
            }
            gettimeofday(&endtime, NULL);
            
            
            /*等待子进程完成后，获取计算时间，计算读写操作所花时间，延时，吞吐量等*/
//
            long alltime=get_time_left(starttime,endtime);//单位是毫秒  指代全部时间
        
            double alltime_s=alltime/1000.0;//单位为秒
            

            //double latency = (alltime) / (double) writetime / concurrency;
            
             double latency = (alltime) / (double)readtime / concurrency;
            //计算延迟（单个IO操作的时间），单位为毫秒

            //double file_kB = (double) block * writetime *concurrency / 1024.0; /* 文件大小，单位为KB */
            
           double file_kB = (double) block * readtime *concurrency / 1024.0; /* 文件大小，单位为KB */
            double ops = file_kB / alltime_s / 1024.0; //计算的是吞吐量，文件大小/执行时间，单位为mb/s
            
            printf("%d,%d,%f,%f\n", block, concurrency, latency, ops);


            concurrency += 4;
        }
        block *= 2;
    }



    
  return 0;
}







