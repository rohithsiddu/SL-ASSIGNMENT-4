#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

typedef struct node
{
    int data;
    bool mark;
    int ref_count;
    struct node *n1;
    struct node *n2;
    struct node *n3;
   
    
}Node;

 Node *array[8];

void display_node(int i)
{
	printf("value=%d\t reference_count=%d freed_size=%d\n",array[i]->data,array[i]->ref_count,sizeof(Node));
}

void set_edge(int so,int dest1,int dest2,int dest3)
{
	if(dest1!=-1)
	{
		array[so]->n1=array[dest1];
		array[dest1]->ref_count+=1;
	}
	if(dest2!=-1)
	{
		array[so]->n2=array[dest2];
		array[dest2]->ref_count+=1;
	}
	if(dest3!=-1)
	{
		array[so]->n3=array[dest3];
		array[dest3]->ref_count+=1;
	}
	
}

void displaynodes(Node* root)
{
	if(root!=NULL)
	{
		printf("value=%d:ref_count=%d\n",root->data,root->ref_count);
	}
	if(root==NULL)
	{
		return;
	}
	displaynodes(root->n1);
	displaynodes(root->n2);
	displaynodes(root->n3);
}

void adjacency_list()
{
	int i=0;
	for(i=0;i<8;i++)
	{
		if(array[i]!=NULL)
		{
			printf("Value=%d: ",array[i]->data);
			if(array[i]->n1!=NULL)
			{
				printf("%d ->",array[i]->n1->data);
			}
			if(array[i]->n2!=NULL)
			{
				printf("%d ->",array[i]->n2->data);
			}
			if(array[i]->n3!=NULL)
			{
				printf("%d ->",array[i]->n3->data);
			}
			printf("NULL\n");
		}
	}
}

int root_is_present(Node* root1,Node* temp)
{
	if(root1==NULL)
	{
		return 0;
	}
	if(root1->data==temp->data)
	{
		return 1;
	}
	
	if(root_is_present(root1->n1,temp))
	{
		return 1;
	}
	
	if(root_is_present(root1->n2,temp))
	{
		return 1;
	}
	if(root_is_present(root1->n3,temp))
	{
		return 1;
	}
 return 0;
}


void garbage_collection_ref_counting(Node* root)
{
	int i=0;
	while(i<8)
	{
		if(root_is_present(root,array[i])==0 )
		{		
			if(array[i]->n1!=NULL)
			{
				array[i]->n1->ref_count-=1;
			}
			if(array[i]->n2!=NULL)
			{
				array[i]->n2->ref_count-=1;
			}
			if(array[i]->n3!=NULL)
			{
				array[i]->n3->ref_count-=1;
			}
			printf("Garbage:");
			display_node(i);
			free(array[i]);
			array[i]=NULL;
		}
		 i++;		
	}
	 
}

void adjacency_Matrix()
{
	int adm[8][8];
	int i,j,k;
	
	for(i=0;i<8;i++)	
	{
		for(j=0;j<8;j++)
		{
			adm[i][j]=0;
		}	
	}
	
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			
		if(array[j]!=NULL&&array[i]!=NULL)
		{
			
			if(array[i]->n1!=NULL)
			{
				if(array[i]->n1->data==array[j]->data&&i!=j)
				{
					adm[i][j]=1;
				}
			}
			if(array[i]->n2!=NULL)
			{
				if(array[i]->n2->data==array[j]->data&&i!=j)
				{
					adm[i][j]=1;
				}
			}
			if(array[i]->n3!=NULL)
			{
				if(array[i]->n3->data==array[j]->data&&i!=j)
				{
					adm[i][j]=1;
				}
			}
		}
		
		}
	}
	
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			printf("%d ",adm[i][j]);		
		}
		printf("\n");
	}
}

void mark_the_Nodes(Node*root,int i,int j)
{
    Node *current, *pre;

    current = root;
        
    while (current != NULL) 
    {
  
        if (current->n1== NULL) 
        {
            current->mark=true;
            current = current->n2;
        }   
        else 
        {
            pre = current->n1;
            while ((pre->n2 != NULL) && (pre->n2 != current))
            {
            	pre = pre->n2;
			}
                
            if (pre->n2 == NULL) 
            {
                pre->n2 = current;
                current = current->n1;
            }
            else 
            {
                pre->n2 = NULL;
                current->mark=true;
                current = current->n2;
            } 
        }
    }   
    
    current = root;
        
    while (current != NULL) 
    {
  
        if (current->n1== NULL) 
        {
            current->mark=true;
            current = current->n3;
        }   
        else 
        {
            pre = current->n1;
            while ((pre->n3 != NULL) && (pre->n3 != current))
            {
            	pre = pre->n3;
			}
                
            if (pre->n3 == NULL) 
            {
                pre->n3 = current;
                current = current->n1;
            }
            else 
            {
                pre->n3 = NULL;
                current->mark=true;
                current = current->n3;
            } 
        }
    }  
    
}

void mark_method(Node* root)
{
	
	if(root!=NULL)
	{
		root->mark=true;
	}
	if(root==NULL)
	{
		return;
	}
	mark_method(root->n1);
	mark_method(root->n2);
	mark_method(root->n3);
}

void sweep_method()
{
	int i;
	for(i=0;i<8;i++)
	{
		if(array[i]->mark==false)
		{
			if(array[i]->n1!=NULL)
			{
				array[i]->n1->ref_count-=1;
			}
			if(array[i]->n2!=NULL)
			{
				array[i]->n2->ref_count-=1;
			}
			if(array[i]->n3!=NULL)
			{
				array[i]->n3->ref_count-=1;
			}
			printf("Garbage:");
			display_node(i);
			free(array[i]);
			array[i]=NULL;
		}
	}
}

int main()
{
	
	//Node number  		   	   0,1,2,3,4,5,6,7	
	//value		
	int val[]={1,2,3,5,7,8,9,10};
	
	
	int i;
	
	for( i=0;i<8;i++)
	{
		Node* newNode =(Node*)malloc(sizeof(Node));
		newNode->data=val[i];
		newNode->n1=NULL;
		newNode->n2=NULL;
		newNode->n3=NULL;
		newNode->ref_count=0;
		newNode->mark=false;		
		array[i]=newNode;
	}
	
	Node*root1=array[3];
	array[3]->ref_count+=1;
	Node*root2=array[0];
	array[0]->ref_count+=1;
	
	set_edge(0,1,6,7);
	set_edge(2,5,7,-1);
	set_edge(3,0,-1,-1);
	set_edge(4,0,5,-1);
	set_edge(5,6,-1,-1);
	

	
	printf("\nAll nodes through Root-1:\n");
	displaynodes(root1);
	
		
	printf("\nAll nodes through Root-2:\n");
	displaynodes(root2);
	
	printf("\n\nAdjacency list :\n");
	adjacency_list();
	
	printf("\n\nAdjacency matrix:\n");
	adjacency_Matrix();
	

	
	printf("\nCalling the mark and sweep garbage collector\n");
	
	mark_method(root1);
	sweep_method();

	



	

	
	printf("\n\nAdjacency list after removal of garbage:\n");
	adjacency_list();
	
	printf("\n\nAdjacency matrix after removal of garbage:\n");
	adjacency_Matrix();
	
}
