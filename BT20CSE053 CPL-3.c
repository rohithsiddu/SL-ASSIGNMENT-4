
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAME_SIZE 64		// Variable name length
#define HEAP_SIZE 999		// heap size, (Required size-1)
typedef enum status
{
  failure,
  success
} status;
typedef struct listAllocated
{
  char name[NAME_SIZE];
  int start, end;
  struct listAllocated *next;
  struct listAllocated *prev;
} listAllocated;
typedef struct freeList
{
  int start, end;
  struct freeList *next;
  struct freeList *prev;
} freeList;
typedef struct heap_tag		// heap structure main in which we are allocating the memory
{
  int size;
  freeList *free;		// free list in heap
  listAllocated *used;		// allocated list in heap
} heap;
void
initializeFree (freeList * f)
{				// setting default values to freelist
  f->start = -1;
  f->end = HEAP_SIZE;
  f->next = NULL;
  f->prev = NULL;
}

void
Heapinitialization (heap * h)
{				// setting size and creating heaps used and freelists
  h->size = HEAP_SIZE;
  h->free = (freeList *) malloc (sizeof (freeList));
  initializeFree (h->free);
  h->used = NULL;
}

heap *
updateAllocatedHeap (heap * main, listAllocated * new)
{				// to add element in allocated list
  listAllocated *a = main->used;
  if (a == NULL)		// initial empty case
    main->used = new;
  else
    {
      if (a->end > new->start)
	{			// if new node to be added before existing allocated nodes
	  new->next = main->used;
	  main->used->prev = new;
	  main->used = new;
	}
      else
	{
	  while (a->next != NULL && a->end < new->start)	// traversing along allocated list to insert with sorting
	    a = a->next;
	  if (a->next == NULL)
	    {			// last node case
	      a->next = new;
	      new->prev = a;
	      new->next = NULL;
	    }
	  else
	    {
	      listAllocated *temp = a->next;
	      a->next = new;
	      new->prev = a;
	      new->next = temp;
	      temp->prev = new;
	    }
	}
    }
  return main;
}

status
allocate (heap * main, char name[NAME_SIZE], int size)
{				// To search for free space and allocate
  freeList *f = main->free;
  int found = 0;
  while (f != NULL && !found)
    {
      if ((f->end - f->start) >= size)
	{			// while loop over free list to find node with required space
	  listAllocated *new = (listAllocated *) malloc (sizeof (listAllocated));	// creating new node to insert
	  new->start = f->start;	// setting start and end points as address
	  (new->end) = (new->start) + size;
	  strcpy (new->name, name);
	  new->next = new->prev = NULL;
	  if ((f->end - f->start) == size)
	    {			// Case if exact space is found and no internal fragmentatio
	      if (f->prev == NULL)
		{		// subcases of first node, single node and last node.
		  if (f->next != NULL)
		    {
		      main->free = f->next;
		      main->free->prev = NULL;
		    }
		  else
		    {
		      f->start = new->end;
		      if (f->start >= f->end)
			{
			  main->free = NULL;
			}
		    }
		}
	      else
		{
		  f->prev->next = f->next;
		  free (f);
		}
	    }
	  else
	    {
	      f->start = new->end;
	    }
	  main = updateAllocatedHeap (main, new);	// updating allocated list with adding new block
	  found = 1;
	  return success;
	}
      else
	{
	  f = f->next;
	}
    }
  if (!found && f == NULL)
    {				// if no sufficient space found, return failure
      return failure;
    }
}

heap *
updateFreeHeap (heap * main, listAllocated * t)
{				// to add new free block along with merging
  freeList *f = main->free;
  if (f == NULL)
    {				// special case of empty free list
      freeList *new = (freeList *) malloc (sizeof (freeList));
      new->start = t->start;
      new->end = t->end;
      new->prev = new->next = NULL;
      main->free = new;
    }
  else if (f->next == NULL && f->end == HEAP_SIZE)
    {				// initial case of first free
      if (t->end == f->start)
	{			// if right merge is possible
	  f->start = t->start;
	}
      else
	{
	  freeList *new = (freeList *) malloc (sizeof (freeList));	// if no merge, then add new node
	  new->start = t->start;
	  new->end = t->end;
	  main->free->prev = new;
	  new->next = main->free;
	  main->free = new;
	  new->prev = NULL;
	}
    }
  else
    {
      if (t->start < f->start)
	{			// if new free node is to be added at beginning
	  if (t->end == f->start)
	    {			// right merge checking
	      f->start = t->start;
	    }
	  else
	    {
	      freeList *new = (freeList *) malloc (sizeof (freeList));	// no merge, then add new node
	      new->start = t->start;
	      new->end = t->end;
	      if (f->prev == NULL)
		{
		  f->prev = new;
		  new->next = f;
		  main->free = new;
		}
	    }
	}
      else
	{
	  while (f->next != NULL && f->end < t->start)	// traverse along freelist to insert with sorting
	    f = f->next;
	  if (f->start == t->end)
	    {			// single free element edge case, right merge checking
	      f->start = t->start;
	    }
	  else if (f->end == t->start)
	    {			// left merge checking
	      f->end = t->end;
	      if (f->next != NULL && f->next->start == f->end)
		{		// checking right merge and clearing extra node after merge
		  freeList *rem = f->next;
		  f->end = f->next->end;
		  f->next = f->next->next;
		  if (f->next != NULL)
		    f->next->prev = f;
		  free (rem);
		}
	    }
	  else
	    {			// general cases
	      freeList *new = (freeList *) malloc (sizeof (freeList));	// making new node to insert
	      new->start = t->start;
	      new->end = t->end;
	      new->prev = new->next = NULL;
	      if (f->prev == NULL)
		{		// checking edge cases of first node,single node
		  if (f->next == NULL)
		    {
		      if (new->start > f->start)
			{	// single node insert before and after cases
			  f->next = new;
			  new->prev = f;
			}
		      else
			{
			  f->prev = new;
			  new->next = f;
			  main->free = new;
			}
		    }
		  else
		    {		// first node case
		      f->prev = new;
		      new->next = f;
		      main->free = new;
		    }
		}
	      else
		{		// remaining general cases updating links
		  f = f->prev;
		  new->next = f->next;
		  new->prev = f;
		  f->next = new;
		  new->next->prev = new;
		}
	    }
	}
    }
  return main;
}

