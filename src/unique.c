#include <stdio.h>
 
int check_user_login_status(char *path, char *user)
{
	log_info("Checking user status...\n");
	FILE *fp;
	char line[50];
	fp=fopen(path,"r");
	if(fp==NULL)
	{
		log_error("Status file not found!\n");
		log_error("Assert user is not logged in.\n");
		return 0;
	}
	while(!feof(fp))
	{
		fgets(line,1000,fp);
        printf("%s", line);
        if (strcmp(line, user))
			return 1;
	}
	fclose(fp);
	return 0;
}