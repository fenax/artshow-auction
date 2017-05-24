#include <gtk/gtk.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include "../common/config.h"
#include "../common/database.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


GtkWidget * view;
GtkWidget * win;

GtkWidget * new_btn;
GtkWidget * edit_btn;


GtkCellRenderer * renderer;
GtkTreeViewColumn * column;
GtkTreeSelection *selection;
GtkListStore * items_store;



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


void open_editor(int id){
	char tmp[256];
	char t[256];

	getcwd(t,sizeof(t));
	t[sizeof(t)-1]='\0';
	puts(t);
//	if(id == -1){
		snprintf(tmp, sizeof(tmp), "%s/edit.out",t);
//	}else{
//		snprintf(tmp, sizeof(tmp), "%s/edit.out %d",
//				t, id);
//	}
	tmp[sizeof(tmp)-1]='\0';
	snprintf(t, sizeof(t),"%d",id);
	printf("running %s \n",tmp);
	int pid = fork();
	if(pid){
		puts("parent");
		int status;
		waitpid (pid, &status, 0);
	}else{
		puts("child");
		execl(tmp,tmp,t,(char  *) NULL);
		perror("lol tout est cassé");
		_exit (EXIT_FAILURE);
	}
}

int add_new_item(GtkButton* btn, gpointer user_data){
	open_editor(-1);
}

int edit_item(GtkButton* btn, gpointer user_data){
	GtkTreeIter iter;
	GtkTreeModel *model;
	
	if (gtk_tree_selection_get_selected (selection, &model,&iter))
	{
		int item;
		
		gtk_tree_model_get (model, &iter, 
			ITEM_ID_COLUMN, &item, -1);
	open_editor(item);
		
	}

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

	
	new_btn =  gtk_button_new_with_label("New Item");
	edit_btn =  gtk_button_new_with_label("Edit Item");

	GtkWidget * box4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

	g_signal_connect (new_btn, "clicked", G_CALLBACK (add_new_item), NULL);
	g_signal_connect (edit_btn, "clicked", G_CALLBACK (edit_item), NULL);

	gtk_box_pack_start( GTK_BOX(box2), new_btn, FALSE,FALSE,0);
	gtk_box_pack_start( GTK_BOX(box2), edit_btn, FALSE,FALSE,0);

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
// |||btn        |btn        |        |||
// ||+-----------+-----------+--------+||
// |+----------------------------------+|
// +------------------------------------+
//
}


