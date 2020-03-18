#define DB_PATH "../data/db/IYF-db.sqlite"

//#include "convert.h"
#include "SqliteModel.h"

SqliteModel::SqliteModel(char* filename)
{
    int rc;
    char* sql;

    // Create database connection
    rc = sqlite3_open(filename, &db);
    if( rc )
    {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
    }
	
    /*
    // Create database read-only connection
    rc = sqlite3_open_v2(filename, &dbro, SQLITE_OPEN_READONLY, NULL);
    if( rc )
    {
      fprintf(stderr, "Can't open read-only database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(dbro);
    }
    */
	
    // Prepare the get user id sql statement
    sql = "select UserID from user where username = ? and password = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &get_uid_stmt, 0);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));

    }
    
    sql = "select facemodelxml from user where userid = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &get_xml_with_uid_stmt, 0);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));

    }

    sql = "select userid from user where username= ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &get_uid_with_uname_stmt, 0);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));

    }
    // Prepare the set face model sql statement
    sql = "update user set facemodelxml=? where userid=?";
    rc = sqlite3_prepare_v2(db, sql, -1, &set_facemod_stmt, 0);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));

    }

    // Prepare the insert user sql statement
    sql = "insert into user (username, password) values (?, ?)";
    rc = sqlite3_prepare_v2(db, sql, -1, &insert_user_stmt, 0);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));

    }
}

SqliteModel::~SqliteModel()
{
    sqlite3_finalize(get_uid_stmt);
    sqlite3_finalize(get_xml_with_uid_stmt);
    sqlite3_finalize(get_uid_with_uname_stmt);
    sqlite3_finalize(insert_user_stmt);
    sqlite3_finalize(set_facemod_stmt);
    sqlite3_close(db);
    sqlite3_close(db);
}

/*
    authUser verifies the username and password of the user in the database. If they match
    it returns the UserID for that user, otherwise it returns 0.
*/
int SqliteModel::authUser(char* username, char* password)
{
    int rc, UserID;

    // Bind params to the sql statement
    rc = sqlite3_bind_text(get_uid_stmt, 1, username, -1, SQLITE_STATIC);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      return 0;
    }

    rc = sqlite3_bind_text(get_uid_stmt, 2, password, -1, SQLITE_STATIC);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      return 0;
    }

    // If a row was found, get the UserID
    if (sqlite3_step(get_uid_stmt) == SQLITE_ROW)
        UserID = sqlite3_column_int(get_uid_stmt, 0);
    // Otherwise set the UserID to 0
    else
        UserID = 0;

    // Reset the statement for next time
    sqlite3_reset(get_uid_stmt);

    return UserID;
}

/*
    insertUser takes a new user's username and password.
    It returns their UserID or 0 if the insert failed.
*/
int SqliteModel::insertUser(char* username, char* password)
{
    int rc;

    // Bind params to the sql statement
    rc = sqlite3_bind_text(insert_user_stmt, 1, username, -1, SQLITE_STATIC);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      return 0;
    }
    rc = sqlite3_bind_text(insert_user_stmt, 2, password, -1, SQLITE_STATIC);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      return 0;
    }

    // Query db
    rc = sqlite3_step(insert_user_stmt);
    if(rc != SQLITE_DONE)
    {
      fprintf(stderr, "Database error code: %d,  %s\n", rc, sqlite3_errmsg(db));
      //sqlite3_close(db);

      // Insert failed, return 0
      return 0;
    }

    // Reset the statement for next time
    sqlite3_reset(insert_user_stmt);

    // Return the UserID
    return authUser(username, password);
}


/*
    getFaceModelXML returns the face model xml string for a user id. If none is found, returns NULL.
*/
char* SqliteModel::getFaceModelXML(int userID)
{
    int rc, nRow, nColumn;
    char* result;

    // Make the sql statement
    //const char* sql = (std::string("select facemodelxml from user where userid=") + stringify(userID)).c_str();

    // Query db
    rc = sqlite3_bind_int(get_xml_with_uid_stmt, 1, userID);
    //rc = sqlite3_get_table(db, sql, &result, &nRow, &nColumn, NULL);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      //sqlite3_close(db);
      return NULL;
    }

    if (sqlite3_step(get_xml_with_uid_stmt) == SQLITE_ROW)
    {
        char* temp = (char*)sqlite3_column_text(get_xml_with_uid_stmt, 0);
        result = new char[strlen(temp)];
        strcpy(result, temp);
    }

    sqlite3_reset(get_xml_with_uid_stmt);

    // If no results, return NULL
    if (nRow < 1)
        return NULL;

    // Otherwise, return the result
    return result;
}


/*
    setFaceModelXML updates the given user's face model xml in the database.
    It returns true for success or false for failure.
*/
bool SqliteModel::setFaceModelXML(int userID, char* faceModelXML)
{
    int rc;

    // Bind params to the sql statement
    rc = sqlite3_bind_text(set_facemod_stmt, 1, faceModelXML, -1, SQLITE_STATIC);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      return 0;
    }
    rc = sqlite3_bind_int(set_facemod_stmt, 2, userID);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      return 0;
    }

    // Query db
    rc = sqlite3_step(set_facemod_stmt);
    if(rc != SQLITE_DONE)
    {
      fprintf(stderr, "Database error code: %d,  %s\n", rc, sqlite3_errmsg(db));
      sqlite3_close(db);

      // Insert failed, return 0
      return false;
    }

    // Reset the statement for next time
    sqlite3_reset(set_facemod_stmt);

    return true;
}


int SqliteModel::getIdForUser(char* username)
{
    int rc, nRow, nColumn;
    int result;

    // Make the sql statement
    //const char* sql = (std::string("select userid from user where username='") + std::string(username) + std::string("'")).c_str();

    // Query db
    //rc = sqlite3_get_table(db, sql, &result, &nRow, &nColumn, NULL);
    rc = sqlite3_bind_text(get_uid_with_uname_stmt, 1, username, -1, SQLITE_STATIC);
    if( rc )
    {
      fprintf(stderr, "Database error: %s\n", sqlite3_errmsg(db));
      return NULL;
    }

    if (sqlite3_step(get_uid_with_uname_stmt) == SQLITE_ROW)
        result = sqlite3_column_int(get_uid_with_uname_stmt, 0);

    sqlite3_reset(get_uid_with_uname_stmt);
    // If no results, return NULL
    if (nRow < 1)
        return NULL;

    // Otherwise, return the result
    return result;
}

/*extern "C" MY_EXPORT Model* getModel()
{
    return new SqliteModel(DB_PATH);
}*/

