
#define Tamany_comanda 50

/*Declaració de variables*/

int Marxa = 0; //Valor llegit de la comunicació serie, possada en marxa
int S=0; //Sortida del valor digital temperatura
int C; // Codi retorn
int temperaturaguarda[9]; // Inicialitzem variable '{' 'U' c n n n n n '}' • c: codi retorn (0 / 1 / 2) • nnnnn: valor temperatura en graus centígrads en 5 xifres, un d'elles pel punt decimal
float temperatura; 
int ValorSensor = 0000 ;
int NumOper;  //longitud array
char operacio[Tamany_comanda];
// Cas A
int temps; //Temps de mostreig (en Ordre de marxa)
const int tempo = temps;
int u;
int v;
// Cas S
  // Número de sortida digital (dos digits)
char valors[7];   // valor sortida
char resposta[7];
char llegir[7];
char entrada ;
const int AnalogIn = A0;  //Es declara el nom de la entrada analògica A0    
int sortida;

/*Prototip de les funcions utilitzades*/

int parametres (char comanda[]);
int protocol(char comanda[], int N);

/*-------------------------------------------------------------------*/


  
void setup()
{ 
 
// S'INICIALITZA EL TIMER 1
    noInterrupts();           /* disable all interrupts                                                        */
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    OCR1A = 62500;            /* compare match register 16MHz/256/1Hz (SALTA LA INTERRUPCIÓ CADA 1s)        */
    TCCR1B |= (1 << WGM12);   /* CTC mode                                                                      */
    TCCR1B |= (1 << CS12);    /* 256 prescaler                                                                 */
    TIMSK1 |= (1 << OCIE1A);  /* enable timer compare interrupt                                                */
    interrupts();             /* enable all interrupts                                                         */
    Serial.begin(9600);       /* abre el Puerto serie                                                          */
}



//           ************************************** INTERRUPCIÓ DEL TIMER 1 *****************************************

ISR(TIMER1_COMPA_vect)          
{
  ValorSensor = analogRead(AnalogIn);
// ADQUISICIÓ DADES


if (Marxa==1) {
      u++;   /* Només incrementem la <<u>> si la adquisició de lectures es troba en Marxa ja que aquesta és la condició 
                                per a contar temps. Si no incrementessim la <<u>> sota aquesta condició el temps aniria contant de tal 
                                 forma que si passem de <<Marxa=0>> a <<Marxa=1>> la <<u>> en aquest moment podria valdre qualsevol
                                 valor diferent de 0 i per tant no estariem contant el temps de mostreig que s'ens solicita en la primera
                                 mostra de la adquisisció. */ 
  
   }
 // Adquisició de mostres
  
    if ((u==temps) && (Marxa==1)) {                          /* Es realitza la adquisició si u==temps i seleccionem marxa d'adquisisció.*/
     
          //ValorSensor = random(0,1023); 
          //ValorSensor = analogRead(AnalogIn);                             // lectura del pin A0 (0 a 1023)
          //temperatura = map(ValorSensor, 0000, 1023, 0000, 12000);           // "Tradueix" el valor llegit a graus
          //temperatura = temperatura/100;
          //Serial.print(temperatura);                             // Escriu el valor de la temperatura per el port serie
          // Serial.print(" grados centigrados \n");
          u=0;            // reiniciem el contador// Escriu les unitats            
      }                  
   
    
      
}      
  
//Funcio per comprovar que el protocol es correcte

