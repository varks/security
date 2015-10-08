/* Varun Kulkarni Somashekhar varun@umail.ucsb.edu */
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
char output_file[1024] = "optr.soltn";
FILE *fp_sol = NULL;


int extract_alphas(char **token) {
	
	char ch;
	int i = 0, k = 0;
	if(!*token)
		return 0;
	int len = strlen(*token);
	char *tmp = *token;
	char *word = (char*)malloc(len + 1);
	while(tmp[i] != '\0') {
		if(isalpha(tmp[i])) {
			word[k] = tmp[i];
			k++;
		}
		i++;
	}
	word[k] = 0;
	if(k)
		fprintf(fp_sol,"%s\n", word);
	free(word);
	return 0;
}

int extract_words(char **fname) {
	
	char *line = NULL;
	int len = 0;
	const char s[2] = " ";
	char *token = NULL;
	int read = 0;
	FILE *fp = fopen(*fname, "r");

	if (fp == NULL) {
		perror("Error opening file to extract words\n");
		return 1;
	}

	while((read = getline(&line, &len, fp)) != -1) {
		token = strtok(line, s);
		if (extract_alphas(&token)) {
			perror("Error extracting only alphas from tokens\n");
			return 1;
		}	
		while( token != NULL ) 
   		{
      		token = strtok(NULL, s);
			if (extract_alphas(&token)) {
				perror("Error extracting only alphas from tokens\n");
				return 1;
			}	
   		}		
	}
	if (line)
		free(line);

	fclose(fp);
	return 0;
}

void usage() {

	printf("Usage: dictobuild -d <dir> -o <output_file\n");
	exit(0);
}

int main(int argc, char **argv) {

	int i = 0;
	char ch;
	char cmd[1024];
	char fname[1024];
	char *dir = ".";
	FILE *fp = NULL;
	char *line = NULL;
	int len = 0;
	int read = 0;
	char *envr = NULL;
	char *envr_token = NULL;
	
	/*getopt */
	while ((ch = getopt(argc, argv, "d:o:")) != -1) {
		switch(ch) {
		case 'd':
				dir = optarg;	
				break;
		case 'o':
				break;
		default:
				usage();

		}
	} 

	fp_sol = fopen(output_file, "a+");
	if (fp_sol == NULL) {
		perror("Error opening output file\n");
		return 1;
	}	
	/* loop thru extensions and call find , append to same file */
	envr = getenv("DICTOEXT");	
	if(!envr) {
		perror("getenv failed!!!\n");
		envr = (char *)malloc(8);
		/* Default */
		strcpy(envr,"txt:md");
	}
	envr_token = strtok(envr,":");
	if(envr_token) {
		sprintf(cmd, "find %s -name '*.%s' >> temp.list", dir,envr_token);
		printf("the cmd is %s\n", cmd); 
		system(cmd);
	}
	while(envr_token != NULL) {
		envr_token = strtok(NULL, ":");
		sprintf(cmd, "find %s -name '*.%s' >> temp.list", dir,envr_token);
		printf("the cmd is %s\n", cmd); 
		system(cmd);
	}
		
	fp = fopen("temp.list", "r");
	if (fp == NULL) {
		perror("Error opening list of files\n");
		return 1;
	}	

	while((read = getline(&line, &len, fp)) != -1) {
		line[read - 1] = 0;
		printf("%s\n", line);
		/* open each file and parse line into words */
		extract_words(&line);
	}

	if(line)
		free(line);

	/* clean up */
	fclose(fp);
	fclose(fp_sol);
	free(envr);
	remove("temp.list");
	return 0;

}
