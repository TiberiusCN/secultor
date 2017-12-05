/*
 * GPL 2
 * Copyright 2017 (c) TiCaN <tican.protonmail.com> github.com/TiberiusCN
 */

#include "secultor.h"

GtkWidget* wMain;
	GtkWidget* vbMain;
		GtkWidget* hbFiles;
			GtkWidget *bFileopen;
			GtkWidget *bFilesave;
		GtkWidget* hbPP;
			GtkWidget *rbPP;
			GtkWidget *cbPP;
			GtkWidget *cbPPAct;
			GtkWidget *bPPApp;
		GtkWidget* hbVM;
			GtkWidget *rbVM;
			GtkWidget *cbVM;
			GtkWidget *cbVMAct;
			GtkWidget *bVMApp;
		GtkWidget* hbGraph;
			GtkWidget *rbGraph;
			GtkWidget *cbGraph;
			GtkWidget *cbGAct;
			GtkWidget *bGApp;
		GtkWidget* hbForge;
			GtkWidget* rbForge;
			GtkWidget* cbForge;
			GtkWidget* cbFAct;
			GtkWidget* bFApp;
		GtkWidget* hbMake;
			GtkWidget* eMake;
			GtkWidget* bMake;
		GtkWidget* hbOutput;			
			GtkWidget* tvOutput;

void ev_fileopen_click()
{
	GtkWidget* dialog;
	dialog = gtk_file_chooser_dialog_new("Open file", 0, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,0);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char* fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		gtk_entry_append_text(GTK_ENTRY(eMake),fname);
		gtk_entry_append_text(GTK_ENTRY(eMake)," ");
		g_free(fname);
	}
	gtk_widget_destroy(dialog);
}

void ev_filesave_click()
{
	GtkWidget* dialog;
	dialog = gtk_file_chooser_dialog_new("Save file", 0, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,0);
	if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char* fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		gtk_entry_append_text(GTK_ENTRY(eMake),fname);
		gtk_entry_append_text(GTK_ENTRY(eMake)," ");
		g_free(fname);
	}
	gtk_widget_destroy(dialog);
}

void ev_make_clicked()
{
	system(gtk_entry_get_text(GTK_ENTRY(eMake)));
	return;
	gchar* com = gtk_entry_get_text(GTK_ENTRY(eMake));
	int len = strlen(com);
	char* data = malloc(len+8);
	memcpy(data,com,len);
	memcpy(" > temp",com+len,8);
	g_free(com);
	system(data);
	free(data);
}

void ev_radio_change(GtkWidget* wdg)
{
	if(wdg == rbPP)
	{
	} else
	if(wdg == rbVM)
	{
	} else
	if(wdg == rbGraph)
	{
	} else
	if(wdg == rbForge)
	{
		GtkTextBuffer* buf = gtk_text_buffer_new(0);
		gtk_text_buffer_set_text(buf,"Example\n\ncom",12);

		g_free(gtk_text_view_get_buffer(GTK_TEXT_VIEW(tvOutput)));
		gtk_text_view_set_buffer(GTK_TEXT_VIEW(tvOutput),buf);

		g_free(buf);
	}
}

