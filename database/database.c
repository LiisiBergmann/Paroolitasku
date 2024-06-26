#include "database.h"

void setKey(char *key){
    sqlite3 *db_ptr;
    char *errMesg = 0;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }

    char hex[SHA256_HEX_SIZE];
    sha256_hex(key, strlen(key), hex);

    char *sql_stmt = sqlite3_mprintf("INSERT INTO key (master) VALUES ('%q');", hex);
    if (sqlite3_exec(db_ptr, sql_stmt, 0, 0, &errMesg) != SQLITE_OK){
        printf("%s\n", errMesg);
        sqlite3_free(errMesg);
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
    }

    sqlite3_free(errMesg);
    sqlite3_close(db_ptr);
}

void getKey(char **key){
    sqlite3 *db_ptr;
    sqlite3_stmt *stmt;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }
/**LIMIT 1 tähendab seda, et siis oleme kindlad, et ta saab ainult ühe rea*/
    char *sql = "SELECT master FROM key LIMIT 1;";
    if (sqlite3_prepare_v2(db_ptr, sql, -1, &stmt, NULL) != SQLITE_OK){
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
        
    }
    if (sqlite3_step(stmt) != SQLITE_ROW){
        sqlite3_finalize(stmt);
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
    }
    /**Teeb võtmest koopia, et see oleks saadab ka väljaspool funkstiooni*/
    char *buf = (char*)sqlite3_column_text(stmt, 0);
    *key = malloc(strlen(buf) * sizeof(char));
    strcpy(*key, buf);

    sqlite3_finalize(stmt);
    sqlite3_close(db_ptr);
}

bool checkKey(char * password){
    char *key;
    getKey(&key);

    char hex[SHA256_HEX_SIZE];
    sha256_hex(password, strlen(password), hex);

    return strcmp(key, hex) == 0;
}

void save(Site site) {
    sqlite3 *db_ptr;
    char *errMesg = 0;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }

    char *key;
    getKey(&key);

    aes_ecb_encrypt((unsigned char *)site.showpas, (unsigned char *)key, 32);
   
    char *sql_stmt = sqlite3_mprintf("INSERT INTO sites (name, url, username, password) VALUES ('%q', '%q', '%q', '%q');", site.showname, site.showurl, site.showuser, site.showpas);
    if (sqlite3_exec(db_ptr, sql_stmt, 0, 0, &errMesg) != SQLITE_OK){
        printf("%s\n", errMesg);
        sqlite3_free(errMesg);
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
    }

    sqlite3_free(errMesg);
    sqlite3_close(db_ptr);   
}

void createTable() {
    sqlite3 *db_ptr;
    char *errMesg;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }

    char* sql_stmt = "CREATE TABLE IF NOT EXISTS sites (name TEXT, url TEXT, username TEXT, password TEXT)";
    if (sqlite3_exec(db_ptr, sql_stmt, 0, 0, &errMesg) != SQLITE_OK){
        printf("%s!\n", errMesg);
        sqlite3_free(errMesg);
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
    }

    sql_stmt = "CREATE TABLE IF NOT EXISTS key (master TEXT)";
    if (sqlite3_exec(db_ptr, sql_stmt, 0, 0, &errMesg) != SQLITE_OK){
        printf("%s!\n", errMesg);
        sqlite3_free(errMesg);
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
    }

    sqlite3_free(errMesg);
    sqlite3_close(db_ptr);
}

int getCountSitesAll() {
    sqlite3 *db_ptr;
    sqlite3_stmt *stmt;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }

    char *sql = "SELECT COUNT (*) FROM sites;";
    if (sqlite3_prepare_v2(db_ptr, sql, -1, &stmt, NULL) == SQLITE_OK){
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int rows = sqlite3_column_int(stmt, 0);

            sqlite3_finalize(stmt);
            sqlite3_close(db_ptr);

            return rows;
        }
        
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db_ptr);
    
    exit(EXIT_FAILURE);
}

