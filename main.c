#include<stdio.h>
#include<string.h>
#include<stdlib.h>

///////////////////////////////////////////////////////

/*
* DICHIARAZIONE DELLE DEFINE
*/
#define TRUE 1
#define FALSE 0
#define BLANK '_'
#define HASH_TABLE_DIM 30
#define TRANSITION_STRING_DIM 10 
#define FIRST_ASCII_LETTER_INT 97 // ASCII 97 = 'a'
#define BUFFER_DIM 100
#define EXPAND_STRING_DIM 20

////////////////////////////////////////////////////////

/*
* VARIABILI GLOBALI
*
* Tabella di hash:
* - Array dei puntatori
* - Struct per la transizione
* - Struct per la lista
* - Struct per la coda delle transizioni da svolgere
* - Variabile per il numero massimo di step
* - Definizione dell'enumerazione per i vari step del parsing del file
* - Array per gli stati di accettazione
*/

typedef struct Transition_s{
   int CurrentState, FinalState;
   char ReadenChar, CharToWrite, NextStep;
} Transition;

typedef struct ToDoQueueObject_s{
   char *String;
   int StepNum;
   int Index;
   Transition NextTransition;
   struct ToDoQueueObject_s *next;
} ToDoQueueObject;

typedef struct HashObject_s{
   Transition tr;
   struct HashObject_s *next;
} HashObject;

typedef enum {
   STARTUP = -1,
   TRANSITION = 0,
   ACC = 1,
   MAX = 2,
   RUN = 3
} FileSection;

int *AccStatesArray;
int AccStatesArrayDim;
// MaxStatus indica il numero del più grande stato 
int MaxStatus;
int MaxStep;
char BufferTemp[BUFFER_DIM + 1];
ToDoQueueObject *QueueHead;

HashObject* HashPointers[HASH_TABLE_DIM];
/////////////////////////////////////////////////////////

/*
* DICHIARAZIONE DEI METODI
*/
HashObject* TransitionHeadInsert(HashObject*, Transition*);
void TransitionQueueHeadInsert(ToDoQueueObject*, Transition, char*, int, int);
Transition GetNextTransition(int, char, char*, int, int);
int TransitionHashFunction(char, char);
void FileParsing();
void ReadAndRun();
char* ExpandString(char*, char side);
void RunMT(char*);
void FreeSingleStringMemory(char*);
void InitHashTable(HashObject**);

/////////////////////////////////////////////////////////

int main(int argc, char *argv[]){
   /*
   * FASE 0: INIZIALIZZAZIONI
   * - Tabella di Hash a NULL
   */
   InitHashTable(HashPointers);

   /*
   * FASE 1: Parsing del file e inserimento dei valori
   */
   FileParsing();
  
   //VISUALIZZAZIONE DEI DATI IN MEMORIA
   /*HashObject *h; 
   int i;
   printf("Transizioni lette\n");
   for(i = 0; i < HASH_TABLE_DIM; i++){
      if(HashPointers[i] != NULL)
	 for(h = HashPointers[i]; h != NULL; h = h -> next)
	    printf("Casella %d - transizione %d %c %c %c %d\n", i, h->tr.CurrentState,
	    h->tr.ReadenChar, h->tr.CharToWrite, h->tr.NextStep, h->tr.FinalState);
   }
   printf("Stati di accettazione\n");
   for(i = 0; i < AccStatesArrayDim; i++)
      if(AccStatesArray[i] == TRUE)
      printf("%d\t", i);
   printf("\nMax %d\n Finish\n", MaxStep);*/

   /*
   * FASE 2: Run del simulatore
   */
   ReadAndRun();

   return 0;
}

