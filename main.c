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
#define HASH_TABLE_DIM 50
#define TRANSITION_STRING_DIM 10
#define FIRST_ASCII_LETTER_INT 97 // ASCII 97 = 'a'
#define BUFFER_DIM 100
#define EXPAND_STRING_DIM 5

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
   int StepNum, Index, FinalState;
   char CharToWrite, NextStep;
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
void TransitionQueueHeadInsert(char, char, int, char*, int, int);
//Transition GetNextTransition(int, char, char*, int, int);
int TransitionHashFunction(char, char);
void FileParsing();
void ReadAndRun();
void MemoryClean();
char* ExpandString(char*, char, int*);
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

   /*
   * FASE 3: Pulizia di tutta la memoria
   */
   MemoryClean();

   //Faccio un ultimo fflush per svuotare il buffer
   //fflush(stdout);

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

   numCharRead = scanf("%s", label);
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
	    numCharRead = scanf("%d", &MaxStep);
	    break;
	 }
      }

      numCharRead = scanf("%s", label);

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
   string[0] = '\0';

   fgetc(stdin);
   while(fgets(BufferTemp, BUFFER_DIM, stdin)){
      char *tmp = string;
      //printf("Letto %s\n", BufferTemp);
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
	 free(string);
	 string = (char *) malloc(sizeof(char));
	 string[0] = '\0';
      }
   }

   if(string[0] != '\0'){
      RunMT(string);
      free(string);
   }
}

char* ExpandString(char *string, char side, int *stringlen){
   char *expandedString;
   char expandeString[EXPAND_STRING_DIM + 1] = { BLANK, BLANK, BLANK, BLANK, BLANK, '\0'};
   int i;

   /*for(i = 0; i < EXPAND_STRING_DIM; i++)
      expandeString[i] = BLANK;
   expandeString[i] = '\0';*/

   if(expandedString = (char *) malloc((*stringlen + EXPAND_STRING_DIM + 1) * sizeof(char)))
   {
      strcpy(expandedString, "\0");

      if(side == 'R'){
    	 strcpy(expandedString, string);
    	 strcat(expandedString, expandeString);
      } else{
    	 strcpy(expandedString, expandeString);
    	 strcat(expandedString, string);
      }
   }

   free(string);

   *stringlen = *stringlen + EXPAND_STRING_DIM + 1;

   return expandedString;
}

void RunMT(char* string){
   //printf("Incomincio la simulazione per la stringa %s\n", string);

   //Inizializzo tutte le variabili
   int BlockedMT = FALSE,
      AcceptedString = FALSE,
      MaxStepOvered = FALSE,
      stepNum,
      index,
      stringlen = strlen(string),
      currentState,
      isFirst = TRUE,
      trCount;
   Transition tr;
   QueueHead = NULL;

   TransitionQueueHeadInsert(string[0], 'S', 0, string, 0, 0);

   while(QueueHead){

      //Parto con la prima transizione della sequenza deterministica
      currentState = QueueHead -> FinalState;
      stepNum = QueueHead -> StepNum;
      string = QueueHead -> String;
      index = QueueHead -> Index;
      string[index] = QueueHead -> CharToWrite;
      stringlen = strlen(string);

      if(QueueHead -> NextStep == 'R')
        index++;
      else if(QueueHead -> NextStep == 'L')
        index--;

      //printf("\nPartenza nuovo percorso %s\n", string);

      //Libero la memoria della transizione in coda e imposto la cosa sulla prossima
      ToDoQueueObject *tmp = QueueHead;
      QueueHead = QueueHead -> next;
      free(tmp);

      trCount = 1;
      while(trCount && stepNum <= MaxStep){

        if(index < 0){
           string = ExpandString(string, 'L', &stringlen);
           index = EXPAND_STRING_DIM - 1;
           //stringlen = strlen(string);
        }
        else if(index == stringlen)
        {
           string = ExpandString(string, 'R', &stringlen);
           //stringlen = strlen(string);
        }

      	//printf("Trovata la transizione da eseguire: %c %c %d\n", tr.CharToWrite, tr.NextStep, tr.FinalState);
      	 //Eseguo le istruzione della transizione

        trCount = 0;

	 char charToWrite;

        int _currentState = currentState;
	char _readenChar = string[index];
	int _index = index;
        int hashValue = TransitionHashFunction(currentState, string[index]);
        HashObject* hashObject = HashPointers[hashValue];
        for(; hashObject; hashObject = hashObject -> next)
          if(hashObject -> tr.CurrentState == _currentState && hashObject -> tr.ReadenChar == _readenChar)
            if(trCount){
               TransitionQueueHeadInsert(hashObject -> tr.CharToWrite, hashObject ->
	       tr.NextStep, hashObject -> tr.FinalState, string, stepNum, _index);
            } else {
              charToWrite = hashObject -> tr.CharToWrite;
              currentState = hashObject -> tr.FinalState;

              if(hashObject -> tr.NextStep == 'R')
                index++;
              else if(hashObject -> tr.NextStep == 'L')
                index--;              

              trCount = 1;
            }
	    
	 string[_index] = charToWrite;

      	 //Incremento il numero di step fatti
      	 stepNum++;

      	 //printf("Index: %d Step numero %d\t Stringa: %s\t Stato corrente %d\n", index, stepNum, string, currentState);
      }

      //Una volta che la macchina si è bloccata controllo se lo stato in cui è arrivato è uno stato di accettazione
      if(stepNum > MaxStep)
	 MaxStepOvered = TRUE;
      else if(stepNum <= MaxStep && AccStatesArray[currentState]){
      	 //In caso affermativo stampo 1 e avvio la procedura per la pulizia della memoria
      	 printf("1\n");
      	 //printf("currentState: %d\n", currentState);
      	 FreeSingleStringMemory(string);
      	 AcceptedString = TRUE;
      }
   }

   if(!AcceptedString){
      if(MaxStepOvered)
	 printf("U\n");
      else
	 printf("0\n");
      fflush(stdout);
   }
}

