#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define DEBUG 0
#define INFTY UINT_MAX

/*Struttura dati per implementare una coda di priorità (heap usando array)
pqueue = priority queue*/
struct pqueue {
  unsigned int value;
  int key;
};

/*Variabile globale che tiene traccia della grandezza dello heap*/
int size = -1;
int d, n, n2;

/*Struttura dati per salvare somma cammini minimi di un grafo e la sua posizione*/
struct bilist {
  unsigned int sum;
  int index;
  struct bilist *next;
  struct bilist *prev;
};

/*Punta sempre al primo elemento della lista*/
struct bilist *head = NULL;
/*Punta sempre all'ultimo elemento della lista*/
struct bilist *tail = NULL;

void swap (int *a, int *b) {
  int temp;
  temp = *b;
  *b = *a;
  *a = temp;
}

void swap_unsigned (unsigned int *a, unsigned int *b) {
  unsigned int temp;
  temp = *b;
  *b = *a;
  *a = temp;
}

/******************************************
*FUNZIONI PER IMPLEMENTARE HEAP CON ARRAY *
******************************************/

/*Ritorna l'indice del nodo PADRE di un dato nodo (dando in ingresso il suo indice)*/
int parent (int i) {
  return (i - 1) / 2;
}

/*Ritorna l'indice del nodo FIGLIO SX di un dato nodo (dando in ingresso il suo indice)*/
int left_child (int i) {
  return ((2 * i) + 1);
}

/*Ritorna l'indice del nodo FIGLIO DX di un dato nodo (dando in ingresso il suo indice)*/
int right_child (int i) {
  return ((2 * i) + 2);
}

/*Sposta il nodo i verso l'ALTO per mantenere la proprietà dello heap*/
void shift_up (int i, struct pqueue *heap) {
  while (i > 0 && heap[parent(i)].value > heap[i].value) {
    /*Scambia il nodo padre col nodo i*/
    swap_unsigned (&heap[parent(i)].value, &heap[i].value);
    swap (&heap[parent(i)].key, &heap[i].key);
    /*Aggiorna il valore di parent*/
    i = parent (i);
  }
}

/*Sposta il nodo i verso il BASSO per mantenere la proprietà dello heap*/
void shift_down (int i, struct pqueue *heap) {
  /*Indice del nodo con valore minimo tra padre e figli*/
  int mini = i;
  int l = left_child (i);
  int r = right_child (i);
  if (l <= size && heap[l].value < heap[mini].value)
    mini = l;
  if (r <= size && heap[r].value < heap[mini].value)
    mini = r;
  /*Se l'indice di partenza NON è già il minimo*/
  if (i != mini) {
    swap_unsigned (&heap[i].value, &heap[mini].value);
    swap (&heap[i].key, &heap[mini].key);
    shift_down (mini, heap);
  }
}

/*Inserisce un nuovo elemento nello heap*/
void insert (unsigned int n, struct pqueue *heap) {
  size ++;
  heap[size].value = n;
  heap[size].key = size;
  shift_up (size, heap);
}

/*Estrae e restituisce LA CHIAVE del valore minimo dallo heap (è sempre la radice)*/
int get_min (struct pqueue *heap) {
  int r = heap[0].key;
  /*Sostituisci il valore alla radice (il più piccolo) con l'ultimo valore (il valore più grande)*/
  heap[0].value = heap[size].value;
  heap[0].key = heap[size].key;
  size --;
  /*Sposta la nuova radice nella giusta posizione*/
  shift_down (0, heap);
  return r;
}

/*Cambia il valore dell'elemento CON CHIAVE k, mantenendo lo heap.
Tutte le altre funzioni richiedevano la POSIZIONE dell'elemento.
change_key invece vuole cambiare il valore di un elemento con specifica.*/
void change_key (int k, unsigned int p, struct pqueue *heap) {
  unsigned int temp;

  /*Cerco l'elemento con chiave k*/
  for (int i = 0; i < size + 1; i ++) {
    if (heap[i].key == k) {
      temp = heap[i].value;
      heap[i].value = p;
      if (p < temp)
        /*shift_up e shift_down hanno come input la POSIZIONE*/
        shift_up (i, heap);
      else
        shift_down (i, heap);
      break;
    }
  }
}

/*restituisce 0 se lo heap è vuoto, 1 altrimenti*/
int is_empty () {
  return size == -1;
}

/*************************************************
* FUNZIONI PER IMPLEMENTARE LISTE BIDIREZIONALI  *
*************************************************/

/*Quando inserisco un nuovo elemento nella lista, lo inserisco nella giusta posizione in base al valore della sua somma
Questo evita di usare un algoritmo di ordinamento per sapere quali sono i primi n grafi del ranking-*/