int protocol(char comanda[],int N) {    

  char inici_trama;
  char fi_trama;
  int codi_return;
  char A ;
  int i;
  int P;

  char prova[Tamany_comanda];
 
     inici_trama = comanda[0];
     fi_trama = comanda[N-1];

   
  /*Primer comprovem que la comanda comença pel caràcter 'A' i acaba amb el caràcter 'Z+'\n'', 
   * despres utilitzem un for per recorrer l'array i mirem que no es repeteixi el caràcter 'A'.
   * Cridem la funció de comprovació d'errors de tipus paràmetres (serà explicada més endevant)
   * i si està tot correcte, la funció retorna un 0 (OK).
   * Per contra, si la funció paràmetres retorna un 1 o els càracters d'inici i fi de trama son diferents, 
   * la funció protocol retornarà un 1(ERROR Protocol) o un 2(Error Paràmetres) respectivament.*/
  

     if ( inici_trama == 'A' && fi_trama == 'Z'){          
                  for (i=1; i<N; i++){
                      if (comanda[i]!= 'A'){
                  
                              P = parametres(comanda);  
                              if (P == 0){
                                codi_return= 0;   //OK
                              }else{
                                codi_return= 2;  //ERROR parametres
                               }
          
                       }  else {
                           codi_return= 1; //ERROR protocol
                          }
                    }    
     }else {
     
       codi_return= 1; //ERROR protocol
            
      }
     return codi_return;    //retorn de la funció protocol
    }

//Funció per comprovar que els paràmetres són correctes

int parametres(char comanda[]) {     

  int tiempo_muestreo; //Temps de mostreig (en Ordre de marxa)
  int Num_Sortida_D; //
  int Num_Entrada_D; //



/* Els paràmetres introduïts a la comanda tenen un format diferent per cada operació, per això s'hagut de fer ús d'un switch. 
 *Bàsicament aquests engloben paràmetres fora de rang o que impliquin un estat invàlid per la comanda
 *La funció paràmetres retornarà un 1(ERROR Paràmetres) o un 0 (tot OK)*/
  
     switch(comanda[1]){

        case 'M': 
 /*   
         *comanda [2] == Marxa             (Si la comanda solicita aturar la adquisició de mostres i ja esta aturada o viceversa)
         *comanda [2] > 1                  (Si el digit és superior a 1 ja que s'espera una entrada booleana 1-Marxa 0-Aturar) 
         *1 > tiempo muestreo > 20         (Si la variable es troba fora de rang, ja que s'espera que estigui entre 1-20s)
        
   */  
              tiempo_muestreo= (operacio[3]-'0')*10 + (operacio[4]-'0');          //Es concatenen les dues posicions de temps per llegir-les com un enter


                 if (((comanda[2]== '0') && (Marxa==0))   ||
                      ((comanda[2]== '1') && (Marxa==1))  ||
                      ((comanda[2]-'0') > 1)              ||
                      ((1 > tiempo_muestreo ))             || 
                      ((tiempo_muestreo > 20))){ 
                     
            
                          return 1;                              // ERROR parametres        
                 }else{
                          return 0;                             //OK
                 }
                   
              
        break;
 
        case 'S': 
        
    /*   
         *comanda [4] > 1                  (Si el digit és superior a 1 ja que s'espera una entrada booleana 1-Marxa 0-Aturar) 
         *13 > Num_sortida_D > 0           (Si el número de sortida digital no és correcte (l'Arduino té 13 sortides)
        
   */  
        
             Num_Sortida_D = (operacio[2]-'0')*10 + (operacio[3]-'0');    //Es concatenen les dues posicions de la sortida digital i les llegim com un enter
        
                   if (((comanda[4]-'0') > 1)    || 
                      ((Num_Sortida_D < 0))       ||
                      ((13 < Num_Sortida_D))){
                     
          
                           return 1;                             // ERROR parametres 
                   }else{
                           return 0;                             //OK
                   }
               
          
         break;

 

        case 'E': 

   /*   
     
         *13 > Num_entrada_D > 0           (Si el número d'entrada digital no és correcte (l'Arduino té 13 sortides)
        
   */ 

             Num_Entrada_D = (operacio[2]-'0')*10 + (operacio[3]-'0');           //Es concatenen les dues posicions d'entrada digital i les llegim com un enter
                
                     if ((13 < Num_Entrada_D) ||
                        (Num_Entrada_D < 0)){
                                     
                         return 1;                          // ERROR parametres 
                     }else{
                         return 0;                          //OK
                      }
                       
                  
         break;

 
        case 'C': 

    /*   
     
         Marxa==0           (Si el programa es troba en mode PARADA --> no hi ha mostres actualitzades)
        
   */

                  if (Marxa==0){

                         return 1;             //ERROR parametres
                  }else{              
                        return 0;            //OK
                   }
      
          break;

       }   
      

  }



