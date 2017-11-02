#include <string.h>
#include "strfuncs.h"

char * substr(char * str, char delimiter)
{
	int i = 0;
	while (str[i] && str[i] != delimiter)
	{
		i++;
	}
	char * sub = (char *) malloc(sizeof(char) * (i + 1));
	memcpy(sub, str, i * sizeof(char));
	sub[i] = '\0';
	return sub;
}

char ** strsplit(char * str, char delimiter, int * w_count)
{
	char ** dest;
	int word_count = 0;
	int i = 0;
	while (str[i])
	{
		if (str[i] == delimiter)
		{
			while (str[i] == delimiter)
			{
				i++;
			}
			word_count++;
		}
		i++;
	}

	dest = (char**) malloc(sizeof(char *) * (word_count + 1));
	word_count = 0;
	int word_start, c_count;
	i = 0;
	while (str[i] && str[i] != '\r' && str[i] != '\n')
	{
		c_count = 0;
		word_start = i;
		while (str[i] != delimiter && str[i] != '\r' && str[i] != '\n')
		{
			c_count++;
			i++;
		}
		char * segment = (char *) malloc(sizeof(char) * (c_count + 1));
		memcpy(segment, str + word_start, (c_count) * sizeof(char));
		segment[c_count] = '\0';
		dest[word_count] = segment;
		word_count++;
		while (str[++i] == delimiter);
	}
	*w_count = word_count;
	return dest;
}

int strtodouble(char * str, double * d, char * e)
{
	*d = strtod(str, &e);
	if (*e != '\0')
	{
		int i = 0;
		while (str[i])
		{
			i++;
		}
		return 0;
	}
	return 1;
}