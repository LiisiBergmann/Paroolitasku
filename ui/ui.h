#include <gtk/gtk.h>

void ui_start(int argc, char **argv);
void open_main();
void new_user_make(GtkButton * button, gpointer userdata);
void login(GtkButton * button, gpointer userdata);
void save_sites(GtkButton * button, gpointer userdata);
void add_sites(GtkButton * button, gpointer userdata);
void load_sites(GtkListBox * listbox);
void show_sites(GtkListBox * list, GtkListBoxRow * row, gpointer userdata);
void show_but(GtkButton *button, gpointer userdata);
void on_search_changed(GtkSearchEntry *entry, gpointer user_data);
void filter_and_load_sites(GtkListBox *listBox, const gchar *search_text);
void delete_sites(GtkButton * button, gpointer userdata);