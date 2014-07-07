/**Calls "interface_palavra_e_dica.h"
 * Creates windows to user interact whith the game
 * Library used for create windows is gtk-2.0/gtk/gtk.h
/*
 * @autor Douglas
 * @autor Isabella
 * @autor Thiago
 * @autor Tiago
 * @since 04/072014
 * @version 2.0
 * 
 * 
*/
#include <gtk-2.0/gtk/gtk.h>
#include "interface_palavra_e_dicas.h"
#include <time.h>


typedef struct InterfaceStruct {/** <Structured used to be able to modify timer and hints and get player information at anytime.*/
    int player;
    GtkWidget *window;
    GtkWidget *timerLabel;
    GtkWidget *hintsLabel[3];
    GtkWidget *inputTextBox;

    time_t startTime;
    wordAndHints wordAndHints;
    int numberOfHints;
    int gotCorrectAnswer;

    int waitingForAnswer;
} Interface;

typedef struct InfoInterfaceStruct {
    int player;
    GtkWidget *window;
    GtkWidget *buttonAndPlayerHbox;
    GtkWidget *playerLabel;
    GtkWidget *infoLabel;
} InfoInterface;

typedef struct GetNameInterfaceStruct {
    GtkWidget *window;
    GtkWidget *inputTextBox;

    int gotName;
    char *name;
} GetNameInterface;

#define GUI_OWN
    #include "interface_gui.h"
#undef GUI_OWN


static Interface interfaces[2];/** <Vector that holds information about open GUIs.*/
static int numberOfPlayers;
static int playerPlaying = 0;
static InfoInterface infoInterface;
static GetNameInterface getNameInterface;


static quitApplication = 0;/** <Closing program control variables.*/
static stopTimeOuts[2] = {0, 0};
static int playerGaveUp = 0;
/**
 * Function type void destroy()
 *
 * If window closed abruptly print that player surrendered and quit program.
 */
void destroy(GtkWidget *widget, gpointer data) {
    
    if(!quitApplication) {/** <If application closed abruptly.*/
        g_print ("Player %s surrendered (closed application)\n", (char *) data);
        quitApplication = 1;
    }
    stopTimeOuts[0] = 1;
    stopTimeOuts[1] = 1;
}

int startInterface(int argc, char *argv[], int nPlayers, int playerToPlay) {
    
    gtk_init(&argc, &argv);/**<Start gtk.*/

    
    if(nPlayers == 2) {/** <Creates windows for all players.*/
        createInfoWindow(&infoInterface);

        sleep(1);
        createWindow(&interfaces[0], 1);
        createWindow(&interfaces[1], 2);
    } else {
        
        createWindow(&interfaces[playerToPlay-1], playerToPlay);/** <If only one window, set correct one.*/
        playerPlaying = playerToPlay;
    }

    
    numberOfPlayers = nPlayers;/** <Saves number of players.*/

    return 1;
}

void closeInterface() {
    quitApplication = 1;
     
    if (interfaces[0].window != NULL && GTK_IS_WIDGET(interfaces[0].window)) {/** <If window hasn't been closed, close it.*/
        gtk_widget_destroy(interfaces[0].window);
    }
    if (interfaces[1].window != NULL && GTK_IS_WIDGET(interfaces[1].window)) {
        gtk_widget_destroy(interfaces[1].window);
    }

    
    quitApplication = 0;/** <Allow application to be restarted.*/
    stopTimeOuts[0] = 1;
    stopTimeOuts[1] = 1;
    numberOfPlayers = 0;
}

int createWindow(Interface *gui, int player) {
    
    gui->player = player;/** <Initializes player conditions.*/
    gui->waitingForAnswer = FALSE;
    gui->gotCorrectAnswer = FALSE;

    
    initializeWindow(&(gui->window), player);/** <Intializes blank window.*/
    
    addItemsToWindow(gui);/** <Adds items to window.*/
    
    gtk_widget_show_all(gui->window);/** <Shows everything in window.*/
}


