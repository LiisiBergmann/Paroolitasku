#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sqlite3.h>
#include <stdbool.h>

#include "../lib/sha256.h"
#include "../lib/aes128.h"

#define FIELD_LEN 256

/**Lühem ja ilusam, selleks kasutan
 * typedefi
*/
typedef struct Site {
    int id;
    char showname[FIELD_LEN];
    char showurl[FIELD_LEN];
    char showuser[FIELD_LEN]; 
    char showpas[FIELD_LEN];
} Site;

void setKey(char *key);
void getKey(char **key);
bool checkKey(char * password);
void save(Site site);
void createTable();
int getCountSitesAll();
Site *getSites(char *keyword, int *count);
void deletes(int id);
bool userExist();
void test();
