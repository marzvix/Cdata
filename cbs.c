/* -- pg 103 -------- cbs.c ------------------ */

#include "cbs.h"

const char *denames [] = {
	"CLIENT_NO",
	"CLIENT_NAME",
	"ADDRESS",
	"CITY",
	"STATE",
	"ZIP",
	"PHONE",
	"AMT_DUE",
	"PROJECT_NO",
	"PROJECT_NAME",
	"AMT_EXPENDED",
	"CONSULTANT_NO",
	"CONSULTANT_NAME",
	"RATE",
	"PAYMENT",
	"EXPENSE",
	"HOURS",
	"DATE_PAID",
	NULL
};

const char eltype [] = "ZAAAANNCZACZACCCND";

const char *elmask [] = {
	"_____",
	"_________________________",
	"_________________________",
	"_________________________",
	"__",
	"_____",
	"(___)___-____",
	"$______.__",
	"_____",
	"_________________________",
	"$_______.__",
	"_____",
	"_________________________",
	"$___.__",
	"$_______.__",
	"$_______.__",
	"__",
	"__/__/__"
};

const char *dbfiles [] = {
	"CLIENTS",
	"PROJECTS",
	"CONSULTANTS",
	"ASSIGNMENTS",
	NULL
};

const int ellen [] = {
	5,25,25,25,2,5,10,8,5,25,9,5,25,5,9,9,2,6
};


const ELEMENT f_clients [] = {
	CLIENT_NO,
	CLIENT_NAME,
	ADDRESS,
	CITY,
	STATE,
	ZIP,
	PHONE,
	AMT_DUE,
	DATE_PAID,
	0
};

const ELEMENT f_projects [] = {
	PROJECT_NO,
	PROJECT_NAME,
	AMT_EXPENDED,
	CLIENT_NO,
	0
};

const ELEMENT f_consultants [] = {
	CONSULTANT_NO,
	CONSULTANT_NAME,
	0
};

const ELEMENT f_assignments [] = {
	CONSULTANT_NO,
	PROJECT_NO,
	RATE,
	0
};

const ELEMENT *file_ele [] = {
	f_clients,
	f_projects,
	f_consultants,
	f_assignments,
	0
};

const ELEMENT x1_clients [] = {
	CLIENT_NO,
	0
};

const ELEMENT *x_clients [] = {
	x1_clients,
	NULL
};

const ELEMENT x1_projects [] = {
	PROJECT_NO,
	0
};

const ELEMENT *x_projects [] = {
	x1_projects,
	NULL
};

const ELEMENT x1_consultants [] = {
	CONSULTANT_NO,
	0
};

const ELEMENT *x_consultants [] = {
	x1_consultants,
	NULL
};

const ELEMENT x1_assignments [] = {
	CONSULTANT_NO,
	PROJECT_NO,
	0
};

const ELEMENT x2_assignments [] = {
	CONSULTANT_NO,
	0
};

const ELEMENT x3_assignments [] = {
	PROJECT_NO,
	0
};

const ELEMENT *x_assignments [] = {
	x1_assignments,
	x2_assignments,
	x3_assignments,
	NULL
};


const ELEMENT **index_ele  [] = {
	x_clients,
	x_projects,
	x_consultants,
	x_assignments,
	NULL
};


#ifdef NULL_IS_DEFINED
	#undef NULL
	#undef NULL_IS_DEFINED
#endif

