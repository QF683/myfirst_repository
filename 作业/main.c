#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

struct fun_para{
	int argc;
	char*argv[];
};
struct fun_para *para;

int file_block(const char* sfile,int pronum){
	int fd=open(sfile,O_RDONLY);
	int fsize=lseek(fd,0,SEEK_END);
	if(fsize%pronum==0) return fsize/pronum;//per thread needs to dispose the size of file
	else return fsize/pronum+1;	
}

int pram_check(int argc,const char*sfile,int pronum){
	if(argc<3){
		printf("ERROR:element num is error\n");
		exit(0);
	}
	if(access(sfile,F_OK)!=0){
		printf("ERROR:file is not exist\n");
		exit(0);
	}
	if(pronum<=0||pronum>100){
		printf("ERROR:process num is not true\n");
		exit(0);
	}
	return 0;
}

void *jobs(void*arg){
	struct fun_para *para;
	para = (struct fun_para *) arg;
	int block_size=atoi((para->argv)[3]);
	int copy_pos=atoi((para->argv)[4]);
	char buffer[block_size];
	bzero(buffer,sizeof(buffer));
	ssize_t readlen;	

	int sfd=open((para->argv)[1],O_RDONLY);//source file
	int dfd=open((para->argv)[2],O_RDWR|O_CREAT,0664);//detination file

	lseek(sfd,copy_pos,SEEK_SET);
	lseek(dfd,copy_pos,SEEK_SET);
	
	readlen=read(sfd,buffer,sizeof(buffer));
	write(dfd,buffer,readlen);
	close(sfd);
	close(dfd);
}


int main(int argc,char*argv[]){
	pthread_t tid;
	int flag=0;
	int pronum;//the number of thread
	if(argv[3]==0) pronum=3;
	else pronum=atoi(argv[3]);
	pram_check(argc,argv[1],pronum);//check the parametre
	int blocksize;
	blocksize=file_block(argv[1],pronum);//segmenting the file
	
	/*create threads,the number of threads is pronum*/
	for(int i=0;i<pronum;i++){
		int pos;
		pos=flag*blocksize;
		char str_blocksize[50];
		char str_pos[50];
		bzero(str_blocksize,sizeof(str_blocksize));
		bzero(str_pos,sizeof(str_pos));
		sprintf(str_blocksize,"%d",blocksize);
		sprintf(str_pos,"%d",pos);
		pthread_create(&tid,NULL,jobs,(void*)para);
	}
	return 0;
}
