

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
/* portul folosit */
#define PORT 2024
#define and &&
#define or ||
char* query;
int logat;
void scrie_client(char *msg,int client)
{
	if (write (client, msg, 1024) < 0)
	{
	  	printf ("[server]Eroare la write() catre client.\n");	
	}
}

void citeste_client(char msg[],int client)
{
	if (read (client, msg, 1024) < 0)
	{
	  	printf ("[server]Eroare la write() catre client.\n");	
	}
}
int verificareString(char *msg,char *msg2)
{
	if(strlen(msg) > strlen(msg2)) return 0;
	for(int i=0;i<strlen(msg2);i++)
		if(msg[i]!=msg2[i])
			return 0;
	return 1;
}
int verificareMsg(char *msg,char *msg2)
{
	for(int i=0;i<strlen(msg2);i++)
		if(msg[i]!=msg2[i])
			return 0;
	return 1;
}


static int callback(void *data,int argc,char **argv,char **azColName)
{
	int i;
	char* aux=malloc(1024);
	bzero(aux,1024);
	for (i =0 ; i<argc;i++)
	{
		bzero(aux,1024);
		sprintf(aux,"%s\n",argv[i]?argv[i]:NULL);
		strcat(query,aux);
	}
	free(aux);
	return 0;
}
int run_command(char *command ,sqlite3 *db ,int rc,const char* data,char* zErrMsg)
{
	
	bzero(query,1024);
	rc=sqlite3_exec(db,command,callback,0,&zErrMsg);
	query[strlen(query)-1]=0;
	if(rc != SQLITE_OK)
	{
		return 0;
	}
	return 1;
}

void add_notificare(sqlite3 *db,int rc,const char* data ,char *zErrMsg , char* msg,char* username)
{
	char *comanda = malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"insert into notificari(username,notificare,seen,data) values ('%s','%s',0,datetime('now'))",username,msg);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(comanda);
}

void send_msg(sqlite3 *db,int rc,const char* data ,char *zErrMsg, int client, char* user_rec,char* username)
{
	char *comanda = malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"select count(*) from user where username='%s'",user_rec);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	

	if(atoi(query)>0)
	{
		scrie_client("send_msg:",client);
		char* msg=malloc(1024);
		bzero(msg,1024);
		citeste_client(msg,client);
		
		bzero(comanda,1024);
		sprintf(comanda,"insert into mesaje(user_send,user_rec,mesaj,seen_send,seen_rec,data) values ('%s','%s','%s',1,0,datetime('now'))",username,user_rec,msg);
		if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			printf("PRAF\n");

		char* notificare=malloc(1024);
		bzero(notificare,1024);
		sprintf(notificare,"%s ti-a trimis un mesaj.",username);
		add_notificare(db,rc,data,zErrMsg,notificare,user_rec);
		free(msg);
		free(notificare);
	}
	else
	{
		scrie_client("Utilizator inexistent",client);
	}
	free(comanda);
	
}

char* nr_mesaje_noi(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client ,char* user_snd,char* user_rec)
{
	char *comanda=malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"SELECT COUNT(*) from mesaje where ((user_send='%s' and user_rec='%s') and seen_rec=0",user_snd,user_rec);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(comanda);
	char* a=malloc(1024);
	bzero(a,1024);
	strcat(a,query);
	return a;
}


char* nr_mesaje(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client ,char* user_snd,char* user_rec)
{
	char *comanda=malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"SELECT COUNT(*) from mesaje where ((user_send='%s' and user_rec='%s') or (user_rec='%s' and user_send='%s'))",user_rec,user_snd,user_snd,user_rec);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(comanda);
	char* a=malloc(1024);
	bzero(a,1024);
	strcat(a,query);
	return a;
}





