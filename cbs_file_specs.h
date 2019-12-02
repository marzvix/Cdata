/*
 * Listing 6.5
 * File Specifications
 */

typedef enum files {
	CLIENTS,
	PROJECTS,
	CONSULTANTS,
	ASSIGNMENTS				
} DBFILE;

const char *dbfiles [] = {
	"CLIENTS",
	"PROJECTS",
	"CONSULTANTS",
	"ASSIGNMENTS",
    NULL    
};