/*Transition GetNextTransition(int currentState, char readenChar, char *string, int stepNum, int index){
   Transition tr;
   int trCount = 0, i, hashValue;
   HashObject *hashObject;

   //Dando il valore finalState -1 capisco se la ricerca di transizioni è dato risultati o meno
   tr.FinalState = -1;

   //printf("Cerco la transizione per lo stato %d e il char %c\n", currentState, possibleReadenChar[i]);
   hashValue = TransitionHashFunction(currentState, readenChar);
   hashObject = HashPointers[hashValue];
   for(; hashObject; hashObject = hashObject -> next)
      if(hashObject -> tr.CurrentState == currentState && hashObject -> tr.ReadenChar == readenChar){

	 if(trCount){
	    Transition tmp;
	    tmp.CharToWrite = hashObject -> tr.CharToWrite;
	    tmp.NextStep = hashObject -> tr.NextStep;
	    tmp.FinalState = hashObject -> tr.FinalState;
	    TransitionQueueHeadInsert(QueueHead, tmp, string, stepNum, index);
	 } else {
	    tr.CharToWrite = hashObject -> tr.CharToWrite;
	    tr.NextStep = hashObject -> tr.NextStep;
	    tr.FinalState = hashObject -> tr.FinalState;
	 }

	 trCount++;
      }

   return tr;
}*/

int TransitionHashFunction(char c1, char c2){
   return ((int)c1 * (int)c2) % HASH_TABLE_DIM;
}

void TransitionQueueHeadInsert(char charToWrite, char nextStep, int finalState, char* string, int stepNum, int index){
   ToDoQueueObject* tmp;
   tmp = QueueHead;

   if(QueueHead = (ToDoQueueObject *) malloc(sizeof(ToDoQueueObject))){
      //Copio la nuova stringa
      QueueHead -> String = (char *) malloc((strlen(string) + 1) * sizeof(char));
      strcpy(QueueHead -> String, string);
      QueueHead -> StepNum = stepNum;
      QueueHead -> CharToWrite = charToWrite;
      QueueHead -> NextStep = nextStep;
      QueueHead -> FinalState = finalState;
      QueueHead -> Index = index;
      QueueHead -> next = tmp;
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

/*
* Funzione che libera la memoria dopo che l'analisi di una stringa è stata completata
*/
void FreeSingleStringMemory(char *string){
   //Svuoto la TO DO list delle transizioni
   while(QueueHead){
      ToDoQueueObject *tmp = QueueHead;
      QueueHead = QueueHead -> next;
      free(tmp);
   }

   //Libero la stringa sulla quale ho fatto le operazioni
   free(string);
}

void MemoryClean(){
   //Libero la memoria delle transazioni
   int i;
   for(i = 0; i < HASH_TABLE_DIM; i++){
      HashObject *head = HashPointers[i];

      while(head)
      {
	 HashObject *tmp = head;
	 head = head -> next;
	 free(tmp);
      }
   }
}

void InitHashTable(HashObject **table){
   int i;
   for(i = 0; i< HASH_TABLE_DIM; i++)
      table[i] = NULL;
}
