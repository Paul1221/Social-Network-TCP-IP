
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#define and &&
#define or ||

void scrie_server(char *msg,int sd)
{
	if (write (sd, msg, 1024) < 0)
	{
	  	printf ("[server]Eroare la write() catre client.\n");	
	}
}

void citire_server(char msg[],int sd)
{
	if (read (sd, msg, 1024) < 0)
	{
	  	printf ("[server]Eroare la write() catre client.\n");	
	}
}

int verificareMsg(char *msg,char *msg2)
{
	for(int i=0;i<strlen(msg2);i++)
		if(msg[i]!=msg2[i])
			return 0;
	return 1;
}

int verificareString(char *msg,char *msg2)
{
	if(strlen(msg) != strlen(msg2)) return 0;
	for(int i=0;i<strlen(msg2);i++)
		if(msg[i]!=msg2[i])
			return 0;
	return 1;
}

void citire_tastatura(char msg[])
{
	do
	{
	read (0, msg, 1024);
	msg[strlen(msg)-1]=0;
	//printf("\n[citit]%s\n",msg);
	}while(msg[0] == 0);
}

int logon(char *msg,int sd)
{
	printf ("[server] %s", msg);
	fflush(stdout);
	bzero(msg,1024);
	citire_tastatura(msg);
		scrie_server(msg,sd);
		
		bzero(msg,1024);
		citire_server(msg,sd);
		if(verificareMsg(msg,"1"))
		{
		return 1;
		}
	return 0;
}


int login(char *msg,int sd)
{
	printf ("[server] %s", msg);
	fflush(stdout);
	bzero(msg,1024);
	citire_tastatura(msg);
		scrie_server(msg,sd);
		
		bzero(msg,1024);
		citire_server(msg,sd);
		if(verificareMsg(msg,"1"))
		{
		return 1;
		}
	return 0;
}

void creare_profil(int sd)
{
	char *msg=malloc(1024);
	
	printf("Introduceti un Nume:");
	fflush(stdout);
	bzero(msg,1024);
	citire_tastatura(msg);
	scrie_server(msg,sd);

	printf("Introduceti un Preume:");
	fflush(stdout);
	bzero(msg,1024);
	citire_tastatura(msg);
	scrie_server(msg,sd);
	
	printf("Introduceti o Varsta:");
	fflush(stdout);
	bzero(msg,1024);
	citire_tastatura(msg);
	scrie_server(msg,sd);

	printf("Introduceti un Oras:");
	fflush(stdout);
	bzero(msg,1024);
	citire_tastatura(msg);
	scrie_server(msg,sd);

	printf("Introduceti o Descriere:");
	fflush(stdout);
	bzero(msg,1024);
	citire_tastatura(msg);
	scrie_server(msg,sd);
	
	printf("Setati vizibilitatea profilului(public/privat):");
	fflush(stdout);
	int da=1;
	while(da)
	{
	bzero(msg,1024);
	citire_tastatura(msg);
	if(verificareString(msg,"privat")==1 or verificareString(msg,"public")==1)
		da=0;
	else
		printf("Setati vizibilitatea profilului(public/privat):");
		fflush(stdout);
	}
	scrie_server(msg,sd);
}

void vizualizare_profil(int sd)
{
	char *msg=malloc(1024);
	
	printf("Nume:");
	fflush(stdout);
	bzero(msg,1024);
	citire_server(msg,sd);
	printf("%s\n",msg);	

	printf("Preume:");
	fflush(stdout);
	bzero(msg,1024);
	citire_server(msg,sd);
	printf("%s\n",msg);
	
	printf("Varsta:");
	fflush(stdout);
	bzero(msg,1024);
	citire_server(msg,sd);
	printf("%s\n",msg);

	printf("Oras:");
	fflush(stdout);
	bzero(msg,1024);
	citire_server(msg,sd);
	printf("%s\n",msg);

	printf("Descriere:");
	fflush(stdout);
	bzero(msg,1024);
	citire_server(msg,sd);
	printf("%s\n",msg);
	
	free(msg);
}

void vizualizare_postari(int sd)
{
	char *msg=malloc(1024);
	int stop=0;
	bzero(msg,1024);
	while(stop!=1)
	{	
		
		if(verificareString(msg,"stop")==0)
		{
			bzero(msg,1024);
			citire_server(msg,sd);
			printf("----------------POSTARI-----------------\n");
			printf("\n%s\n",msg);
			printf("________________________________________\n");

			printf("next pentru urmatoarea pagina|back pentru anterioara|stop pentru oprire\n");
			bzero(msg,1024);
			citire_tastatura(msg);
			scrie_server(msg,sd);
		}
		else
			stop=1;
	}
	free(msg);
}	



void vizualizare_notificari(int sd)
{
	char *msg=malloc(1024);
	int stop=0;
	bzero(msg,1024);
	while(stop!=1)
	{	
		
		if(verificareString(msg,"stop")==0)
		{
			bzero(msg,1024);
			citire_server(msg,sd);
			printf("---------------NOTIFICARI---------------\n");
			printf("\n%s\n",msg);
			printf("________________________________________\n");

			printf("next pentru urmatoarea pagina|back pentru anterioara|stop pentru oprire\n");
			bzero(msg,1024);
			citire_tastatura(msg);
			scrie_server(msg,sd);
		}
		else
			stop=1;
	}
	free(msg);
}