void read_msg(sqlite3 *db,int rc,const char* data ,char *zErrMsg, int client, char* user_send,char* username)
{

	char *comanda = malloc(1024);
	char* msg=malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"select count(*) from user where username='%s'",user_send);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF1\n");

	if(atoi(query)>0)
	{
		scrie_client("MSG:",client);
		int i=0;
		int nr=atoi(nr_mesaje(db,rc,data,zErrMsg,client,user_send,username));
		printf("%d mesaje",nr);
		do
		{
			//bzero(comanda,1024);
			//sprintf(comanda,"update mesaje INNER JOIN (select * from mesaje  where ((user_send='paul' and user_rec='da') or (user_rec='paul' and user_send='da'))  order by ID DESC limit 5 offset 0)  set seen_rec=1 where (user_send='%paul' and user_rec='da')",i,username,user_send);
			//if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			//	printf("PRAF\n");


			bzero(comanda,1024);
			sprintf(comanda,"select mesaj,user_send,data from (select * from mesaje order by ID DESC) where ((user_send='%s' and user_rec='%s') or (user_rec='%s' and user_send='%s')) limit 5 offset %d",username,user_send,username,user_send,i);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");


			
			scrie_client(query,client);
			bzero(msg,1024);
			citeste_client(msg,client);
			if(verificareString(msg,"next")==1)
				if(i<=nr-5)			
					i=i+5;
			if(verificareString(msg,"back")==1)
				if(i>=5)
					i=i-5;
		}while(verificareString(msg,"stop")==0 );
	}
	else
		scrie_client("Utilizator inexistent",client);

	free(comanda);
	free(msg);
}


char* nr_postari(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client ,char* username)
{
	char *comanda=malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"SELECT COUNT(*) from postari where username='%s'",username);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(comanda);
	char* a=malloc(1024);
	bzero(a,1024);
	strcat(a,query);
	return a;
}

char* nr_postari_totale(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client ,char* username)
{
	char *comanda=malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"SELECT COUNT(*) from (select distinct ID,username,vizibilitate,postare,data from postari join prieteni on username1=username where username2='%s')",username);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(comanda);
	char* a=malloc(1024);
	bzero(a,1024);
	strcat(a,query);
	return a;
}



void feed(sqlite3 *db,int rc,const char* data ,char *zErrMsg,int client,char* username)
{
	scrie_client("Feed:",client);
	int i=0;
	int nr=atoi(nr_postari_totale(db,rc,data,zErrMsg,client,username ));
	char *comanda = malloc(1024);
	char *msg=malloc(1024);
	bzero(msg,1024);
	bzero(comanda,1024);
	do
	{
		bzero(comanda,1024);
		sprintf(comanda,"select postare,username,data from (select distinct ID,username,vizibilitate,postare,data from postari join prieteni on username1=username where username2='%s' order by ID DESC) limit 5 offset %d",username,i);
		if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			printf("PRAF\n");


		scrie_client(query,client);
		bzero(msg,1024);
		citeste_client(msg,client);
		if(verificareString(msg,"next")==1)
			if(i<=nr-5)			
				i=i+5;
		if(verificareString(msg,"back")==1)
			if(i>=5)
				i=i-5;
	}while(verificareString(msg,"stop")==0 );
	free(comanda);
	free(msg);
}




