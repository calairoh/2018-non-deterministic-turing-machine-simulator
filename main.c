#include<stdio.h>
#include<string.h>
#include<stdlib.h>

///////////////////////////////////////////////////////

/*
* DICHIARAZIONE DELLE DEFINE
*/
#define TRUE 1
#define FALSE 0
#define HASH_TABLE_DIM 30
#define TRANSITION_STRING_DIM 10 
#define FIRST_ASCII_LETTER_INT 97 // ASCII 97 = 'a'
#define BUFFER_DIM 100 

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

HashObject* HashPointers[HASH_TABLE_DIM];
/////////////////////////////////////////////////////////

/*
* DICHIARAZIONE DEI METODI
*/
HashObject* TransitionHeadInsert(HashObject*, Transition*);
int TransitionHashFunction(char, char);
void FileParsing();
void ReadAndRun();
void RunMT(char*);
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
   /*int i;
   HashObject *h;
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
	    numCharRead = scanf("%d %c %c %c %d", &tr.CurrentState, &tr.ReadenChar, &tr.CharToWrite, &tr.NextStep, &tr.FinalState);

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

      if(strchr(BufferTemp, '\n')){
	 RunMT(string);
	 free(string);
	 string = (char *) malloc(sizeof(char)); 
      }
   }
}

void RunMT(char* string){
   //Inizializzo tutte le variabili
   int BlockedMT = FALSE;
   int stepNum = 0;
   int index = 0;
   int stringlen = strlen(string);
   int CurrentState = 0;

   while(!BlockedMT){
      //Esegui le transizioni

   }
}

Transition GetNextTransition(int currentState, char readenChar, char *string, int stepNum){
   Transition tr = NULL;
   int trCount = 0;
   int hashValue = TransitionHashFunction(currentState, readenChar);
   HashObject *hashObject = HashPointers[hashValue];

   for(; hashObject; hashObject = hashObject -> next)
      if(hashObject -> tr.CurrentState == currentState && hashObject -> tr.ReadenChar = readenChar)
	 if(trCount)
	    break; //AddToQueue
	 else {
	    //Il valore dello stato corrente e del carattere letto non li ritorno dato che non mi
	    //servono
	    tr->CharToWrite = hashObject -> tr.CharToWrite;
	    tr->NextStep = hashObject -> tr.NextStep;
	    tr->FinalState = hashObject -> tr.FinalState;
	 }

   return tr;
}

int TransitionHashFunction(char c1, char c2){
   return ((int)c1 + (int)c2) % HASH_TABLE_DIM;
}

ToDoQueueObject* TransitionQueueHeadInsert(ToDoQueueObject *head, Transition *transition, char* string, int stepNum){
   ToDoQueueObject* tmp;
   tmp = head;

   if(head = (ToDoQueueObject *) malloc(sizeof(ToDoQueueObject))){
      //Copio la nuova stringa
      head -> String = (char *) malloc((strlen(string) + 1) * sizeof(char));
      strcpy(head -> String, string);

      
   }
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

void InitHashTable(HashObject **table){
   int i;
   for(i = 0; i< HASH_TABLE_DIM; i++)
      table[i] = NULL;
}
