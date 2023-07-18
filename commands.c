#include <gtk/gtk.h>
#include "main.h"
#include "global.h"

int open_file(char * filename, struct Document * document) {
   
    // Delete buffer
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_start_iter(document->buffer, &start);
    gtk_text_buffer_get_end_iter(document->buffer, &end);

    gtk_text_buffer_delete(document->buffer, &start, &end);
    
    // Get file
    char * contents;
    gsize len;
    GError * err = NULL;
    g_file_get_contents(filename, &contents, &len, &err);

    if (err != NULL) {
        fprintf (stderr, "Unable to read file: %s\n", err->message);
        g_error_free (err);
        return 1;
    }
    
    // Insert file
    gtk_text_buffer_insert(document->buffer, &start, contents, len);
    gtk_text_buffer_set_modified(document->buffer, FALSE);
    strcpy(document->name, filename);
    free(contents);
    
    return 0;
}

void open_command(GtkWidget * self, struct Document * document) {
    
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File", GTK_WINDOW(gtk_widget_get_toplevel(self)), action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);

    gint res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        char * filename = gtk_file_chooser_get_filename (chooser);
        open_file(filename, document);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

void new_command() {
    main(0, NULL);
}

void save_command(GtkWidget * self, struct Document * document) {

    if (gtk_text_buffer_get_modified(document->buffer) == FALSE)
        return;
    
    // Collect all text
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_start_iter(document->buffer, &start);
    gtk_text_buffer_get_end_iter(document->buffer, &end);

    char * text = gtk_text_buffer_get_text(document->buffer, &start, &end, 0);
    
    if (document->name[0] == '\0') {
        printf("You have not opened a file yet. %s\n", document->name);
        return;
    }
    
    FILE * f = fopen(document->name, "w");
    fprintf(f, text);
    fclose(f);

}

int save_as_file(char * filename, struct Document * document) {
    
    strcpy(document->name, filename);

    // Collect all text
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_start_iter(document->buffer, &start);
    gtk_text_buffer_get_end_iter(document->buffer, &end);

    char * text = gtk_text_buffer_get_text(document->buffer, &start, &end, 0);

    FILE * f = fopen(document->name, "w");
    fprintf(f, text);
    fclose(f);

    return 0;
}

void save_as_command(GtkWidget * self, struct Document * document) {
    
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File", GTK_WINDOW(gtk_widget_get_toplevel(self)), action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Save"), GTK_RESPONSE_ACCEPT, NULL);

    gint res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        char * filename = gtk_file_chooser_get_filename (chooser);
        save_as_file(filename, document);
        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

void dialog_callback(GtkDialog * self, gint response, struct Document * document) {
    switch (response) {
        case 0:
            gtk_main_quit();
            break;
        case 1:
            return;
        case 2:
            save_as_command(GTK_WIDGET(self), document);
            break;
    }
}

void exit_command(GtkWidget * self, struct Document * document) {
    
    if (gtk_text_buffer_get_modified(document->buffer) == FALSE) {
        gtk_main_quit();
        return;
    }

    GtkWidget * close = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(self)), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Do you want to save your changes before closing this document?");
    
    gtk_dialog_add_buttons(GTK_DIALOG(close), "No", 0, "Cancel", 1, "Yes", 2, NULL);
    g_signal_connect(close, "response", G_CALLBACK(dialog_callback), document);
    
    gtk_dialog_run (GTK_DIALOG (close));
    gtk_widget_destroy (close);
}