void view_postari(sqlite3 *db,int rc,const char* data ,char *zErrMsg,int client,char* msg ,char* username)
{
	scrie_client("Postari:",client);
	strcpy(msg,msg+11);
	int i=0;
	int nr=atoi(nr_postari(db,rc,data,zErrMsg,client , msg ));
	char *comanda = malloc(1024);
	bzero(comanda,1024);
	if(strcmp(username,msg)!=0)
	{
		char* notificare=malloc(1024);
		sprintf(notificare,"%s s-a uitat la postarile tale.",username);
		add_notificare(db,rc,data,zErrMsg,notificare,msg);
		free(notificare);
	}
	
	bzero(comanda,1024);
	sprintf(comanda,"select count(*) from prieteni where username1='%s' and username2='%s'",msg,username);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
		
	if(atoi(query)!=0)
	{
		do
		{
			bzero(comanda,1024);
			sprintf(comanda,"select postare,data from (select * from postari order by ID DESC) where username='%s' limit 5 offset %d",msg,i);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");


			
			scrie_client(query,client);
			bzero(msg,1024);
			citeste_client(msg,client);
			if(verificareString(msg,"next")==1)
				if(i<=nr-5)			
					i=i+5;
			if(verificareString(msg,"back")==1)
				if(i>=5)
					i=i-5;
		}while(verificareString(msg,"stop")==0 );
	}
	else
	{
		do
		{
			bzero(comanda,1024);
			sprintf(comanda,"select postare,data from (select * from postari order by ID DESC) where username='%s' and vizibilitate='public' limit 5 offset %d",msg,i);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");


			
			scrie_client(query,client);
			bzero(msg,1024);
			citeste_client(msg,client);
			if(verificareString(msg,"next")==1)
				if(i<=nr-5)			
					i=i+5;
			if(verificareString(msg,"back")==1)
				if(i>=5)
					i=i-5;
		}while(verificareString(msg,"stop")==0 );
	}
	free(comanda);
}


char* nr_notificari_noi(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client ,char* username)
{
	char *comanda=malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"SELECT COUNT(*) from notificari where username='%s' and seen=0",username);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(comanda);
	char* a=malloc(1024);
	bzero(a,1024);
	strcat(a,query);
	return a;
}


char* nr_notificari(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client ,char* username)
{
	char *comanda=malloc(1024);
	bzero(comanda,1024);
	sprintf(comanda,"SELECT COUNT(*) from notificari where username='%s'",username);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(comanda);
	char* a=malloc(1024);
	bzero(a,1024);
	strcat(a,query);
	return a;
}






void view_notificari(sqlite3 *db,int rc,const char* data ,char *zErrMsg,int client ,char* username)
{
	scrie_client("Notificari:",client);
	char* msg=malloc(1024);
	bzero(msg,1024);
	int i=0;
	int nr=atoi(nr_notificari(db,rc,data,zErrMsg,client , username ));
	char *comanda = malloc(1024);
	bzero(comanda,1024);
	do
	{
		bzero(comanda,1024);
		sprintf(comanda,"update notificari set seen=1 where username='%s' order by ID DESC limit 5 offset %d",username,i);
		if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			printf("PRAF1\n");


		bzero(comanda,1024);
		sprintf(comanda,"select notificare,data from (select * from notificari order by ID DESC) where username='%s' limit 5 offset %d",username,i);
		if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			printf("PRAF2\n");


		
		scrie_client(query,client);
		bzero(msg,1024);
		citeste_client(msg,client);
		if(verificareString(msg,"next")==1)
			if(i<=nr-5)			
				i=i+5;
		if(verificareString(msg,"back")==1)
			if(i>=5)
				i=i-5;
	}while(verificareString(msg,"stop")==0 );
	free(msg);
	free(comanda);
}

void creare_profil(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client ,char *user)
{
	char *Nume=malloc(1024);
	bzero(Nume,1024);
	char *Prenume=malloc(1024);
	bzero(Prenume,1024);
	char *Varsta=malloc(1024);
	bzero(Varsta,1024);
	char *Oras=malloc(1024);
	bzero(Oras,1024);
	char *Descriere=malloc(1024);
	bzero(Descriere,1024);
	char *comanda=malloc(1024);
	bzero(comanda,1024);
	char *vizibilitate=malloc(1024);
	bzero(vizibilitate,1024);

	citeste_client(Nume,client);
	

	citeste_client(Prenume,client);
	
	
	citeste_client(Varsta,client);
	

	citeste_client(Oras,client);
	

	citeste_client(Descriere,client);
	
	
	citeste_client(vizibilitate,client);
	
	
	bzero(comanda,1024);
	sprintf(comanda,"insert into prieteni(username1,username2) values ('%s','%s')",user,user);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	
	bzero(comanda,1024);
	sprintf(comanda,"insert into Profil(username,Nume,Prenume,Varsta,Oras,Descriere,vizibilitate) values ('%s','%s','%s','%s','%s','%s','%s')",user,Nume,Prenume,Varsta,Oras,Descriere,vizibilitate);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");

	add_notificare(db,rc,data,zErrMsg,"Bine ai venit",user);
	free(Nume);
	free(Prenume);
	free(Varsta);
	free(Oras);
	free(Descriere);
	free(vizibilitate);
	free(comanda);
}





