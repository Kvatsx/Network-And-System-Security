#ifdef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

int checkCurrentUser(char * filename);
void showAclList(char * filename);
void showFileDetails(char * filename);
void PrintUserDetails(void);
int checkFilePermissions(char * filename);
int getPermission(char * permission);
int AddAclEntry(char * filename);

#endif