/*Crea un nuovo nodo dati somma e posizione*/
struct bilist *new_node (unsigned int s, int i) {
    struct bilist *new = malloc (sizeof (struct bilist));
    new -> index = i;
    new -> sum = s;
    new -> next = NULL;
    new -> prev = NULL;
    return new;
}

/*Inserisce un nuovo nodo nella lista dati somma e posizione*/
void push (unsigned int s, int i) {
  struct bilist *new = new_node (s, i);

  /*Se è il primo elemento della lista*/
  if (head == NULL) {
    head = new;
    tail = new;
    return;
  }

  /*Se il nuovo nodo ha somma MINORE rispetto ad head*/
  if (s < head -> sum) {
    /*Inseriscilo prima di head e aggiorna head*/
    new -> next = head;
    head -> prev = new;
    head = new;
  }

  /*Altrimenti scorri la lista per trovare la giusta posizione del nuovo nodo*/
  else {
    struct bilist *temp = head;

    while (temp -> next != NULL && s > temp -> next -> sum) {
      temp = temp -> next;
    }
    /*Aggiorna la lista*/
    new -> next = temp -> next;

    /*Se il nuovo nodo NON viene inserito alla fine della lista*/
    if (new -> next != NULL) {
      /*Allora new -> next -> prev NON è NULL e deve essere modificato*/
      new -> next -> prev = new;
    }
    /*Altrimenti aggiorna la coda*/
    else {
      tail = new;
    }

    new -> prev = temp;
    temp -> next = new;
  }
}

/*Inserisce nella lista ed elimina l'ultimo elemento, se necessario*/
void push_and_delete (unsigned int s, int i) {
  struct bilist *temp = tail;

  /*Se s è maggiore o uguale di sum dell'ultimo elemento allora non fai nulla ed esci*/
  if (s >= temp -> sum)
    return;
  /*Altrimenti aggiorna la lista, elimina l'ultimo elemento e fai push del nuovo nodo*/
  else {
    /*Se è presente un solo elemento nella lista*/
    if (temp == head) {
      head = NULL;
      tail = NULL;
      free (temp);
      push (s, i);
    }
    /*Altrimenti*/
    else {
      tail = temp -> prev;
      temp -> prev -> next = NULL;
      free (temp);
      push (s, i);
    }
  }
}

/*********************
*     DEBUGGING      *
*********************/
#ifdef DEBUG

/*Stampa array partendo dall'indice 0*/
void print_vector (unsigned int *vector, int n) {
  for (int i = 0; i < n; i ++) {
    printf ("%u ", vector[i]);
  }
  printf ("\n");
}

/*Stampa heap partendo dalla radice (se è corretto allora è sempre vuoto)*/
void print_heap (struct pqueue *heap) {
  for (int i = 0; i < size + 1; i ++) {
    printf ("Value: %u\n", heap[i].value);
    printf ("Key: %d\n", heap[i].key);
  }
  printf ("\n");
}

/*Stampa matrice*/
void pretty_print (unsigned int **mat, int n) {
  int r, c;

  for (r = 0; r < n; r ++) {
    for (c = 0; c < n; c ++) {
      printf ("%u ", mat[r][c]);
    }
    printf ("\n");
  }
}
#endif

/*******************************************
* LEGGE VELOCEMENTE UNSIGNED INT DA INPUT  *
*******************************************/
unsigned int fast_scan () {
	unsigned int n = 0;
	char ch = getchar_unlocked ();
  /*Ignora caratteri che non sono numeri*/
	while (ch < '0' && ch > '9') {
		ch = getchar_unlocked ();
	}
  /*Finchè il carattere letto è un numero*/
	while (ch >= '0' && ch <= '9') {
    /*Trasformalo da carattere in int, e mettilo al giusto posto nel numero*/
		n = n * 10 + ch - '0';
		ch = getchar_unlocked();
	}
	return n;
}

/**********************
* OVERFLOW DETECTION  *
**********************/
/*Ritorna 1 se a + b genera overflow, 0 altrimenti*/
int add_ovf (unsigned int a, unsigned int b)
{
    if (a > INFTY - b)
        return 1;

    return 0;
}

/****************************
* DEALLOCA TUTTA LA MATRICE *
****************************/
void deleteMatrix(unsigned int **m,unsigned int d){
   int c;
    for (c = 0; c < d; c ++) {
      free(m[c]);
    }
    free(m);
}

/****************************************************
* GENERA DINAMICAMENTE UNA MATRICE DI n x n INTERI  *
****************************************************/

unsigned int **create_mat (int n) {
  unsigned int **m = (unsigned int **)malloc(n * sizeof (unsigned int *));
  int r, c;
  for (c = 0; c < n; c ++) {
    *(m + c) = malloc (n * sizeof (unsigned int));
    m[c] = (unsigned int *)malloc(n * sizeof(unsigned int));
  }
  for (r = 0; r < n; r ++) {
    for (c = 0; c < n; c ++) {
      m[r][c] = fast_scan();
      if (m[r][c] == 0)
        m[r][c] = INFTY;
    }
  }
  return m;
  deleteMatrix(m,d);
}