Site *getSites(char *keyword, int *count) {
    sqlite3 *db_ptr;
    char *errMesg;
    int nRows, nCols;
    char **results;
    char *sql;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }
    if (keyword == NULL){
        sql = "SELECT rowid, * FROM sites;";
    }
    else{         /*otsib nagu mis oleks täpne vaste*/
        sql = sqlite3_mprintf("SELECT rowid, * FROM sites WHERE name LIKE '%%%q%%';", keyword);
    }
    
    
    if (sqlite3_get_table(db_ptr, sql, &results, &nRows, &nCols, &errMesg) != SQLITE_OK ){
        printf("%s!\n", errMesg);
        sqlite3_free(errMesg);
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
    }

    *count = nRows;

    /*Küsime mälu*/
    Site * sites = malloc(nRows * sizeof(Site));

    for (int i = 1; i <= nRows; i++)
    {
        /**result muutuja on array stringidest
         * esimesed neli elementi on päis
         * ja siis tulevad kirjed, nelja kaupa
        */
        
        sites[i - 1].id = atoi(results[i * nCols]);
        

        /**strncpy funktsioon kopeerib char pointer sisu char arraysse*/
        strcpy(sites[i - 1].showname, results[i * nCols + 1]); 
        strcpy(sites[i - 1].showurl, results[i * nCols + 2]);
        strcpy(sites[i - 1].showuser, results[i * nCols + 3]);
        strcpy(sites[i - 1].showpas, results[i * nCols + 4]);

        char *key;
        getKey(&key);
        aes_ecb_decrypt((unsigned char *)sites[i - 1].showpas, (unsigned char *)key, 32);
    }

    sqlite3_free(errMesg);
    sqlite3_free_table(results);
    sqlite3_close(db_ptr);

    return sites;
}

void deletes(int id){
   sqlite3 *db_ptr;
    char *errMesg = 0;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }

    char *sql_stmt = sqlite3_mprintf("DELETE FROM sites WHERE rowid = %d;", id);
    if (sqlite3_exec(db_ptr, sql_stmt, 0, 0, &errMesg) != SQLITE_OK){
        printf("%s\n", errMesg);
        sqlite3_free(errMesg);
        sqlite3_close(db_ptr);
        exit(EXIT_FAILURE);
    }

    sqlite3_free(errMesg);
    sqlite3_close(db_ptr);   
}

bool userExist(){
	printf("controlling\n");
    sqlite3 *db_ptr;
    sqlite3_stmt *stmt;

    if (sqlite3_open("database.db", &db_ptr) != SQLITE_OK){
        printf("Database opening error! \n");
        exit(EXIT_FAILURE);
    }

    char *sql = "SELECT COUNT (*) FROM key;";
    if (sqlite3_prepare_v2(db_ptr, sql, -1, &stmt, NULL) == SQLITE_OK){
        if (sqlite3_step(stmt) == SQLITE_ROW){
            int rows = sqlite3_column_int(stmt, 0);

            sqlite3_finalize(stmt);
            sqlite3_close(db_ptr);

            if (rows == 0){
                return false;
            }
            return true;
            
        }
        
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db_ptr);
    
    printf("close\n");
    exit(EXIT_FAILURE);
    
}

void test(){
    
    createTable();

    setKey("voti");

    printf("%d\n", checkKey("voti"));

    Site test = {
        0,
        "Facebook",
        "https://www.youtube.com/",
        "maria.mets",
        "maria1234"
    };

    save(test);
    
    int sitecount;
                      /*NULL ära otsi, või string"" mille järgi ot"*/
    Site *sites = getSites(NULL, &sitecount);
    for (int i = 0; i < sitecount; i++){
        printf("%d, %s, %s, %s, %s \n", sites[i].id, sites[i].showname, sites[i].showurl, sites[i].showuser, sites[i].showpas);
    }

    deletes(10);
    
}
