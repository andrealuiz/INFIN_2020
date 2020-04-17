/***************************************************************************
                          main.c  -  server
                             -------------------
    begin                : lun feb  4 15:30:41 CET 2002
    copyright            : (C) 2002 by A. Moreno
    copyright            : (C) 2020 by A. Fontquerni
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

#define SERVER_PORT_NUM		5001
#define SERVER_MAX_CONNECTIONS	4

#define REQUEST_MSG_SIZE	1024
/************************
*
*
* tcpServidor
*
*
*/

void intrcadena(float numero, char *cadena){ // Funció amb variable numero i punter de cadena. 
    sprintf(cadena,"%2.2f", numero);		 // Serveix per entrar un float a la variable numero
											 // i la retorna en un char en la variable cadena.
}

int main(int argc, char *argv[])
{
	
	/*Declaració de variables*/
	
	struct sockaddr_in	serverAddr;
	struct sockaddr_in	clientAddr;
	unsigned int			sockAddrSize;
	int			sFd;
	int			newFd;
	int 		result;
	char		buffer[256];
	char		missatge[256];
	int 		mitjana;
	int			i;
	int			j;
	char		tempa[5]; // temp antiga
	char		tempmx[5]; // temp màxim
	char		tempmn[5]; // temp mínim
	float		mayor= 00.00;
	float 		menor= 99.99;
	float		muestras [10];
	int			mogua= 0; // mostres guardades
	
	
	/*Introduim 12 temperatures de les quals s'agafaran les darreres 10*/

	printf ("Introduce 12 temperaturas :\n"); 
	for (i = 0; i <= 12;){  			// Entrem tantes temperatures com
										// marqui la condició i <= __, (L'expressió __
		scanf("%f", &muestras[i]);		// representa el valor del array circular). 
			 			 
		if (i > 10){  					// Una vegada i supera el valor de la 
			muestras[10] = muestras[i]; // condició i > __, les mostres que introduim
		} 								// es guardaran a la posició muestras[10].
				
		i ++; 			
		mogua ++;		
		
		if (mogua > 10){			// Si les mostres guardades superan en valor que hi ha al if anterior. 
			
			for (j = 1; j <= 10;){	// Inicialitzem j = 1 i fem un bucle perque desde muestras[1] 
									// fins a muestras[__] baixin una posició.
				muestras[j-1] = muestras[j];
				j ++;		
			
			} 
			mogua --; 
		}
		
	}
	
	
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
	
		/*Començem a explicar cada cas*/			

		if (buffer[0] == '{') {    // si segueix el protocol establert
		
		switch (buffer[1]){  // posició 1 del buffer conté la lletra
	
		/*Posem en marxa/parem adquisició*/
			
		case 'M':  
			
			if (buffer[2] == 48){ // si v=0 parem execució
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
				
				mitjana = buffer[5] - '0';	// Això representa: ((int)a) - ((int)'0'). Representa que com el valor 0 en ASCII es mes petit que els valors seguents,
											// la diferencia entre ells dona el valor del caracter que representa
								
					
			} else {
				sprintf(missatge,"{M2}\n"); // ERROR paràmetres: paràmetres fora rang o estat invàlid per la comanda	
				break;	
			}
					
			if (buffer[6] != '}'){  // si la última posició del buffer no es un claudator
				
				sprintf(missatge,"{M1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
				
			} else {  // d'altre banda retornar cadena guardada a la variable missatge
					
				sprintf(missatge,"{M0}\n"); // OK
					
			}		
			
			break;
			
		/*Mostrem mostra més antiga*/
	
		case 'U':
			
	
			intrcadena(muestras[0],tempa); // Útilitzem la funció intrcadena per guardar muestras[0] dintre la cadena tempa
	
			if (buffer[2] == '}'){ // si la última posició de buffer es un claudator
				
			
				if (mogua <= 0){	// Si ja no queden mostres guardades 
								
					muestras[0]=-300;	
								
				} 
										
				if (muestras[0] < 10 && muestras[0] >-10){ // Si muestras[0] es menor a 10 i muestras[0] >-10, guardar cadena a missatge
				
					sprintf(missatge,"{U00%.2f}",muestras[0]);  
				
				} 
				/* Si muestras[0] <=-300, printejar 
				*i guardar "{00-----}" en missatge*/
				else if (muestras[0] <=-300){ 
											  
					sprintf(missatge,"{00-----}");
						
				} else {	
						
				sprintf(missatge,"{U0%.2f}",muestras[0]);	// guardar cadena a missatge
						
				}	
				
					
				/* Establint __ valors de mostres com a màxim, a mida que  
				 * s'afegeixin més, que la mostra més antiga desapareixi i 
				 * que les demes es moguin a una posició menor*/ 
				 
				for (i = 0; i <= 10;){ 
		
					muestras[i-1] = muestras[i];  
						
					i ++;	
							 
				}
				
				mogua = mogua -1;	// El numero de mostres també anirà reduint
						
			} else {
			
				sprintf(missatge,"{U1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda
			
			} 
			
			break;
			
		/*Mostrem mostra més gran*/
			
		case 'X':	
		
			/*Calculem el màxim dintre de l'array. Comparem muestras[i] amb la variable mayor*/
										
			for (int i=0; i<10; i++){	
				
				if (muestras[i]> mayor){ 	
					
					mayor=muestras[i]; //s'actualitza la variable mayor amb la mostra més gran
				} 
			}
			
			intrcadena(mayor, tempmx);	// Útilitzem la funció intrcadena per guardar, mayor dintre la cadena tempmx
			
			
			if (buffer[2] != '}'){  // si la última posició es diferent a un claudator
			
				sprintf(missatge,"{X1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
			
			} else {  
			
				if (mayor < 10){ 									// si mayor es inferior a 10
					
					sprintf(missatge,"{X00%.2f}",mayor); 			// guardar cadena a missatge
					
					
				} else { 
						
					sprintf(missatge,"{X0%.2f}",mayor);				 // si el buffer conte el claudator guardar cadena a missatge
					
				} 
			}
			
			
			
		
			break;
			
			
		/*Mostrem mostra més petita*/
			
		
		case 'Y':	// cas 'Y'
		
			/*Calculem el mínim dintre de l'array. Comparem muestras[i] amb la variable menor*/
		
									
			for (int i=0; i<10; i++){	
					
				if (muestras[i]< menor){  
						
					menor=muestras[i];  //s'actualitza la variable menor amb la mostra més petita
				} 
			}
				
			intrcadena(menor, tempmn); // Útilitzem la funció intrcadena per guardar, menor dintre la cadena tempmn
					
													
			if (buffer[2] != '}'){	// si la última posició es diferent a un claudator
			
				sprintf(missatge,"{Y1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
			
			} else {
			
				if (menor < 10){
					
					sprintf(missatge,"{Y00%.2f}",menor); // guardar cadena a missatge
					
				} else {
						
					sprintf(missatge,"{Y0%.2f}",menor); // guardar cadena a missatge
						
				}
			}
			
		
		
			break;
			
		/*Reset màxim i mínim*/
			
		case 'R':	// cas 'R'
			
			menor= 99.99;
			
			mayor= 00.00;
		 
			if (buffer[2] != '}'){  // si la última posició es diferent a un claudator
				
			sprintf(missatge,"{R1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda
			
			} else {
				
			sprintf(missatge,"{R0}\n");	//OK
					
			}
			
			break;
			
			
		/*Número de mostres guardades*/
		
		case 'B':	// cas 'B'

			if (buffer[2] != '}'){  // si la última posició es diferent a un claudator
			
				sprintf(missatge,"{B1}\n"); // ERROR protocol: error format trama «{...}\n» rebuda o comanda desconeguda	
			
			} else {
			
			sprintf(missatge,"{B0%4.i}\n", mogua);	//OK
			
			}
		 
		
			break;
				
			
		}
		
		/*Enviar*/
		
		result = write(newFd, missatge, strlen(missatge)+1); //+1 per enviar el 0 final de cadena
		printf("Missatge enviat a client(bytes %d): %s\n",	result, missatge); 

		/*Tancar el socket fill*/
		result = close(newFd);
		
		}	
	}


}