/**************************************************************************
* ALGORITMO DIJKSTRA USANDO CODE CON PRIORITA' IMPLEMENTATE TRAMITE HEAP  *
**************************************************************************/
unsigned int *dijkstra (unsigned int **matrix, int n, struct pqueue *heap) {
  unsigned int *vector = NULL;
  int min;

  vector = malloc (n * sizeof (unsigned int));
  /*Nodo da considerare (in questo caso il nodo è sempre 0)*/
  vector[0] = 0;
  for (int i = 1; i < n; i ++) {
    vector[i] = INFTY;
  }

  /*Riempimento heap per algoritmo*/
  for (int i = 0; i < n; i ++) {
    insert (vector[i], heap);
  }

  /*Parte principale dell'algoritmo di Dijkstra*/
  while (is_empty() != 1) {
    min = get_min (heap);

    for (int i = 0; i < n; i ++) {
      /*Overflow check: se vector[min] + matrix[min][i] > INFTY vai alla prossima iterazione (trova una strada migliore)*/
      if (add_ovf (vector[min], matrix[min][i]) == 1) {
        continue;
      }
      if ((vector[min] + matrix[min][i]) < vector[i]) {
        vector[i] = vector[min] + matrix[min][i];
        change_key (i, vector[i], heap);
      }
    }
  }
  return vector;
}

/***************************************
* STAMPA CLASSIFICA DA HEAD FINO AD N  *
***************************************/
void print_list (int n) {
  /*Contatore per sapere quando raggiungo n*/
  int counter = 0;
  struct bilist *temp = head;
  while (temp != NULL && counter < n) {
     if(temp->next==NULL){
        printf ("%d\n", temp -> index);
     }else{
	printf ("%d ", temp -> index);
	temp = temp -> next;
     }
    counter ++;
  }
}

/*********************************************************************************
* ESEGUE E RESTITUISCE LA SOMMA DEI VALORI DEL VETTORE RESTITUITO DALL'ALGORITMO *
*********************************************************************************/
unsigned int sum_array (unsigned int *vector, int n) {
  unsigned int sum = 0;

  for (int i = 0; i < n; i ++) {
    if (vector[i] == INFTY)
      vector[i] = 0;

    /*Overflow check: se sum + vector[i] > INFTY metti sum = INFTY ed esci*/
    if (add_ovf (sum, vector[i]) == 1) {
      sum = INFTY;
      break;
    }

    sum += vector[i];
  }

  return sum;
}

/*****************
*      MAIN      *
*****************/

int main (int argc, char * argv[]) {
  int counter = 0;
  unsigned int **matrix = NULL;
  unsigned int *vector = NULL;
  char string[15];
  /*Legge d ed n usando fast_scan (usa getchar_unlocked)*/
  d = fast_scan();
  n = fast_scan();

  /*Dichiarazione struttura dati dello heap*/
  struct pqueue heap[d];

  /*Legge una nuova linea di stringhe fino alla fine dell'input, e la salva in "string"*/
  while (fgets (string, 15, stdin) != NULL) {

    /*Se la stringa letta è "AggiungiGrafo"*/
    if (strcmp (string, "AggiungiGrafo\n") == 0) {
      matrix = create_mat (d);
      vector = dijkstra (matrix, d, heap);

      /****************
      *     DEBUG     *
      ****************/
      if (DEBUG) {
        printf ("DEBUG Heap %d: ", counter);
        print_heap (heap);
        printf ("DEBUG Matrix %d:\n", counter);
        pretty_print (matrix, d);
        printf ("DEBUG Array Dj %d: ", counter);
        print_vector (vector, d);
      }

      /*Inserisci nella lista se counter è minore di n*/
      if (counter < n)
        push (sum_array (vector, d), counter);
      /*Altrimenti*/
      else
        push_and_delete (sum_array (vector, d), counter);

      /****************
      *     DEBUG     *
      ****************/
      if (DEBUG) {
        printf ("DEBUG Array Sum: %u\n", sum_array(vector, d));
        printf ("\n");
      }

      /*Libero memoria*/
      deleteMatrix(matrix,d);
      free (vector);
      counter ++;
    }

    /*Se la stringa letta è "TopK"*/
    else if (strcmp (string, "TopK\n") == 0) {
		if(counter==0){
    	    		printf("\n");
    		}
      /*Se il numero di grafi inseriti è minore di n stampo fino a counter (= numero di grafi inseriti)*/
      if (counter < n)  {
        n2 = counter;
        print_list (n2);
      }
      /*Altrimenti stampo fino ad n*/
      else {
        print_list (n);
        printf ("\n");
      }
    }
  }

  return 0;
}