void FileParsing(){
   /*
   * LETTURA DELLE TRANSIZIONI
   */

   // Variabili di supporto per il parsing del file
   FileSection FileSectionIndex = TRANSITION;
   Transition tr;
   int accState, numCharRead;
   char label[4];

   scanf("%s", label);
   while((int)FileSectionIndex < (int)RUN){
      switch(FileSectionIndex){
         case TRANSITION: {
	    //numCharRead = scanf("%d %c %c %c %d", &tr.CurrentState, &tr.ReadenChar, &tr.CharToWrite, &tr.NextStep, &tr.FinalState);

	    while(scanf("%d %c %c %c %d", &tr.CurrentState, &tr.ReadenChar, &tr.CharToWrite, &tr.NextStep, &tr.FinalState)) {
	       int HashValue = TransitionHashFunction(tr.CurrentState, tr.ReadenChar);
	       HashObject *head = TransitionHeadInsert(HashPointers[HashValue], &tr);
	       HashPointers[HashValue] = head;

	       if(tr.FinalState > MaxStatus)
		  MaxStatus = tr.FinalState;
	    }
	    break;
	 }
	 case ACC: {
	    while(scanf("%d", &accState))
	       AccStatesArray[accState] = TRUE;
	    break;
	 }
	 case MAX: {
	    // Uso la variabile finalState solo per non dichiararne di nuove
	    scanf("%d", &MaxStep);
	    break;
	 }
      }
      
      scanf("%s", label);
       
      //Incremento il FileSectionIndex dato che sono passato alla sezione del file successiva
      (int)FileSectionIndex++;

      // Se arrivo al punto del file dove vengono dichiarati gli stati di accettazione
      // mi preparo allocando spazio per l'array che conterrà gli stati di accettazione.
      AccStatesArrayDim = MaxStatus + 1;
      if(FileSectionIndex == ACC)
	 if(AccStatesArray = (int *) calloc(AccStatesArrayDim, sizeof(int))) ;
	 else printf("Errore nell'allocamento della memoria per l'array degli stati di accettazione. AccStatesArrayDim: %d", AccStatesArrayDim);
   }
}

void ReadAndRun(){
   char *ret;
   char *string = (char *) malloc(sizeof(char));
   string[1] = '\0';

   fgetc(stdin);
   while(fgets(BufferTemp, BUFFER_DIM, stdin)){
      char *tmp = string;
      
      if(string = (char *) malloc((strlen(BufferTemp) + strlen(string) + 1) * sizeof(char))){
	 string[0] = '\0';
	 strcat(string, tmp);
	 strcat(string, BufferTemp);

	 //Libero la vecchia stringa
	 free(tmp);
      }

      if(ret = strchr(string, '\n')){
	 ret[0] = '\0';
	 RunMT(string);
	 string = (char *) malloc(sizeof(char)); 
      }
   }
}

char* ExpandString(char *string, char side){
   char *expandedString;
   char expandeString[EXPAND_STRING_DIM];
   int i;

   for(i = 0; i < EXPAND_STRING_DIM; i++)
      expandeString[i] = BLANK;
   

   if(expandedString = (char *) malloc((strlen(string) + EXPAND_STRING_DIM + 1) * sizeof(char)))
   {
      if(side == 'R'){
	 strcat(expandedString, string);
	 strcat(expandedString, expandeString);
      } else{
	 strcat(expandedString, expandeString);
	 strcat(expandedString, string);
      }
   }

   free(string);

   return expandedString;
}

void RunMT(char* string){
   //printf("Incomincio la simulazione per la stringa %s\n", string);
   
   //Inizializzo tutte le variabili
   int BlockedMT = FALSE,
      AcceptedString = FALSE,
      MaxStepOvered = FALSE,
      stepNum = 0,
      index = 0,
      stringlen = strlen(string),
      currentState = 0;
   Transition tr;
   QueueHead = NULL;

   tr = GetNextTransition(currentState, string[index], string, stepNum, index);
   TransitionQueueHeadInsert(QueueHead, tr, string, stepNum, index); 

   while(QueueHead){
      printf("Partenza nuovo percorso %s\n", string);

      //Parto con la prima transizione della sequenza deterministica
      tr = QueueHead -> NextTransition;
      stepNum = QueueHead -> StepNum;
      string = QueueHead -> String;
      index = QueueHead -> Index;

      //Libero la memoria della transizione in coda e imposto la cosa sulla prossima
      ToDoQueueObject *tmp = QueueHead;
      QueueHead = QueueHead -> next;
      free(tmp);

      //Imposto la MT come non bloccata
      BlockedMT = FALSE;

      while(!BlockedMT && stepNum <= MaxStep){
	 printf("Trovata la transizione da eseguire: %c %c %d\n", tr.CharToWrite, tr.NextStep, tr.FinalState);     
	 //Eseguo le istruzione della transizione
	 if(tr.FinalState == -1){
	    BlockedMT = TRUE;
	    //printf("BLOCCATO\n");
	 }
	 else {
	    string[index] = tr.CharToWrite;
	    currentState = tr.FinalState;
	    
	    if(tr.NextStep == 'R')
	       index++;
	    else if(tr.NextStep == 'L')
	       index--;
	    
	    if(index < 0){
	       string = ExpandString(string, 'L');
	       index = EXPAND_STRING_DIM - 2;
	       stringlen = strlen(string);
	    }
	    else if(index == stringlen)
	    {
	       string = ExpandString(string, 'R');
	       stringlen = strlen(string);
	    }	 

	    //Incremento il numero di step fatti
	    stepNum++;
	    printf("Step numero %d\t Stringa: %s\t Stato corrente %d\n", stepNum, string, currentState);

	    //Prendo la possima transizione dato lo stato corrente e il carattere letto
	    tr = GetNextTransition(currentState, string[index], string, stepNum, index);
	 }
      }

      //Una volta che la macchina si è bloccata controllo se lo stato in cui è arrivato è uno stato di accettazione
      if(stepNum > MaxStep)
	 MaxStepOvered = TRUE;
      else if(stepNum <= MaxStep && AccStatesArray[currentState]){
	 //In caso affermativo stampo 1 e avvio la procedura per la pulizia della memoria
	 printf("1\n");
	 FreeSingleStringMemory(string);
	 AcceptedString = TRUE;
      }
   }

   if(!AcceptedString)
      if(MaxStepOvered)
	 printf("U\n");
      else
	 printf("0\n");
}