int initializeWindow(GtkWidget **window, int player) {/** <Initiate player's window.*/
    
    if (player != 1 && player != 2) {/** <If not valid player, raise error.*/
        printf("Error, invalid player number window opened\n");
        return 0;
    }

    
    *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);/** <Initialize window.*/

    
    char buf[50];/** <Set title.*/
    sprintf(buf, "megasenha - player %d", player);
    gtk_window_set_title(GTK_WINDOW(*window), buf);

    
    gtk_window_set_default_size(GTK_WINDOW(*window), 597, 177);/** <Set size and position and connects close window with destroy function.*/
    gtk_container_set_border_width (GTK_CONTAINER (*window), 10);
    if(player == 1) {
        gtk_window_move(GTK_WINDOW(*window), gdk_screen_width()*1/10, gdk_screen_height()/2 - 150);
        gtk_signal_connect (GTK_OBJECT(*window), "destroy", GTK_SIGNAL_FUNC (destroy), (gpointer) "1");
    } else {
        gtk_window_move(GTK_WINDOW(*window), gdk_screen_width()*9/10 - 500, gdk_screen_height()/2 - 150);
        gtk_signal_connect (GTK_OBJECT(*window), "destroy", GTK_SIGNAL_FUNC (destroy), (gpointer) "2");
    }

    
    sprintf(buf, "p%d.png", player);/** <Sets and creates icons for windows and tell windows manager not to put them together.*/
    gtk_window_set_icon(GTK_WINDOW(*window), createPixbuf(buf));
    sprintf(buf, "player %d", player);
    gtk_window_set_wmclass(GTK_WINDOW (*window), buf, "megasenha"); 

    return 1;
}

/** Function createPixbuf
  *
  * Generates icon image used by window for task bar.
  */ 
GdkPixbuf *createPixbuf(const gchar * filename)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if(!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
    }

    return pixbuf;
}

void addItemsToWindow(Interface *gui) {
    
    GtkWidget *parentVbox;/** <Creates vertical box to hold everything.*/
    parentVbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (gui->window), parentVbox);

    
    GtkWidget *timerHintsHbox;/** <Creates horizontal box to hold timer and hints.*/
    timerHintsHbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(parentVbox), timerHintsHbox, FALSE, FALSE, 0);

    
    addTimer(&(gui->timerLabel), timerHintsHbox);/** <Adds timer, hints and input text boxes.*/
    addHints(gui, timerHintsHbox);
    addTextBox(gui, parentVbox);
}

void addTimer(GtkWidget **label, GtkWidget *parentHbox) {
    
    GtkWidget *frame;/** <Creates timer frame.*/
    frame = gtk_frame_new("Timer");
    gtk_widget_set_size_request (frame, 100, 100);
    gtk_box_pack_start(GTK_BOX(parentHbox), frame, FALSE, FALSE, 0);

    
    *label = gtk_label_new ("Stopped");/**<Initializes it with word "Stopped".*/
    gtk_container_add (GTK_CONTAINER (frame), *label);
}

void addHints(Interface *gui, GtkWidget *parentHbox) {
    
    GtkWidget *frame;/** <Creates frame to hold hints.*/
    frame = gtk_frame_new("Hints");
    gtk_box_pack_start(GTK_BOX(parentHbox), frame, TRUE, TRUE, 0);

    
    GtkWidget *vbox;/** <Creates vertical box to align three hints.*/
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (frame), vbox);

    
    int i;/** <Writes "no hints yet" in three labels.*/
    for(i=0; i<3; i++) {
        gui->hintsLabel[i] = gtk_label_new ("No hints yet");
        gtk_box_pack_start(GTK_BOX(vbox), gui->hintsLabel[i], TRUE, TRUE, 0);
    }
}
/**Function void addTextBox
 * 
 * Adds input text box to window.
*/
void addTextBox(Interface *gui, GtkWidget *parentVbox) {
    
    GtkWidget *vbox;/** <Generates vertical box that holds items.*/
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(parentVbox), vbox, FALSE, FALSE, 0);

    
    GtkWidget *frame;/**<Generates input box frame.*/
    frame = gtk_frame_new ("Write word");
    gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

    
    createTextBox(gui, frame);/** <Create input box.*/
}

