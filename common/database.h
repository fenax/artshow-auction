#ifndef DATABASE_H
#define DATABASE_H

#include "../common/config.h"
#include <sqlite3.h>
#include <string.h>

enum
{
   ITEM_ID_COLUMN,
   TITLE_COLUMN,
   AUTHOR_COLUMN,
   ITEM_PICTURE_COLUMN,
   ITEM_INFO_COLUMN,
   START_BID_COLUMN,
   N_COLUMNS
};

struct item{
	int id;
	char* title;
	char* author;
	char* picture;
	int picture_size;
	char* info;
	int start_bid;
};

void free_null(char** var){
	if(*var){
		free(*var);
		*var = NULL;
	}
}

void item_struct_free(struct item * item){
	free_null(&(item->title));
	free_null(&(item->author));
	free_null(&(item->picture));
	free_null(&(item->info));
	item->picture_size = 0;
	item->start_bid=0;
}

int save_item(struct item item){
	int rc;
	sqlite3 *database;
	sqlite3_stmt * statement;

	char query[] = "INSERT INTO items (title, author, info, start_bid, picture,id) VALUES (?,?,?,?,?,?);";
	char query2[] = "INSERT INTO items (title, author, info, start_bid, picture) VALUES (?,?,?,?,?);";


	rc = sqlite3_open(DB_FILENAME ,&database);
	if(rc != SQLITE_OK)
	{
		puts(sqlite3_errmsg(database));
		sqlite3_close(database);
		return -1;
	}
	if(item.id == -1){
		rc = sqlite3_prepare_v2(database,query2, sizeof(query2), &statement, NULL);
	}else{
		rc = sqlite3_prepare_v2(database,query, sizeof(query), &statement, NULL);
	}
	if(rc != SQLITE_OK) {
		puts ("sqlite prepare select not ok");
		puts (sqlite3_errmsg(database));
	        
		sqlite3_finalize(statement);
    		sqlite3_close(database);
		return -1;
	}

	if(item.id != -1){
		sqlite3_bind_int(statement, 6,item.id);
	}

	sqlite3_bind_text(statement, 1, item.title,-1,SQLITE_STATIC);
	sqlite3_bind_text(statement, 2, item.author,-1,SQLITE_STATIC);
	sqlite3_bind_text(statement, 3, item.info, -1,SQLITE_STATIC);
	sqlite3_bind_int(statement, 4, item.start_bid);
	sqlite3_bind_blob(statement, 5, item.picture,
		       item.picture_size,SQLITE_STATIC);
	
	rc = sqlite3_step(statement);


	if(rc != SQLITE_DONE){
	    puts(sqlite3_errmsg(database));
	}
	rc = sqlite3_last_insert_rowid(database);

	sqlite3_finalize(statement);
	sqlite3_close(database);
	return rc;

}

struct item get_item(int id){
	int rc;
	sqlite3 *database;
	sqlite3_stmt * statement;

	struct item ret;
	char query[] = "SELECT * FROM items WHERE id=?;";

	bzero(&ret,sizeof(ret));

	rc = sqlite3_open(DB_FILENAME ,&database);
	if(rc != SQLITE_OK)
	{
		puts(sqlite3_errmsg(database));
		sqlite3_close(database);
		return ret;
	}


	rc = sqlite3_prepare_v2(database,query, sizeof(query), &statement, NULL);
	if(rc != SQLITE_OK) {
		puts ("sqlite prepare select not ok");
		puts (sqlite3_errmsg(database));
		ret.id = 0;
	        
		sqlite3_finalize(statement);
    		sqlite3_close(database);
		return ret;
	}
	
	rc = sqlite3_bind_int(statement,1, id);
	if(rc != SQLITE_OK) {
		puts ("sqlite bind select not ok");
		puts (sqlite3_errmsg(database));
		ret.id = 0;
	        
		sqlite3_finalize(statement);
    		sqlite3_close(database);
		return ret;
	}

