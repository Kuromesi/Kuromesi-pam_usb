#include <mysql/mysql.h>
#include <stdio.h>
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
        printf("Failed to query: %s\n", mysql_error(mysql));  
        return;
    } else {
        printf("\nQuery successfully!\n");  
    }
    res = mysql_store_result(mysql);
    return res;
} 

int main() 
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
    char *query = "SELECT * FROM user_status WHERE username=\"kuromesi\";";
    res = mysqldb_query(conn, query);
    char *username;
    char *ip;
    while (row = mysql_fetch_row(res)) {  
        username = row[0];
        ip = row[1];
        printf("username is %s", username);
        printf("\n");
    }  
    mysql_free_result(res);
    mysql_close(conn);
    printf("finish! \n");
    return 0;
}