void sendText(GtkWidget *widget, gpointer data)
{
    
    Interface *gui = (Interface *) data;/** <Simplify notation.*/
    int numberOfHints = gui->numberOfHints;
    
    if(numberOfHints > 3 || !gui->waitingForAnswer) {/** <If already 3 hints over or time done do nothing.*/
        return;
    }

    
    char *answer = (char *) gtk_entry_get_text(GTK_ENTRY(gui->inputTextBox));/** <Get answer from entry box.*/

    
    if(strcmp(answer, gui->wordAndHints.word) != 0) {/** <Checks if right answer.*/ 
        
        if(numberOfHints < 3) {/** <Show new hint if less than three so far or set to not waiting player anymore.*/
            updateHintLabel(&(gui->hintsLabel[numberOfHints]), gui->wordAndHints.hints[numberOfHints]);
        } else {
            gui->waitingForAnswer = FALSE;
        }
        
        gui->numberOfHints++;/** <If wrong increase number of hints used.*/
    
    } else {/** <If correct set player to no more waitingand got correct answer.*/
        gui->waitingForAnswer = FALSE;
        gui->gotCorrectAnswer = TRUE;
    }

    
    g_print ("Player sent value \"%s\"\n", (char *) gtk_entry_get_text(GTK_ENTRY(gui->inputTextBox)));/** <Empty entry box.*/
    gtk_entry_set_text (GTK_ENTRY(gui->inputTextBox), "");
}

void createTextBox(Interface *gui, GtkWidget *frame)
{
    
    GtkWidget *hbox;/** <Creates horizontal box to hold text box and enter button.*/
    hbox = gtk_hbox_new(FALSE, 0);

    
    gui->inputTextBox = gtk_entry_new();/** <Creates input box and binds enter key (activate) to sendText function.*/
    gtk_entry_set_text (GTK_ENTRY (gui->inputTextBox), "");
    gtk_box_pack_start (GTK_BOX (hbox), gui->inputTextBox, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(gui->inputTextBox), "activate", GTK_SIGNAL_FUNC(sendText), (gpointer) gui);

    
    GtkWidget *button;/** <Creates enter button and binds click to sendText function.*/
    button = gtk_button_new_with_label("Enter");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(sendText), (gpointer) gui);

    gtk_container_add(GTK_CONTAINER(frame), hbox);
}

void updateTimerLabel (GtkWidget **label, double timePassed) {
    
    char buf[50], *color;/** <Calculates remaining time.*/
    int remainingTime = 15-timePassed;

    
    if(remainingTime <= 5) {/** <If less than five seconds left print red, else blue.*/
        color = "red";
    } else {
        color = "blue";
    }

    sprintf(buf, "<span foreground=\"%s\" font=\"25\">%d</span>", color, remainingTime);

    
    if(remainingTime == 0) {/** <If time over and wrong answer print red, if right answer print OK! in green.*/
        color = "red";
        sprintf(buf, "<span foreground=\"%s\" font=\"42.5\">%s</span>", color, "X");
    }
    if(remainingTime == -5) {
        color = "green";
        sprintf(buf, "<span foreground=\"%s\" font=\"30\">%s</span>", color, "OK!");
        remainingTime = 0;
    }

    
    gtk_label_set_markup(GTK_LABEL(*label), buf);/** <Sets timer text.*/
    gtk_widget_show(*label);
}

gboolean timerHandler(Interface *gui) {
    
    if(quitApplication || stopTimeOuts[gui->player - 1] ) {/** <If application quit or stop time outs is on, return false to stop time out.*/
        stopTimeOuts[gui->player - 1] = 0;
        return FALSE;
    }

    
    time_t now;/** <Gets difference between current and start time.*/
    double timePassed;
    now = time(NULL);
    timePassed = difftime(now, gui->startTime);

    
    if(!gui->waitingForAnswer) {/** <If not waiting for answer anymore set time to 0 or (if answer was correct) -5.*/
        timePassed = 15;
        if(gui->gotCorrectAnswer) {
            timePassed = 20;
        }
    }

    
    updateTimerLabel(&(gui->timerLabel), timePassed);/** <Update timer.*/

    
    if(timePassed >= 15) {/** <If time over stop timeouts.*/
        gui->waitingForAnswer = FALSE;
        return FALSE;
    }
    return TRUE;
}

