#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "help.h"

void free_table(node*, int); 
node* add_node(node*,char*,char*,int);
void rem_whites(char*);
char* next_word(char*);
void rem_word(char*);
int method(char*);
void first_pass(char*,node*,int);

void pre_assembler(char* file_name)
{
	char temp_name[max_file_name], s[max_line_len], line[max_line_len], word[max_line_len], name[max_macro_name], val[max_macro_val];
	char* cmd_list[] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop",".data",".string",".entry",".extern"};
	FILE *file ,*new_file;
	int i, macros_num = 0, flag = 0, macro_flag = 0, line_num = 0;
	node *temp;
	node *head = (node*)malloc(sizeof(node));
	if (!head) /* If could not create new macro */
	{
    	printf("\nProblem with file %s.as: could not create new macro\n", file_name); /* Print error message */
		free(head);
		return;
	}

	strcpy(temp_name,file_name);
	file = fopen(strcat(temp_name,".as"), "r"); /* Open the file */
	if (!file) /* If the file could not be opened */
	{
		printf("\nProblem with file %s.as: Could not open file %s.as\n", file_name, file_name); /* Print error message */
		free(head);
		return;
	}

	strcpy(temp_name,file_name);
	new_file = fopen(strcat(temp_name,".am"), "w"); /* Create new file */
	if (!new_file) /* If the file could not be created */
	{
		printf("\nProblem with file %s.as: could not create file %s.am\n", file_name, file_name); /* Print error message */
		free(head);
		return;
	}
	
	for (i=0; i<strlen(val); val[i++] = '\0');
	
	while (fgets(line,max_line_len,file)) /* If there is still line */
	{	
		line_num++;
		strcpy(s, line);
		rem_whites(s); /* Remove all the white chars from the beginning of s */
		strcpy(word, next_word(s)); /* Save the first word of s in word */

		if (macro_flag) 
		{
			if (!strcmp(word,"endmacr")) /* If word is endmacr */
			{
				rem_word(s);  /* Remove the first word from s */
				rem_whites(s); /* Remove all the white chars from the beginning of s */

				if (*s) /* If s is not empty */
				{
					printf("\nProblem with file %s.as, line %d: you cant write after the endmacr, in the same line\n", file_name, line_num); /* Print error message */
					free_table(head,macros_num); /* Free the macros table */
					fclose(file); /* Close the file */
					fclose(new_file); /* Close the file */
					return;
				}
				
				head = add_node(head, name, val, 0); /* Add the macro to the macro table */
				macros_num++;

				macro_flag = 0;
			}
			else
			{
				if ((max_macro_val - strlen(val) - 1) > strlen(line)) /* If tere is enough space in val */
				{
    				strncat(val, line, max_macro_val - strlen(val) - 1); /* Add the line to val */
					val[strlen(val)] = '\0';
				} 
				else 
				{
    				printf("\nProblem with file %s.as, line %d: macro value is too big\n", file_name, line_num); /* Print error message */
    				free_table(head,macros_num); /* Free the macros table */
    				fclose(file); /* Close the file */
    				fclose(new_file); /* Close the file */
    				return;
				}
			}
		}
		else if (!strcmp(word,"macr")) /* If word is macr */
		{
			rem_word(s); /* Remove the first word from s */
			rem_whites(s); /* Remove all the whites from the beginning of s */
			strcpy(name,next_word(s)); /* Save the next word of s in name */
			rem_word(s); /* Remove the first word from s */
			rem_whites(s); /* Remove all the whites from the beginning of s */
			
			if (*s) /* If s is not empty */
			{
				printf("\nProblem with file %s.as, line %d: you cant write after the name of the macro, in the same line\n", file_name, line_num); /* Print error message */
				free_table(head,macros_num); /* Free the macros table */
				fclose(file); /* Close the file */
				fclose(new_file); /* Close the file */
				return;
			}
			
			for(i=0; i<cmd_num; i++)
			{
				if (!strcmp(name,cmd_list[i])) /* If the name is a command name */
				{
					printf("\nProblem with file %s.as, line %d: macro name can't be command name\n", file_name, line_num); /* Print error message */
					free_table(head,macros_num); /* Free the macros table */
					fclose(file); /* Close the file */
					fclose(new_file); /* Close the file */
					return;
				}
			}

			if (method(name) == 3) /* If the name is a register name */
			{
				printf("\nProblem with file %s.as, line %d: macro name can't be register name\n", file_name, line_num); /* Print error message */
				free_table(head,macros_num); /* Free the macros table */
				fclose(file); /* Close the file */
				fclose(new_file); /* Close the file */
				return;
			}
			
			temp = head;
			for (i=0; i < macros_num; i++) 
			{
				if (!strcmp(name,temp->name)) /* If the name is a name of other macro */
				{
					printf("\nProblem with file %s.as, line %d: there is macro with this name\n", file_name, line_num); /* Print error message */
					free_table(head,macros_num); /* Free the macros table */
    				fclose(file); /* Close the file */
    				fclose(new_file); /* Close the file */
    				return;
				}

				temp = temp->next; /* Move to the next macro */
			}
			
			macro_flag = 1; 
		}
		else
		{
			flag = 0;
			
			rem_word(s); /* Remove the first word from s */
			rem_whites(s); /* Remove all the whites from the beginning of s */

			temp = head;
			for (i=0; i<macros_num; i++) 
			{
				if (!strcmp(word,temp->name) && !*s) /* If word is macro and s empty */
				{
					fputs(temp->val, new_file); /* Put the value of the macro in the .am file */
					flag = 1;
					break;
				}

				temp = temp->next; /* Move to the next macro */
			}
			
			if (!flag) /* If word is not macro */
			{
				fputs(line, new_file); /* Put the line in the .am file */
				flag = 0;
			}
		}
	}

	fclose(file); /* Close the file */
	fclose(new_file); /* Close the file */
	
	first_pass(file_name,head,macros_num);
}
