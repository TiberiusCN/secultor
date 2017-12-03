/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include "sculpo.h"

int main(int argc, char** argv)
{
	gtk_init(&argc,&argv);
	GtkWidget* wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wnd),"GNU Sculpo");
	gtk_widget_set_size_request(wnd,100,100);
	
	GtkWidget* vbMain = gtk_vbox_new(FALSE,5);
	gtk_container_add(GTK_CONTAINER(wnd),vbMain);
	
	//----------VIRTUAL MACHINE ZONE---------/
	GtkWidget* hbVM = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbVM,FALSE,FALSE,0);
	
	GList* vmlist = 0; 
	vmlist = g_list_append(vmlist,"CUT20P");
	vmlist = g_list_append(vmlist,"Progress");
	vmlist = g_list_append(vmlist,"DMU50");
	vmlist = g_list_append(vmlist,"Parpas");

	GtkWidget* cbVM = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbVM),vmlist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(cbVM)->entry),"Виртуальная машина");
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbVM)->entry),FALSE);
	gtk_box_pack_start(GTK_BOX(hbVM),cbVM,TRUE,TRUE,0);


	
	//--------PREPROCESSOR ZONE---------//
	GtkWidget* hbPP = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbPP,FALSE,FALSE,0);

	GList *pplist = 0;
	pplist = g_list_append(pplist,"ISO 2 PVF");
	pplist = g_list_append(pplist,"Gerber 2 PVF");
	pplist = g_list_append(pplist,"ISO 2 Vis");
	pplist = g_list_append(pplist,"Gerber 2 Vis");

	GtkWidget* cbPP = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbPP),pplist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(cbPP)->entry),"Препроцессор");
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbPP)->entry),FALSE);
	gtk_box_pack_start(GTK_BOX(hbPP),cbPP,TRUE,TRUE,0);

	//--------FORGE ZONE--------//
	GtkWidget* hbF = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbF,FALSE,FALSE,0);

	GList *flist = 0;
	flist = g_list_append(flist,"ogllaf forge");

	GtkWidget* cbF = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbF),flist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(cbF)->entry),"Печь");
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbF)->entry),FALSE);
	gtk_box_pack_start(GTK_BOX(hbF),cbF,TRUE,TRUE,0);

	gtk_widget_show_all(wnd);
	
	g_signal_connect(wnd,"delete-event",G_CALLBACK(gtk_main_quit),0);

	gtk_main();

	return 0;
}