int main(int argc, char** argv)
{
	gtk_init(&argc,&argv);

	wMain = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wMain),"GNU Secultor");
	gtk_widget_set_size_request(wMain,600,400);

	GSList* radgr = 0;

	vbMain = gtk_vbox_new(FALSE,5);
	hbFiles = gtk_hbox_new(FALSE,5);
	bFileopen = gtk_button_new_with_label("open");
	bFilesave = gtk_button_new_with_label("save");
	hbPP = gtk_hbox_new(FALSE,5);
	rbPP = gtk_radio_button_new(radgr); radgr = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbPP));
	cbPP = gtk_combo_new();
	cbPPAct = gtk_combo_new();
	bPPApp = gtk_button_new_with_label("append");
	hbVM = gtk_hbox_new(FALSE,5);
	rbVM = gtk_radio_button_new(radgr); radgr = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbVM)); 
	cbVM = gtk_combo_new();
	cbVMAct = gtk_combo_new();
	bVMApp = gtk_button_new_with_label("append");
	hbGraph = gtk_hbox_new(FALSE,5);
	rbGraph = gtk_radio_button_new(radgr); radgr = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbGraph));
	cbGraph = gtk_combo_new();
	cbGAct = gtk_combo_new();
	bGApp = gtk_button_new_with_label("append");
	hbForge = gtk_hbox_new(FALSE,5);
	rbForge = gtk_radio_button_new(radgr); radgr = gtk_radio_button_get_group(GTK_RADIO_BUTTON(rbForge));
	cbForge = gtk_combo_new();
	cbFAct = gtk_combo_new();
	bFApp = gtk_button_new_with_label("append");
	hbMake = gtk_hbox_new(FALSE,5);
	eMake = gtk_entry_new();
	bMake = gtk_button_new_with_label("Make");
	hbOutput = gtk_hbox_new(FALSE,5);
	tvOutput = gtk_text_view_new();

	gtk_container_add(GTK_CONTAINER(wMain),vbMain);
	gtk_box_pack_start(GTK_BOX(vbMain),hbFiles,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbPP,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbVM,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbGraph,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbForge,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbMake,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbOutput,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbFiles),bFileopen,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbFiles),bFilesave,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbPP),rbPP,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbPP),cbPP,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbPP),cbPPAct,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbPP),bPPApp,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbVM),rbVM,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbVM),cbVM,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbVM),cbVMAct,TRUE,TRUE,0); 
	gtk_box_pack_start(GTK_BOX(hbVM),bVMApp,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbGraph),rbGraph,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbGraph),cbGraph,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbGraph),cbGAct,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbGraph),bGApp,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbForge),rbForge,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbForge),cbForge,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbForge),cbFAct,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbForge),bFApp,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbMake),eMake,TRUE,TRUE,0);
	gtk_box_pack_start(GTK_BOX(hbMake),bMake,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbOutput),tvOutput,TRUE,TRUE,0);

	g_signal_connect(GTK_RADIO_BUTTON(rbPP),"clicked",G_CALLBACK(ev_radio_change),0);
	g_signal_connect(GTK_RADIO_BUTTON(rbVM),"clicked",G_CALLBACK(ev_radio_change),0);
	g_signal_connect(GTK_RADIO_BUTTON(rbGraph),"clicked",G_CALLBACK(ev_radio_change),0);
	g_signal_connect(GTK_RADIO_BUTTON(rbForge),"clicked",G_CALLBACK(ev_radio_change),0);
	g_signal_connect(wMain,"delete-event",G_CALLBACK(gtk_main_quit),0);
	g_signal_connect(bMake,"clicked",G_CALLBACK(ev_make_clicked),0);
	
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbPP)->entry),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbPPAct)->entry),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbVM)->entry),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbVMAct)->entry),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbGraph)->entry),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbGAct)->entry),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbForge)->entry),FALSE);
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbFAct)->entry),FALSE);

/*
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

	GtkWidget* bVMOpenFile = gtk_button_new_with_label("Open");
	gtk_box_pack_start(GTK_BOX(hbVM),bVMOpenFile,FALSE,FALSE,0);
	g_signal_connect(bVMOpenFile,"clicked",G_CALLBACK(ev_openfile_click),0);

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
	
	GtkWidget* hbF = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbF,FALSE,FALSE,0);

	GList *flist = 0;
	flist = g_list_append(flist,"ogllaf forge");

	GtkWidget* cbF = gtk_combo_new();
	gtk_combo_set_popdown_strings(GTK_COMBO(cbF),flist);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(cbF)->entry),"Печь");
	gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(cbF)->entry),FALSE);
	gtk_box_pack_start(GTK_BOX(hbF),cbF,TRUE,TRUE,0);
	
	GtkWidget* hbCommand = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbMain),hbCommand,FALSE,FALSE,0);

	enCommand = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbCommand),enCommand,TRUE,TRUE,0);

	GtkWidget* bMake = gtk_button_new_with_label("Make");
	gtk_box_pack_start(GTK_BOX(hbCommand),bMake,FALSE,FALSE,0);
	g_signal_connect(bMake,"clicked",G_CALLBACK(ev_make_clicked),0);*/

	gtk_widget_show_all(wMain);


	gtk_main();

	return 0;
}
