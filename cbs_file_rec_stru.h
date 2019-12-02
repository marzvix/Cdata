/*
 * Listing 6.7
 * File Record Structures
 */

struct clients {
    char client_no          [6];
    char client_name       [26];
    char address           [26];
    char city              [26];
    char state              [3];
    char zip                [6];
    char phone             [11];
    char amt_due            [9];
    char project_no         [6];
    char project_name      [26];
    char amt_expended      [10];
    char consultant_no      [6];
    char consultant_name   [26];
    char rate               [6];
    char payment           [10];
    char expense           [10];
    char hours              [3];
    char date_paid          [7];
};

struct projects {
    char project_no         [6];
    char project_name      [26];
    char amt_expended      [10];
	char client_no          [6];
};

struct consultants {
    char consultant_no      [6];
    char consultant_name   [26];
};

struct assignments {
    char consultant_no      [6];
	char project_no         [6];
    char rate               [6];
};