void logOn(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client , char* msg)
{
	char *pass=malloc(1024);
	char *comanda=malloc(1024);
	char *user=malloc(1024);
	bzero(user,1024);
	strcpy(msg,msg+7);
	strcat(user,msg);
	sprintf(comanda,"select password from User where username='%s'",msg);
	
	
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	
	if(strlen(query)==0)
	{
		scrie_client("Alege o parola:",client);

		bzero(msg,1024);
		citeste_client(msg,client);
		

		
		bzero(pass,1024);
		strcat(pass,msg);
		bzero(comanda,1024);
		
		sprintf(comanda,"insert into User(username,password) values ('%s','%s')",user,pass);
		
		


		if(run_command(comanda ,db , rc,data, zErrMsg)!=0)
		{
			
			scrie_client("1",client);
			creare_profil(db,rc,data,zErrMsg,client,user);	
		}	
		else
			scrie_client("0",client);


	}
	else
	{
		scrie_client("0",client);
	}
	free(pass);
	free(user);
	free(comanda);
}


int logIn(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client , char* msg, char username[])
{
	char *comanda=malloc(1024);
	strcpy(msg,msg+6);
	char *user=malloc(1024);
	bzero(user,1024);
	strcat(user,msg);
	if(msg[0]!='\n')
	{

		bzero(comanda,1024);
		sprintf(comanda,"select count(*) from User where username='%s'",msg);
		if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			printf("PRAF\n");
	
		if(atoi(query)!=0)
		{

			bzero(comanda,1024);
			sprintf(comanda,"select password from User where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");

			scrie_client("Introduceti parola:",client);
			
			citeste_client(msg,client);
			if(strcmp(msg,query)==0)
				{
					scrie_client("1",client);
					bzero(username,1024);
					strcat(username,user);
					
					return 1;
				}	
			else
				{scrie_client("0",client);return 0;}
		}
		else
			{scrie_client("0",client);return 0;}
	
	}
	else
		{scrie_client("0",client);return 0;}

	
	free(comanda);
}

