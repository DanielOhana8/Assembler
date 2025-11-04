#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"

/* Function to add node to table */
node* add_node(node* head, char* name, char* val, int num) 
{
	node* new_node = (node*)malloc(sizeof(node)); /* Create a new node */
	if (!new_node) /* If could not create new macro */
	{
    	printf("\nCould not create new node\n"); /* Print error message */
		exit(0);
	}
	
	new_node->name = (char*)malloc(max_line_len*sizeof(char));
	new_node->val = (char*)malloc(max_line_len*sizeof(char));
	if (!name || !val) /* If could not create new_node->name and new_node->val */
	{
		printf("\nCould not create new node\n"); /* Print error message */ 
		exit(0);
	}

	strcpy(new_node->name, name); /* Save name in new_node->name */
	strcpy(new_node->val, val); /* Save val in new_node->val */
	new_node->num = num; /* Save num in new_node->num */
	
	if (head->name) /* If there is not nodes in the table */
		new_node->next = head; /* Set the next of the new node to head */
	else
	{
		free(head);
		new_node->next = NULL; /* Set the next of the new node to NULL */
	}
	
	return new_node;
}

/* Function to free table */
void free_table(node* head,int num) 
{
	int i;
	node* temp;
		
	if (!num) /* If there is not nodes in the table */
	{
		free(head);
		return;
	}

	for (i=0; i<num; i++) 
	{
		temp = head;
		head = head->next; /* Move to the next node */
		
		free(temp->name);
		free(temp->val);
		free(temp);
	}
}
