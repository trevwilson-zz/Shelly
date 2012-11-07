#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <signal.h>



void f();
int ppid;
int cpid;
int cSuspend = 0;
int enteredElse = 0;

char processName[20][20];
char processStatus[20][20];
char pastCommand[200][30];
int processPID[20];

int main()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);


	ssize_t nread;
	int fd;
	char buf[1024] = "abc 123 ", cwd[1024];
	char exit[] = "exit";
	DIR *currentDirectory;
	ppid = getpid();
	int processPosition = 0;
	int out = 0;
	int in = 0;

	currentDirectory = opendir(".");
	getcwd(cwd, sizeof(cwd));

	int z;
	for(z=0;z<20;z++)
	{
		processName[z][0] = '\0';
	}


	int commandNum = 0;

	printf("Welcome to the shelly shell.\n");
	
	do
	{


		
		char *parse;//tokenize and save each word to parse (temp)
	        parse = strtok(buf, " ");
		char *cLine[20];//holds user input by word

		int c = 0;//word count
	

		while(parse!=NULL)
		{
			cLine[c] = parse;
			parse = strtok(NULL, " ");
			++c;
		
		}
		int u;
		for(u=0; u<c;u++)
		{
			if(!strcmp(cLine[u],">"))
			{
				out = 1;
			}
			if(!strcmp(cLine[u],"<"))
			{
				in = 1;
			}
		}

		if(!strcmp(cLine[0],"history"))
		{
			int y;
			for(y=0; y<commandNum;y++)
			{
				printf("%d)\t%s\n",y+1,pastCommand[y]);
			}
		}
		else if(out)
		{
			
		
		}
		else if(in)
		{
		
		
		}
		else if(cLine[0][0]=='!')
		{
			char temp[30];
			strcpy(temp,cLine[0]+1);
			int tempV = atoi(temp);
			strcpy(buf,pastCommand[tempV-1]);
			continue;
		
		}

		else if(cLine[0][0]=='.' && cLine[0][1]=='/')
		{
		
			int i = fork();	
			if(i==0)
			{
				signal(SIGINT,SIG_DFL);
				signal(SIGTSTP,SIG_DFL);
				if(c>1&&!strcmp(cLine[1],"&"))
				{
					setpgid(0,0);
				
				}
				execvp(cLine[0],NULL);
			}
			else
			{
				cpid = i;//set child pid
				signal(SIGTSTP,f);
				while(1)
				{
					int s;
					int *sPtr = &s;
					int j = waitpid(i,sPtr,WNOHANG);//return immediately if child has not finished. thus loop.
					if(c>1&&!strcmp(cLine[1],"&"))
					{
						
						strcpy(processName[processPosition],cLine[0]);
						strcpy(processStatus[processPosition],"Running");
						processPID[processPosition]=i;
						processPosition++;
						break;
					}
					if (cSuspend) {
						cSuspend = 0;
						printf("\n");
						//UPDATE JOBS HERE
						 strcpy(processName[processPosition],cLine[0]);
						 strcpy(processStatus[processPosition],"Stopped");
						 processPID[processPosition]=i;
						 processPosition++;

						 break;
				    	}
					
					
					
					if(WTERMSIG(s)==SIGINT)
					{
						printf("\n");
						break;
					}
					if(j==i)//when child returns normally it will give us its PID. Check and finish.
					{
						break;
					}
				}
			}
		}
		else if(!strcmp(cLine[0],"jobs"))
		{
			int i;
			for(i=0;i<20;i++)
			{
				if(processName[i][0] != '\0')
				{
					printf("%d) ",i+1);
					printf("%s\t%s\n",processName[i],processStatus[i]);
					if(!strcmp(processStatus[i],"Terminated"))
					{
						processStatus[i][0]='\0';
						processName[i][0]='\0';
					}
					if(!strcmp(processStatus[i],"Done"))
					{
						processStatus[i][0]='\0';
						processName[i][0]='\0';
					}
				
				}
			
			}
			
		}
		else if(!strcmp(cLine[0],"kill") && (cLine[1][0]=='%'))
		{
			int index = ((cLine[1][1])-'0')-1;
			kill(processPID[index],SIGINT);	
			strcpy(processStatus[index],"Terminated");
			
		}
		else if(!strcmp(cLine[0],"kill"))
		{
			int p = atoi(cLine[1]);
			int x;
			for(x=0;x<10;x++)
			{
				if(processPID[x]==p)
				{
					kill(p,SIGINT);
					processPID[x]=0;
					strcpy(processStatus[x],"Terminated");
				}
			}
		}

		else if(!strcmp(cLine[0],"pwd")){
			if(c>1){
				printf("Additional arguments not recognized. Please use only pwd\n");
			}
			else{
				printf("%s\n",cwd);
			}
		}

		else if(!strcmp(cLine[0],"ls")){
		//	else{
				DIR * directory;
				struct dirent *entry;
				if(c > 1){
					char nwd[1024];
					strcpy(nwd,cwd);
					strcat(nwd,"/");
					strcat(nwd,cLine[1]);
					if(opendir(nwd) != NULL){
						directory = opendir(nwd);
					}
					else{
						printf("Not a valid directory\n");
					}
				}
				else{
					directory = opendir(cwd);
				}
				while(1){
					entry=readdir(directory);
					if(entry==NULL)
						break;
					printf("%s\n",entry->d_name);
		//		}
			}
		}		
	
		else if(!strcmp(cLine[0],"cd")){
			DIR * directory;
			char nwd[1024];
			strcpy(nwd,cwd);

			if(cLine[1][0]=='.'){
				if(cLine[1][1]=='.'){
					char temp[16] = "/";
					strcat(temp,cLine[1]);
					strcat(nwd,temp);
				}
				else{
					char temp[1024];
					strcat(nwd,strncpy(temp,((cLine[1])+1),1023));
				}
			}
			else{
				char temp[16] = "/";
				strcat(nwd,temp);
				strcat(nwd,cLine[1]);
			}

			if(opendir(nwd) != NULL){
				strcpy(cwd,nwd);
				directory=opendir(cwd);
				currentDirectory=directory;
				chdir(cwd);
				getcwd(cwd,sizeof(cwd));
				perror("success");
			}
			else{
				perror("error");
			}
		}
		else
		{
			if(enteredElse)
			{
				char temp[1024];
				int i;
				strcpy(temp,cLine[0]);
				for(i=1; i<c; i++)
				{
					strcat(temp, " ");
					strcat(temp,cLine[i]);
				}
				
				system(temp);
			}
			enteredElse = 1;
		
		}
	
		printf("$helly> ");
		fflush(stdout);

		buf[0] = '\0';
		fgets(buf,1024,stdin);


		int j;
		for(j=0;j<1024;j++){
			if(buf[j]=='\n'){
				buf[j]='\0';
				break;
			}
		}

		if(!strcmp(processStatus[processPosition-1],"Running"))
		{
			int q = waitpid(processPID[processPosition-1],NULL,WNOHANG);
			if(q==processPID[processPosition-1])
			{
				strcpy(processStatus[processPosition-1],"Done");
			}
		}
		if(buf[0]!='!'){
		strcpy(pastCommand[commandNum],buf);
		commandNum++;
		}
			

	}while(strcmp(buf,"exit")!=0);
	
	
	return 0;
}


void f()
{
	signal(SIGTSTP,SIG_IGN);
	kill(cpid,SIGTSTP);
	cSuspend = 1;
}
