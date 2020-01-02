/* -- pg 101 ------ cbs.h ----------------------- */

#define APPLICATION_H

typedef enum elements {
	CLIENT_NO=1,
	CLIENT_NAME,
	ADDRESS,
	CITY,
	STATE,
	ZIP,
	PHONE,
	AMT_DUE,
	PROJECT_NO,
	PROJECT_NAME,
	AMT_EXPENDED,
	CONSULTANT_NO,
	CONSULTANT_NAME,
	RATE,
	PAYMENT,
	EXPENSE,
	HOURS,
	DATE_PAID,
	TermElement = 32367
} ELEMENT;

typedef enum files {
	CLIENTS,
	PROJECTS,
	CONSULTANTS,
	ASSIGNMENTS,
	TermFile = 32367
} DBFILE;

struct clients {
	char client_no [6];
	char client_name [26];
	char address [26];
	char city [26];
	char state [3];
	char zip [6];
	char phone [11];
	char amt_due [9];
	char date_paid [7];
};

struct projects {
	char project_no [6];
	char project_name [26];
	char amt_expended [10];
	char client_no [6];
};

struct consultants {
	char consultant_no [6];
	char consultant_name [26];
};

struct assignments {
	char consultant_no [6];
	char project_no [6];
	char rate [6];
};

#include "cdata.h"
