export int len(adr str);
export int str_comp(adr str1, adr str2);
export int str_concat(adr str1, adr str2, adr pointer);
export int str_toInt(adr input);
export int str_split(adr str, char sep, adr buff1, adr buff2);
export int str_find(adr in, char find);
export int str_subString(int start, int end, adr str, adr buff);
export int int_toString(int input, adr buffer);
export char ascii(int input);
export float charToFloat(char a);
export int str_toFloat(adr input);
export int sPrintFloat(adr input);
export int float_toString(adr input, adr obuff, int signed);
export adr str_reverse(adr in, adr buff);
export char str_at(adr str, int i);
export int str_forEach(adr s, adr foo, adr args);
export bool char_isDigit(char c);