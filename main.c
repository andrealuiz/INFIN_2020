/***************************************************************************
                          main.c  -  threads
                             -------------------
    begin                : ene 30 19:49:08 CET 2002
    copyright            : (C) 2002 by A. Moreno
    email                : amoreno@euss.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define SERVER_PORT_NUM		5001
#define SERVER_MAX_CONNECTIONS	4

#define REQUEST_MSG_SIZE	1024
#include <pthread.h>
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
#include <sys/wait.h>
#include <termios.h>
#include <sys/time.h>
#include <fcntl.h>                                                             
#include <sys/ioctl.h>  


#define BAUDRATE B9600                                                
//#define MODEMDEVICE "/dev/ttyS0"        //Conexió IGEP - Arduino
#define MODEMDEVICE "/dev/ttyACM0"         //Conexió directa PC(Linux) - Arduino                                   
#define _POSIX_SOURCE 1 					/* POSIX compliant source */       
#define TAMMUES 100  

pthread_t thread;
pthread_mutex_t mutex;


int	ConfigurarSerie(void);
void Enviar(int fd,char *missatge);
void Rebre(int fd,char *buf);
void TancarSerie(int fd);   
void intrcadena(float numero, char *cadena);   
void* codi_fill(void* parametre);                                                    
struct termios oldtio,newtio; 



int  		v, z = 0;
float		muestras [TAMMUES];
float		mayor=00.00;
float 		menor=99.99;
int 		temps;
int 		most_mitjana;
int 		Novamarxa=0;
int			mogua = 0;
char		tempa[5]; // temp antiga
char		tempmx[5]; // temp màxim
char		tempmn[5]; // temp mínim