Transition GetNextTransition(int currentState, char readenChar, char *string, int stepNum, int index){
   char possibleReadenChar[] = { readenChar, BLANK };
   Transition tr;
   int trCount = 0, i, hashValue;
   HashObject *hashObject;

   //Dando il valore finalState -1 capisco se la ricerca di transizioni è dato risultati o meno
   tr.FinalState = -1;

   for(i = 0; i < 2; i++){
      printf("Cerco la transizione per lo stato %d e il char %c\n", currentState, possibleReadenChar[i]);
      hashValue = TransitionHashFunction(currentState, possibleReadenChar[i]);
      hashObject = HashPointers[hashValue];
      for(; hashObject; hashObject = hashObject -> next)
	 if(hashObject -> tr.CurrentState == currentState && hashObject -> tr.ReadenChar == possibleReadenChar[i]){
	    //Il valore dello stato corrente e del carattere letto non li ritorno dato che non mi servono
	    tr.CharToWrite = hashObject -> tr.CharToWrite;
	    tr.NextStep = hashObject -> tr.NextStep;
	    tr.FinalState = hashObject -> tr.FinalState;

	    if(trCount){
	       TransitionQueueHeadInsert(QueueHead, tr, string, stepNum, index);
	       //printf("Aggiunta una transizione alla coda\n");
	    }

	    trCount++;
	 }
   }

   //if(tr.FinalState == -1)
     // printf("Nessuna transizione trovata\n");

   return tr;
}

int TransitionHashFunction(char c1, char c2){
   return ((int)c1 + (int)c2) % HASH_TABLE_DIM;
}

void TransitionQueueHeadInsert(ToDoQueueObject *head, Transition transition, char* string, int stepNum, int index){
   ToDoQueueObject* tmp;
   tmp = head;

   if(head = (ToDoQueueObject *) malloc(sizeof(ToDoQueueObject))){
      //Copio la nuova stringa
      head -> String = (char *) malloc((strlen(string) + 1) * sizeof(char));
      strcpy(head -> String, string);
      head -> StepNum = stepNum;
      head -> NextTransition = transition;
      head -> Index = index;
      head -> next = tmp;      
   }

   QueueHead = head;
}

HashObject* TransitionHeadInsert(HashObject *head, Transition *transition){
   HashObject* tmp;
   tmp = head;

   if(head = (HashObject *) malloc(sizeof(HashObject))){
      head -> tr.CurrentState = transition -> CurrentState;
      head -> tr.ReadenChar = transition -> ReadenChar;
      head -> tr.CharToWrite = transition -> CharToWrite;
      head -> tr.NextStep = transition -> NextStep;
      head -> tr.FinalState = transition -> FinalState;
      //head -> tr = transition;	 
      head -> next = tmp;
   }

   return head;
}

/*
* Funzione che libera la memoria dopo che l'analisi di una stringa è stata completata
*/
void FreeSingleStringMemory(char *string){
   //Svuoto la TO DO list delle transizioni
   for(; QueueHead; QueueHead = QueueHead -> next){
      ToDoQueueObject *tmp = QueueHead;
      free(tmp);
   }

   //Libero la stringa sulla quale ho fatto le operazioni
   free(string);
}

void InitHashTable(HashObject **table){
   int i;
   for(i = 0; i< HASH_TABLE_DIM; i++)
      table[i] = NULL;
}
