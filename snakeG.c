#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define Left 0xff51       //these are numerical values for symbol i.e for left symbol it's 0xff51 ,hexadecimal value
#define Up 0xff52
#define Right 0xff53
#define Down 0xff54
#define d 0x064
#define a 0x061
#define w 0x077
#define s 0x073

/****************************some UI function prototype*****************************************************************/
void howtoplay(GtkWidget *widget, gpointer window);
void show_about_game(GtkWidget *widget, gpointer data);
void show_about_gtk(GtkWidget *widget, gpointer data);
GdkPixbuf *create_pixbuf(const gchar * filename);

/***********************************************************************************************************************/
GtkWidget *image_p[50];   /*segment storage for snake 1*/
GtkWidget *image_q[50];	 /*segment storage for snake 1*/
GtkWidget *food_img;    /*segment storage for food*/
GtkWidget *score_vbox;
GtkWidget *frame1;
GtkWidget *table_body;

/*snakes at first*/
int snake_p[50][2] = {
				{2,2},
				{3,2},
				{4,2},
				{5,2},
				{6,2},
				{7,2},
				{8,2}
}; 
int snake_q[50][2] = {				
				{2,47},
				{3,47},
				{4,47},
				{5,47},
				{6,47},
				{7,47},
				{8,47}
}; 
int segs_p = 7; 		//number of active snake segments
int segs_q = 7;

int generate_new_food = 1; 	//BOOLEAN.
int food[1][2];			//stogare for food
 
int first_run = 1; //BOOLEAN
/******************************************************************************************************************************************
*Random food generator function
*******************************************************************************************************************************************/
void food_gen() {
	int r, c, i, again = 0;
	/*A big loop to remove the overlaping of snake body and food*/	
	do {
		srand(time(0));
		do {
			r = random() % 100;
		}while(r > 49);
		
		do {
			c = random() % 100;
		}while(c > 39);
		
		food[0][0] = c;
		food[0][1] = r;
		for(i = 0; i < segs_p; i++) {
			if( (snake_p[i][0] == food[0][0]) && (snake_p[i][1] == food[0][1]) )
				again = 1;
		}
		
		for(i = 0; i < segs_q; i++) {
			if( (snake_q[i][0] == food[0][0]) && (snake_q[i][1] == food[0][1]) )
				again = 1;
		}
	}while(again == 1);
	
	food_img = gtk_image_new_from_file("food.png");
	gtk_grid_attach(GTK_GRID(table_body), food_img, food[0][0], food[0][0]+1, food[0][1], food[0][1]+1);
	gtk_widget_show(food_img);
	
	g_print("c: %d,r: %d\n", c, r); 	//trace value or print value to terminal
	generate_new_food = 0;
}
/*****************************************************************************************************************************************
      Clean the already drawn snake 
*****************************************************************************************************************************************/
void clear_p() {
	int i;
	for(i = 0; i < segs_p; i++)
			gtk_widget_hide(image_p[i]);
}
void clear_q() {
	int i;
	for(i = 0; i < segs_q; i++)
			gtk_widget_hide(image_q[i]);
}
/*****************************************************************************************************************************************
    Check if the snake just touched the food
******************************************************************************************************************************************/
int ate_p() {
	if( (snake_p[segs_p][0] == food[0][0]) && (snake_p[segs_p][1] == food[0][1]) )
		return 1;
	return 0;
}
int ate_q() {
	if( (snake_q[segs_q][0] == food[0][0]) && (snake_q[segs_q][1] == food[0][1]) )
		return 1;
	return 0;
}	
/*****************************************************************************************************************************************/
/******************************************************************************************************************************************
  Check for collision. Returns BOOLEAN
******************************************************************************************************************************************/
//snake 1 banging with itself
int collision_p() {
	int i;
	for(i = 0; i < segs_p-1; i++) {
		if( (snake_p[segs_p][0] == snake_p[i][0]) && (snake_p[segs_p][1] == snake_p[i][1]) ) 
			return 1;
	}
	return 0;
}
//snake 2 banging with itself
int collision_q() {
	int i;
	for(i = 0; i < segs_q-1; i++) {
		if( (snake_q[segs_q][0] == snake_q[i][0]) && (snake_q[segs_q][1] == snake_q[i][1]) )	
			return 1;
	}
	return 0;
}
//snakes banging with each other
int two_collision() {
	int i;
	for(i = 0; i < segs_q-1; i++) {
		if( (snake_p[segs_p][0] == snake_q[i][0]) && (snake_p[segs_p][1] == snake_q[i][1]) )
			return 1;
	}
	for (i = 0; i < segs_p-1; i++) {
		if( (snake_q[segs_q][0] == snake_p[i][0]) && (snake_q[segs_q][1] == snake_p[i][1]) )
			return 1;
	}
	return 0;
}
/***********************************************************************************************************************************/	