status
Free (heap * main, char name[NAME_SIZE])
{				// to search for element to free and update lists
  listAllocated *temp = main->used;
  int found = 0;
  while (temp != NULL && !found)
    {				// traversing over allocated list to find element
      if (strcmp (temp->name, name) == 0)
	{
	  if (temp->prev == NULL)
	    {
	      if (temp->next == NULL)
		{		// if found block is single code
		  if (main->free == NULL)
		    {		// free list null special case
		      main->free = (freeList *) malloc (sizeof (freeList));
		    }
		  initializeFree (main->free);
		  free (temp);
		  main->used = NULL;
		}
	      else
		{		// first node case, updating lists
		  main->used = temp->next;
		  main->used->prev = temp->next = temp->prev = NULL;
		  main = updateFreeHeap (main, temp);	// updating free lists
		  free (temp);
		}
	    }
	  else if (temp->next == NULL)
	    {			// last node case
	      temp->prev->next = NULL;
	      main = updateFreeHeap (main, temp);	// updating free lists
	      free (temp);
	    }
	  else
	    {			// remaining general cases
	      listAllocated *t = temp;
	      temp->prev->next = temp->next;
	      temp->next->prev = temp->prev;
	      temp->next = temp->prev = NULL;
	      main = updateFreeHeap (main, temp);	// updating free lists
	      free (t);
	    }
	  found = 1;
	  return success;
	}
      else
	{
	  temp = temp->next;
	}
    }
  if (!found && temp == NULL)
    {				// if block not fund return failure
      return failure;
    }
}

void
printHeap (heap * main)
{				// print heap status
  printf ("\nAllocated Blocks:\n\n");
  listAllocated *temp = main->used;
  freeList *f = main->free;
  if (temp == NULL)
    printf ("No Allocated blocks\n");
  printf ("Name            Size            Start         End\n");
  while (temp != NULL)		// All allocated blocks sorted
    {
      printf ("%s\t\t%d\t\t%d\t\t%d\n", temp->name, temp->end - temp->start,
	      temp->start + 1, temp->end);
      temp = temp->next;
    }
  printf ("\nFree Blocks:\n\n");
  printf ("Size            Start           End\n");
  if (f == NULL)
    printf ("No Free blocks\n");
  while (f != NULL)		// All Free blocks sorted
    {
      printf ("%d\t\t%d\t\t%d\n", f->end - f->start, f->start + 1, f->end);
      f = f->next;
    }
  printf ("\n");
}

int
main ()
{				// Main program with menu
  heap main;
  Heapinitialization (&main);
  int end = 0;
  while (!end)
    {
      int key;
      printf
	("Enter :\n 1 to allocate \n 2 to free \n 3 to print heap \n 4 to exit: ");
      printf ("\nEnter your choice: ");
      scanf ("%d", &key);
      if (key == 1)
	{
	  char name[NAME_SIZE];
	  int size;
	  printf ("Enter name and size : ");
	  scanf ("%s %d", name, &size);
	  status result = allocate (&main, name, size);
	  if (result == success)
	    printf ("SUCCESS!!\n");
	  else
	    printf ("FAILURE!!\n");
	}
      if (key == 2)
	{
	  char name[NAME_SIZE];
	  printf ("Enter name to free : ");

	  scanf ("%s", name);
	  status result = Free (&main, name);
	  if (result == success)
	    printf ("SUCCESS!!\n");
	  else
	    printf ("FAILURE!!\n");
	}
      if (key == 3)
	{
	  printHeap (&main);
	}
      if (key == 4)
	{
	  end = 1;
	}
    }
  return 0;
}

