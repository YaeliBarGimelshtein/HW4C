#ifndef __MYMACROS__
#define __MYMACROS__

#include <stdio.h>

#define CHECK_RETURN_0(a){\
	if((a)==NULL)\
		return 0;\
}

#define CHECK_RETURN_NULL(a){\
	if((a)==NULL)\
		return NULL;\
}

#define CHECK_MSG_RETURN_0(a, msg){\
	if((a)==NULL){\
		printf(#msg "\n");\
		return 0;\
	}\
}


#define CHECK_0_MSG_COLSE_FILE(a,str,fp){\
	if((a)==0){\
		printf(#str "\n");\
		fclose(fp);\
		return 0;\
	}\
}

#define CHECK_NULL__MSG_COLSE_FILE(a,str,fp){\
	if((a)==NULL){\
		printf(#str "\n");\
		fclose(fp);\
		return 0;\
	}\
}

#define MSG_CLOSE_RETURN_0(fp,str){\
	printf(#str "\n");\
	fclose(fp);\
	return 0;\
}



#endif