void loop(){

  /* Comprovació de dades al monitor Serie amb la funció Serial.available i lectura de la comanda introduïda amb la funció Serial.readBytesUntil. 
   *  Aquesta ens permetrà guardar la comanda a la variable 'operació' i la llargada de l'array a la variable 'NumOper'.
   */
  int j;
  if (Serial.available()>0){
    
    size_t NumOper = Serial.readBytesUntil('\n', operacio, Tamany_comanda);
   
 
 
    for (j=0;j<NumOper; j++){

     // Serial.print(operacio[j]);
      
    }
    
 // Serial.println();
  
  switch(operacio[1]){

        case 'M': 
      
      
   /* ◦ Operació 'M': Marxa / Parada conversió. Posar en marxa o parar l'adquisició. Es
    *   selecciona el temps de mostreig.*/
         
                  
           C = protocol(operacio, NumOper);  // funció per comprovar que el protocol es correcte
          
          
          if (C == 0){                    //si no hi ha cap ERROR s'executa la operació del cas M

            Marxa = operacio[2]-'0';
            temps= (operacio[3]-'0')*10+ (operacio[4]-'0');
        
          }
              
           sprintf(resposta, "AM%iZ", C);  // Enviem resposta
           Serial.print(resposta); 
           break;
        
        case 'S':    
          
        /* ◦ Operació 'S': SORTIDA (n) Digital amb valor (v 'operacio[4]').*/
           
           
           C = protocol(operacio, NumOper);  // funció per comprovar que el protocol es correcte
    
          
          sortida= (operacio[2]-'0')*10+ (operacio[3]-'0');
          pinMode (sortida, OUTPUT);        //declarem variables com a OUTPUT del Arduino
          
          if (C == 0){
            
            if (operacio[4] == '1'){
        
                  digitalWrite(sortida, HIGH);
                  S = digitalRead (sortida);
                 // Serial.print("La sortida digital esta en l'estat (HIGH=1/LOW=0):");
                 // Serial.println(S);
                  
            } else {
                 
                  digitalWrite (sortida, LOW);
                  S = digitalRead (sortida);
                  //Serial.print("La sortida digital esta en l'estat (HIGH=1/LOW=0):");
                  //Serial.println(S);
            }
          } else {
              
              //Serial.println("Error en protocol o parametres");
           }
          
          sprintf(resposta, "AS%iZ", C);
          Serial.print(resposta);
          break;
          
        case 'E':

        /* ◦ Operació 'E': ENTRADA (n) Digital.*/
          
          entrada= (operacio[2]-'0')*10+ (operacio[3]-'0');
          
          pinMode (entrada, INPUT);   //declarem variables com a INPUT del Arduino
    
         
          C=protocol(operacio, NumOper);  //funció per comprovar que el protocol es correcte

          if(C == 0){
            
            v=digitalRead (entrada);                  
            sprintf(resposta, "AE%i%iZ", C, v);
            Serial.print(resposta);
          
          } else {
            
           // Serial.println("Error en protocol o parametres");
            sprintf(resposta, "AE%iZ", C);
            Serial.print(resposta);
            
          }
         
          break;
          
        case 'C':
          
        /* ◦ Operació 'C': CONVERTIDOR Analògic-Digital. El Sistema Embedded sol·licita la
        darrera mostra. El Control Sensor envia la dada. El format està en l'escala de
         convertidor (0000 .. 1023), en el Control Sensor no es converteix a graus
         centígrads.*/
          
          C = protocol(operacio,NumOper); //funció per comprovar que el protocol es correcte

          if (C == 0){
                
               // Serial.print("Darrera mostra escala convertidor: "); 
                //Serial.println(ValorSensor);
                sprintf(resposta,"AC%i%04iZ", C, ValorSensor);
                Serial.print(resposta); 
              
               
            
          } else {
              
              // Serial.println("Error en protocol o parametres");
               sprintf(resposta,"AC%i9999Z", C);
               Serial.print(resposta);
              
          }
            break;
    }
  }
  
}