void display_score() {
	int score1;
	int score2;
	score1 = (segs_p - 7) * 10;
	score2 =  (segs_q - 7) * 10;

	static GtkWidget *score1_label = NULL;		//this is static so that it will remember previous score
	static	GtkWidget *score2_label = NULL;
	
	gtk_widget_hide(score1_label);
	gtk_widget_hide(score2_label);
	
	char score1_char[10];
	char score2_char[20];
	
	int i, j, c;
	i = 0;
	do {       					// generate digits in reverse order 
   	    score1_char[i++] = score1 % 10 + '0';       // get next digit 
    	}while((score1 /= 10) > 0);     		// delete it 
    	score1_char[i] = '\0';
    	
    	i = 0;
    	for(j = strlen(score1_char) - 1; i < j; j--) {
		i++;
	    	c = score1_char[i];        
	    	score1_char[i] = score1_char[j];
    		score1_char[j] = c;
   	}
    	
	i = 0;
	do {
		score2_char[i++] = score2 % 10 + '0';
	}while((score2 /= 10) > 0);
	score2_char[i] = '\0';
	
	i = 0;
    	for(j = strlen(score2_char) - 1; i < j; j--) {
		i++;
	    	c = score2_char[i];        
	    	score2_char[i] = score2_char[j];
    		score2_char[j] = c;
   	}
    	
	score1_label = gtk_label_new(score1_char);
	score2_label = gtk_label_new(score2_char);
	
	gtk_box_pack_start(GTK_BOX(score_vbox), score1_label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(score_vbox), score2_label, TRUE, TRUE, 0);
	
	gtk_widget_show(score1_label);
	gtk_widget_show(score2_label);
}
/**************************************************************************************************************************************/
/**************************************************************************************************************************************
The main() function.Window,frames, menu, table packing etc are initialized and drawn inside this function.
Also, keypress event is handled and functions are called accordingly
***************************************************************************************************************************************/
int main(int argc, char **argv) {
	GtkWidget *window;
	GtkWidget *menu_bar;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *frame2, *frame1;

	gtk_init(&argc, &argv);
	/*  this will Create the MAIN WINDOW and give properties to it  */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "snakeG");
	gtk_widget_set_size_request(GTK_WIDGET(window), 800, 600);
	gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
        gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("snake.png"));
