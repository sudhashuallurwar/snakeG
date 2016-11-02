/********************************************************************************
* snakeG v1.0- snakeG.c                                                         *
*                                                                               *
* Copyright (C) 2016 Sudhanshu Sagar Allurwar                                   *                                                      
* License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html> *
* This is free software: you are free to change and redistribute it.            *
* There is NO WARRANTY, to the extent permitted by law.                         *
*                                                                               *
*********************************************************************************/

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#define Right 0xff53		//these are numerical values for symbol i.e for left symbol it's 0xff51 ,hexadecimal value
#define Left 0xff51
#define Up 0xff52
#define Down 0xff54		
#define d 0x064
#define a 0x061
#define w 0x077
#define s 0x073
#define DELAY 0.0009
/****************************some UI function prototype*****************************************************************/
void show_about_gtk(GtkWidget *widget, gpointer data);
GdkPixbuf *create_pixbuf(const gchar * filename);
void mode1(GtkWidget *widget, gpointer data);
void howtoplay(GtkWidget *widget, gpointer window);
void show_about_game(GtkWidget *widget, gpointer data);
void game_over();
void capson();
/***********************************************************************************************************************/
GtkWidget *image_p[100];   		/*segment storage for snake 1*/
GtkWidget *image_q[100];	 	/*segment storage for snake 1*/
GtkWidget *food_img;   			/*segment storage for food*/
GtkWidget *window;
GtkWidget *score_vbox;
GtkWidget *frame1;
GtkWidget *table_body;
GtkWidget *score1_label = NULL;		
GtkWidget *score2_label = NULL; 
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
 
