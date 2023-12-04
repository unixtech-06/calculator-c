#include <gtk/gtk.h>
#include "../lib/expression_parser.h"

static GtkWidget *entry;
static GtkWidget *expression_label;

static 
void on_button_clicked(GtkWidget *widget, gpointer data) 
{
    const gchar *label = gtk_button_get_label(GTK_BUTTON(widget));
    if (label == NULL) {
        fprintf(stderr, "Error: Null label in button.\n");
        return;
    }

    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (current_text == NULL) {
        fprintf(stderr, "Error: Null text in entry widget.\n");
        return;
    }

    if (strcmp(label, "=") == 0) {
        gtk_label_set_text(GTK_LABEL(expression_label), current_text);

        double result = calculate_with_parentheses(current_text);
        char result_str[64];
        snprintf(result_str, sizeof(result_str), "%f", result);
        gtk_entry_set_text(GTK_ENTRY(entry), result_str);
    } else {
        gchar *new_text = g_strdup_printf("%s%s", current_text, label);
        if (new_text == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for new text.\n");
            return;
        }
        gtk_entry_set_text(GTK_ENTRY(entry), new_text);
        g_free(new_text);
    }
}

int 
main(int argc, char *argv[]) 
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    const gchar *buttons[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "(", ")",
        "C", "=", "+", ""}; // 括弧とクリアボタンを追加

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    expression_label = gtk_label_new("");
    gtk_label_set_justify(GTK_LABEL(expression_label), GTK_JUSTIFY_LEFT);
    gtk_grid_attach(GTK_GRID(grid), expression_label, 0, 0, 4, 1);

    entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 1, 4, 1);

    for (int i = 0; i < 5; ++i) { // 行数を5に増やす
        for (int j = 0; j < 4; ++j) {
            if (*buttons[i * 4 + j] == '\0') continue; // 空文字列の場合はボタンを作成しない
            button = gtk_button_new_with_label(buttons[i * 4 + j]);
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
            gtk_grid_attach(GTK_GRID(grid), button, j, i + 2, 1, 1);
        }
    }

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

