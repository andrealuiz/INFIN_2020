#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>



#define REQUEST_MSG_SIZE 1024
#define REPLY_MSG_SIZE 500
#define SERVER_PORT_NUM 5001

void Enviar_Recibir(char missatge [20], char buffer[20]);
void ImprimirMenu(void);

 /************************
*
*
* tcpClient
*
*
*/

int main(int argc, char *argv[]){

char input;
int inputsmods;
int inputsmods1;
int inputsmods2;
char enviado[10];
char recibido[10];
char mostrar[10];
ImprimirMenu(); //mostrem el menu
input = getchar();

/* Demano usuari */

while (input != 's'){ //Mentre no s'apreti 's' per sortir del menu
		
		memset (enviado,'\0',10); //resetejar variables enviado y recibido (buffer y missatge)
		memset (recibido,'\0',10);
		
			switch (input)
			{
				case '1':
					printf("Posar en marxa/parar adquisició\n");
					printf("Selecciona v: ");
					scanf("%d", &inputsmods);
					printf("Selecciona el Temps[2 dígits]: ");
					scanf("%d", &inputsmods1);
					printf("Selecciona el número de mostres[1 dígit]: ");
					scanf("%d", &inputsmods2);
					if (inputsmods1 < 10){
						sprintf(enviado,"{M%d0%d%d}",inputsmods,inputsmods1,inputsmods2); //guardem la cadena a enviado
					} else {
						sprintf(enviado,"{M%d%d%d}",inputsmods,inputsmods1,inputsmods2); //guardem la cadena a enviado
					}
					Enviar_Recibir(enviado, recibido); // cridem la funcio per fer comunicacio amb server
					sprintf(mostrar,"%c%c",recibido[1],recibido[2]);
					printf("%s",mostrar);
					ImprimirMenu(); //tornem a mostrar el menu per seleccionar la opcio novament
					break;
								   
				case '2':

					printf("La mostra mes antiga es:\n");                          
					strcpy(enviado,"{U}"); //Copiar missatge a enviado
					Enviar_Recibir(enviado, recibido);
					sprintf(mostrar,"%c%c%c%c%c",recibido[3],recibido[4],recibido[5],recibido[6],recibido[7]);
					printf("%s",mostrar);
					ImprimirMenu();
					break;

				case '3':
					printf("La mostra máxima es:\n");
					strcpy(enviado,"{X}"); //Copiar missatge a enviado
					Enviar_Recibir(enviado, recibido);
					sprintf(mostrar,"%c%c%c%c%c",recibido[3],recibido[4],recibido[5],recibido[6],recibido[7]);
					printf("%s",mostrar);  
					ImprimirMenu();						 
					break;

				case '4':
					printf("La mostra mínima es:\n");
					strcpy(enviado,"{Y}"); //Copiar missatge a enviado
					Enviar_Recibir(enviado, recibido);
					sprintf(mostrar,"%c%c%c%c%c",recibido[3],recibido[4],recibido[5],recibido[6],recibido[7]);
					printf("%s",mostrar); 
					ImprimirMenu();						   
					break;

				case '5':
					printf("Reset máxim i mínim\n");
					strcpy(enviado,"{R}"); //Copiar missatge a enviado
					Enviar_Recibir(enviado, recibido);
					sprintf(mostrar,"%c%c",recibido[1],recibido[2]);
					printf("%s",mostrar); 
					ImprimirMenu();							
					break;

				case '6':
					printf("El número de mostres guardades son:\n");
					strcpy(enviado,"{B}"); //Copiar missatge a enviado
					Enviar_Recibir(enviado, recibido);
					sprintf(mostrar,"%c%c%c%c",recibido[3],recibido[4],recibido[5],recibido[6]);
					printf("%s",mostrar);
					ImprimirMenu();							
					break;


			case 0x0a: //Això és per enviar els 0x0a (line feed) que s'envia quan li donem al Enter
				break;
			default:
				printf("Opció incorrecta\n");
				printf("He llegit 0x%hhx \n",input);
				break;
		}
		
		input = getchar(); //guardem la opcio seleccionada a la variable input per tornar al switch
	}
return 0;
}

void Enviar_Recibir(char missatge [20], char buffer[20]) {    // es crea una funcio amb els parametres d'entrada buffer i missatge 
		struct 	sockaddr_in serverAddr;    // incluim totes les variabes del socket
		char    serverName[] = "127.0.0.1"; //AdreÃ§a IP on està  el servidor 88.10.160.166
		int 	sockAddrSize;
		int 	sFd;
		int 	result;


		/*Crear el socket*/
		sFd=socket(AF_INET,SOCK_STREAM,0);

		/*Construir l'adreÃ§a*/
		sockAddrSize = sizeof(struct sockaddr_in);
		bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
		serverAddr.sin_family=AF_INET;
		serverAddr.sin_port=htons (SERVER_PORT_NUM);
		serverAddr.sin_addr.s_addr = inet_addr(serverName);

		/*ConexiÃ³*/
		result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
		if (result < 0)
		{
		printf("Error en establir la connexiÃ³\n");
		exit(-1);
		}
	//	printf("\nConnexiÃ³ establerta amb el servidor: adreÃ§a %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
		

		/*Enviar*/
		strcpy(buffer,missatge);
		result = write(sFd, buffer, strlen(buffer));
		//printf("Missatge enviat a servidor(bytes %d): %s\n", result, missatge);


		/*Rebre*/
		result = read(sFd, buffer, 256);
		//printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer);
		

		/*Tancar el socket*/
		close(sFd);


}
void ImprimirMenu(void){

			printf("\n\nMenu:\n");
			printf("--------------------\n");
			printf("1: Opció M, posar en marxa/parar adquisició \n");
			printf("2: Opció U, mostra mes antiga\n");
			printf("3: Opció X, mostra máxima\n");
			printf("4: Opció Y, mostra mínima\n");
			printf("5: Opció R, reset máxim i mínim\n");
			printf("6: Opció B, nombre de mostres guardades\n");
			printf("s: Sortir\n");
			printf("--------------------\n");
}







