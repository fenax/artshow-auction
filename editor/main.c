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

#include <gtk/gtk.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#define DB_FILENAME "auction.sqlite"

GtkWidget * view;
GtkWidget * win;

GtkWidget * author_box;
GtkWidget * title_box;
GtkTextBuffer * info_buffer;
GtkWidget * info_box;
GtkWidget * start_bid_box;
GtkWidget * picture;

GtkWidget * new_btn;
GtkWidget * commit_btn;
GtkWidget * load_picture_btn;
GtkCellRenderer * renderer;
GtkTreeViewColumn * column;
GtkTreeSelection *selection;
GtkListStore * items_store;

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

int compare_treeview_editor(){
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected (selection, &model,&iter))
	{
		gchar * title;
		gchar * author;
		gchar * info;
		int start_bid;
		int result;
		GValue val = G_VALUE_INIT;
		
		g_value_init (&val, G_TYPE_UINT);
		g_object_get_property(G_OBJECT(info_buffer),"text",&val);

		gtk_tree_model_get (model, &iter, 
			TITLE_COLUMN, &title,
		       	AUTHOR_COLUMN, &author,
		        ITEM_INFO_COLUMN, &info,
			START_BID_COLUMN, &start_bid,	-1);
	result = strcmp (gtk_entry_get_text(GTK_ENTRY(author_box)),author)==0 &&
		 strcmp (gtk_entry_get_text(GTK_ENTRY(title_box)),title)==0 &&
		 strcmp (g_value_get_string(&val), info)==0 &&
		 atoi(gtk_entry_get_text(GTK_ENTRY(start_bid_box))) == start_bid;
	g_free(title);
	g_free(author);
	g_free(info);
	return result;
//info_buffer;
//start_bid_box;
//GtkWidget * picture;


	}else return 0;
}

void tree_selection_changed_cb(GtkTreeSelection *selection, gpointer data){
	GtkTreeIter iter;
	GtkTreeModel *model;
	if (gtk_tree_selection_get_selected (selection, &model,&iter))
	{
		gchar * item;
		gtk_tree_model_get (model, &iter, 
			TITLE_COLUMN, &item, -1);
	}
}



int commit_line(GtkButton* btn, gpointer user_data){

}

int load_image(GtkButton* btn, gpointer user_data){

}

int add_new_item(GtkButton* btn, gpointer user_data){

}


int activate(GtkApplication* app,
	       gpointer user_data){
	items_store=
	       gtk_list_store_new (N_COLUMNS, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
			           G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

	printf("%X\n",items_store);
	fill_items_store(items_store);

	view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(items_store));
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("Title", renderer,
							 "text", TITLE_COLUMN,
							 NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW (view),column);

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (selection), "changed",
                  G_CALLBACK (tree_selection_changed_cb),
                  NULL);

	GtkWidget * box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);

	gtk_box_pack_start (GTK_BOX(box), GTK_WIDGET(view),TRUE, TRUE,0);

	GtkWidget * box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	gtk_box_pack_start (GTK_BOX(box), GTK_WIDGET(box2),FALSE,FALSE,0);

	GtkWidget * box3 = gtk_grid_new();

	title_box = gtk_entry_new();
	author_box = gtk_entry_new();
	start_bid_box = gtk_entry_new();

	gtk_grid_attach(GTK_GRID(box3), gtk_label_new("Title : "),0,0,1,1);
	gtk_grid_attach(GTK_GRID(box3), title_box, 1,0,1,1);
	gtk_grid_attach(GTK_GRID(box3), gtk_label_new("Author : "),0,1,1,1);
	gtk_grid_attach(GTK_GRID(box3), author_box, 1,1,1,1);
	gtk_grid_attach(GTK_GRID(box3), gtk_label_new("Starting Bid : "),0,2,1,1);
	gtk_grid_attach(GTK_GRID(box3), start_bid_box, 1,2,1,1);

	gtk_box_pack_start (GTK_BOX(box2), box3, FALSE,FALSE,0);

	picture = gtk_image_new_from_file("");//get the broken file image

	gtk_box_pack_start (GTK_BOX(box2), picture, TRUE,FALSE,0);

	new_btn =  gtk_button_new_with_label("New Item");
	commit_btn =  gtk_button_new_with_label("Commit");
	load_picture_btn = gtk_button_new_with_label("Load Picture");

	GtkWidget * box4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

	gtk_box_pack_start( GTK_BOX(box4), new_btn, FALSE,FALSE,0);
	g_signal_connect (new_btn, "clicked", G_CALLBACK (add_new_item), NULL);
	g_signal_connect (commit_btn, "clicked", G_CALLBACK (commit_line), NULL);
	g_signal_connect (load_picture_btn, "clicked", G_CALLBACK (load_image), NULL);

	gtk_box_pack_start( GTK_BOX(box4), commit_btn, FALSE,FALSE,0);
	gtk_box_pack_start( GTK_BOX(box4), load_picture_btn, FALSE,FALSE,0);
	gtk_box_pack_start( GTK_BOX(box2), box4, FALSE,FALSE,0);
	gtk_box_pack_start( GTK_BOX(box), gtk_label_new("Info :"),FALSE,FALSE,0);

	info_buffer = gtk_text_buffer_new(NULL);
	info_box = gtk_text_view_new_with_buffer(info_buffer);

	gtk_box_pack_start( GTK_BOX(box), info_box,TRUE,TRUE,0);




	win = gtk_application_window_new (app);
	  gtk_window_set_title (GTK_WINDOW (win), "Window");
 // gtk_window_set_default_size (GTK_WINDOW (win), 200, 200);

	gtk_container_add(GTK_CONTAINER(win),box);

	gtk_widget_show_all(GTK_WIDGET(win));

	

}

int main(int argc, char ** argv){
       
//	  gtk_init (&argc, &argv);
	GtkApplication * app;
	int status;
	app = gtk_application_new("info.loutre.auction", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
	status = g_application_run (G_APPLICATION (app), argc,argv);
	g_object_unref(app);



	
// +- window ---------------------------+
// |+- box ----------------------------+|
// ||+- view -------------------------+||
// |||                                |||
// ||+--------------------------------+||
// |+----------------------------------+|
// ||+- box2 ----+-----------+--------+||
// |||+- box3 --+|+ picture +|+- box4+|||
// ||||Title :  |||         |||commit||||
// ||||Author : |||         |||      ||||
// ||||Start Bid|||         |||      ||||
// |||+---------+|+---------+|+------+|||
// ||+-----------+-----------+--------+||
// |+----------------------------------+|
// ||Info                              ||
// |+----------------------------------+|
// ||infobox                           ||
// |+----------------------------------+|
// +------------------------------------+
//
}