int view_profile(sqlite3 *db ,int rc ,const char* data ,char *zErrMsg , int client , char* msg , char *user)
{
	int logat;
	char *comanda=malloc(1024);
	
	strcpy(msg,msg+13);
	logat=msg[0]-'0';
	strcpy(msg,msg+1);
	
	bzero(comanda,1024);
	sprintf(comanda,"select vizibilitate from profil where username='%s'",msg);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	if(strlen(query)==0)
	{
		scrie_client("Profil inexistent",client);
		return 0;
	}
	if(logat==0)
	{
		if (verificareString(query,"public"))
			{
				scrie_client("Profil:",client);
			if(strcmp(user,msg)!=0 and strlen(user) != 0)
			{
				char *notificare=malloc(1024);
				bzero(notificare,1024);
				sprintf(notificare,"%s ti-a vizualizat profilul",user);
				
				add_notificare(db,rc,data,zErrMsg,notificare,msg);
				
				free(notificare);
				
			}
			bzero(comanda,1024);
			
			sprintf(comanda,"select nume from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			
			
			bzero(comanda,1024);
			sprintf(comanda,"select prenume from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			

			bzero(comanda,1024);
			sprintf(comanda,"select varsta from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			
			
			bzero(comanda,1024);
			sprintf(comanda,"select oras from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			

			bzero(comanda,1024);
			sprintf(comanda,"select descriere from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			}
		else
		{
		scrie_client("Trebuie sa fii logat",client);
		return 0;
		}
	}
	else
	{
		bzero(comanda,1024);
		sprintf(comanda,"select count(*) from prieteni where username1='%s' and username2='%s'",msg,user);
		if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			printf("PRAF\n");
		
		

		if(atoi(query)>0)
		{
			scrie_client("Profil:",client);
			if(strcmp(user,msg)!=0 and strlen(user) != 0)
			{
				char *notificare=malloc(1024);
				bzero(notificare,1024);
				sprintf(notificare,"%s ti-a vizualizat profilul",user);
				
				add_notificare(db,rc,data,zErrMsg,notificare,msg);
				
				free(notificare);
				
			}
			bzero(comanda,1024);
			
			sprintf(comanda,"select nume from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			
			
			bzero(comanda,1024);
			sprintf(comanda,"select prenume from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			

			bzero(comanda,1024);
			sprintf(comanda,"select varsta from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			
			
			bzero(comanda,1024);
			sprintf(comanda,"select oras from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
			

			bzero(comanda,1024);
			sprintf(comanda,"select descriere from profil where username='%s'",msg);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");
			
			scrie_client(query,client);
		}
		else
		{
			scrie_client("Trebuie sa fiti prieteni pentru a vizualiza profilul",client);
		}
	}
	free(comanda);
}


void add_post(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client , char* msg,char* username)
{
	char *comanda=malloc(1024);
	char *vizibilitate=malloc(1024);

	int i;
	bzero(vizibilitate,1024);
	strcpy(msg,msg+9);
	if(msg[0]=='1')
	{
		bzero(vizibilitate,1024);
		strcat(vizibilitate,"privat");
	}
	else
	{
		bzero(vizibilitate,1024);
		strcat(vizibilitate,"public");
	}
	strcpy(msg,msg+1);
	
	scrie_client("",client);
	bzero(comanda,1024);
	sprintf(comanda,"insert into postari(username,vizibilitate,postare,data) values ('%s','%s','%s',datetime('now'))",username,vizibilitate,msg);
	if(run_command(comanda ,db , rc,data, zErrMsg)==0)
		printf("PRAF\n");
	free(vizibilitate);
	
	free(comanda);
}

void accept_req(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client , char* msg,char* username)
{
char *comanda=malloc(1024);

	if(logat==1)
	{
		strcpy(msg,msg+11);
		scrie_client("Add_friend:",client);
			


			bzero(comanda,1024);
			sprintf(comanda,"select count(*) from friend_req where username1='%s' and username2='%s'",msg,username);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");


			if(atoi(query)>0)
			{
				scrie_client("Prieten adaugat",client);


				bzero(comanda,1024);
				sprintf(comanda,"insert into prieteni(username1,username2) values ('%s','%s')",username,msg);
				if(run_command(comanda ,db , rc,data, zErrMsg)==0)
					printf("PRAF\n");
	

				bzero(comanda,1024);
				sprintf(comanda,"insert into prieteni(username1,username2) values ('%s','%s')",msg,username);
				if(run_command(comanda ,db , rc,data, zErrMsg)==0)
					printf("PRAF\n");

						
				char *notificare=malloc(1024);
				bzero(notificare,1024);
				sprintf(notificare,"Tu si %s ati devenit prieteni",username);
				add_notificare(db,rc,data,zErrMsg,notificare,msg);
			

			
				bzero(notificare,1024);
				sprintf(notificare,"Tu si %s ati devenit prieteni",msg);
				add_notificare(db,rc,data,zErrMsg,notificare,username);
				free(notificare);


				
				bzero(comanda,1024);
				sprintf(comanda,"delete from friend_req where username1='%s' and username2='%s'",msg,username);
				if(run_command(comanda ,db , rc,data, zErrMsg)==0)
					printf("PRAF\n");

	
			}
			else
			{
				char* mesaj=malloc(1024);
				bzero(mesaj,1024);
				sprintf(mesaj,"Nu ai nici o cerere de la %s",msg);
				scrie_client(mesaj,client);
				free(mesaj);
			}
	}

}


void reject_req(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client , char* msg,char* username)
{
	char *comanda=malloc(1024);
	if(logat==1)
	{
		strcpy(msg,msg+11);
		scrie_client("Add_friend:",client);
			


			bzero(comanda,1024);
			sprintf(comanda,"select count(*) from friend_req where username1='%s' and username2='%s'",msg,username);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");


			if(atoi(query)>0)
			{
				scrie_client("Cererea a fost stearsa",client);	
				bzero(comanda,1024);
				sprintf(comanda,"delete from friend_req where username1='%s' and username2='%s'",msg,username);
				if(run_command(comanda ,db , rc,data, zErrMsg)==0)
					printf("PRAF\n");
			}
			else
			{
				char* mesaj=malloc(1024);
				bzero(mesaj,1024);
				sprintf(mesaj,"Nu ai nici o cerere de la %s",msg);
				scrie_client(mesaj,client);
				free(mesaj);
			}
	}
}



void add_friend(sqlite3 *db,int rc,const char* data ,char *zErrMsg , int client , char* msg,char* username)
{

	char *comanda=malloc(1024);

	if(logat==1)
	{
		strcpy(msg,msg+11);


		bzero(comanda,1024);
		sprintf(comanda,"select count(*) from user where username='%s'",msg);
		if(run_command(comanda ,db , rc,data, zErrMsg)==0)
			printf("PRAF\n");


		if(atoi(query)!=0)
		{
			bzero(comanda,1024);
			sprintf(comanda,"select count(*) from prieteni where username1='%s' and username2='%s'",msg,username);
			if(run_command(comanda ,db , rc,data, zErrMsg)==0)
				printf("PRAF\n");

			if(atoi(query)==0)		
			{
				scrie_client("Add_friend:",client);
				


				bzero(comanda,1024);
				sprintf(comanda,"select count(*) from friend_req where username1='%s' and username2='%s'",msg,username);
				if(run_command(comanda ,db , rc,data, zErrMsg)==0)
					printf("PRAF\n");


				if(atoi(query)>0)
				{
					scrie_client("Prieten adaugat",client);


					bzero(comanda,1024);
					sprintf(comanda,"insert into prieteni(username1,username2) values ('%s','%s')",username,msg);
					if(run_command(comanda ,db , rc,data, zErrMsg)==0)
						printf("PRAF\n");
		

					bzero(comanda,1024);
					sprintf(comanda,"insert into prieteni(username1,username2) values ('%s','%s')",msg,username);
					if(run_command(comanda ,db , rc,data, zErrMsg)==0)
						printf("PRAF\n");

							
					char *notificare=malloc(1024);
					bzero(notificare,1024);
					sprintf(notificare,"Tu si %s ati devenit prieteni",username);
					add_notificare(db,rc,data,zErrMsg,notificare,msg);
				

				
					bzero(notificare,1024);
					sprintf(notificare,"Tu si %s ati devenit prieteni",msg);
					add_notificare(db,rc,data,zErrMsg,notificare,username);
					free(notificare);


					
					bzero(comanda,1024);
					sprintf(comanda,"delete from friend_req where username1='%s' and username2='%s'",msg,username);
					if(run_command(comanda ,db , rc,data, zErrMsg)==0)
						printf("PRAF\n");

		
				}
				else
				{
					
					bzero(comanda,1024);
					sprintf(comanda,"select count(*) from friend_req where username1='%s' and username2='%s'",msg,username);
					if(run_command(comanda ,db , rc,data, zErrMsg)==0)
						printf("PRAF\n");

					if(atoi(query)==0)
					{
						scrie_client("S-a trimis cererea",client);
						bzero(comanda,1024);
						sprintf(comanda,"insert into friend_req(username1,username2) values ('%s','%s')",username,msg);
						if(run_command(comanda ,db , rc,data, zErrMsg)==0)
							printf("PRAF\n");
						
								
						char *notificare=malloc(1024);
						bzero(notificare,1024);
						sprintf(notificare,"%s ti-a trimis o cerere de prietenie",username);
						add_notificare(db,rc,data,zErrMsg,notificare,msg);
						free(notificare);

						
					}
					else
						scrie_client("O cerere a fost deja trimisa",client);
					
					
				}
				
				
			}
			else
				scrie_client("Sunteti deja prieteni",client);
		}
		else
			scrie_client("Utilizatorul nu exista",client);
		
	}
	else
		scrie_client("Trebuie sa fii logat",client);
	
	
	free(comanda);
}


void rulare_server(struct sockaddr_in from , int sd )
{
	char msg[1024];
	char* username=malloc(1024);
	logat=0;
	sqlite3 *db;
	char *zErrMsg=0;
	const char* data=0;
	int rc=sqlite3_open("baza_server.db",&db);

	
	while (1)
   	 {

      int client;
      int length = sizeof (from);
      int user;
      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      client = accept (sd, (struct sockaddr *) &from, &length);

      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
      else
	{
      user=fork();
      if(user<0)
	perror("[server]Eroare fork user\n");
      if(user==0)
      {
		while(1)
		{
		      /* s-a realizat conexiunea, se astepta mesajul */
		      bzero (msg, 1024);
		      printf ("[server]Asteptam mesajul...\n");
		      fflush (stdout);
		      
		      /* citirea mesajului */
		      citeste_client(msg,client);
			
		      printf ("[server]Mesajul a fost receptionat...%s\n", msg);


			

			if(logat==1)
			{
				
				if(verificareMsg(msg,"quit")==1)
				{
				scrie_client(msg,client);
				close (client);
				exit(0);
				}
				else
				if(verificareMsg(msg,"feed:"))
					feed(db,rc,data,zErrMsg,client,username);
				else
				if(verificareMsg(msg,"view_profile:"))
					view_profile(db,rc,data,zErrMsg,client,msg,username);
				else
				if(verificareMsg(msg,"view_posts:"))
					view_postari(db,rc,data,zErrMsg,client,msg,username);
				else
				if(verificareMsg(msg,"add_post:"))
					add_post(db,rc,data,zErrMsg,client,msg,username);
				else
				if(verificareMsg(msg,"add_friend:"))
					add_friend(db,rc,data,zErrMsg,client,msg,username);
				else
				if(verificareMsg(msg,"accept_req:"))
					accept_req(db,rc,data,zErrMsg,client,msg,username);
				else
				if(verificareMsg(msg,"reject_req:"))
					reject_req(db,rc,data,zErrMsg,client,msg,username);
				else
				if(verificareMsg(msg,"nr_notificari"))
					scrie_client(nr_notificari_noi(db,rc,data,zErrMsg,client,username),client);
				else
				if(verificareMsg(msg,"notifications"))
					view_notificari(db,rc,data,zErrMsg,client,username);
				else
				if(verificareMsg(msg,"send_msg"))
				{
					send_msg(db,rc,data,zErrMsg,client,msg+9,username);
				}
				else
				if(verificareMsg(msg,"read_msg"))
					read_msg(db,rc,data,zErrMsg,client,msg+9,username);

				else
				if (write (client, "err", 1024) <= 0)
				 	printf ("[server]Eroare la write() catre client.\n");

			}
			else
			{
				if(verificareMsg(msg,"quit")==1)
				{
				scrie_client(msg,client);
				close (client);
				exit(0);
				}
				else
				if(verificareMsg(msg,"login:")==1)
				{
					if(logIn(db,rc,data,zErrMsg,client,msg,username))
					{
						logat=1;
					}
					
				}
				else
				if(verificareMsg(msg,"view_profile:")==1)
					view_profile(db,rc,data,zErrMsg,client,msg,username);
				else
				if(verificareMsg(msg,"signin:")==1)
					logOn(db,rc,data,zErrMsg,client,msg);
				else
				if (write (client, "Trebuie sa fii logat", 1024) <= 0)
					 printf ("[server]Eroare la write() catre client.\n");
			}
	      
	      }
        }
      }

    				
}/* while */	

}






/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main()
{
  query=malloc(1024);
  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int sd;			//descriptorul de socket 
  
  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }


  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }

  rulare_server(from,sd);
  
}