void startTimer(Interface *gui) {
    
    if(stopTimeOuts[gui->player - 1] ) {/** <If stop time outs is on but there's no timeout running, set it to false.*/
        stopTimeOuts[gui->player - 1] = 0;
    }
    
    gui->startTime = time(NULL);/** <Sets starting time to current.*/
    
    
    g_timeout_add(1000, (GSourceFunc) timerHandler, (gpointer) gui);/** <Starts timeout to update timer every second and updates timerLabel to full time.*/
    updateTimerLabel(&(gui->timerLabel), 0);
}

void updateHintLabel (GtkWidget **label, char *newHint) {
    
    char buf[50];/** <Updates hint label in blue to new hint.*/
    sprintf(buf, "<span foreground=\"blue\" font=\"20\">%s</span>", newHint);

    gtk_label_set_markup(GTK_LABEL(*label), buf);
}

void restartHints (Interface *gui, wordAndHints newWordAndHint) {
    
    gui->waitingForAnswer = TRUE;/** <Restarts player stats for hints.*/
    gui->gotCorrectAnswer = FALSE;
    gui->wordAndHints = newWordAndHint;
    gui->numberOfHints = 1;

    
    updateHintLabel(&(gui->hintsLabel[0]), newWordAndHint.hints[0]);/** <Updates first hint to new first and hides other two.*/
    updateHintLabel(&(gui->hintsLabel[1]), "---");
    updateHintLabel(&(gui->hintsLabel[2]), "---");
}

int newWord(int player, wordAndHints newWordAndHint) {
    
    gtk_widget_grab_focus(interfaces[player-1].inputTextBox);/**<Brings player window to front and set focus to text entry box.*/
    gtk_window_present (GTK_WINDOW(interfaces[player-1].window));
    
    gtk_entry_set_text (GTK_ENTRY(interfaces[player-1].inputTextBox), "");/**<Restarts interface (hint, timer and entry box).*/
    restartHints(&interfaces[player-1], newWordAndHint); 
    startTimer(&interfaces[player-1]);
    restartHints(&interfaces[player-1], newWordAndHint); 
    startTimer(&interfaces[player-1]);
}

int updateInterface(void) {
    
    while (!quitApplication && gtk_events_pending ()) {/**<While changes to do and application hasn't been closed update.*/
        gtk_main_iteration ();
    }

    
    return !quitApplication;/** <Return if application still working*/
}
/**Funtion integer waintingPlayer
 * Returns true if any player still guessing.
*/
int waitingPlayer(void) {
    if(numberOfPlayers == 1) {
        if(interfaces[playerPlaying - 1].waitingForAnswer) {
            return 1;
        }
        return 0;
    }
    int i;
    for(i =0; i < numberOfPlayers; i++) {
        if(interfaces[i].waitingForAnswer) {
            return 1;
        }
    }
    return 0;
}
/**Function integer getPlayerScore
 * -1 if no correct answer, number of used hints otherwise
*/
int getPlayerScore(int player) {
    if(interfaces[player-1].gotCorrectAnswer) {
        return interfaces[player-1].numberOfHints;
    } else {
        return -1;
    }
}
/**Function integer createInfoWindow
 * Creates info window.
*/
int createInfoWindow(InfoInterface *infoGui) {
    
    initializeInfoWindow(&(infoGui->window));/** <Initializes blank info window.*/
    
    addItemsToInfoWindow(infoGui);/** <Adds items to window.*/
    
    gtk_widget_show_all(infoGui->window);/** <Shows everything in window.*/
}
/**Function integer initializeInfoWindow
 * Initiate info window.
*/
int initializeInfoWindow(GtkWidget **window) {
    
    *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);/** <Initialize window.*/

    
    char buf[50];/** <Set title.*/
    sprintf(buf, "megasenha - info window");
    gtk_window_set_title(GTK_WINDOW(*window), buf);

    
    gtk_window_set_default_size(GTK_WINDOW(*window), 797, 107);/** <Set size and position and connects close window with destroy function.*/
    gtk_container_set_border_width (GTK_CONTAINER (*window), 10);
    gtk_window_move(GTK_WINDOW(*window), gdk_screen_width()/2 - 797/2, gdk_screen_height()*1/20);