guint key_p = Right; 		//default defined key
guint key_p_old;
guint key_q = d;
guint key_q_old;
/******************************************************************************************************************************************
*function to generate food at random places
*******************************************************************************************************************************************/
void food_gen() {
	int r, c, i, again = 0;
	do {				/*A big loop to remove the overlaping of snake body and food*/	
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
	
	food_img = gtk_image_new_from_file("./data/food.png");
	gtk_table_attach_defaults(GTK_TABLE(table_body), food_img, food[0][0], food[0][0]+1, food[0][1], food[0][1]+1);
	
	gtk_widget_show(food_img);
	g_print("c: %d,r: %d\n",c,r);		//trace value or print value to terminal
	generate_new_food=0;
}
/****************************************************************************************************************************************/
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
/*****************************************************************************************************************************************/
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
int collision_p() {		//snake 1 banging with itself
	int i;
	for(i = 0; i < segs_p - 1; i++) {
		if( (snake_p[segs_p][0] == snake_p[i][0]) && (snake_p[segs_p][1] == snake_p[i][1]) ) {
			game_over();
			return 1;
		}
	}
	return 0;
}
int collision_q() {		//snake 2 banging with itself
	int i;
	for(i = 0; i < segs_q - 1; i++) {
		if( (snake_q[segs_q][0] == snake_q[i][0]) && (snake_q[segs_q][1] == snake_q[i][1]) ) {	
			game_over();
			return 1;
		}
	}
	return 0;
}
int two_collision() {		//snakes banging with each other
	int i;
	for(i = 0; i < segs_q - 1; i++) {
		if( (snake_p[segs_p][0] == snake_q[i][0]) && (snake_p[segs_p][1] == snake_q[i][1]) ) {
			game_over();
			return 1;
		}	
	}
	for (i = 0; i < segs_p-1; i++) {
		if( (snake_q[segs_q][0] == snake_p[i][0]) && (snake_q[segs_q][1] == snake_p[i][1])) {
			game_over();
			return 1;
		}	
	}
	return 0;
}
/***********************************************************************************************************************************/	
void display_score() {
	int score1;
	int score2;
	score1 = (segs_p - 7) * 10;
	score2 = (segs_q - 7) * 10;

	gtk_widget_hide(score1_label);
	gtk_widget_hide(score2_label);
	
	char score1_char[10];
	char score2_char[20];
	
	int i, j, c;
	i = 0;
	do {       					// generate digits in reverse order 
   	    score1_char[i] = score1 % 10 + '0';		 // get next digit
	    i++;       
    	}while((score1 =score1 / 10) > 0);     		// delete it 
    	score1_char[i] = '\0';
    	
    	i = 0;
    	for(j = strlen(score1_char)-1; i < j; j--) {
		i++;
	    	c = score1_char[i];        
	    	score1_char[i] = score1_char[j];
    		score1_char[j] = c;
   	}
    	
	i = 0;
	do {
		score2_char[i] = score2 % 10 + '0';
		i++;
	}while((score2 = score2 / 10) > 0);
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
	
	gtk_box_pack_start(GTK_BOX(score_vbox), score1_label, TRUE, 0, 0);
	gtk_box_pack_start(GTK_BOX(score_vbox), score2_label, TRUE, 0, 0);
	
	gtk_widget_show(score1_label);
	gtk_widget_show(score2_label);
}
/******************************************************************************************************************************************
  function to move snakes
*******************************************************************************************************************************************/
gint move1(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	display_score();

	if(generate_new_food == 1)
		food_gen();
	int i;
	/***************************************************if right key is pressed******************************************************/
	if(key_p == Right) {					//keyval from gtk/gdkkeysyms.h; keyvalue for 'right' key
		if(collision_p() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
		clear_p();

		if(ate_p() == 1) {	
			segs_p++;
			snake_p[segs_p][0] = snake_p[segs_p-1][0] + 1;
			snake_p[segs_p][1] = snake_p[segs_p-1][1];
			gtk_widget_hide(food_img);
			generate_new_food = 1;
		}

		if((snake_p[segs_p][0]) > 38)
			snake_p[segs_p][0] = 0;
		else
			snake_p[segs_p][0] = snake_p[segs_p-1][0] + 1;
		snake_p[segs_p][1] = snake_p[segs_p-1][1];

		for(i = 0; i < segs_p; i++) {
			snake_p[i][0] = snake_p[i+1][0];
			snake_p[i][1] = snake_p[i+1][1];
	
			image_p[i] = gtk_image_new_from_file("./data/segment_p.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_p[i], snake_p[i][0], snake_p[i][0]+1, snake_p[i][1], snake_p[i][1]+1);
			gtk_widget_show(image_p[i]);
		}
		key_p_old = key_p;
	}
	/******************************************************if left key is pressed***********************************************/
	if(key_p == Left) {
		if(collision_p() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
 		clear_p();

		if(ate_p() == 1) {	
			segs_p++;
			snake_p[segs_p][0] = snake_p[segs_p-1][0] - 1;
			snake_p[segs_p][1] = snake_p[segs_p-1][1];
			gtk_widget_hide(food_img);
			generate_new_food = 1;	
		}
	
		if((snake_p[segs_p][0]) < 1)
			snake_p[segs_p][0] = 39;
		else
			snake_p[segs_p][0] = snake_p[segs_p-1][0] - 1;
		snake_p[segs_p][1] = snake_p[segs_p-1][1];

		for(i = 0; i < segs_p; i++) {
			snake_p[i][0] = snake_p[i+1][0];
			snake_p[i][1] = snake_p[i+1][1];
	
			image_p[i] = gtk_image_new_from_file("./data/segment_p.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_p[i], snake_p[i][0], snake_p[i][0]+1, snake_p[i][1], snake_p[i][1]+1);
			gtk_widget_show(image_p[i]);
		}
		key_p_old=key_p;
	}
	/**************************************************if up key is pressed*************************************************/
	if(key_p == Up) {
		if(collision_p() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
		clear_p();

		if(ate_p() == 1) {	
			segs_p++;
			snake_p[segs_p][0] = snake_p[segs_p-1][0];
			snake_p[segs_p][1] = snake_p[segs_p-1][1] - 1;
			gtk_widget_hide(food_img);
			generate_new_food = 1;
		}

		snake_p[segs_p][0] = snake_p[segs_p-1][0];
		if((snake_p[segs_p][1]) < 1)
			snake_p[segs_p][1] = 49;
		else
			snake_p[segs_p][1] = snake_p[segs_p-1][1] - 1;		

		for (i = 0; i < segs_p; i++) {
			snake_p[i][1] = snake_p[i+1][1];
			snake_p[i][0] = snake_p[i+1][0];
			
			image_p[i] = gtk_image_new_from_file("./data/segment_p.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_p[i], snake_p[i][0], snake_p[i][0]+1, snake_p[i][1], snake_p[i][1]+1);
			gtk_widget_show(image_p[i]);
		}
		key_p_old = key_p;
	}
	/************************************************if down key is pressed**************************************************/
	if(key_p == Down) {
		if(collision_p() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
		clear_p();

		if(ate_p() == 1) {	
			segs_p++;
			snake_p[segs_p][0] = snake_p[segs_p-1][0];
			snake_p[segs_p][1] = snake_p[segs_p-1][1] + 1;
			gtk_widget_hide(food_img);
			generate_new_food = 1;
		}

		snake_p[segs_p][0] = snake_p[segs_p-1][0];
		if (snake_p[segs_p][1] > 48)
			snake_p[segs_p][1] = 0;
		else
			snake_p[segs_p][1] = snake_p[segs_p-1][1] + 1;

		for(i = 0; i < segs_p; i++) {
			snake_p[i][1] = snake_p[i+1][1];
			snake_p[i][0] = snake_p[i+1][0];
			
			image_p[i] = gtk_image_new_from_file("./data/segment_p.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_p[i], snake_p[i][0], snake_p[i][0]+1, snake_p[i][1], snake_p[i][1]+1);
			gtk_widget_show(image_p[i]);
		}
		key_p_old = key_p;
	}
	return TRUE;
}
////////////////////////////////////////////////////////move function for second snake/////////////////////////////////////////////////////
gint move2(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	display_score();

	if(generate_new_food == 1)
		food_gen();
	int i;
	/***************************************************if d key is pressed******************************************************/
	if(key_q == d) {
		if(collision_q() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
		clear_q();

		if(ate_q() == 1) {	
			segs_q++;
			snake_q[segs_q][0] = snake_q[segs_q-1][0] + 1;
			snake_q[segs_q][1] = snake_q[segs_q-1][1];
			gtk_widget_hide(food_img);
			generate_new_food = 1;
		}

		if((snake_q[segs_q][0]) > 38)
			snake_q[segs_q][0] = 0;
		else
			snake_q[segs_q][0] = snake_q[segs_q-1][0] + 1;
		snake_q[segs_q][1] = snake_q[segs_q-1][1];

		for(i = 0; i < segs_q; i++) {
			snake_q[i][0] = snake_q[i+1][0];
			snake_q[i][1] = snake_q[i+1][1];
			
			image_q[i] = gtk_image_new_from_file("./data/segment_q.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_q[i], snake_q[i][0], snake_q[i][0]+1, snake_q[i][1], snake_q[i][1]+1);
			gtk_widget_show(image_q[i]);
		}
		key_q_old = key_q;
	}
	/***************************************************if q key is pressed******************************************************/
	if(key_q == a) {
		if(collision_q() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
		clear_q();
		
		if(ate_q() == 1) {	
			segs_q++;
			snake_q[segs_q][0] = snake_q[segs_q-1][0] - 1;
			snake_q[segs_q][1] = snake_q[segs_q-1][1];
			gtk_widget_hide(food_img);
			generate_new_food = 1;	
		}
	
		if((snake_q[segs_q][0]) < 1)
			snake_q[segs_q][0] = 39;
		else
			snake_q[segs_q][0] = snake_q[segs_q-1][0] - 1;
		snake_q[segs_q][1] = snake_q[segs_q-1][1];

		for(i = 0;i < segs_q; i++) {
			snake_q[i][0] = snake_q[i+1][0];
			snake_q[i][1] = snake_q[i+1][1];
			
			image_q[i] = gtk_image_new_from_file("./data/segment_q.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_q[i], snake_q[i][0], snake_q[i][0]+1, snake_q[i][1], snake_q[i][1]+1);
			gtk_widget_show(image_q[i]);
		}
		key_q_old = key_q;
	}
	/***************************************************if w key is pressed******************************************************/
	if(key_q == w) {
		if(collision_q() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
		clear_q();
		
		if(ate_q() == 1) {	
			segs_q++;
			snake_q[segs_q][0] = snake_q[segs_q-1][0];
			snake_q[segs_q][1] = snake_q[segs_q-1][1] - 1;
			gtk_widget_hide(food_img);
			generate_new_food = 1;
		}

		snake_q[segs_q][0] = snake_q[segs_q-1][0];
		if((snake_q[segs_q][1]) < 1)
			snake_q[segs_q][1] = 49;
		else
			snake_q[segs_q][1] = snake_q[segs_q-1][1] - 1;		

		for (i = 0; i < segs_q; i++) {
			snake_q[i][1] = snake_q[i+1][1];
			snake_q[i][0] = snake_q[i+1][0];
			
			image_q[i] = gtk_image_new_from_file("./data/segment_q.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_q[i], snake_q[i][0], snake_q[i][0]+1, snake_q[i][1], snake_q[i][1]+1);
			gtk_widget_show(image_q[i]);
		}
		key_q_old = key_q;
	}
	/***************************************************if s key is pressed******************************************************/
	if(key_q == s) {
		if(collision_q() == 1 || two_collision() == 1) {
			g_print("Dead");
			return FALSE;
		}
		clear_q();
		
		if(ate_q() == 1) {	
			segs_q++;
			snake_q[segs_q][0] = snake_q[segs_q-1][0];
			snake_q[segs_q][1] = snake_q[segs_q-1][1] + 1;
			gtk_widget_hide(food_img);
			generate_new_food = 1;
		}

		snake_q[segs_q][0] = snake_q[segs_q-1][0];
		if(snake_q[segs_q][1] > 48)
			snake_q[segs_q][1] = 0;
		else
			snake_q[segs_q][1] = snake_q[segs_q-1][1] + 1;
		
		for(i = 0; i < segs_q; i++) {
			snake_q[i][1] = snake_q[i+1][1];
			snake_q[i][0] = snake_q[i+1][0];
			
			image_q[i] = gtk_image_new_from_file("./data/segment_q.png");
			gtk_table_attach_defaults(GTK_TABLE(table_body), image_q[i], snake_q[i][0], snake_q[i][0]+1, snake_q[i][1], snake_q[i][1]+1);
			gtk_widget_show(image_q[i]);
		}
	key_q_old = key_q;
	}
	return TRUE;
}
/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************
      Flips the key and old_key variable as per the keystroke feed by event handler
*****************************************************************************************************************************************/
gint switch_key(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	static double start;
	double elapsed;
	
	elapsed = (((double)clock() - start) / CLOCKS_PER_SEC);

	if(elapsed < DELAY)
		return TRUE;
	
	guint temp_key = event->keyval;
	switch(temp_key) {
		case Right:
			if(key_p_old != Left) {
				key_p = Right;
				key_p_old = Right;
			}
			break;
		case Left:
			if(key_p_old != Right) {
				key_p = Left;
				key_p_old = Left;
			}
			break;
		case Up:
			if(key_p_old != Down) {	
				key_p = Up;
				key_p_old = Up;
			}
			break;
		case Down:
			if(key_p_old != Up) {
				key_p = Down;
				key_p_old = Down;
			}
			break;
		case d:
			if(key_q_old != a) {
				key_q = d;
				key_q_old = d;
			}
			break;
		case a:
			if(key_q_old != d) {
				key_q = a;
				key_q_old = a;
			}
			break;
		case w:
			if(key_q_old != s) {	
				key_q = w;
				key_q_old = w;
			}
			break;
		case s:
			if(key_q_old != w) {
				key_q = s;
				key_q_old = s;
			}
			break;
	}
	start = (double)clock();
	return TRUE;			
}
/**************************************************************************************************************************************/
/**************************************************************************************************************************************
The main() function. This function contains most of the GTK+ codes. Window,
frames, menu, table packing etc are initialized and drawn inside this function.
Also, keypress event is handled and functions are called accordingly
***************************************************************************************************************************************/
int main(int argc, char **argv) {
	if(argc == 2) {				//this is code for -h option
		if(!strcmp(argv[1], "-h")) {
			FILE *fp;
			char ch; 
			fp = fopen("./usage/usage.txt", "r");
			if(fp == NULL) {
				perror("fopen can't open file\n you can read it from usage folder");
				return 0;
			}
			while(fscanf(fp, "%c", &ch) != -1) {
				printf("%c", ch);
			}
			fclose(fp);
		}
		else
			perror("usage:-  ./project -h  ");
		return 0;
	}
	if(argc > 2) {
		perror("usage:- ./project -h  ");
		return 0;	
	}
	/**************************GTK code start**********************************************/
	GtkWidget *menu_bar;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *frame2, *frame1;

	GdkColor color, color1;		//gdkcolor is structure which contains green, blue, red as it's member
	color.green = 0xfeee;
	color.blue = 0xffee;
	color1.green = 0xffee;
	
	gtk_init (&argc, &argv);		//MOST IMP 
	/*  this will Create the MAIN WINDOW and assign attributes to it  */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "snakeG");
	gtk_widget_set_size_request(GTK_WIDGET(window), 800, 600);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
        gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("./data/Snake.png"));
	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);		//give colour to bground

	capson();						//warning for capslock
	g_timeout_add(100, (GSourceFunc) move1, NULL);		//move snake 1 i.e multithreading
	g_timeout_add(100, (GSourceFunc) move2, NULL);	 //move snake 2    this function call move2 at regular interval.(interval(ms), funtion to be called, gpointer data)
	g_signal_connect(window, "key-press-event", G_CALLBACK(switch_key), NULL); 
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
	gtk_widget_modify_bg(frame2_box, GTK_STATE_NORMAL, &color1);	//give colour to bground
	
	GtkWidget *info_vbox;
	info_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *key_navi;
	key_navi = gtk_image_new_from_file("./data/keyN.png");
	gtk_box_pack_start(GTK_BOX(info_vbox), key_navi, TRUE, TRUE, 0);
	
	score_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	gtk_box_pack_start(GTK_BOX(frame2_box), info_vbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(frame2_box), score_vbox, TRUE, TRUE, 0);

	GtkWidget *score_label;
	score_label = gtk_label_new ("Score:\n");
	
	gtk_box_pack_start(GTK_BOX(score_vbox), score_label, TRUE, TRUE, 0);
	
	gtk_box_pack_end(GTK_BOX(hbox), frame2, TRUE, TRUE, 1);
	gtk_box_pack_end(GTK_BOX(hbox), frame1, TRUE, TRUE, 1);
	
	/*********************************Creating Menu***************************************************/	
	menu_bar = gtk_menu_bar_new();	
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, TRUE, TRUE, 4);		// add menu_bar to main vertical box
	/**********************************File Menu Start***********************************************/
	GtkWidget *file_menu_box;
	GtkWidget *file;
	GtkWidget *quit;

	file_menu_box = gtk_menu_new();
	file = gtk_menu_item_new_with_label("snakeG");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), file_menu_box);		//this will give rectangle box to file to add submenu item
	
	quit = gtk_menu_item_new_with_label("quit");
	
	gtk_menu_shell_append(GTK_MENU_SHELL(file_menu_box), quit);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);
	/***************File Menu End**********************/

	/**************Tools Menu Start**********************/
	GtkWidget *tools_menu_box;
	GtkWidget *tools;
	GtkWidget *highscore;
	
	tools_menu_box = gtk_menu_new();
	tools = gtk_menu_item_new_with_label("Tools");
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(tools), tools_menu_box);

	highscore = gtk_menu_item_new_with_label("Highscore");
	
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
        g_signal_connect(how_to_play, "activate", G_CALLBACK(howtoplay), (gpointer) window);

	g_signal_connect(quit, "activate", G_CALLBACK(gtk_main_quit), NULL);
       	g_signal_connect_swapped(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        /*********************************table to draw the snake on (table packing)*******************************************/
	table_body = gtk_table_new(50, 40, TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(table_body), 1);
	gtk_table_set_col_spacings(GTK_TABLE(table_body), 1);

	int i;
	for(i = 0; i < segs_p; i++) {
		image_p[i] = gtk_image_new_from_file("./data/segment_p.png");
		gtk_table_attach_defaults(GTK_TABLE(table_body), image_p[i], snake_p[i][0], snake_p[i][0]+1, snake_p[i][1], snake_p[i][1]+1);
		gtk_widget_show(image_p[i]);
	}
	for(i = 0; i < segs_q; i++) {
		image_q[i] = gtk_image_new_from_file("./data/segment_q.png");
		gtk_table_attach_defaults(GTK_TABLE(table_body), image_q[i], snake_q[i][0], snake_q[i][0]+1, snake_q[i][1], snake_q[i][1]+1);
		gtk_widget_show(image_q[i]);
	}
	gtk_container_add(GTK_CONTAINER(frame1), table_body);
	
	gtk_widget_show_all(window);	
	gtk_main();
	return 0;
}
/*******************************************************************************************************************************************
function for dialog boxes
**********************************************************************************************************************************************************/
void capson() {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup
	(
		GTK_WINDOW(window),
	        GTK_DIALOG_DESTROY_WITH_PARENT,
        	GTK_MESSAGE_WARNING,
        	GTK_BUTTONS_OK,
        	"<b>IMP:- check if caps lock is on.....!!!</b>\n\n please turn it off to move second snake"
        ); 
	gtk_window_set_title(GTK_WINDOW(dialog), "Warning");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void game_over() {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new_with_markup
	(
		GTK_WINDOW(window),
	        GTK_DIALOG_DESTROY_WITH_PARENT,
        	GTK_MESSAGE_WARNING,
        	GTK_BUTTONS_OK,
        	"<b>!!!..GAME OVER..!!!</b>\n\n go for new game"
        );
	gtk_window_set_title(GTK_WINDOW(dialog), "Game over");
	gtk_dialog_run(GTK_DIALOG(dialog));   	//when we do this it's always model dialog	
	gtk_widget_hide(dialog);
}

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
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("./data/Snake.png", NULL);
	const gchar *authors[] = {"sudhanshu allurwar", NULL};
	const char *lic ="License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
                      "This is free software: you are free to change and redistribute it.\n"
                      "There is NO WARRANTY, to the extent permitted by law.\n";
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SnakeG");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.00"); 
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "GPL");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "you will definitely enjoy");
	gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
	gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), lic);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf);
	pixbuf = NULL;
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
        	"1. Use the arrow keys to move the worm. \n\n2. Collect food to score more and don't collide with other snake and with yourself also.\n\n3. And Continue"
        );
	gtk_window_set_title(GTK_WINDOW(dialog), "How To Play");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void show_about_gtk(GtkWidget *widget, gpointer data) {
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("./data/gtk.png", NULL);

	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "GTK+");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "3.0"); 
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), "GPL");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), "GTK is a used for writing GUI applications.\nfor more info visit this website\n");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://www.gtk.org");
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
	g_object_unref(pixbuf);
	pixbuf = NULL;
	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}	
