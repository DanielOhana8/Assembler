#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "help.h"

/* The function gets string and returns the first word of the string */
char* next_word(char *str)
{
	int i;
	char s[max_line_len];
	char *word;

	for (i=0; !(isspace(*(str+i)) || *(str+i) == ','); i++) /* Loop until the char is white char or comma */
		*(s+i) = *(str+i); /* Save the char in s */

	*(s+i) = '\0'; /* End of string */

	word = s;

	return word;
}

/* The function gets string and remove the white chars in the beginning */
void rem_whites(char *s)
{
	char *str = s;
  
	while (isspace(*s)) 
		s++; /* Skip the white char */
		
	while (*s) 
		*str++ = *s++; /* Copy the string without white chars in the beginning */

	*str = '\0'; 
}

/* The function gets string and remove the first word of the string */
void rem_word(char *s)
{
	char *str = s;
	
	while (!(isspace(*s) || *s == ',')) /* While s dont start with white char or comma */
		s++; /* Remove the char */
			
	while (*s) 
		*str++ = *s++; /* Copy the string without white chars in the beginning */

	*str = '\0'; 
}

/* The function gets string and checks if the string is a number */
int is_num(char* s) 
{
	int i = 0;
	
	if (*s == '-' || *s == '+') /* If s starts with - or + */
		i++; /* skip the - or + */

	for (; *(s+i); i++) 
	{
		if (!isdigit(*(s+i))) /* Is the char is not digit */
			return 0;
	}
	
	return 1;
}

/* The function gets string and checks if the string is a command */
int is_cmd(char *s)
{
	int i;
	char* cmd_list[] = {"mov","cmp","add","sub","lea","clr","not","inc","dec","jmp","bne","red","prn","jsr","rts","stop"};
	
	for (i=0; i < cmd_num-4; i++)
	{
		if (!strcmp(s,cmd_list[i])) /* If the string is a command */
			return 1;
	}
	
	return 0;
}

/* The function gets string and returns the number of the method of the string */
int method(char *s)
{
	if (*s == '#' && is_num(s+1)) /* If s is # and then numbers */
		return 0;

	if (*s == '*' && *(s+1) == 'r' && isdigit(*(s+2)) && atoi(s+2) >= 0 && atoi(s+2) <= 7 && *(s+3) == '\0') /* If s is * and then r and then number between 0 and 7 */
		return 2;

	if (*s == 'r' && isdigit(*(s+1)) && atoi(s+1) >= 0 && atoi(s+1) <= 7 && *(s+2) == '\0') /* If s is r and then number between 0 and 7 */
		return 3;

	return 1;
}
