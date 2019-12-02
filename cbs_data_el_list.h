
/* Data Element     NameData      TypeLength */
/* client_no        numeric                5 */
/* client_name      alphanumeric          25 */
/* address          alphanumeric          25 */
/* city             alphanumeric          25 */
/* state            alphanumeric           2 */
/* zip              numeric                5 */
/* phone            numeric               10 */
/* amt_due          currency               8 */
/* project_no       numeric                5 */
/* project_name     alphnumeric           25 */
/* amt_expended     currency               9 */
/* consultant_no    numeric                5 */
/* consultant_name  alphnumeric           25 */
/* rate             currency               5 */
/* payment          currency               9 */
/* expense          currency               9 */
/* hours            numeric                2 */
/* date_paid        date                   6 */

/*
 * Listing 6.1
 * Data Element Dictionary
 */

typedef enum elements	{
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
	 AMD_EXPENDED,
	 CONSULTANT_NO,
	 CONSULTANT_NAME,
	 RATE,
	 PAYMENT,
	 EXPENSE,
	 HOURS,
	 DATE_PAID
} ELEMENT;
