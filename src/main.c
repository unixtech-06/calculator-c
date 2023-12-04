/*	$Project: calculator-c, Version: 2023, Last Maintenance: 2023/12/5 1:25 $	*/
/*
 * Copyright (c) 2023, Ryosuke
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author's Email: test@gmail.com
 * File: main.c
 */


#include <gtk/gtk.h>
#include "../lib/expression_parser.h"

static GtkWidget *entry;
static GtkWidget *expression_label;

/* Callback function for button click events. */
static void
on_button_clicked(GtkWidget *widget, gpointer data)
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

/* Main function to set up the GTK application and run the main loop. */
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
		"C", "=", "+", ""};

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

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (*buttons[i * 4 + j] == '\0') continue;
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
