#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "help.h"

void free_table(node*,int);
void rem_whites(char*);
void rem_word(char*);
char* next_word(char*);
int is_cmd(char*);
int method(char*);

void second_pass(char* file_name, node* head, int labels_num, int IC, int DC)
{
	FILE *file, *ob_file, *ent_file, *ext_file;	
	char  temp_name[max_file_name], line[max_file_name], s[max_file_name], word[max_file_name], second_word[max_file_name], cmd[max_file_name];
	int i, t = 0, line_num = 0, word_num = 100;
	unsigned int a = 0, b = 0;
	node *temp;
	
	strcpy(temp_name,file_name);
	file = fopen(strcat(temp_name,".am"), "r"); /* Open the file */
	if (!file) /* If the file could not be opened */
	{
		printf("\nProblem with file %s: Could not open file %s.am\n", temp_name, temp_name); /* Print error message */
		free(head);
		return;
	}
	
	strcpy(temp_name,file_name);
	ob_file = fopen(strcat(temp_name,".ob"), "w"); /* Create new file */
	if (!ob_file) /* If the file could not be created */
	{
		printf("\nProblem with file %s.as: could not create file %s.ob\n", file_name, file_name); /* Print error message */
		free(head);
		return;
	}
	
	strcpy(temp_name,file_name);
	ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
	if (ent_file) /* If the file exists */
	{
		fclose(ent_file); /* Close the ent file */
		remove(temp_name); /* Remove the ent file */
	}
	
	strcpy(temp_name,file_name);
	ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
	if (ext_file) /* If the file exists */
	{
		fclose(ext_file); /* Close the ext file */
		remove(temp_name); /* Remove the ext file */
	}
	
	fprintf(ob_file,"   %d %d\n",IC-100,DC);
	
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
			strcpy(word, next_word(s)); /* Save the next word of s in word */ 
			rem_word(s); /* Remove the first word of s from s */
			rem_whites(s); /* Remove all the white chars from the beginning of s */
		}
		
		strcpy(cmd, word); /* Save word in cmd */

		if (is_cmd(cmd))
		{	
			if (!strcmp(cmd,"cmp")) 
				a |= (1<<11); /* Save the number of the command in bits 11-14 of a */
			else if (!strcmp(cmd,"add"))
				a |= (1<<12); /* Save the number of the command in bits 11-14 of a */
			else if (!strcmp(cmd,"sub"))
			{
				a |= (1<<11);
				a |= (1<<12); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"lea"))
				a |= (1<<13);
			else if (!strcmp(cmd,"clr"))
			{
				a |= (1<<11);
				a |= (1<<13); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"not"))
			{
				a |= (1<<12);
				a |= (1<<13); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"inc"))
			{
				a |= (1<<11);
				a |= (1<<12);
				a |= (1<<13); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"dec"))
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			else if (!strcmp(cmd,"jmp"))
			{
				a |= (1<<11);
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"bne"))
			{
				a |= (1<<12);
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"red"))
			{
				a |= (1<<11);
				a |= (1<<12);
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"prn"))
			{
				a |= (1<<13);
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"jsr"))
			{
				a |= (1<<11);
				a |= (1<<13);
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"rts"))
			{
				a |= (1<<12);
				a |= (1<<13);
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			}
			else if (!strcmp(cmd,"stop"))
			{
				a |= (1<<11);
				a |= (1<<12);
				a |= (1<<13);
				a |= (1<<14); /* Save the number of the command in bits 11-14 of a */
			}
			
			a |= (1<<2); /* turn on the third bit of a */
			
			if (!strcmp(cmd,"mov") || !strcmp(cmd,"cmp") || !strcmp(cmd,"add") || !strcmp(cmd,"sub") || !strcmp(cmd,"lea")) /* If the command has 2 opernds */ 
			{
				strcpy(word, next_word(s)); /* Save the first word of s in word */
				rem_word(s); /* Remove the first word from s */
				rem_whites(s); /* Remove all the white chars from the beginning of s */
				
				a |= (1<<(method(word)+7)); /* save the method of word in a */
				
				*s = ' ';
				rem_whites(s); /* Remove the comma */
				
				strcpy(second_word, next_word(s)); /* Save the first word of s in second word */
				rem_word(s); /* Remove the first word from s */
				rem_whites(s); /* Remove all the white chars from the beginning of s */
				
				a |= (1<<(method(second_word)+3)); /* save the method of second word in a */
				
				fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
				word_num++;
				a = 0;
				
				if (method(word) == 0) /* If the number of the method of the first operand is 0 */
				{
					*word = ' '; 
					rem_whites(word); /* Remove the # */
				
					b = atoi(word); /* Save the number in b */
					a |= (1<<2); /* turn on bit 2 of a */
					
					for (i=0; i<12; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+3)); /* save the number in bits 3-14 of a */
					}
					
					fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
					word_num++;
					a = 0;
					b = 0;
				}
				else if (method(word) == 1) /* If the number of the method of the first operand is 1 */
				{
					temp = head;
					for (i=0; i<labels_num; i++)
					{
						if (!strcmp(word,temp->name)) /* If word is temp->name */
						{
							if (!strcmp(temp->val,".external")) /* If the label is external */
							{
								a |= 1; /* Turn on the first bit of a */
								
								strcpy(temp_name,file_name);
								ext_file = fopen(strcat(temp_name,".ext"), "a"); /* Create or open the ext file */
								if (!ext_file) /* If the file could not be created */
								{
									printf("\nProblem with file %s.am: could not open file %s.ext\n", file_name, file_name); /* Create or open the ext file */
									fclose(file); /* Close the file */
									free_table(head,labels_num); /* Free the labels table */
									
									strcpy(temp_name,file_name);
									remove(strcat(temp_name,".ob")); /* Remove the ob file */
									
									strcpy(temp_name,file_name);
									ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
									if (ent_file) /* If the file exists */
									{
										fclose(ent_file); /* Close the ent file */
										remove(temp_name); /* Remove the ent file */
									}
						
									return;
								}
						
								fprintf(ext_file, "%s %04d\n", temp->name, word_num); /* Print the name and word_num to the ext file */
								
								fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
								word_num++;
								a = 0;
								
								t = 1;
						
								fclose(ent_file); /* close the ent file */
								break;
							}
							else /* If the label is not external */
							{
								a |= (1<<1); /* Turn on the second bit of a */
								b = temp->num; /* Save the number of the label in b */
								
								for (i=0; i<12; i++)
								{
									if (b & (1<<i))
										a |= (1<<(i+3)); /* save the number in bits 3-14 of a */
								}
								
								fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
								word_num++;
								a = 0;
								b = 0;
								
								t = 1;
								
								break;
							}
						}

						temp = temp->next; /* Move to the next label */
					}
					
					if (!t) /* If didn't found a label */
					{
						printf("\nProblem with file %s.am, line %d: unknown label\n", file_name, line_num); /* Print error message */
						fclose(file); /* Close the file */
						free_table(head,labels_num); /* Free the labels table */
						
						strcpy(temp_name,file_name);
						remove(strcat(temp_name,".ob")); /* Remove the ob file */
									
						strcpy(temp_name,file_name);
						ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
						if (ext_file) /* If the file exists */
						{
							fclose(ext_file); /* Close the ext file */
							remove(temp_name); /* Remove the ext file */
						}
							
						strcpy(temp_name,file_name);
						ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
						if (ent_file) /* If the file exists */
						{
							fclose(ent_file); /* Close the ent file */
							remove(temp_name); /* Remove the ent file */
						}
						
						return;
					}
					t = 0;
				}
				else if ((method(word) == 2 || method(word) == 3) && (method(second_word) == 2 || method(second_word) == 3))  /* If word and second_word are registers */
				{
					a |= (1<<2); /* Turn on the third bit of a */
					
					if (method(word) == 2) /* If the number of the method of the first operand is 2 */
					{
						*word = ' ';
						rem_whites(word); /* Remove the * */
					}
					
					if (method(second_word) == 2) /* If the number of the method of the second operand is 2 */
					{
						*second_word = ' ';
						rem_whites(second_word); /* Remove the * */
					}
					
					*word = ' ';
					rem_whites(word); /* Remove the r */
					
					*second_word = ' ';
					rem_whites(second_word); /* Remove the r */
					
					b = atoi(word); /* save the number of the first register in b */
					for (i=0; i<3; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+6)); /* save the number of the first register in bits 3-5 of a */
					}
					
					b = atoi(second_word); /* save the number of the second register in b */
					for (i=0; i<3; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+3)); /* save the number of the second register in bits 6-8 of a */
					}
					
					fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
					word_num++;
					a = 0;
					b = 0;
					
					continue; /* Move to the next line */
				}
				else if (method(word) == 2 || method(word) == 3) /* If the number of the method of the first operand is 2 or 3 */
				{
					a |= (1<<2); /* Turn on the third bit of a */
					
					if (method(word) == 2) /* If the number of the method of the first operand is 2 */
					{
						*word = ' ';
						rem_whites(word); /* Remove the * */
					}
					
					*word = ' ';
					rem_whites(word); /* Remove the r */
					
					b = atoi(word); /* save the number of the register in b */
					for (i=0; i<3; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+6)); /* save the number of the register in bits 6-8 of a */
					}
					
					fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
					word_num++;
					a = 0;
					b = 0;
				}
				
				if (method(second_word) == 0) /* If the number of the method of the second operand is 0 */
				{
					*second_word = ' '; 
					rem_whites(second_word); /* Remove the # */
				
					b = atoi(second_word); /* save the number in b */
					a |= (1<<2); /* Turn on the third bit of a */
					
					for (i=0; i<12; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+3)); /* save the number in bits 3-14 of a */
					}
					
					fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
					word_num++;
					a = 0;
					b = 0;
				}
				else if (method(second_word) == 1) /* If the number of the method of the second operand is 1 */
				{
					temp = head;
					for (i=0; i<labels_num; i++)
					{
						if (!strcmp(second_word,temp->name)) /* If second_word is temp->name */
						{
							if (!strcmp(temp->val,".external")) /* If the label is external */
							{
								a |= 1; /* Turn on the first bit of a */
								
								strcpy(temp_name,file_name);
								ext_file = fopen(strcat(temp_name,".ext"), "a"); /* Create new file */
								if (!ext_file) /* If the file could not be created */
								{
									printf("\nProblem with file %s.am: could not open file %s.ext\n", file_name, file_name); /* Create or open the ext file */
									fclose(file); /* Close the file */
									free_table(head,labels_num); /* Free the labels table */
									
									strcpy(temp_name,file_name);
									remove(strcat(temp_name,".ob")); /* Remove the ob file */
							
									strcpy(temp_name,file_name);
									ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
									if (ent_file) /* If the file exists */
									{
										fclose(ent_file); /* Close the ent file */
										remove(temp_name); /* Remove the ent file */
									}
						
									return;
								}
								
								fprintf(ext_file, "%s %04d\n", temp->name, word_num); /* Print the name and word_num to the ext file */
								
								fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
								word_num++;
								a = 0;
								
								t = 1;
						
								fclose(ent_file); /* Close the ent file */
								break;
							}
							else /* If the label is not external */
							{
								a |= (1<<1); /* Turn on the second bit of a */
								b = temp->num; /* Save the number of the label in b */
								
								for (i=0; i<12; i++)
								{
									if (b & (1<<i))
										a |= (1<<(i+3)); /* save the number in bits 3-14 of a */
								}
								
								fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
								word_num++;
								a = 0;
								b = 0;
								
								t = 1;
								
								break;
							}
						}
						
						temp = temp->next; /* Move to the next label */
					}
					
					if (!t) /* If didn't found a label */
					{
						printf("\nProblem with file %s.am, line %d: unknown label\n", file_name, line_num); /* Print error message */
						fclose(file); /* Close the file */
						free_table(head,labels_num); /* Free the label table */

						strcpy(temp_name,file_name);
						remove(strcat(temp_name,".ob")); /* Remove the ob file */
							
						strcpy(temp_name,file_name);
						ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
						if (ext_file) /* If the file exists */
						{
							fclose(ext_file); /* Close the ext file */
							remove(temp_name); /* Remove the ext file */
						}
									
						strcpy(temp_name,file_name);
						ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
						if (ent_file) /* If the file exists */
						{
							fclose(ent_file); /* Close the ent file */
							remove(temp_name); /* Remove the ent file */
						}
						
						return;
					}
					t = 0;
				}
				else if (method(second_word) == 2 || method(second_word) == 3) /* If the number of the method of the second operand is 2 or 3 */
				{
					a |= (1<<2); /* Turn on the third bit of a */
					
					if (method(second_word) == 2) /* If the number of the method of the second operand is 2 */
					{
						*second_word = ' ';
						rem_whites(second_word); /* Remove the * */
					}
					
					*second_word = ' ';
					rem_whites(second_word); /* Remove the r */
					
					b = atoi(second_word); /* Save the number of the register in b */
					for (i=0; i<3; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+3)); /* save the number in bits 3-5 of a */
					}
					
					fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
					word_num++;
					a = 0;
					b = 0;
				}
			}
			else if (strcmp(cmd,"rts") && strcmp(cmd,"stop")) /* If the command has 1 opernd */ 
			{
				strcpy(word,next_word(s)); /* Save the next word of s in word */
				rem_word(s); /* Remove the first word from s */
				rem_whites(s); /* Remove all the white chars from the beginning of s */
				
				a |= (1<<(method(word)+3)); /* save the method of word in a */
				
				fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
				word_num++;
				a = 0;
				
				if (method(word) == 0) /* If the number of the method of the operand is 0 */
				{
					*word = ' '; 
					rem_whites(word); /* Remove the # */
				
					b = atoi(word); /* Save the number in b */
					a |= (1<<2); /* Turn on the third bit of a */
					
					for (i=0; i<12; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+3)); /* save the number in bits 3-14 of a */
					}
					
					fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
					word_num++;
					a = 0;
					b = 0;
				}
				else if (method(word) == 1) /* If the number of the method of the operand is 1 */
				{
					temp = head;
					for (i=0; i<labels_num; i++)
					{
						if (!strcmp(word,temp->name)) /* If word is temp->name */
						{
							if (!strcmp(temp->val,".external")) /* If the label is external */
							{
								a |= 1; /* Turn on the first bit of a */
								
								strcpy(temp_name,file_name);
								ext_file = fopen(strcat(temp_name,".ext"), "a"); /* Create new file */
								if (!ext_file) /* If the file could not be created */
								{
									printf("\nProblem with file %s.am: could not open file %s.ext\n", file_name, file_name); /* Create or open the ext file */
									fclose(file); /* Close the file */
									free_table(head,labels_num); /* Free the labels table */
									
									strcpy(temp_name,file_name);
									remove(strcat(temp_name,".ob")); /* Remove the ob file */
							
									strcpy(temp_name,file_name);
									ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
									if (ent_file) /* If the file exists */
									{
										fclose(ent_file); /* Close the ent file */
										remove(temp_name); /* Remove the ent file */
									}
						
									return;
								}
								
								fprintf(ext_file, "%s %04d\n", temp->name, word_num); /* Print the name and word_num to the ext file */
								
								fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
								word_num++;
								a = 0;
								
								t = 1;
						
								fclose(ent_file); /* Close the ent file */
								break;
							}
							else /* If the label is not external */
							{
								a |= (1<<1); /* Turn on the second bit of a */
								b = temp->num; /* Save the number of the label in b */
								
								for (i=0; i<12; i++)
								{
									if (b & (1<<i))
										a |= (1<<(i+3)); /* save the number in bits 3-14 of a */
								}
								
								fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
								word_num++;
								a = 0;
								b = 0;
								
								t = 1;
							}
						}
						
						temp = temp->next; /* Move to the next label */
					}
					
					if (!t) /* If didn't found a label */
					{
						printf("\nProblem with file %s.am, line %d: unknown label\n", file_name, line_num); /* Print error message */
						fclose(file); /* Close the file */
						free_table(head,labels_num); /* Free the labels table */
						
						strcpy(temp_name,file_name);
						remove(strcat(temp_name,".ob")); /* Remove the ob file */
							
						strcpy(temp_name,file_name);
						ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
						if (ext_file) /* If the file exists */
						{
							fclose(ext_file); /* Close the ext file */
							remove(temp_name); /* Remove the ext file */
						}
							
						strcpy(temp_name,file_name);
						ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
						if (ent_file) /* If the file exists */
						{
							fclose(ent_file); /* Close the ent file */
							remove(temp_name); /* Remove the ent file */
						}
						
						return;
					}
					t = 0;
				}
				else if (method(word) == 2 || method(word) == 3) /* If the number of the method of the operand is 2 or 3 */
				{
					a |= (1<<2); /* Turn on the third bit of a */
					
					if (method(word) == 2) /* If the number of the method of the operand is 2 */
					{
						*word = ' ';
						rem_whites(word); /* Remove the * */
					}
					
					*word = ' ';
					rem_whites(word); /* Remove the r */
					
					b = atoi(word); /* Save the number of the register in b */
					for (i=0; i<3; i++)
					{
						if (b & (1<<i))
							a |= (1<<(i+3)); /* save the number in bits 3-5 of a */
					}
					
					fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
					word_num++;
					a = 0;
					b = 0;
				}
			}
			else
			{		
				fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the ob file in octal base */
				word_num++;
				a = 0;
			}
		}
		else if (!strcmp(cmd,".entry")) /* If the command is .entry */
		{
			while (*s) /* If s is not empty */
			{
				strcpy(word,next_word(s)); /* Save the next word of s in word */
				rem_word(s); /* Remove the first word from s */
				rem_whites(s); /* Remove all the white chars from the beginning of s */
			
				temp = head;
				for (i=0; i<labels_num; i++)
				{
					if (!strcmp(word,temp->name)) /* If word is temp->name */
					{
						if (!strcmp(temp->val,".external")) /* If the label is external */
						{
							printf("\nProblem with file %s.am, line %d: label can't be both external end entry \n", file_name, line_num); /* Print error message */
							fclose(file); /* Close the file */
							free_table(head,labels_num); /* Free the labels table */
							
							strcpy(temp_name,file_name);
							remove(strcat(temp_name,".ob")); /* Remove the ob file */
							
							strcpy(temp_name,file_name);
							ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
							if (ext_file) /* If the file exists */
							{
								fclose(ext_file); /* Close the ext file */
								remove(temp_name); /* Remove the ext file */
							}
								
							strcpy(temp_name,file_name);
							ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
							if (ent_file) /* If the file exists */
							{
								fclose(ent_file); /* Close the ent file */
								remove(temp_name); /* Remove the ent file */
							}
						
							return;
						}
					
						strcpy(temp_name,file_name);
						ent_file = fopen(strcat(temp_name,".ent"), "a"); /* Create new file */
						if (!ent_file) /* If the file could not be created */
						{
							printf("\nProblem with file %s.am: could not open file %s.ent\n", file_name, file_name); /* Create or open the ent file */
							fclose(file); /* Close the file */
							free_table(head,labels_num); /* Free the labels table */
							
							strcpy(temp_name,file_name);
							remove(strcat(temp_name,".ob")); /* Remove the ob file */
							
							strcpy(temp_name,file_name);
							ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
							if (ext_file) /* If the file exists */
							{
								fclose(ext_file); /* Close the ext file */
								remove(temp_name); /* Remove the ext file */
							}
							
							return;
						}
						
						fprintf(ent_file, "%s %d\n", temp->name, temp->num); /* Print the name and num of the label to the ent */
						t = 1;
						
						fclose(ent_file); /* Close the ent file */
						break;
					}	
					
					temp = temp->next; /* Move to the next label */
				}
				
				if (!t) /* If didn't found a label */
				{
					printf("\nProblem with file %s.am, line %d: unknown label \n", file_name, line_num); /* Print error message */
					fclose(file); /* Close the file */
					free_table(head,labels_num); /* Free the labels table */
					
					strcpy(temp_name,file_name);
					remove(strcat(temp_name,".ob")); /* Remove the ob file */
							
					strcpy(temp_name,file_name);
					ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
					if (ext_file) /* If the file exists */
					{
						fclose(ext_file); /* Close the ext file */
						remove(temp_name); /* Remove the ext file */
					}
							
					strcpy(temp_name,file_name);
					ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
					if (ent_file) /* If the file exists */
					{
						fclose(ent_file); /* Close the ent file */
						remove(temp_name); /* Remove the ent file */
					}
						
					return;
				}
				t = 0;
				
				if (*s) /* If s is not empty */
				{
					if (*s == ',') /* If s starts with comma */
					{
						*s = ' ';
						rem_whites(s); /* Remove the comma */
						
						if (!*s) /* If s is empty */
						{
							printf("\nProblem with file %s.am, line %d: there is not label after the comma\n", file_name, line_num); /* Print error message */
							
							strcpy(temp_name,file_name);
							remove(strcat(temp_name,".ob")); /* Remove the ob file */
						
							strcpy(temp_name,file_name);
							ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
							if (ext_file) /* If the file exists */
							{
								fclose(ext_file); /* Close the ext file */
								remove(temp_name); /* Remove the ext file */
							}
							
							strcpy(temp_name,file_name);
							ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
							if (ent_file) /* If the file exists */
							{
								fclose(ent_file); /* Close the ent file */
								remove(temp_name); /* Remove the ent file */
							}
					
							t = 1;
							break;
						}
					}
					else /* If s don't starts with comma */
					{
						printf("\nProblem with file %s.am, line %d: there is not comma between the labels\n", file_name, line_num); /* Print error message */
						
						strcpy(temp_name,file_name);
						remove(strcat(temp_name,".ob")); /* Remove the ob file */
						
						strcpy(temp_name,file_name);
						ext_file = fopen(strcat(temp_name,".ext"), "r"); /* Check if the file exists */
						if (ext_file) /* If the file exists */
						{
							fclose(ext_file); /* Close the ext file */
							remove(temp_name); /* Remove the ext file */
						}
							
						strcpy(temp_name,file_name);
						ent_file = fopen(strcat(temp_name,".ent"), "r"); /* Check if the file exists */
						if (ent_file) /* If the file exists */
						{
							fclose(ent_file); /* Close the ent file */
							remove(temp_name); /* Remove the ent file */
						}
					
						t = 1;
						break;
					}
				}
			}
			
			if (t) /* If there was an error */
				break;
		}
		else if (!strcmp(cmd,".data")) /* If the command is .data */
		{
			while (*s) /* While s is not empty */
			{
				strcpy(word,next_word(s)); /* Save the next word of s in word */
				rem_word(s); /* Remove the first word from s */
				rem_whites(s); /* Remove all the white chars from the beginning of s */
				
				sprintf(word,"%o",atoi(word)); /* save word as octal number in word */

				if (strlen(word) > 5) /* If the number has more than 5 digits */
        			fprintf(ob_file,"%04d %.5s\n", word_num, &word[strlen(word) - 5]); /* Print the last 5 digits to the obb file */
    			else 
        			fprintf(ob_file,"%04d %05d\n", word_num, atoi(word)); /* Print the number to the obb file */
        		word_num++;
 
				if (*s == ',') /* If s starts with comma */
				{
					*s = ' ';
					rem_whites(s); /* Remove the comma  */
				}
			} 
		}
		else if (!strcmp(cmd,".string")) /* If the command is .string */
		{
			strcpy(word,next_word(s)); /* Save the next word of s in word */
			rem_word(s); /* Remove the first word from s */
			rem_whites(s); /* Remove all the white chars from the beginning of s */
			
			*word = ' ';
			rem_whites(word); /* Remove the " from the beginning of word */
			
			*(word+(strlen(word)-1)) = '\0'; /* Remove the " from the end of word */
			
			for (i=0; *(word+i); i++)
			{
				a = (int)(*(word+i)); /* Save the ascii of the char in a */
				
				fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print a to the obb file */
				word_num++;
				a = 0;
			}
			
			fprintf(ob_file,"%04d %05o\n",word_num,a); /* Print the '\0' to the obb file */
			word_num++;
		}
	}
	
	free_table(head,labels_num); /* Free the labels table */
	fclose(file); /* Close the file */
	
	return;
}