//    gtk_signal_connect (GTK_OBJECT(*window), "destroy", GTK_SIGNAL_FUNC (destroy), (gpointer) "1");

    
    gtk_window_set_icon(GTK_WINDOW(*window), createPixbuf("info.jpg"));/** <Sets and creates icons for windows and tell windows manager not to put them together.*/
    gtk_window_set_wmclass(GTK_WINDOW (*window), "infoWindow", "megasenha"); 

    gtk_widget_show_all(*window);

    return 1;
}

void addItemsToInfoWindow(InfoInterface *gui) {
    
    GtkWidget *parentHbox;/** Creates horizontal box to hold everything.*/
    parentHbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (gui->window), parentHbox);

    
    addPlayer(&(gui->playerLabel), parentHbox);/** <Adds timer, hints and input text boxes.*/
    addInfoLabel(gui, parentHbox);
    addQuitButton(gui, parentHbox);
}

void addInfoLabel(InfoInterface *gui, GtkWidget *parentVbox) {

    
    GtkWidget *frame;/** <Creates frame to hold hints.*/
    frame = gtk_frame_new("Info");
    gtk_box_pack_start(GTK_BOX(parentVbox), frame, TRUE, TRUE, 0);

    
    gui->infoLabel = gtk_label_new ("No info yet");/** <Writes "no info yet" in labels.*/
    gtk_container_add (GTK_CONTAINER (frame), gui->infoLabel);
}

void addPlayer(GtkWidget **label, GtkWidget *parentHbox) {
    
    GtkWidget *frame;/** <Creates timer frame.*/
    frame = gtk_frame_new("Player");
    gtk_widget_set_size_request (frame, 100, 100);
    gtk_box_pack_start(GTK_BOX(parentHbox), frame, FALSE, FALSE, 0);

    
    *label = gtk_label_new ("None");/** <Initializes it with word "Stopped".*/
    gtk_container_add (GTK_CONTAINER (frame), *label);
}
/** Function type void addQuitButton
 * Adds input text box to window.
*/
void addQuitButton(InfoInterface *gui, GtkWidget *parentHbox) {
    
    GtkWidget *frame;/** <Generates input box frame.*/
    frame = gtk_frame_new ("Surrender");
    gtk_box_pack_start (GTK_BOX (parentHbox), frame, FALSE, FALSE, 0);

    
    GtkWidget *button;/** <Creates give up button.*/
    button = gtk_button_new_with_label("Give up");
    gtk_container_add (GTK_CONTAINER (frame), button);
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(quitGame), (gpointer) gui);
}

void quitGame(GtkWidget *widget, gpointer data)
{
    
    InfoInterface *gui = (InfoInterface *) data;/** <Simplify notation.*/

    if(waitingPlayer() || !playerPlaying) {
        if(!playerPlaying) {
            g_print ("Player tried to give up, impossible in first round\n", playerPlaying);
            updateInfoLabel("Impossible to give up in first part!", "red");
        } else {
            g_print ("Player %d tried to give up, only possible in time between words\n", playerPlaying);
            updateInfoLabel("Only allowed to give up between words!", "red");
        }
        return;
    }

    
    updateInfoLabel("Player gave up!", "red");/** <Show that player gave up.*/
    playerGaveUp = 1;
    g_print ("Player %d gave up\n", playerPlaying);
}

void updateInfoLabel (char *newInfo, char *color) {
    
    char buf[150];/** <Updates hint label in blue to new hint.*/
    sprintf(buf, "<span foreground=\"%s\" font=\"20\">%s</span>", color, newInfo);

    gtk_label_set_markup(GTK_LABEL(infoInterface.infoLabel), buf);
}

void updatePlayerLabel (int newPlayer) {
    
    char buf[50];/** <Updates hint label in blue to new hint.*/
    sprintf(buf, "<span foreground=\"green\" font=\"40\">%d</span>", newPlayer);

    gtk_label_set_markup(GTK_LABEL(infoInterface.playerLabel), buf);
}

int gaveUp() {
    return playerGaveUp;
}

