#include <stdio.h>
#include "unique.h"
#include <mysql/mysql.h>
#include "log.h"
#include <stdlib.h>

MYSQL_RES *mysqldb_query(MYSQL *mysql, char *query)
{
    int t;
    char *head = "SELECT * FROM ";
    //char query[50] = {0};
    MYSQL_RES *res;
    MYSQL_ROW row;


    //sprintf(query, "%s%s", head, TABLE_NAME);
    t = mysql_real_query(mysql, query, strlen(query));

    if (t) {
        //log.error("Failed to query: %s\n", mysql_error(mysql));
        return;
    } //else {
        //printf("\nQuery successfully!\n");
    //}
    res = mysql_store_result(mysql);
    return res;
}

int check_user_login_status(char *pass_user)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char server[] = "192.168.135.145";
    char user[] = "root";
    char password[] = "test207";
    char database[] = "wazuh";

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server,user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
        char query[50] = {0};
        // char *query = "SELECT * FROM user_status WHERE username=\"%s\";";
        sprintf(query, "SELECT * FROM user_status WHERE username=\"%s\";", pass_user);
        res = mysqldb_query(conn, query);
    char *username;
	char *ip;
    if (res == NULL) {
        log_error("Query database failed, assert user is not logged in.\n");
        mysql_free_result(res);
        mysql_close(conn);
        return 0;
    }
    while (row = mysql_fetch_row(res)) {
        username = row[0];ip = row[1];
        mysql_free_result(res);
        mysql_close(conn);
        log_error("User %s has logged in at %s.\n", pass_user, ip);
        return 1;
    }
    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}

int test(char *path, char *user)
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

