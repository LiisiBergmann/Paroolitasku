#include "../database/database.h"
#include "ui.h"

void ui_start(int argc, char **argv) 
{
	printf("Program starts...\n");
	
	GtkWidget *window;
    GtkBuilder *builder;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    if(!builder)
    {
		printf("Failed to create GTK builder!\n");
		return;
	}
	printf("Builder created\n");	
    gtk_builder_add_from_file(builder, "paroolitasku2.xml", NULL);
    
    createTable();
    
    if(userExist())
    {
		printf("login window opens\n");
		
		window = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
		g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
		
		GtkWidget *login_entry = GTK_WIDGET(gtk_builder_get_object(builder, "enter2"));
		GtkWidget *login_button = GTK_WIDGET(gtk_builder_get_object(builder, "login"));
    
		GObject *login_data = g_object_new(G_TYPE_OBJECT, NULL);
		g_object_set_data(G_OBJECT(login_data), "enter2", login_entry);
		g_object_set_data(G_OBJECT(login_data), "window2", window);
    
		g_signal_connect(login_button, "clicked", G_CALLBACK(login), login_data);
		
		
    } else
    {
		printf("make new user window opens\n");
		window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
		g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

		GtkWidget *make_entry = GTK_WIDGET(gtk_builder_get_object(builder, "enter"));
		GtkWidget *make_button = GTK_WIDGET(gtk_builder_get_object(builder, "Make"));
	
		GObject *data = g_object_new(G_TYPE_OBJECT, NULL);
		g_object_set_data(G_OBJECT(data), "enter", make_entry);
		g_object_set_data(G_OBJECT(data), "window", window);
	
		g_signal_connect(make_button, "clicked", G_CALLBACK(new_user_make), builder);
    }
    
    gtk_widget_show(window);
    
	gtk_main();
}

//avab peamise akna
void open_main()
{
	printf("Open main function called\n");
	
	GtkWidget *window;
    GtkBuilder *builder;

    builder = gtk_builder_new();
    if(!builder)
    {
		printf("Failed to create GTK builder!\n");
		return;
	}
	
    gtk_builder_add_from_file(builder, "paroolitasku2.xml", NULL);
    
    window = GTK_WIDGET(gtk_builder_get_object(builder, "info"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *listBox = GTK_WIDGET(gtk_builder_get_object(builder, "list"));
    g_signal_connect(listBox, "row-selected", G_CALLBACK(show_sites), NULL);

    GtkWidget *add = GTK_WIDGET(gtk_builder_get_object(builder, "add2"));
    g_signal_connect(add, "clicked", G_CALLBACK(add_sites), listBox);
    
    GtkWidget *search_entry = GTK_WIDGET(gtk_builder_get_object(builder, "search"));
	g_signal_connect(search_entry, "search-changed", G_CALLBACK(on_search_changed), listBox);	
    
    gtk_widget_show(window);

    load_sites(GTK_LIST_BOX(listBox));
}	

//loob uue kasutaja
void new_user_make(GtkButton * button, gpointer userdata)
{
	printf("New user make function called");
    (void)button;
	GtkBuilder *builder = GTK_BUILDER(userdata);
    GtkWidget *make_entry = GTK_WIDGET(gtk_builder_get_object(builder, "enter"));
    if (!GTK_IS_WIDGET(make_entry))
    {
        g_error("Failed to get make_entry widget.");
        return;
    }

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    if (!GTK_IS_WIDGET(window))
    {
        g_error("Failed to get window widget.");
        return;
    }
    
	char *password = (char *)gtk_entry_get_text(GTK_ENTRY(make_entry));
	printf("password entered: %s\n", password);
    if (strlen(password) != 0) 
    {
        setKey(password);
        gtk_widget_hide(window);
        open_main();
    }

}

//logib kasutaja sisse
void login(GtkButton * button, gpointer userdata)
{
    (void)button;
    GtkWidget *login_entry = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "enter2"));
    if (!GTK_IS_WIDGET(login_entry))
    {
        g_error("Failed to get login_entry widget.");
        return;
    }

    GtkWidget *window2 = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "window2"));
    if (!GTK_IS_WIDGET(window2))
    {
        g_error("Failed to get window2 widget.");
        return;
    }
    
	char *password = (char *)gtk_entry_get_text(GTK_ENTRY(login_entry));
    if (checkKey(password)) 
    {
		gtk_widget_hide(window2);
		open_main();
    }
}