	rc = sqlite3_step(statement);
	if(rc==SQLITE_ROW){
		char * blob;

		GValue id = G_VALUE_INIT;
		GValue title = G_VALUE_INIT;
		GValue author = G_VALUE_INIT;
		GValue info = G_VALUE_INIT;
		GValue start_bid = G_VALUE_INIT;
		GValue picture = G_VALUE_INIT;

		ret.id =  sqlite3_column_int(statement,  0);
		ret.title = strdup(   sqlite3_column_text(statement, 1));
		ret.author = strdup(  sqlite3_column_text(statement, 2));
		ret.info = strdup(    sqlite3_column_text(statement, 3));
		ret.start_bid  = ( sqlite3_column_int(statement,  4));
		blob = sqlite3_column_blob(statement, 5);

		if(blob){
			int len = sqlite3_column_bytes(statement, 5);
			ret.picture = malloc (len);
			ret.picture_size=  len;
			memcpy(ret.picture, blob, len);
		}else{
			ret.picture = NULL;
			ret.picture_size = 0;
		}
		

	}
	sqlite3_finalize(statement);
	sqlite3_close(database);
	return ret;
}

int fill_items_store(GtkListStore * store){
	int rc;
	sqlite3 * database;
	sqlite3_stmt * statement;
	GtkTreeIter iter;

	rc = sqlite3_open(DB_FILENAME ,&database);
	if(rc != SQLITE_OK)
	{
		puts(sqlite3_errmsg(database));
		goto cleanup_1;
	}
	
	char create_table [] = "CREATE TABLE IF NOT EXISTS items (id INTEGER PRIMARY KEY ON CONFLICT REPLACE AUTOINCREMENT, title, author, info, start_bid, picture);";
	
	rc = sqlite3_prepare_v2(database, create_table,-1 /*sizeof(create_table)*/,&statement,NULL);
	if(rc != SQLITE_OK){
		puts("sqlite prepare not ok");
		puts(sqlite3_errmsg(database));
		goto cleanup_2;
	}

	rc = sqlite3_step(statement);
	if(rc != SQLITE_DONE){
	    puts("sqlite step not ok");
	    puts(sqlite3_errmsg(database));
	    goto cleanup_2;
	}
	sqlite3_finalize(statement);

	char query[] = "SELECT * FROM items;";
	rc = sqlite3_prepare_v2(database,query, sizeof(query), &statement, NULL);
	if(rc != SQLITE_OK) {
		puts ("sqlite prepare select not ok");
		puts (sqlite3_errmsg(database));
		goto cleanup_2;
	}

	rc = sqlite3_step(statement);
	while(rc==SQLITE_ROW){
		GValue id = G_VALUE_INIT;
		GValue title = G_VALUE_INIT;
		GValue author = G_VALUE_INIT;
		GValue info = G_VALUE_INIT;
		GValue start_bid = G_VALUE_INIT;
		GValue picture = G_VALUE_INIT;
		g_value_set_int   ( &id,      sqlite3_column_int(statement,  0));
		g_value_set_string( &title,   sqlite3_column_text(statement, 1));
		g_value_set_string( &author,  sqlite3_column_text(statement, 2));
		g_value_set_string( &info,    sqlite3_column_text(statement, 3));
		g_value_set_int   ( &start_bid,sqlite3_column_int(statement,  4));
		g_value_set_string( &picture, sqlite3_column_text(statement, 5));

		gtk_list_store_append(store, &iter);		
		gtk_list_store_set_value(store,&iter,ITEM_ID_COLUMN,   &id  );
		gtk_list_store_set_value(store,&iter,TITLE_COLUMN,     &title );
		gtk_list_store_set_value(store,&iter,AUTHOR_COLUMN,    &author );
		gtk_list_store_set_value(store,&iter,ITEM_INFO_COLUMN, &info );
		gtk_list_store_set_value(store,&iter,START_BID_COLUMN, &start_bid);
		gtk_list_store_set_value(store,&iter,ITEM_PICTURE_COLUMN,&picture);

		rc = sqlite3_step(statement);
	}

	if(rc != SQLITE_DONE){
	    puts(sqlite3_errmsg(database));
	    goto cleanup_2;
	}


    cleanup_2:
        sqlite3_finalize(statement);

    cleanup_1:
    	sqlite3_close(database);
		gtk_list_store_append(store, &iter);		
	
	printf("%X\n",store);
	
	return 0;
}
#endif //DATABASE_H
