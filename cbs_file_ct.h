/*
 * Listing 6.6
 * File Contents
 */

const ELEMENT f_clients [] = {
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

								  