int main(int argc, char *argv[])
{
	
	
	struct sockaddr_in	serverAddr;
	struct sockaddr_in	clientAddr;
	unsigned int		sockAddrSize;
	
	int			sFd;
	int			newFd;
	int 		result;
	char		buffer[256];
	char		missatge[256];
	
		
	pthread_mutex_init(&mutex,NULL);
 
	pthread_create(&thread, NULL, codi_fill, 0); //Es crea el thread fill

     // Proces Pare
	printf ("\n Proces Pare\n");
	   
	while(1){  	
		
				
		/*Preparar l'adreça local*/
		sockAddrSize=sizeof(struct sockaddr_in);
		bzero ((char *)&serverAddr, sockAddrSize); //Posar l'estructura a zero
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT_NUM);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		/*Crear un socket*/
		sFd=socket(AF_INET, SOCK_STREAM, 0);
		
		/*Nominalitzar el socket*/
		result = bind(sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
		
		/*Crear una cua per les peticions de connexió*/
		result = listen(sFd, SERVER_MAX_CONNECTIONS);
		
		/*Bucle s'acceptació de connexions*/
		
		while(1){
			
			printf("\nServidor esperant connexions\n");

			/*Esperar conexió. sFd: socket pare, newFd: socket fill*/
			newFd=accept(sFd, (struct sockaddr *) &clientAddr, &sockAddrSize);
			printf("Connexión acceptada del client: adreça %s, port %d\n",	inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

			/*Rebre*/
			memset( buffer, 0, 256 );
			result = read(newFd, buffer, 256);
			printf("Missatge rebut del client(bytes %d): %s\n",	result, buffer);
		
			pthread_mutex_lock(&mutex);// inici mutex			

			if (buffer[0] == '{') {    // si hi ha claudator a la primera posició entrar al switch
			
			switch (buffer[1])  // posició 1 del buffer conté la lletra:
			{
					case 'M':  // cas 'M'
						
						if (buffer[2] == 48){ // si v=0 parem execució
							v=0;
							Novamarxa=1;
							
							break;
							} 
						if (buffer[2] > 49){ // si v=0 parem execució
							sprintf(missatge,"{M2}\n"); // ERROR paràmetres: paràmetres fora rang o estat invàlid per la comanda
							break;
							} 
						
						if (buffer[3] >= 50 && buffer[4] >= 49){ // Si buffer[3] es >= a 2 i buffer[4] >= a 1 llavors:
							sprintf(missatge,"{M2}\n"); // ERROR paràmetres: paràmetres fora rang o estat invàlid per la comanda	
							break;
						}
							
						if (buffer [3] > 50){
							sprintf(missatge,"{M2}\n"); // ERROR paràmetres: paràmetres fora rang o estat invàlid per la comanda	
							break;
						}
						
						
							
						if (buffer[5] >= 49){
							
							most_mitjana = buffer[5] - '0';	// Això representa: ((int)a) - ((int)'0'). Representa que com el valor 0 en ASCII es mes petit que els valors seguents,
														// la diferencia entre ells dona el valor del caracter que representa
							
							} else {
								sprintf(missatge,"{M2}\n"); // ERROR paràmetres: paràmetres fora rang o estat invàlid per la comanda	
								break;	
								}
								
							if (buffer[6] != '}'){  // si la última posició del buffer no es un claudator
							
							sprintf(missatge,"{M1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
							
							} else {  // d'altre banda retornar cadena guardada a la variable missatge
								
								Novamarxa=1;
								v=1;
								 
								temps= (buffer[3]-'0')*10+ (buffer[4]-'0');
							
								sprintf(missatge,"{M0}\n"); // OK
							
							}		
							
							break;
							
						
					case 'U':	// cas 'U'
					
											
						if (buffer[2] == '}'){ // si la última posició de buffer es un claudator
							
										
									if (mogua >= TAMMUES && (z != TAMMUES -1) ){ 
										
										sprintf(missatge,"{U0%.2f}",muestras[z+1]);
									
									}else{ sprintf(missatge,"{U0%.2f}",muestras[0]);
										
									}
						} else {
							
							sprintf(missatge,"{U1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda
							
							
						} 
						
						break;
						
						
					case 'X':	// cas 'X'
						
							
							if (buffer[2] != '}'){  // si la última posició es diferent a un claudator
							
								sprintf(missatge,"{X1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
							
							} else {  
							
							if (mayor < 10){ // si mayor es inferior a 10:
								
								sprintf(missatge,"{X00%.2f}",mayor); // guardar cadena a missatge
								
								} else { 
									
									sprintf(missatge,"{X0%.2f}",mayor);
									
									} 
							} // si el buffer conte el claudator guardar cadena a missatge
							
							
							
						break;
								
					
					case 'Y':	// cas 'Y'
																											
								
																								
							if (buffer[2] != '}'){	// si la última posició es diferent a un claudator
							
							sprintf(missatge,"{Y1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
							
							} else {
							
								if (menor < 10){
									
									sprintf(missatge,"{X00%.2f}",menor); // guardar cadena a missatge
									
									} else {
										
										sprintf(missatge,"{X0%.2f}",menor); // guardar cadena a missatge
										
										}
							}
							
							
							break;
							
					case 'R':	// cas 'R'
					 
							mayor=00.00; // reiniciar variable mayor i menor
							menor=99.00; 
					
							if (buffer[2] != '}'){  // si la última posició es diferent a un claudator
								
							sprintf(missatge,"{R1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda
							
							} else {
							
							sprintf(missatge,"{R0}\n");	//OK
									
							}
						
							
					
							break;
							
						
					
					case 'B':	// cas 'B'
			
							if (buffer[2] != '}'){  // si la última posició es diferent a un claudator
							
							sprintf(missatge,"{B1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
							
							} else {
							
							sprintf(missatge,"{B0%4.i}\n", mogua);	//OK
							
							}
						 
					
							break;
						
			
	
			}
			
			pthread_mutex_unlock(&mutex);	// Acaba mutex
			
			/*Enviar*/
			
			result = write(newFd, missatge, strlen(missatge)+1); //+1 per enviar el 0 final de cadena
			printf("Missatge enviat a client(bytes %d): %s\n",	result, missatge); 

			/*Tancar el socket fill*/
			result = close(newFd);
			
			}
			
		}

		pthread_join(thread, NULL);

		printf("Proces pare PID(%d)\n",getpid());

		return EXIT_SUCCESS;
}
}

void* codi_fill(void* parametre){ // codi thread fill
	
	int 		x,  fd; 
   	char 		buf[255];
	char 		missatge[255];                                                                                                                              
	float 		mitjana= 00.00;
	float 		resmitjana = 00.00;
    int 		v2  = 0;
    float 		prova = 00.00;
	char		tempbin [7];                                                         
	int 		buf2;
	
	/*Variables que utilitzem per reescalar la temperatura i convertir-la a graus*/
	 
	long in_min = 0000;
	long in_max =1023; 
	long out_min = 00000; 
	long out_max = 12000;
	float map;
	float tempgraus = 00.00;
	
	
  	fd = ConfigurarSerie();        //Obrim el port sèrie
	
	
	
	/*Enviem el missatge 1*/
	
	pthread_mutex_lock(&mutex); // inici mutex
	printf ("\n Proces Fill\n");
	memset(missatge,'\0', 255);			//Reset dels arrays missatge i buf
	memset(buf,'\0', 255);
	sprintf(missatge,"AM%i%02iZ", v, temps);
	
	pthread_mutex_unlock(&mutex);	// Acabar mutex
	
	printf ("%s\n", buf);
	
	memset(muestras,'\0', TAMMUES*sizeof(float));
	
	
	/*S'envia la comanda ACZ i rebem les temperatures en bucle*/

	while (1){ 
	
		if (Novamarxa == 1){
		
			Novamarxa=0;
			memset(missatge,'\0', 255);
			sprintf(missatge,"AM%i%02iZ", v, temps);
			Enviar(fd,missatge);
			Rebre(fd,buf);
			
		if (v==0){
			
			v2=0;
			
		}else{
			
			v2=1;
		}	
							
		}
		if (v2==1){
			
		
		memset(missatge,'\0', 255);
		memset(buf,'\0', 255);
		sprintf(missatge,"ACZ");
		sleep (temps*2);
		Enviar(fd,missatge);
		Rebre(fd,buf);
		pthread_mutex_lock(&mutex);// inici mutex
		
		sprintf(tempbin,"%c%c%c%c",buf[3],buf[4],buf[5],buf[6]); 		//Guardem les posicions de l'array qque continguin la temperatura a la variable tempbin
		tempbin [4] = 0;
	
		
		
	/*Es fa la conversió, primer de l'array a integer, i després a graus*/
	
		buf2= atoi(tempbin);
		if (((in_max - in_min) + out_min) == 0){
			printf ("Error el denominador es 0\n\n");
		}else{
			map = (buf2 - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
			tempgraus = map/100;
			printf ("Ultima temperatura es %02.2fºC\n\n",tempgraus);
		}
		
	/*Creem l'array circular*. Quan muestras[z] estigui ple de valors de temperatures, és a dir quan s'hagin fet 100 lectures, 
	 * les noves lectures s'aniran afegint al principi de l'array de muestras, fins repetir la condició (z== TAMMUES)*/
			
		if (z >= TAMMUES){  
			
			z=0;
			muestras[z] = tempgraus;
			
		}else{ 
											
			muestras[z] = tempgraus;
			
			
		}
		
		
	/*Fem la mitjana de les mostres especificada a la primera comanda (most_mitjana)*/			
		
	for (int l=0; l<= most_mitjana -1; l++){
		
		z = z-l;
		
		if (z < 0){
			
			x = TAMMUES+z;
			prova = muestras[x];
			
		} else {
			
			prova = muestras[z];
				
		}
		
		mitjana = mitjana + prova;
		z=z+l;

		}
		if (most_mitjana == 0){
			
			printf ("Error el denominador es 0\n\n");
			
		}else{
			
			resmitjana = mitjana / most_mitjana;
			
		}
	/*Creem dos comparadors de màxim i mínim històric. 
	* Si la nova lectura és major o menor que la guardada a la variable "mayor" i "menor", respectivament, s'actualitza el seu valor*/
		
		if (muestras[z]> mayor){ 	
									
			mayor=muestras[z];
		} 
								
		if (muestras[z]< menor){ 
										
			menor=muestras[z];
			
		} 
		
		mogua ++;			//Comptador per lectures realitzades
		
		
		printf("-----------------------Informació sobre les lectures--------------------\n\n");
		printf("La temperatura minima es: %2.2fºC \n",menor);
		printf("La temperatura maxima es: %2.2fºC\n",mayor);
		printf("El numero de mostres realitzades es: %i \n",mogua);
		printf("La mitjana de les %i ultimes mostres es: %02.2fºC\n\n\n",most_mitjana, resmitjana);
	
		mitjana=0;
		
	/*Enviem la comanda "AS131Z"* o "AS130Z" per fer pampallugues amb el LED 13 amb cada lectura realitzada*/
		
		
		
		if (mogua%2 != 0){
			
			memset(missatge,'\0', 255);
			memset(buf,'\0', 255);
			sprintf(missatge,"AS131Z");
			sleep (1);
			
			pthread_mutex_unlock(&mutex);
			
			Enviar(fd,missatge);
			Rebre(fd,buf);
			
		}else{
			
			memset(missatge,'\0', 255);
			memset(buf,'\0', 255);
			sprintf(missatge,"AS130Z");
			sleep (1);
			
			pthread_mutex_unlock(&mutex);
			
			Enviar(fd,missatge);
			Rebre(fd,buf);
			
		}
		z++;
		
		} // if
		
	} // for
	
	
  	sleep(2); // Retard de 2 segons
  	pthread_exit(NULL);
	return NULL;
}





/////////////////////////ARDUINO-SERVER/////////////////////////

int	ConfigurarSerie(void)
{
	int fd;
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
	if (fd <0) {perror(MODEMDEVICE); exit(-1); }

	tcgetattr(fd,&oldtio); /* save current port settings */

	bzero(&newtio, sizeof(newtio));
	//newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	sleep(2); //Per donar temps a que l'Arduino es recuperi del RESET
	return fd;
}              



void TancarSerie(int fd)
{
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
}
void Enviar(int fd,char *missatge)
{
	int res=0;
	printf ("Enviar missatge: %s\n", missatge);
	res = write(fd,missatge,strlen(missatge));
	
	if (res <0) {tcsetattr(fd,TCSANOW,&oldtio); perror(MODEMDEVICE); exit(-1); }
	
	
}
void Rebre(int fd,char *buf){
	
	int i = 0;
	int res = 0;
	int bytes = 0;
	
	ioctl(fd, FIONREAD, &bytes);
	
	/*Llegir el array (buf) i acumular el nombre de bytes (res) fins arribar al caràcter Z */

	do
	
	{
		res = res + read(fd,buf+i,1);
		
		i++;
	
	}while (buf[i-1] != 'Z');
	
		
	printf ("Rebre buf: %s\n", buf);
}

void intrcadena(float numero, char *cadena){ // Funció amb variable numero i punter de cadena, 
    sprintf(cadena,"%2.2f", numero);		 // serveix per guardar variables float a les respectives cadenes
}

