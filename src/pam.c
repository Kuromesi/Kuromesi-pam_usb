/*
 * Copyright (c) 2003-2007 Andrea Luzzardi <scox@sig11.org>
 *
 * This file is part of the pam_usb project. pam_usb is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * pam_usb is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define PAM_SM_AUTH
#include <security/pam_modules.h>
#include <security/_pam_macros.h>

#include "version.h"
#include "conf.h"
#include "log.h"
#include "local.h"
#include "device.h"
#include "unique.h"

#include <stdio.h>
#include <mysql/mysql.h>
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

PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh, int flags,
		int argc, const char **argv)
{
	t_pusb_options	opts;
	const char		*service;
	const char		*user;
	const char		*tty;
	char			*conf_file = PUSB_CONF_FILE;
	int				retval;

	pusb_log_init(&opts);
	retval = pam_get_item(pamh, PAM_SERVICE,
			(const void **)(const void *)&service);
	if (retval != PAM_SUCCESS)
	{
		log_error("Unable to retrieve the PAM service name.\n");
		return (PAM_AUTH_ERR);
	}

	if (pam_get_user(pamh, &user, NULL) != PAM_SUCCESS || !user || !*user)
	{
		log_error("Unable to retrieve the PAM user name.\n");
		return (PAM_AUTH_ERR);
	}

	if (check_user_login_status(user))
	{
		return (PAM_AUTH_ERR);
	}

	if (argc > 1)
		if (!strcmp(argv[0], "-c"))
			conf_file = (char *)argv[1];
	if (!pusb_conf_init(&opts))
		return (PAM_AUTH_ERR);
	
	int flag = pusb_conf_parse(conf_file, &opts, user, service);
	if (flag == 0)
		return (PAM_AUTH_ERR);
	if (flag == 2)
	{
		log_info("User \"%s\": Use password authentication.\n", user);
                return (PAM_AUTH_ERR);		
	}

	if (!opts.enable)
	{
		log_debug("Not enabled, exiting...\n");
		return (PAM_IGNORE);
	}

	log_info("pam_usb v%s\n", PUSB_VERSION);
	log_info("Authentication request for user \"%s\" (%s)\n",
			user, service);

	if (pam_get_item(pamh, PAM_TTY,
				(const void **)(const void *)&tty) == PAM_SUCCESS)
	{
		if (tty && !strcmp(tty, "ssh"))
		{
			log_debug("SSH Authentication, aborting.\n");
			return (PAM_AUTH_ERR);
		}
	}
	if (!pusb_local_login(&opts, user))
	{
		log_error("User \"%s\": Access denied.\n", user);
		return (PAM_AUTH_ERR);
	}
	if (pusb_device_check(&opts, user))
	{	
		log_info("User \"%s\": Access granted.\n", user);
		if (flag == 3) 
			return (PAM_SUCCESS);
		log_error("User \"%s\": Password is still needed.\n", user);
		return (PAM_AUTH_ERR);
	}
	log_error("User \"%s\": Access denied.\n", user);
	return (PAM_AUTH_ERR);
}

PAM_EXTERN
int pam_sm_setcred(pam_handle_t *pamh,int flags,int argc,
		const char **argv)
{
	return (PAM_SUCCESS);
}

#ifdef PAM_STATIC

struct pam_module _pam_usb_modstruct = {
	"pam_usb",
	pam_sm_authenticate,
	pam_sm_setcred,
	NULL,
	NULL,
	NULL,
	NULL
};

#endif