//lisab uusi veebi lehe andmeid
void add_sites(GtkButton * button, gpointer userdata)
{
    (void) button;

    GtkWidget *window;
    GtkBuilder *builder;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "paroolitasku2.xml", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "paroolid"));

    GtkWidget *name = GTK_WIDGET(gtk_builder_get_object(builder, "enterweb"));
    GtkWidget *url = GTK_WIDGET(gtk_builder_get_object(builder, "enterurl"));
    GtkWidget *username = GTK_WIDGET(gtk_builder_get_object(builder, "eusername"));
    GtkWidget *password = GTK_WIDGET(gtk_builder_get_object(builder, "epassword"));

    GObject *data = g_object_new(G_TYPE_OBJECT, NULL);
    g_object_set_data(G_OBJECT(data), "enterweb", name);
    g_object_set_data(G_OBJECT(data), "enterurl", url);
    g_object_set_data(G_OBJECT(data), "eusername", username);
    g_object_set_data(G_OBJECT(data), "epassword", password);
    g_object_set_data(G_OBJECT(data), "paroolid", window);
    g_object_set_data(G_OBJECT(data), "list", GTK_LIST_BOX(userdata));

    GtkWidget *save = GTK_WIDGET(gtk_builder_get_object(builder, "savenew"));
    g_signal_connect(save, "clicked", G_CALLBACK(save_sites), data);

    gtk_widget_show(window);
}

//salvestab veebilehe andmeid
void save_sites(GtkButton *button, gpointer userdata)
{
    (void) button;
    
    GtkWidget *username = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "eusername"));
    GtkWidget *password = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "epassword"));
    GtkWidget *url = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "enterurl"));
    GtkWidget *name = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "enterweb"));
    GtkWidget *window = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "paroolid"));
    GtkWidget *listBox = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "list"));

    Site site;
    site.id = 0;
    strcpy(site.showname, gtk_entry_get_text(GTK_ENTRY(name)));
    strcpy(site.showurl, gtk_entry_get_text(GTK_ENTRY(url)));
    strcpy(site.showuser, gtk_entry_get_text(GTK_ENTRY(username)));
    strcpy(site.showpas, gtk_entry_get_text(GTK_ENTRY(password)));
    
    if (strlen(site.showname) && strlen(site.showurl) && strlen(site.showuser) && strlen(site.showpas)) {
        
        save(site);

        gtk_widget_hide(window);

        load_sites(GTK_LIST_BOX(listBox));
    }
    
}

//laeb listi veebi nimesid
void load_sites(GtkListBox * listBox)
{
	gtk_container_foreach(GTK_CONTAINER(listBox), (void *) gtk_widget_destroy, NULL);
    
    int count;
    Site *sites = getSites(NULL, &count);
    
    for (int i = 0; i < getCountSitesAll(); i++)
    {
		GtkWidget *row = gtk_list_box_row_new();
        g_object_set_data(G_OBJECT(row), "id", (gpointer) &sites[i].id);
        gtk_container_add(GTK_CONTAINER(row), gtk_label_new(sites[i].showname));
        gtk_list_box_insert(GTK_LIST_BOX(listBox), row, i);
    }
    gtk_widget_show_all(GTK_WIDGET(listBox));
    gtk_list_box_unselect_all(listBox);
}	

//naitab valitud veebilehe andmeid
void show_sites(GtkListBox * list, GtkListBoxRow * row, gpointer userdata)
{
    (void) userdata;

    GtkWidget *window;
    GtkBuilder *builder;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "paroolitasku2.xml", NULL);
    
    if (row == NULL) {
        printf("Error: Received NULL row pointer in show_sites(). Aborting...\n");
        return;
    }
    
    // Add the main info window widget to the userdata
    GtkWidget *window_info = GTK_WIDGET(gtk_builder_get_object(builder, "info"));
    if (window_info != NULL && GTK_IS_WIDGET(window_info)) {
		gtk_widget_hide(window_info);
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "showpass"));
	
    int *id = (int *) g_object_get_data(G_OBJECT(row), "id");
    
    int count;
    Site *site = getSites(NULL, &count);	

    GtkWidget *name = GTK_WIDGET(gtk_builder_get_object(builder, "showname"));
    gtk_label_set_text(GTK_LABEL(name), site[*id - 1].showname);
	
    GtkWidget *url = GTK_WIDGET(gtk_builder_get_object(builder, "showurl"));
    gtk_label_set_text(GTK_LABEL(url), site[*id - 1].showurl);

    GtkWidget *username = GTK_WIDGET(gtk_builder_get_object(builder, "showuser"));
    gtk_label_set_text(GTK_LABEL(username), site[*id - 1].showuser);

    GtkWidget *password = GTK_WIDGET(gtk_builder_get_object(builder, "showpas"));
    gtk_entry_set_text(GTK_ENTRY(password), site[*id - 1].showpas);
    
    GtkWidget *show = GTK_WIDGET(gtk_builder_get_object(builder, "ava"));
    
    GObject *datas = g_object_new(G_TYPE_OBJECT, NULL);
    g_object_set_data(G_OBJECT(datas), "ava", password);
    g_signal_connect(show, "clicked", G_CALLBACK(show_but), datas);
   

    GObject *data = g_object_new(G_TYPE_OBJECT, NULL);
    g_object_set_data(G_OBJECT(data), "id", (gpointer) id);
    g_object_set_data(G_OBJECT(data), "list", list);
    g_object_set_data(G_OBJECT(data), "showpass", window);

    GtkWidget *delete = GTK_WIDGET(gtk_builder_get_object(builder, "Remove"));
    g_signal_connect(delete, "clicked", G_CALLBACK(delete_sites), data);

    gtk_widget_show(window);
}

