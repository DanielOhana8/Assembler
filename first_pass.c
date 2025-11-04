#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"

void rem_whites(char*);
void rem_word(char*);
char* next_word(char*);
node* add_node(node*,char*,char*,int);
void free_table(node*,int);
int is_num(char*);
int is_cmd(char*);
int method(char*);
void second_pass(char*,node*,int,int,int);

void first_pass(char* file_name, node* macro_head, int macros_num)
{
	FILE *file;	
	char temp_name[max_file_name], s[max_line_len], line[max_line_len], word[max_line_len], second_word[max_line_len], name[max_line_len], cmd[max_line_len];
	char* cmd_list[] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop",".data",".string",".entry",".extern"};
	int i, t = 0, labels_num = 0, line_num = 0, IC = 100, DC = 0, error = 0, DC_add = 0;
	node *temp;
	node *head = (node*)malloc(sizeof(node));
	if (!head) /* If could not create new macro */
	{
		printf("\nProblem with file %s.as: could not create new macro\n", file_name); /* Print error message */
		free(head);
		return;
	}
	
	strcpy(temp_name,file_name);
	file = fopen(strcat(temp_name,".am"), "r"); /* Open the file */
	if (!file) /* If the file could not be opened */
	{
		printf("\nProblem with file %s: Could not open file %s.am\n", temp_name, temp_name); /* Print error message */
		free(head);
		return;
	}
	
	while (fgets(line,max_line_len,file)) /* If there is still line */
	{
		line_num++;
		strcpy(s, line);
		rem_whites(s); /* Remove all the white chars from the beginning of s */
		
		if (*s == ';' || !*s) /* If the line is comment line or empty line */
			continue; /* Move to the next line */
			
		strcpy(word, next_word(s)); /* Save the first word of s in word */
		rem_word(s); /* Remove the first word from s */
		rem_whites(s); /* Remove all the white chars from the beginning of s */

		if (*(word+(strlen(word)-1)) == ':') /* If its a label */
		{
			*(word+(strlen(word)-1)) = '\0';
			strcpy(name,word); /* save in name the first word without the : */
		
			temp = macro_head;
			for (i=0; i<macros_num; i++)
			{
				if (!strcmp(name,temp->name)) /* If there is macro with the name of the label */ 
				{
					printf("\nProblem with file %s.am, line %d: label name can't be macro name\n", file_name, line_num); /* Print error message */
					error = 1;
					t = 1;
					break;
				}
					
				temp = temp->next; /* Move to the next macro */
			}
			
			if (!isalpha(*name)) /* If the first char of name is not a letter */
			{
				printf("\nProblem with file %s.am, line %d: label name should start with a letter\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}
			
			for(i=1; i<strlen(name); i++)
			{
				if (!isalpha(*(name+i)) && !isdigit(*(name+i))) /* If there is char in name that is not letter or number */
				{
					printf("\nProblem with file %s.am, line %d: illigal label name\n", file_name, line_num); /* Print error message */
					error = 1;
					t = 1;
					break;
				}
			}

			for(i=0; i<cmd_num; i++)
			{
				if (!strcmp(name,cmd_list[i])) /* If the name of the label is a command name */
				{
					printf("\nProblem with file %s.am, line %d: label name can't be command name\n", file_name, line_num); /* Print error message */
					error = 1;
					break;
				}
			}

			if (method(name) == 3) /* If the name of the label is a register name */
			{
				printf("\nProblem with file %s.am, line %d: label name can't be register name\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}
				
			temp = head;
			for (i=0; i < labels_num; i++) 
			{
				if (!strcmp(name,temp->name)) /* If there is label with this name */
				{
					printf("\nProblem with file %s.am, line %d: there is label with this name\n", file_name, line_num); /* Print error message */
					error = 1;
					t = 1;
					break;
				}

				temp = temp->next; /* Move to the next label */
			}

			if (t) /* If there was an error */
			{
				t = 0;
				continue; /* Move to the next line */
			}
			
			strcpy(cmd,next_word(s)); /* Save the next word of s in cmd */
			rem_word(s); /* Remove the first word from s */
			rem_whites(s); /* Remove all the white chars from the beginning of s */
	
			if (!strcmp(cmd,".string") || !strcmp(cmd,".data")) /* If the command is .string or .data */
			{
				head = add_node(head,name,".data",DC); /* Add the label to the labels table */
				labels_num++;
			}
			else if (is_cmd(cmd)) /* If the command is not .string, .data, .extern or .entry */
			{
				head = add_node(head,name,".code",IC); /* Add the label to the labels table */
				labels_num++;
			}
			else if (!strcmp(cmd,".extern") || !strcmp(cmd,".entry")) /* If the command is .extern or .entry */
			{
				while(*s) /* If s is not empty */
				{
					strcpy(word,next_word(s)); /* Save the next word of s in word */
					rem_word(s); /* Remove the first word from s */
					rem_whites(s); /* Remove all the white chars from the beginning of s */
		
					if (*s) /* If s is not empty */
					{
						if (*s == ',') /* If s starts with comma */
						{
							*s = ' ';
							rem_whites(s); /* Remove the comma */
							
							if (!*s) /* If s is empty */
							{
								printf("\nProblem with file %s.am, line %d: there is not label after the comma\n", file_name, line_num); /* Print error message */
								error = 1;
								break;
							}
						}
						else /* If s does not starts with comma */
						{
							printf("\nProblem with file %s.am, line %d: you should write commas between the labels\n", file_name, line_num); /* Print error message */
							error = 1;
							break;
						}
					}
						
					if (!strcmp(cmd,".extern")) /* If the command is .extern */
					{
						head = add_node(head,word,".external",0); /* Add the label to the labels table */
						labels_num++;
					}
				}
					
				if (error) /* If there was an error */
					continue; /* Move to the next line */
			}			
		}
		else
		{
			strcpy(cmd,word); /* save word in cmd */
			
			if (!strcmp(cmd,".extern") || !strcmp(cmd,".entry")) /* If the command is .extern or .entry */
			{
				while(*s) /* If s is not empty */
				{
					strcpy(word,next_word(s)); /* Save the next word of s in word */
					rem_word(s); /* Remove the first word from s */
					rem_whites(s); /* Remove all the white chars from the beginning of s */
		
					if (*s) /* If s is not empty */
					{
						if (*s == ',') /* If s starts with comma */
						{
							*s = ' ';
							rem_whites(s); /* Remove the comma */
							
							if (!*s) /* If s is empty */
							{
								printf("\nProblem with file %s.am, line %d: there is not label after the comma\n", file_name, line_num); /* Print error message */
								error = 1;
								break;
							}
						}
						else /* If s does not starts with comma */
						{
							printf("\nProblem with file %s.am, line %d: you should write commas between the numbers labels\n", file_name, line_num); /* Print error message */
							error = 1;
							break;
						}
					}
						
					if (!strcmp(cmd,".extern")) /* If the command is .extern */
					{
						head = add_node(head,word,".external",0); /* Add the label to the labels table */
						labels_num++;
					}
				}
					
				if (error) /* If there was an error */
					continue; /* Move to the next line */
			}	
		}
		
		if (!strcmp(cmd,".extern") || !strcmp(cmd,".entry")) /* If the command is .extern or .entry */
			continue; /* Move to the next line */
		else if (!strcmp(cmd,".string")) /* If the command is .string */
		{
			if (!*s) /* If s is empty */
			{
				printf("\nProblem with file %s.am, line %d: missing string after .string\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}
			
			if (*(s+(strlen(s)-1)) == '\n') /* If there is \n in the end of s */
				*(s+(strlen(s)-1)) = '\0'; /* Remove the \n */
			
			if (*s != '\"' || *(s+(strlen(s)-1)) != '\"') /* If word does not begin and end with " */
			{
				printf("\nProblem with file %s.am, line %d: missing string after .string\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}

			DC += strlen(s)-1; /* Update DC */
		}
		else if (!strcmp(cmd,".data")) /* If the command is .data */
		{	
			if (!*s) /* If s is empty */
			{
				printf("\nProblem with file %s.am, line %d: you should write numbers after .data\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}

			while(*s) /* While s is not empty */
			{
				strcpy(word,next_word(s)); /* Save the next word of s in word */
				rem_word(s); /* Remove the first word from s */
				rem_whites(s); /* Remove all the white chars from the beginning of s */
				
				if (!is_num(word)) /* If word is not number */
				{
					printf("\nProblem with file %s.am, line %d: you should write numbers after .data\n", file_name, line_num); /* Print error message */
					error = 1;
					break;
				}

				if (*s) /* If s is not empty */
				{
					if (*s == ',') /* If s starts with comma */
					{
						*s = ' ';
						rem_whites(s); /* Remove the comma */
						
						if (!*s) /* If s is empty */
						{
							printf("\nProblem with file %s.am, line %d: there is not number after the comma\n", file_name, line_num); /* Print error message */
							error = 1;
							break;
						}
					}
					else /* If s does not starts with comma */
					{
						printf("\nProblem with file %s.am, line %d: you should write commas between the numbers after .data\n", file_name, line_num); /* Print error message */
						error = 1;
						break;
					}
				}

				DC_add++; /* Update DC_add */
			}
			
			DC += DC_add; /* Update DC */
			DC_add = 0;

			if (error) /* If there was an error */
				continue; /* Move to the next line */
		}
		else if (is_cmd(cmd)) 
		{
			if (!strcmp(cmd,"rts") || !strcmp(cmd,"stop")) /* If the command is rts or stop */
			{
				if (*s) /* If s is not empty */
				{
					printf("\nProblem with file %s.am, line %d: too much operands for this command\n", file_name, line_num); /* Print error message */
					error = 1;
					continue; /* Move to the next line */
				}

				IC++; /* Update IC */
				continue; /* Move to the next line */
			}

			if (!*s) /* If s is empty */
			{
				printf("\nProblem with file %s.am, line %d: missing operands after the command\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}

			strcpy(word,next_word(s)); /* Save the next word of s in word */
			rem_word(s); /* Remove the first word from s */
			rem_whites(s); /* Remove all the white chars from the beginning of s */
			
			if  (method(word) == 0) /* If the number of the method of the first operand is 0 */
			{
				if (!strcmp(cmd,"lea") || !strcmp(cmd,"clr") || !strcmp(cmd,"not") || !strcmp(cmd,"inc") || !strcmp(cmd,"dec") || !strcmp(cmd,"jmp") || !strcmp(cmd,"bne") || !strcmp(cmd,"red") || !strcmp(cmd,"jsr"))
				{
					printf("\nProblem with file %s.am, line %d: illigal operand for this command\n", file_name, line_num); /* Print error message */
					error = 1;
					continue; /* Move to the next line */
				}
			}
			else if (method(word) == 2) /* If the number of the method of the first operand is 2 */
			{
				if (!strcmp(cmd,"lea"))
				{
					printf("\nProblem with file %s.am, line %d: illigal operand for this command\n", file_name, line_num); /* Print error message */
					error = 1;
					continue; /* Move to the next line */
				}
			}
			else if (method(word) == 3) /* If the number of the method of the first operand is 3 */
			{
				if (!strcmp(cmd,"lea") || !strcmp(cmd,"jmp") || !strcmp(cmd,"bne") || !strcmp(cmd,"jsr"))
				{
					printf("\nProblem with file %s.am, line %d: illigal operand for this command\n", file_name, line_num); /* Print error message */
					error = 1;
					continue; /* Move to the next line */
				}
			}

			if (!strcmp(cmd,"clr") || !strcmp(cmd,"not") || !strcmp(cmd,"inc") || !strcmp(cmd,"dec") || !strcmp(cmd,"jmp") || !strcmp(cmd,"bne") || !strcmp(cmd,"red") || !strcmp(cmd,"prn") || !strcmp(cmd,"jsr"))
			{
				if (*s) /* If s is not empty */
				{
					printf("\nProblem with file %s.am, line %d: too much operands for this command\n", file_name, line_num); /* Print error message */
					error = 1;
					continue; /* Move to the next line */
				}

				IC += 2; /* Update IC */
				continue; /* Move to the next line */
			}

			if (!*s) /* If s is empty */
			{
				printf("\nProblem with file %s.am, line %d: missing operands for this command\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}

			if (*s == ',') /* If s starts with comma */
			{
				*s = ' ';
				rem_whites(s); /* Remove the comma */
			}
			else /* If s does not starts with comma */
			{
				printf("\nProblem with file %s.am, line %d: missing comma between the operands\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}
			
			strcpy(second_word,next_word(s)); /* Save the next word of s in second_word */
			rem_word(s); /* Remove the first word from s */
			rem_whites(s); /* Remove all the white chars from the beginning of s */
			
			if (*s) /* If s is not empty */
			{
				printf("\nProblem with file %s.am, line %d: too much operands for this command\n", file_name, line_num); /* Print error message */
				error = 1;
				continue; /* Move to the next line */
			}

			if  (method(second_word) == 0) /* If the number of the method of the second operand is 0 */
			{
				if (!strcmp(cmd,"mov") || !strcmp(cmd,"sub") || !strcmp(cmd,"add") || !strcmp(cmd,"lea"))
				{
					printf("\nProblem with file %s.am, line %d: illigal operand for this command\n", file_name, line_num); /* Print error message */
					error = 1;
					continue; /* Move to the next line */
				}
			}

			if ((method(word) == 2 || method(word) == 3) && (method(second_word) == 2 || method(second_word) == 3))  /* If word and second_word are registers */
				IC += 2; /* Update IC */
			else
				IC += 3; /* Update IC */
		}
		else
		{
			printf("\nProblem with file %s.am, line %d: unknown command\n", file_name, line_num); /* Print error message */
			error = 1;
			continue; /* Move to the next line */
		}
	}
	
	temp = head;
	for (i=0; i<labels_num; i++) 
	{
		if (!strcmp(temp->val,".data")) /* If temp->val is .data */
			temp->num += IC; /* Add the IC to temp->num */

		temp = temp->next; /* Move to the next label */
	}
	
	fclose(file); /* Close the file */
	free_table(macro_head,macros_num); /* Free the macros table */

	if (error) /* If there was an error */
	{
		free_table(head,labels_num); /* Free the labels table */
		return;
	}
	
	second_pass(file_name,head,labels_num,IC,DC);
}