void send_msg(int sd)
{
	char* msg=malloc(1024);
	bzero(msg,1024);
	printf("Introduceti textul:\n");
	citire_tastatura(msg);
	scrie_server(msg,sd);
	free(msg);
}


void anunt_nr_notificari(int sd)
{
	scrie_server("nr_notificari",sd);
	char* msg=malloc(1024);
	bzero(msg,1024);
	citire_server(msg,sd);
	printf("Ai %s notificari noi\n",msg);
	free(msg);
}

void add_friend(int sd)
{
	char* msg=malloc(1024);
	bzero(msg,1024);
	citire_server(msg,sd);
	printf("%s\n",msg);
	free(msg);
}

void rulare_program(int sd)
{
	int ruleaza=1;
	int logat=0;
	char *msg = malloc(1024);
	//time_t timp_notificare=time(NULL);
	
	while(ruleaza)
  	{
  	
	/*verificare notificare*/
	if(logat==1)
		anunt_nr_notificari(sd);
		/*if(timp_notificare +30 >= time(NULL))
		{	
			
				
			timp_notificare=timp_notificare+30;
		}
		/* citirea mesajului */
 	bzero (msg, 1024);
  	printf ("[client]Introduceti comanda: ");
  	fflush (stdout);
  	citire_tastatura(msg);
  	
	if(verificareString(msg,"login"))
	{
		
		strcat(msg,":");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"feed"))
	{
		strcat(msg,":");
	}
	if(verificareString(msg,"signin"))
	{
		
		strcat(msg,":");
		
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"view_posts"))
	{
		
		strcat(msg,":");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"view_profile"))
	{
		
		strcat(msg,":");
		if(logat)
			strcat(msg,"1");
		else
			strcat(msg,"0");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"add_friend"))
	{
		
		strcat(msg,":");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"send_msg"))
	{
		
		strcat(msg,":");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"read_msg"))
	{
		
		strcat(msg,":");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"accept_req"))
	{
		
		strcat(msg,":");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"reject_req"))
	{
		
		strcat(msg,":");
		char* user=malloc(1024);	
		bzero (user, 1024);
		printf ("[client]Introduceti username-ul: ");
  		fflush (stdout);
  		citire_tastatura(user);
		strcat(msg,user);
		free(user);
	}
	if(verificareString(msg,"add_post"))
	{
		
		strcat(msg,":");
		if(logat)
		{
		char* text=malloc(1024);	
		bzero (text, 1024);
		printf("Setati vizibilitatea profilului(public/privat):");
		fflush(stdout);
		int da=1;
		while(da)
		{
		bzero(text,1024);
		citire_tastatura(text);
		if(verificareString(text,"privat")==1 or verificareString(text,"public")==1)
			da=0;
		else
			printf("Setati vizibilitatea profilului(public/privat):");
			fflush(stdout);
		}
		
		if(verificareString(text,"privat"))
			strcat(msg,"1");
		else
			strcat(msg,"0");
		
		bzero (text, 1024);
		printf ("[client]Introduceti textul: ");
  		fflush (stdout);
  		citire_tastatura(text);
		strcat(msg,text);
		free(text);
		}
		else
		{
		printf("Trebuie sa fii logat!\n");
		bzero(msg,1024);
		}
	}
	

  	/* trimiterea mesajului la server */
  	scrie_server(msg,sd);
	bzero(msg,1024);
  	/* citirea raspunsului dat de server 
     	(apel blocant pina cind serverul raspunde) */
  	citire_server(msg,sd);
    	
	
  	if(verificareMsg(msg,"quit"))
	{
		close (sd);
		ruleaza=0;
	}
	else
	if(verificareMsg(msg,"Introduceti parola:"))
	{
		if(logat==0)
		{
		if(login(msg,sd))
			{
			printf("Autentificarea a avut succes!\n");
			logat=1;
						
			}
		else
			{
			printf("Autentificarea nu a avut succes!\n");
			}
		}
		else
		printf("Esti deja logat\n");
	}
	else
	if(verificareMsg(msg,"Alege o parola:"))
	{
		if(logat==0)
		{
		if(logon(msg,sd))
		{
			printf("Inregistrarea a avut succes!\n");
			creare_profil(sd);
		}
		else
			printf("Inregistrarea nu a avut succes!\n");
		}
		else
		printf("Esti deja logat\n");
	}
	else
	if(verificareMsg(msg,"Profil:"))
	{
		vizualizare_profil(sd);
	}
	else
	if(verificareMsg(msg,"Notificari:"))
	{
		vizualizare_notificari(sd);
	}
	else
	if(verificareMsg(msg,"Postari:"))
	{
		vizualizare_postari(sd);
	}
	else
	if(verificareMsg(msg,"Feed:"))
	{
		vizualizare_postari(sd);
	}
	else
	if(verificareMsg(msg,"send_msg:"))
	{
		send_msg(sd);
	}
	else
	if(verificareMsg(msg,"MSG:"))
	{
		vizualizare_postari(sd);
	}
	else
	if(verificareString(msg,"Add_friend:"))
	{
		add_friend(sd);
	}
	else
	{
		printf("%s\n",msg);
	}

  }
}

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  		// mesajul trimis

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      printf ("Eroare la socket().\n");
    }


  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;

  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);

  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }


	rulare_program(sd);
  
}