void show_but(GtkButton *button, gpointer userdata)
{
	(void) button;
	
	GtkWidget *entry = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "ava"));
	
	gboolean show = gtk_entry_get_visibility(GTK_ENTRY(entry));
	if(show)
	{	
		gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    }
    else
    {
		gtk_entry_set_visibility(GTK_ENTRY(entry), TRUE);
	}
	
}	
	
//kasitleb otsingu muutmist
void on_search_changed(GtkSearchEntry *entry, gpointer user_data)
{
    const gchar *search_text = gtk_entry_get_text(GTK_ENTRY(entry));
    GtkListBox *listBox = GTK_LIST_BOX(user_data);
    
    // Clear existing items in the list box
    gtk_container_foreach(GTK_CONTAINER(listBox), (void *) gtk_widget_destroy, NULL);

    // Reload sites matching the search query
    filter_and_load_sites(listBox, search_text);
}

// Filtrid ja laadib veebilehtede loendi vastavalt otsingule
void filter_and_load_sites(GtkListBox *listBox, const gchar *search_text)
{
    // Fetch sites matching the search query from the database
    int count;
    Site *filtered_sites = getSites((char *)search_text, &count);

    // Add filtered sites to the list box
    for (int i = 0; i < count; i++)
    {
        GtkWidget *row = gtk_list_box_row_new();
        g_object_set_data(G_OBJECT(row), "id", (gpointer) &filtered_sites[i].id);
        gtk_container_add(GTK_CONTAINER(row), gtk_label_new(filtered_sites[i].showname));
        gtk_list_box_insert(GTK_LIST_BOX(listBox), row, i);
    }

    // Show the list box
    gtk_widget_show_all(GTK_WIDGET(listBox));
}

//kustutab valitud parooli
void delete_sites(GtkButton * button, gpointer userdata)
{
    (void) button;

    // Check if the userdata is NULL
    if (userdata == NULL) {
        printf("Error: Userdata is NULL in delete_sites.\n");
        return;
    }
    
    // Get the ID from the userdata
    int *id = (int *) g_object_get_data(G_OBJECT(userdata), "id");
    if (id == NULL) {
        printf("Error: Failed to retrieve ID from userdata in delete_sites.\n");
        return;
    }

    // Debugging print: Print the retrieved ID
    printf("ID retrieved from userdata: %d\n", *id);

    // Get the list box widget from the userdata
    GtkWidget *listBox = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "list"));
    if (listBox == NULL || !GTK_IS_WIDGET(listBox)) {
        printf("Error: Failed to retrieve list box widget from userdata in delete_sites.\n");
        return;
    }

    // Debugging print: Print success message for retrieving list box widget
    printf("List box widget retrieved successfully.\n");

    // Get the showpass window widget from the userdata
    GtkWidget *window = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "showpass"));
    if (window == NULL || !GTK_IS_WIDGET(window)) {
        printf("Error: Failed to retrieve showpass window widget from userdata in delete_sites.\n");
        return;
    }
	
	gtk_widget_hide(window);
	
    // Debugging print: Print success message for retrieving showpass window widget
    printf("Showpass window widget retrieved successfully.\n");

    // Get the main info window widget from the userdata
    GtkWidget *window_info = GTK_WIDGET(g_object_get_data(G_OBJECT(userdata), "info"));
    if (window_info != NULL && GTK_IS_WIDGET(window_info)) {
        gtk_widget_show(window_info);
    }

    // Debugging print: Print success message for retrieving main info window widget
    printf("Main info window widget retrieved successfully.\n");

    // Delete the site with the given ID
    deletes(*id);
    
    // Reload the sites list
    load_sites(GTK_LIST_BOX(listBox));
}