void startGetNameInterface() {
    
    initializeGetNameWindow(&(getNameInterface.window));/** <Intializes blank window.*/
    
    addItemsToGetNameWindow(&getNameInterface);/** <Adds items to window.*/
    
    gtk_widget_show_all(getNameInterface.window);/** <Shows everything in window.*/
}
/**Function type integer initializeGetNameWindow
 * Initiate get name window.
*/
int initializeGetNameWindow(GtkWidget **window) {
    
    *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);/**<Initialize window.*/

    
    char buf[50];/** <Set title.*/
    sprintf(buf, "megasenha - type your name");
    gtk_window_set_title(GTK_WINDOW(*window), buf);

    
    gtk_window_set_default_size(GTK_WINDOW(*window), 397, 107);/** <Set size and position and connects close window with destroy function.*/
    gtk_container_set_border_width (GTK_CONTAINER (*window), 10);
    gtk_window_move(GTK_WINDOW(*window), gdk_screen_width()/2 - 397/2, gdk_screen_height()*9/20);
//    gtk_signal_connect (GTK_OBJECT(*window), "destroy", GTK_SIGNAL_FUNC (destroy), (gpointer) "1");

    
    gtk_window_set_icon(GTK_WINDOW(*window), createPixbuf("info.jpg"));/** <Sets and creates icons for windows and tell windows manager not to put them together.*/
    gtk_window_set_wmclass(GTK_WINDOW (*window), "infoWindow", "megasenha"); 

    gtk_widget_show_all(*window);

    return 1;
}

void addItemsToGetNameWindow(GetNameInterface *gui) {
    
    GtkWidget *parentHbox;/** <Creates horizontal box to hold everything.*/
    parentHbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (gui->window), parentHbox);

    
    addGetNameTextBox(gui, parentHbox);/** <Adds timer, hints and input text boxes.*/
}
/**Function type void addGetNameTextBox
 * Adds input text box to get name window.
*/
void addGetNameTextBox(GetNameInterface *gui, GtkWidget *parentHbox) {
    
    GtkWidget *vbox;/** <Generates vertical box that holds items.*/
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(parentHbox), vbox, FALSE, FALSE, 0);

    
    GtkWidget *frame;/** <Generates input box frame.*/
    frame = gtk_frame_new ("Write your name");
    gtk_box_pack_start (GTK_BOX (parentHbox), frame, TRUE, TRUE, 0);

    
    createGetNameTextBox(gui, frame);/**<Create input box.*/
}

void createGetNameTextBox(GetNameInterface *gui, GtkWidget *frame)
{
    
    GtkWidget *hbox;/** <Creates horizontal box to hold text box and enter button.*/
    hbox = gtk_hbox_new(FALSE, 0);

    
    gui->inputTextBox = gtk_entry_new();/** <Creates input box and binds enter key (activate) to sendText function.*/
    gtk_entry_set_text (GTK_ENTRY (gui->inputTextBox), "");
    gtk_box_pack_start (GTK_BOX (hbox), gui->inputTextBox, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(gui->inputTextBox), "activate", GTK_SIGNAL_FUNC(sendName), (gpointer) gui);

    
    GtkWidget *button;/** <Creates enter button and binds click to sendText function.*/
    button = gtk_button_new_with_label("Enter");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(sendName), (gpointer) gui);

    gtk_container_add(GTK_CONTAINER(frame), hbox);
}

void sendName(GtkWidget *widget, gpointer data)
{
    
    GetNameInterface *gui = (GetNameInterface *) data;/** <Simplify notation.*/
    
    
    gui->name = (char *) gtk_entry_get_text(GTK_ENTRY(gui->inputTextBox));/** <Get name from entry box.*/

    
    if(strcmp(gui->name, "") != 0) {/** <Checks if right answer and if yes, sets player name as received.*/ 
        gui->gotName = 1;
    }

    
    g_print ("Player sent value \"%s\"\n", (char *) gtk_entry_get_text(GTK_ENTRY(gui->inputTextBox)));/** <Print player name.*/
}

char *getName()
{
    return getNameInterface.name;
}

int updateGetName()
{
    if(getNameInterface.gotName)
        return 0;

    gtk_main_iteration ();
    return 1;
}
