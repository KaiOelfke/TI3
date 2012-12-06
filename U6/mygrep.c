#include <stdio.h>
#define BUFFER_SIZE (10)


int strlength(char *str)
{
	int length = 0;
	while (str[length] != '\0')
		length++;
	return length;
}


int main(int argc, char *argv[])
{
	if (argc != 3 && argc != 2)
	{
		perror("Programm muss mit mygrep file_name filter oder stdin filter aufgerufen werden.");
		return 1;
	}
	FILE* file = NULL;
	if (argc == 3)
		file = fopen(argv[2],"r");
	else
		file = stdin;
	FILE* tempFile = fopen(".tempgrepfile", "w+");
	FILE* outputFile = fopen(".tempgrepoutputfile", "w+");
	char *filter = argv[1];
	int filterLength = strlength(filter);
	if (file != NULL && tempFile != NULL)
	{
		char tbuffer[BUFFER_SIZE];
		char* buffer = tbuffer;
		char contains = 0;
		buffer = fgets(buffer, BUFFER_SIZE, file);
		while (buffer != NULL)
		{
			int bufferLength = strlength(buffer);
			bufferLength++;
			if (contains == 0)
			{
				int pos = 0;
				while (buffer[pos] != '\0' && pos + filterLength < bufferLength + 1)
				{
					contains = 1;
					for (int i = 0; i < filterLength; i++)
						if (buffer[pos+i] != filter[i])
						{
							contains = 0;
							break;
						}
					if (contains == 1)
						break;
					pos++;
				}
			}
			if (contains == 1)
			{
				char tempBuffer[BUFFER_SIZE];
				rewind(tempFile);
				while (fgets(tempBuffer, BUFFER_SIZE, tempFile) != NULL)
					fprintf(outputFile, "%s",tempBuffer);
				fprintf(outputFile,"%s", buffer);
				freopen(".tempgrepfile", "w+", tempFile);
				if (buffer[bufferLength-2] == '\n')
					contains = 0;
			}
			else
			{
				if (buffer[bufferLength-2] == '\n')
					freopen(".tempgrepfile", "w+", tempFile);
				else
					fputs(buffer, tempFile);
			}
			if (argc == 3)
				buffer = fgets(buffer, BUFFER_SIZE, file);
			else 
				buffer = fgets(buffer, BUFFER_SIZE, stdin);
		}
		fclose(tempFile);
		remove(".tempgrepfile");
		if (file) fclose(file);
		char outputBuffer[BUFFER_SIZE];
		rewind(outputFile);
		while (fgets(outputBuffer, BUFFER_SIZE, outputFile) != NULL)
			printf("%s",outputBuffer);
		int outputBufferLength = strlength(outputBuffer);
		if (outputBuffer[outputBufferLength - 1] != '\n')
			putchar('\n');
		fclose(outputFile);
		remove(".tempgrepoutputfile");
		return 0;
	}
	else
	{
		perror("Konnte Datei nicht lesen oder keine Swapdatei erstellen.");
		if (file) fclose(file);
		fclose(tempFile);
		remove(".tempgrepfile");
		fclose(outputFile);
		remove(".tempgrepoutputfile");
		return 1;
	}
}