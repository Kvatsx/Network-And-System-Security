#ifdef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

int checkCurrentUser(char * filename, int type);
void showAclList(char * filename);
void showFileDetails(char * filename, char * actual);
void PrintUserDetails(void);
int checkFilePermissions(char * filename);
int getPermission(char * permission);
int AddAclEntry(char * filename);
int checkPath(char * filename);
char * ActualPath(char * filename);
// char * SlashParser(char * arr);

#endif
