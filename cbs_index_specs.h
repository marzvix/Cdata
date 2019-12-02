/*
 * Listing 6.8
 * Index Specifications
 */

const ELEMENT x1_clients [] = {
    CLIENT_NO,
	0,
};

const ELEMENT *x_clients [] = {
	x1_clients,
	NULL
};

const ELEMENT x1_projects [] = {
    PROJECT_NO,
	0,
};

const ELEMENT *x_projects [] = {
	x1_projects,
	NULL
};

const ELEMENT x1_consultants [] = {
    PROJECT_NO,
	0,
};

const ELEMENT *x_consultants [] = {
	x1_consultants,
	NULL
};

const ELEMENT x1_assignments [] = {
    CONSULTANT_NO,
    PROJECT_NO,
	0,
};

const ELEMENT x2_assignments [] = {
    CONSULTANT_NO,
	0,
};

const ELEMENT x3_assignments [] = {
    PROJECT_NO,
	0,
};

const ELEMENT *x_assignments [] = {
	x1_assignments,
	x2_assignments,
	x3_assignments,
	NULL
};

const ELEMENT **index_ele [] = {
	x1_clients,
	x2_projects,
	x3_assignments,
	NULL
};


