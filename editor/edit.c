
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include "../common/config.h"
#include "../common/database.h"

GtkWidget * win;

GtkWidget * author_box;
GtkWidget * title_box;
GtkTextBuffer * info_buffer;
GtkWidget * info_box;
GtkWidget * start_bid_box;
GtkWidget * picture;
GtkWidget * id_label;
int id;

void * picture_buffer;
size_t picture_buffer_size;


GtkWidget * commit_btn;
GtkWidget * load_picture_btn;


int commit_line(GtkButton* btn, gpointer user_data){
	struct item item;
	char tmp[16];
	GtkTextIter begin;
	GtkTextIter end;
	item.title  = gtk_entry_get_text(GTK_ENTRY(title_box));
	item.author = gtk_entry_get_text(GTK_ENTRY(author_box));
	item.picture = picture_buffer;
	item.picture_size = picture_buffer_size;
	gtk_text_buffer_get_bounds(info_buffer,&begin,&end);
	item.info = gtk_text_iter_get_text(&begin,&end);
	item.id = id;
	item.start_bid = gtk_spin_button_get_value(GTK_SPIN_BUTTON(start_bid_box));
	id = save_item(item);
	snprintf(tmp, sizeof(tmp),"%d",id);
	gtk_label_set_text(GTK_LABEL(id_label),tmp);
}

int load_image(GtkButton* btn, gpointer user_data){

}


int activate(GtkApplication* app,
	       char* arg){
	struct item item;
	
  	GdkPixbufLoader *loader;
    	GdkPixbuf *pixbuf;
	
	char* endarg;
	id = strtol(arg,&endarg,10);
	if (arg == endarg) id = -1;

	item = get_item(id);


	GtkWidget * box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);

	GtkWidget * box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	
	gtk_box_pack_start (GTK_BOX(box), GTK_WIDGET(box2),FALSE,FALSE,0);

	GtkWidget * box3 = gtk_grid_new();

	title_box = gtk_entry_new();
	author_box = gtk_entry_new();
	start_bid_box = gtk_spin_button_new(NULL,10,0);

	gtk_entry_set_text(GTK_ENTRY(title_box),item.title);
	gtk_entry_set_text(GTK_ENTRY(author_box),item.author);
	gtk_spin_button_set_value(GTK_ENTRY(start_bid_box),item.start_bid);
	id_label = gtk_label_new("");

	gtk_grid_attach(GTK_GRID(box3), gtk_label_new("ID : "),0,0,1,1);
	gtk_grid_attach(GTK_GRID(box3), id_label, 1,0,1,1);

	gtk_grid_attach(GTK_GRID(box3), gtk_label_new("Title : "),0,1,1,1);
	gtk_grid_attach(GTK_GRID(box3), title_box, 1,1,1,1);
	gtk_grid_attach(GTK_GRID(box3), gtk_label_new("Author : "),0,2,1,1);
	gtk_grid_attach(GTK_GRID(box3), author_box, 1,2,1,1);
	gtk_grid_attach(GTK_GRID(box3), gtk_label_new("Starting Bid : "),0,3,1,1);
	gtk_grid_attach(GTK_GRID(box3), start_bid_box, 1,3,1,1);

	gtk_box_pack_start (GTK_BOX(box2), box3, FALSE,FALSE,0);


	if(item.picture_size>0){
		loader = gdk_pixbuf_loader_new ();
    		gdk_pixbuf_loader_write (loader, item.picture, item.picture_size, NULL);
    		pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);
		picture = gtk_image_new_from_pixbuf(pixbuf);
		picture_buffer = item.picture;
		picture_buffer_size = item.picture_size;
	}else{
		picture = gtk_image_new_from_file("");//get the broken file image
		picture_buffer = NULL;
		picture_buffer_size = 0;

	}


	gtk_box_pack_start (GTK_BOX(box2), picture, TRUE,FALSE,0);

	commit_btn =  gtk_button_new_with_label("Commit");
	load_picture_btn = gtk_button_new_with_label("Load Picture");

	GtkWidget * box4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

	g_signal_connect (commit_btn, "clicked", G_CALLBACK (commit_line), NULL);
	g_signal_connect (load_picture_btn, "clicked", G_CALLBACK (load_image), NULL);

	gtk_box_pack_start( GTK_BOX(box4), commit_btn, FALSE,FALSE,0);
	gtk_box_pack_start( GTK_BOX(box4), load_picture_btn, FALSE,FALSE,0);
	gtk_box_pack_start( GTK_BOX(box2), box4, FALSE,FALSE,0);
	gtk_box_pack_start( GTK_BOX(box), gtk_label_new("Info :"),FALSE,FALSE,0);

	info_buffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(info_buffer),item.info,-1);
	info_box = gtk_text_view_new_with_buffer(info_buffer);

	gtk_box_pack_start( GTK_BOX(box), info_box,TRUE,TRUE,0);




	win = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (win), "Window");

	gtk_container_add(GTK_CONTAINER(win),box);

	gtk_widget_show_all(GTK_WIDGET(win));

	item_struct_free(&item);
}

int main(int argc, char ** argv){
       
	GtkApplication * app;
	int status;
	app = gtk_application_new("info.loutre.auction", G_APPLICATION_FLAGS_NONE);
	char * arg;
	if(argc <= 1){
		arg = "-1";
	}else{
		arg = argv[1];
	}

	g_signal_connect (app, "activate", G_CALLBACK (activate), arg);
	status = g_application_run (G_APPLICATION (app), 0, NULL);
	g_object_unref(app);



	
// +- window ---------------------------+
// |+- box ----------------------------+|
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