/***********Creating a vbox inside the window to hold menubar and table for game and all other packing stuffs****************************/
	
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	hbox  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_end(GTK_BOX(vbox),hbox, TRUE, TRUE, 0);
	
	frame1 = gtk_frame_new("SNAKE");
	frame2 = gtk_frame_new("Game info and score");
	
	GtkWidget *frame2_box;
	frame2_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(frame2), frame2_box);
	
	GtkWidget *info_vbox;
	info_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	GtkWidget *key_navi;
	key_navi = gtk_image_new_from_file("keyN.png");
	gtk_box_pack_start(GTK_BOX(info_vbox), key_navi, TRUE, TRUE, 0);
	
	score_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	gtk_box_pack_start(GTK_BOX(frame2_box), info_vbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(frame2_box), score_vbox, TRUE, TRUE, 0);

	GtkWidget *white1;
	GtkWidget *white2;
	GtkWidget *white3;
	
	white1=gtk_label_new("   ");
	white2=gtk_label_new("   ");
	white3=gtk_label_new("   ");
	GtkWidget *score_label;
	score_label = gtk_label_new ("Score:\n");
	
	gtk_box_pack_end(GTK_BOX(score_vbox), white1, TRUE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(score_vbox), white2, TRUE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(score_vbox), white3, TRUE, TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(score_vbox), score_label, TRUE, TRUE, 0);
	
	gtk_box_pack_end(GTK_BOX(hbox), frame2, TRUE, TRUE, 1);
	gtk_box_pack_end(GTK_BOX(hbox), frame1, TRUE, TRUE, 1);
	/*********************************Creating Menu***************************************************/	
	menu_bar = gtk_menu_bar_new();	
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, TRUE, TRUE, 4);		// add menu_bar to main vertical box
	/**********************************File Menu Start***********************************************/
	GtkWidget *file_menu_box;
	GtkWidget *file;
	GtkWidget *new;
	GtkWidget *mode;
	GtkWidget *quit;

	file_menu_box = gtk_menu_new();
	file = gtk_menu_item_new_with_label("snakeG");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu_box);		//this will give rectangle box to file to add submenu item
	
	new = gtk_menu_item_new_with_label("new game");
	mode = gtk_menu_item_new_with_label("mode");
	quit = gtk_menu_item_new_with_label("quit");
	
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_box), new);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_box), mode);
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_box), quit);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);
	/***************File Menu End**********************/

	/**************Tools Menu Start**********************/
	GtkWidget *tools_menu_box;
	GtkWidget *tools;
	GtkWidget *setting;
	GtkWidget *highscore;
	
	tools_menu_box = gtk_menu_new();
	tools = gtk_menu_item_new_with_label("Tools");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools), tools_menu_box);

	setting = gtk_menu_item_new_with_label("Settings");
	highscore = gtk_menu_item_new_with_label("Highscore");
	
	gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu_box), setting);
	gtk_menu_shell_append(GTK_MENU_SHELL(tools_menu_box), highscore);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tools);
	/***************Tools Menu End**********************/
		
	/**************Help Menu Start**********************/
	GtkWidget *help_menu_box;
	GtkWidget *help;
	GtkWidget *how_to_play;
	GtkWidget *about_game;
	GtkWidget *about_gtk;

	help_menu_box = gtk_menu_new();
	help = gtk_menu_item_new_with_label("Help");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), help_menu_box);

	how_to_play = gtk_menu_item_new_with_label("How To Play");
	about_game = gtk_menu_item_new_with_label("About the game");
	about_gtk = gtk_menu_item_new_with_label("About GTK");
	
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu_box), how_to_play);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu_box), about_game);
	gtk_menu_shell_append(GTK_MENU_SHELL(help_menu_box), about_gtk);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help);
	/***************Help Menu End**********************/	
	
	/***************Dialog box calls**********************/
	//Show dialog Boxes for about gtk in main window
	g_signal_connect(about_gtk, "activate", G_CALLBACK(show_about_gtk), window); 
 
 	//Show dialog Boxes for about game in main window
 	g_signal_connect(about_game, "activate", G_CALLBACK(show_about_game), window); 
        
        //Show dialog Boxes for how to play in main window
        g_signal_connect(how_to_play, "activate", G_CALLBACK(howtoplay), window);

	g_signal_connect(quit, "activate", G_CALLBACK(gtk_main_quit), NULL);
       	g_signal_connect_swapped(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(new, "activate", G_CALLBACK(main), NULL);		//this calls whole new game
        
	/******table to draw the snake on (table packing)****************/
	/*/table_body=gtk_table_new(50,40,TRUE);
	table_body = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(table_body), 1);
	//gtk_grid_set_col_spacing(GTK_GRID(table_body),1);
	gtk_widget_show(table_body);
	
	//gtk_widget_show(window);
	int i;
	for(i = 0; i < segs_p; i++) {
		image_p[i] = gtk_image_new_from_file("segment_p.png");
		gtk_grid_attach(GTK_GRID(table_body), image_p[i], snake_p[i][0], snake_p[i][0]+1, snake_p[i][1], (snake_p[i][1])+1);
		gtk_widget_show( image_p[i] );
	}
	for(i = 0; i < segs_q; i++) {
		image_q[i] = gtk_image_new_from_file("segment_q.png");
		gtk_grid_attach(GTK_GRID(table_body), image_q[i], snake_q[i][0], snake_q[i][0]+1, snake_q[i][1], (snake_q[i][1])+1);
		gtk_widget_show( image_q[i] );
	}
	gtk_container_add(GTK_CONTAINER(frame1),table_body);*/
	
	gtk_widget_show_all(window);	
	gtk_main();
	return 0;
}
/*******************************************************************************************************************************************
function for dialog boxes
**********************************************************************************************************************************************************/
GdkPixbuf  *create_pixbuf(const gchar * filename) {
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if(!pixbuf) {
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	return pixbuf;
}
void show_about_game(GtkWidget *widget, gpointer data) {
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("snake.png", NULL);
	
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SnakeG");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.00"); 
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "GPL");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "you will definitely enjoy");
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}
void show_about_gtk(GtkWidget *widget, gpointer data) {
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("gtk.png", NULL);

	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "GTK+");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "3.0"); 
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "GPL");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "GTK is a used for writing GUI applications.\n for more info visit this website\n");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://www.gtk.org");
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf), pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}	
void howtoplay(GtkWidget *widget, gpointer window) {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new
	(
		window,
	        GTK_DIALOG_DESTROY_WITH_PARENT,
        	GTK_MESSAGE_INFO,
        	GTK_BUTTONS_OK,
        	"1. Use the arrow keys to move the worm. \n\n2. Collect food to score more.\n\n3. And Continue"
        );
	gtk_window_set_title(GTK_WINDOW(dialog), "How To Play");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
