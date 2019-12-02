/*
 * Listing 6.4
 * Data Element Display Characteristics
 */
/*
  A = alphanumeric
  C = currency
  Z = numeric, zero-filled
  N = numeric, space-filled
  D = date
 */
const char eltype [] = "ZAAAANNCZACZACCCND";
const char *elmask [] =	{
	 "_____",
	 "_________________________",
	 "_________________________",
	 "_________________________",
	 "__",
	 "_____",
	 "(__)____-____",
	 "$_______.__",
	 "_____",
	 "_________________________",
	 "$_______.__",
	 "_____",
	 "_________________________",
	 "$___.__",
	 "$_______.__",
	 "$_______.__",
     "__",
	 "__/__/__",
} ELEMENT;
