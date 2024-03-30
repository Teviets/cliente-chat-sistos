#include <gtk/gtk.h>

typedef struct {
    GtkTextView *entrada;
    GtkTextView *salida;
} EntradaSalida;

static void enviar_texto(GtkWidget *widget, gpointer data) {
    EntradaSalida *es = (EntradaSalida *)data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(es->entrada);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    gchar *texto = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    // Obtener el texto ingresado y agregarlo al área de salida
    gtk_text_buffer_insert_at_cursor(gtk_text_view_get_buffer(es->salida), texto, -1);
    g_free(texto);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;

    GtkWidget *box;
    GtkWidget *entrada;
    GtkWidget *salida;
    GtkWidget *send;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_title(GTK_WINDOW(window), "Chat UVG");

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    entrada = gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(box), entrada, TRUE, TRUE, 0);

    salida = gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(box), salida, TRUE, TRUE, 0);

    send = gtk_button_new_with_label("Enviar");
    EntradaSalida es = {GTK_TEXT_VIEW(entrada), GTK_TEXT_VIEW(salida)};
    g_signal_connect(send, "clicked", G_CALLBACK(enviar_texto), &es);
    gtk_box_pack_start(GTK_BOX(box), send, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
