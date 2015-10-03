/*
 * simple-ls.c
 * Extremely low-power ls clone.
 * ./simple-ls .
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

void quickSort(char *v[], int lest, int right);
void swap(char *v[], int i, int j);
void regularPrint(DIR *dp);

void 
swap(char **v, int i, int j) {
	char *temp;

	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}

void
quickSort(char *v[], int left, int right) {
	int i, last;
	void swap(char *v[], int i, int j);
	if (left >= right) /* do nothing if array contains */
		return;
	/* fewer than two elements */
	swap(v, left, (left + right)/2);
	last = left;
	for (i = left+1; i <= right; i++)
		if (strcmp(v[i], v[left]) < 0)
			swap(v, ++last, i);
	swap(v, left, last);
	quickSort(v, left, last-1);
	quickSort(v, last+1, right);
}

void regularPrint(DIR *dp) {
	struct dirent *dirp;
	char *temp[1024];
	char *p;
	int i, len=0;

	while ((dirp = readdir(dp)) != NULL) {
		p = malloc(strlen(dirp->d_name)+1);
		strcpy(p,dirp->d_name);
		temp[len++]=p;
	}

	quickSort(temp,0,len-1);
	for(i=0;i<len;i++) {
		printf("%s%s",temp[i], (i<len-1) ? " " : "\n" );
	}
}

int
main(int argc, char **argv) {

	DIR *dp;
	struct stat sb;
	bool flag=false;
	char cwd[1024];
	
	if(getcwd(cwd, sizeof(cwd)) == NULL) {
		fprintf(stderr, "can't get current working directory: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(argc == 1) {
		if ((dp = opendir(".")) == NULL) {
			fprintf(stderr, "can't open '.'\n");
			exit(EXIT_FAILURE);
		}

		regularPrint(dp);		
		closedir(dp);
	}

	argc--;
	argv++;

	if(argc > 1) {
		flag = true;
	}

	while (argc > 0) {
		if(flag) {
			printf("%s:\n", argv[0]);
		}

		if(stat(argv[0], &sb) != -1) {

			if(!S_ISDIR(sb.st_mode)) {
				printf("%s ", argv[0]);
				printf("\n");
			}

			else {
				if ((dp = opendir(argv[0])) == NULL) {
					fprintf(stderr, "can't open '%s'\n", argv[0]);
					exit(EXIT_FAILURE);
				}

				if (chdir(argv[0]) == -1) {
					fprintf(stderr, "can't chdir to '%s': %s\n", argv[0], strerror(errno));
					exit(EXIT_FAILURE);
				}

				regularPrint(dp);
				closedir(dp);
			}
		}
		else {
			fprintf(stderr, "Can't stat %s: %s\n", argv[0], strerror(errno));
		}
		argc--;
		argv++;
		if (chdir(cwd) == -1) {
				fprintf(stderr, "can't chdir to '%s': %s\n", cwd, strerror(errno));
				exit(EXIT_FAILURE);
			}
		printf("%s", (argc > 0) ? "\n" : "");
	}
	return(EXIT_SUCCESS);
}
