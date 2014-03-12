#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>
#include<string.h>
#include<errno.h>
#include<wait.h>
#include<signal.h>
#include<fcntl.h>
typedef struct bjobs{
	int check;
	char name[100];
	int pd;
}bjobs;
struct stat st[100];
pid_t forejob=0;
bjobs job[100];
int fore=0;
int sizeofhost=0;
char nameofhost[100];
char *username;
char curdir[100];
char homepath[100];
char curpath[100];
pid_t pid;
pid_t p;
char cmd[100][100];
char some1[100];
int flag=0;
void perform_command();
struct stat diradd;
int total;
int jbc=0;
void term_child();
//void prompt();
void sig_handle();
void z_handle();
void redirect();
int red=0;
int piped=0;
/*------------------------------------------------MAIN------------------------------------------------*/
int main()
{

	//	signal(SIGINT, SIG_IGN);
	//	signal(SIGTSTP, SIG_IGN);
	//	signal(SIGTSTP, z_handle);
	signal(SIGINT, sig_handle);
	signal(SIGTERM, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	//	signal(SIGCHLD,SIG_IGN);
	signal(SIGCHLD,term_child);
	int j=0;
	char c;
	sizeofhost=gethostname(nameofhost,100);
	username=getenv("USER");
	strcpy(curdir,"~");
	getcwd(curpath,100);
	strcpy(homepath,curpath);
	pid=fork();

	if(pid==0)
	{

		execlp("/usr/bin/clear","clear",NULL);

	}
	else
		wait(NULL);
	printf("<%s@%s:%s>",username,nameofhost,curdir);
	int i=0;
	int k=0;
	total=0;
	int index=0;
	int space=0;
	while(1)
	{

		signal(SIGTSTP, z_handle);
		fore=0;
		forejob=0;
		flag=0;
		scanf("%c",&c);
		if(space==0 && (c==' ' || c=='\t'))
		{	space=1;
			while(c==' ' || c=='\t' )
				scanf("%c",&c);
		}
		space=1;
		some1[k]=c;
		k++;
		for(i=0;i<100;i++)
			for(j=0;j<100;j++)
				cmd[i][j]=0;
		if(c=='\n')
		{
			j=0;
			i=0;
			index=0;
			while(1)
			{
				while(1)
				{
					if(some1[i]!=' ' && some1[i]!='\t' &&some1[i]!='\n')
					{
						cmd[index][j]=some1[i];
						j++;
						i++;
					}
					else
					{
						j=0;
						while(some1[i+1]=='\t' || some1[i+1]==' ')
							i++;
						index++;
						break;
					}
				}
				if(some1[i]=='\n')
				{
					total=index;
					break;
				}
				i++;

			}
			if(cmd[i][total-1]==0)
				total--;
			red=0;
			for(i=0;i<total;i++)
			{	char ab[100]={0};
				for(j=0;cmd[i][j]!=0;j++)
				{
					ab[j]=cmd[i][j];


				}
				ab[j]=0;

				//printf("%d\n",strcmp(ab,"|"));
				if(strcmp(ab,">")==0||strcmp(ab,"<")==0||strcmp(ab,"|")==0||strcmp(ab,">>")==0)
				{

					red=1;
					break;
				}

			}
			if(red==0)
			{
				perform_command();
			}
			else
			{
				redirect();
			}
			for(j=0;j<100;j++)
				some1[j]=0;
			k=0;
			space=0;
			piped=0;
			red=0;
			getcwd(curdir,100);
			char tmp[100];
			if(strcmp(curdir,homepath)==0)
				strcpy(curdir,"~");
			if(strncmp(curdir,homepath,strlen(homepath))==0)
			{
				j=0;
				for(i=strlen(homepath);curdir[i]!=0;i++)
				{	curdir[j]=curdir[i];
					j++;
				}
				curdir[j]=0;
				strcpy(tmp,"~");
				strcat(tmp,curdir);
				strcpy(curdir,tmp);
			}

			printf("<%s@%s:%s>",username,nameofhost,curdir);
		}
	}
	return 0;
}
/*---------------------------------------------I/O REDIRECTION---------------------------------------------------------*/
int agn=0;
void redirect()
{
	char *earg[100];
	int i;
	char tmp[100];
	for(i=0;i<100;i++)
		earg[i]=NULL;

	for(agn=0;agn<=total;agn++)
	{	
		for(i=0;cmd[agn][i]!=0;i++)
		{
			tmp[i]=cmd[agn][i];
		}
		tmp[i]=0;
		earg[agn]=strdup(tmp);
	}
	earg[agn]=NULL;
	int last;
	int notpipe=0;

	last=agn-1;
	int saved_stdout=dup(1);
	int saved_stdin=dup(STDIN_FILENO);
	//	printf("%s\n",earg[last-2]);
	if(strcmp(earg[last-1],">")==0 || strcmp(earg[1],"<")==0||strcmp(earg[1],">>")==0)
	{

		notpipe=1;
		char file[100]={0};
		int j;
		strcpy(file,earg[last]);
		int exist=stat(file,st);
		//	if(exist==-1)
		//	{
		if(strcmp(earg[last-1],">")==0||strcmp(earg[last-1],">>")==0)
		{	
			int fd1;
			cmd[last][0]=0;
			cmd[last-1][0]=0;
			if(strcmp(earg[last-1],">")==0)
				fd1=open(file,O_CREAT|O_TRUNC|O_WRONLY,0640);
			else if(strcmp(earg[last-1],">>")==0)
				fd1=open(file,O_APPEND|O_WRONLY,0640);
			dup2(fd1,1);
			close(fd1);

			if(strcmp(earg[1],"<")!=0)
			{	perform_command();
			}
		}
		if(strcmp(earg[1],"<")==0)
		{
			int j=3;
			for(i=1;cmd[i][0]!=0;i++)
			{	strcpy(cmd[i],cmd[j]);
				j++;
			}
			//for(i=0;cmd[i][0]!=0;i++)
			//	printf("%s\n",cmd[i]);
			strcpy(file,earg[2]);
			int fd=open(file,O_RDONLY,0640);

			if(fd!=-1)
			{

				dup2(fd,STDIN_FILENO);
				close(fd);
				perform_command();
			}
			else
				perror(file);


		}

		//	printf("%d\n",notpipe);
		/*	else
			{
			int fd1=open(file,O_TRUNC|S_IRWXU);
			printf("YES\n");

			}
			*/
	}
	if(notpipe==0)
	{
	//	printf("notpipe\n");
		perform_command();
	}

	dup2(saved_stdin,STDIN_FILENO);
	close(saved_stdin);
	dup2(saved_stdout,1);
	close(saved_stdout);


}

/*----------------------------------------SIG INT HANDLER---------------------------------------------------------*/
void sig_handle(int signumber)
{
	if(fore==0)
	{
		printf("\n<%s@%s:%s>",username,nameofhost,curdir);
		fflush(stdout);
	}
}
/*----------------------------------------SIGT STP HANDLER---------------------------------------------------------*/
void z_handle(int signumber)
{
	//	printf("in z handler%d\n",forejob);
	//	pid_t stdin_PGID, stdout_PGID, stderr_PGID;

	/* Get the PGIDs for stdin, stdout, and stderr.  */
	//	   stdin_PGID = tcgetpgrp(STDIN_FILENO);
	if(forejob!=0)
	{
		int i;
		job[jbc].pd=forejob;
		char tmp[100];

		for(i=0;cmd[0][i]!=0;i++)
			tmp[i]=cmd[0][i];
		tmp[i]=0;
		strcpy(job[jbc].name,tmp);
		job[jbc].check=1;
		pid_t pid1=job[jbc].pd;
		//kill(pid1,SIGCONT);
		jbc++;

		if(signumber==SIGTSTP)
		{
			kill(pid1,SIGTSTP);
		}
	}
	if(forejob==0)
	{
		printf("\n<%s@%s:%s>",username,nameofhost,curdir);
		fflush(stdout);
	}
}
/*----------------------------------------COMMAND EXECUTION--------------------------------------------------------*/
void perform_command()
{
	int i;
	for(i=0;cmd[i][0]!=0;i++)
	if(strcmp(cmd[i],"|")==0)
	{
		piped=1;


	}

	char tmp[100],tmp2[100],tmp3[100];
	agn=0;
	for(i=0;cmd[agn][i]!=0;i++)
		tmp[i]=cmd[agn][i];
	tmp[i]=0;
	//	for(i=0;tmp[i]!=0;i++)
	//		printf("%c",tmp[i]);

	char bin1[100],bin2[100];
	strcpy(bin1,"/usr/bin/");
	strcpy(bin2,"/bin/");
	strcat(bin1,tmp);
	strcat(bin2,tmp);

	agn=1;
	for(i=0;cmd[agn][i]!=0;i++)
		tmp2[i]=cmd[agn][i];
	tmp2[i]=0;
	agn=2;
	for(i=0;cmd[agn][i]!=0;i++)
		tmp3[i]=cmd[agn][i];
	tmp3[i]=0;


	if(strcmp(tmp,"fg")==0)
	{
		pid=-10;
		if(tmp2[0]==0)
			printf("Invalid Arguments to Command 'fg'\n");
		else
		{

			int p=atoi(tmp2);
			int count=0;
			for(i=0;i<jbc;i++)
			{
				if(job[i].check==1)
				{
					count++;
					if(count==p)
					{
						pid=job[i].pd;
						job[i].check=0;
						break;
					}
				}
			}
			if(pid!=-10)
			{
				printf("%s [ %d ]      				Foreground\n",job[i].name,pid);
				fore=1;
				forejob=pid;
				kill(pid,SIGCONT);
				waitpid(pid,NULL,WUNTRACED);
				forejob=0;
				printf("%s [ %d ] 				Terminated\n",job[i].name,pid);
				fore=0;
			}
			else if(count==0)
			{
				printf("job does not exits\n");
			}
		}


	}
	else if(strcmp(tmp,"overkill")==0)
	{
		for(i=0;i<jbc;i++)
		{
			if(job[i].check==1)
			{	
				job[i].check=0;
				kill(job[i].pd,9);

			}
		}

	}
	else if(strcmp(tmp,"kjob")==0)
	{
		pid=-10;
		if(tmp2[0]==0||tmp3[0]==0)
			printf("Invalid Arguments to Command 'kjob'\n");
		else{

			int p=atoi(tmp2);
			int count=0;

			int signum=atoi(tmp3);


			for(i=0;i<jbc;i++)
			{
				if(job[i].check==1)
				{
					count++;
					if(count==p)
						pid=job[i].pd;

				}
			}
			if(pid!=-10)
			{
				kill(pid,signum);
			}
			else
			{
				printf("No such process exists\n");
			}
		}

	}
	else if(strcmp(tmp,"jobs")==0)
	{
		int count=0;
		for(i=0;i<jbc;i++)
		{
			if(job[i].check==1)
			{
				count++;
				printf("[%d] %s [%d]\n",count,job[i].name,job[i].pd);
			}
		}

		if(count==0)
		{
			printf("No Background Jobs in Process\n");
		}

	}
	else if(strcmp(tmp,"pinfo")==0)
	{
		if(tmp2[0]==0)
		{	pid=getpid();
			sprintf (tmp2,"%d",pid);
			//printf("%s\n",tmp2);
		}
		else
		{
			for(i=0;tmp2[i]!=0;i++)
			{	if(tmp2[i]>'9' || tmp2[i]<'0')
				{
					printf("INVALID PID\n");
					return;
				}
			}

		}
		/*	{
			strcpy(pid,tmp2);
			strcpy(tmp2,pid);
			}
			*/
		strcpy(tmp,"/proc/");
		strcpy(tmp3,"/proc/");
		printf("pid -- %s\n",tmp2);
		strcat(tmp,tmp2);
		strcat(tmp,"/status");
		strcat(tmp3,tmp2);
		strcat(tmp3,"/exe");
		//printf("%s\n",tmp);
		char r[1000]={0};
		char r1[1000]={0};
		//readlink(tmp3,r1,100);


		int fd1=open(tmp,0);
		int readcheck=read(fd1,r,1000);
		char *rs=NULL;

		//printf("%s\n",r);
		rs=strtok(r,"\n");
		char out[100];
		while(rs!=NULL)
		{

			for(i=0;i<100;i++)
				out[i]=0;
			if(strncmp(rs,"State",strlen("State"))==0)
			{
				strcpy(out,"Process ");
				strcat(out,rs);
				printf("%s\n",out);

			}
			else if(strncmp(rs,"VmSize",strlen("VmSize"))==0)
			{
				//printf("%s\n",rs);
				strcpy(out,"Memory -- ");
				int j=strlen(out);
				char *m;
				for(m=rs+12;*m!=0;)
				{
					out[j]=*m;
					j++;
					m++;
				}
				out[j]=0;
				printf("%s\n",out);

			}


			rs=strtok(NULL,"\n");

		}
		readlink(tmp3,r1,100);
		strcpy(curdir,r1);
		if(strcmp(curdir,homepath)==0)
			strcpy(curdir,"~");
		if(strncmp(curdir,homepath,strlen(homepath))==0)
		{
			int	j=0;
			for(i=strlen(homepath);curdir[i]!=0;i++)
			{	curdir[j]=curdir[i];
				j++;
			}
			curdir[j]=0;
			strcpy(tmp,"~");
			strcat(tmp,curdir);
			strcpy(curdir,tmp);
		}
		printf("%s\n",curdir);

	}
	else if(strcmp(tmp,"cd")==0)
	{	
		char path[100];

		strcpy(tmp2,homepath);
		agn=1;
		for(i=0;cmd[agn][i]!=0;i++)
			tmp[i]=cmd[agn][i];
		tmp[i]=0;
		int dir_valid=chdir(tmp);
		if(dir_valid==0)
		{
			printf("Directory changed succesfully\n");
		}
		else
		{
			switch(dir_valid){
				case EACCES: perror("Permission denied\n");
					     break;
				case EIO: perror("An input output error occured\n");
					  break;
				case ENAMETOOLONG: perror("Path is too long\n");
						   break;
				case ENOTDIR: perror("A component of path not a directory\n");
					      break;
				case ENOENT: perror("No such file or directory\n");
					     break;

				default: printf("Couldn't change directory\n");
			}
		}

	}
	else if(strcmp(tmp,"quit")==0)
	{
		exit(0);
	}
	else	{
		char *earg[100];
		for(i=0;i<100;i++)
			earg[i]=NULL;

		for(agn=0;agn<=total;agn++)
		{	
			for(i=0;cmd[agn][i]!=0;i++)
			{
				tmp[i]=cmd[agn][i];
			}
			tmp[i]=0;
			earg[agn]=strdup(tmp);
		}
		earg[agn]=NULL;
		if(red==1)
		{
			earg[agn-2]=NULL;
		}
		int last=agn-1;
		int status;
		strcpy(tmp,earg[last]);
		if(strcmp(tmp,"&")==0)
		{
			earg[agn-1]=NULL;
			flag=1;
		}

		//		for(i=0;earg[i]!=NULL;i++)
		//			printf("i %s\n",earg[i]);
		//for(i=0;earg[i]!=NULL;i++)
		//printf("%s\n",earg[i]);
		int pfd[2];
		if(piped!=1)
		{
			if(flag==1)
			{
				pid=fork();
				if(pid==0)
				{
					setpgid(0,0);
					int ko=execvp(*earg,earg);
					if(ko==-1)
						printf("NOT A VALID COMMAND\n");
					_exit(0);

				}
				else if(pid<0)
				{
					printf("Could not create process\n");
				}
				else if(pid>0)
				{
					int status;
					waitpid(pid,&status,WNOHANG);
					job[jbc].pd=pid;

					strcpy(job[jbc].name,earg[0]);
					for(i=1;i<last;i++)
					{
						strcat(job[jbc].name," ");
						strcat(job[jbc].name,earg[i]);

					}
					//	printf("%s\n",job[jbc].name);
					job[jbc].check=1;
					jbc++;
					if(status==-1)
					{
						printf("Child Process Exited abnormally\n");
					}
				}

			}
			else if(flag==0)
			{
				pid=fork();
				if(pid==0)
				{
					//	printf("%s\n",earg);
					int ko=execvp(*earg,earg);
					if(ko==-1)
						printf("NOT A VALID COMMAND\n");
					_exit(0);
				}
				else{
					fore=1;
					forejob=pid;
					//printf("%d\n",forejob);
					int status;
					waitpid(pid,&status,WUNTRACED);
					forejob=0;
					fore=0;
				}

			}
		}
		else if(piped==1)
		{
			int k=0;
			char * earg[100];
			int saved_stdin=dup(0);
			int np[2];
			int op[2];
			int saved_stdout=dup(1);
			//printf("%d\n",saved_stdin);

			int j;
			i=0;
			int count=0;
			//	dup2(op[1],0); 
			int tot=0;
			for(i=0;cmd[i][0]!=0;i++)
				tot++;
			i=0;
			while(cmd[k][0]!=0)
			{
				count++;
				for(i=0;i<100;i++)
				{
					earg[i]=NULL;
				}

				int man=0;
				while(strcmp(cmd[k],"|")!=0 && cmd[k][0]!=0)
				{

					//		printf("%s k %d--\n",cmd[k],strcmp(cmd[k],"|"));

					for(j=0;cmd[k][j]!=0;j++)
					{
						tmp[j]=cmd[k][j];
					}
					tmp[j]=0;
					earg[man]=strdup(tmp);
					k++;
					man++;

				}
				earg[man]=NULL;


				pipe(np);
				//	printf("op0 %d op1 %d np0 %d np1 %d",op[0],op[1],np[0],np[1]);
				pid=fork();
				if(count>1 )
				{
					dup2(op[0],0);
					close(op[1]);
					close(op[0]);
				}



				if(pid==0)
				{
			//		for(i=0;earg[i]!=NULL;i++)
			//			printf(" %s--\n k %d\n",earg[i],k);
					if(count>0 && k<tot )
					{
						dup2(np[1],1);
						close(np[0]);
						close(np[1]);

					}

					int ko=execvp(*earg,earg);
					if(ko==-1)
						printf("NOT A VALID COMMAND\n");
				}
				else
				{
					wait(NULL);
					if(k<tot)
					{
					op[0]=np[0];
					op[1]=np[1];
					}
				}

				k++;
			}
			close(op[0]);
			close(op[1]);
			close(np[0]);
			close(np[1]);
			dup2(saved_stdin,0); 
			dup2(saved_stdout,1); 
			close(saved_stdin);
			close(saved_stdout);
		}

	}

}
/*----------------------------------------SIG CHLD HANDLER---------------------------------------------------------*/
void term_child()
{
	int i,st;
	p=waitpid(-1,&st,WNOHANG);
	if(p>0)
	{
		char *rs=NULL;

		for(i=0;i<jbc;i++)
		{
			if(job[i].pd==p)
			{
				rs=strtok(job[i].name," ");

				printf("\n%s with pid %d exited normally\n",rs,p);
				job[i].check=0;
				break;
			}

		}	

		printf("<%s@%s:%s>",username,nameofhost,curdir);
		if(fore==0)
		{
			fflush(stdout);
		}

	}
}

