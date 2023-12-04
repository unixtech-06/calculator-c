#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

GtkWidget *entry;
GtkWidget *expression_label;

int 
get_precedence(char op) {
	switch (op) {
		case '*':
		case '/':
			return 2;
		case '+':
		case '-':
			return 1;
		default:
			return 0;
	}
}

double 
apply_operator(double val1, double val2, char op) {
	switch (op) {
		case '+': return val1 + val2;
		case '-': return val1 - val2;
		case '*': return val1 * val2;
		case '/': 
			  if (val2 == 0) {
				  fprintf(stderr, "Error: Division by zero.\n");
				  return 0.0;
			  }
			  return val1 / val2;
		default: 
			  return 0.0;
	}
}

double 
calculate(const char *expression) {
	double numbers[100];
	char operators[100];
	int num_top = -1, op_top = -1;

	for (int i = 0; expression[i] != '\0'; i++) {
		if (isspace(expression[i])) continue;

		if (isdigit(expression[i])) {
			double value = 0;
			while (isdigit(expression[i])) {
				value = value * 10 + (expression[i] - '0');
				i++;
			}
			i--;
			numbers[++num_top] = value;
		} else if (expression[i] == '(') {
			operators[++op_top] = expression[i];
		} else if (expression[i] == ')') {
			while (op_top != -1 && operators[op_top] != '(') {
				double val2 = numbers[num_top--];
				double val1 = numbers[num_top--];
				char op = operators[op_top--];

				numbers[++num_top] = apply_operator(val1, val2, op);
			}
			op_top--;
		} else {
			while (op_top != -1 && get_precedence(operators[op_top]) >= get_precedence(expression[i])) {
				double val2 = numbers[num_top--];
				double val1 = numbers[num_top--];
				char op = operators[op_top--];

				numbers[++num_top] = apply_operator(val1, val2, op);
			}
			operators[++op_top] = expression[i];
		}
	}

	while (op_top != -1) {
		double val2 = numbers[num_top--];
		double val1 = numbers[num_top--];
		char op = operators[op_top--];

		numbers[++num_top] = apply_operator(val1, val2, op);
	}

	return numbers[num_top];
}

void 
on_button_clicked(GtkWidget *widget, gpointer data) {
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

		double result = calculate(current_text);
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
main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *grid;
	GtkWidget *button;
	const gchar *buttons[] = {
		"7", "8", "9", "/",
		"4", "5", "6", "*",
		"1", "2", "3", "-",
		"0", ".", "=", "+"};
	int i, j;

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

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j) {
			button = gtk_button_new_with_label(buttons[i * 4 + j]);
			g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
			gtk_grid_attach(GTK_GRID(grid), button, j, i + 2, 1, 1);
		}
	}

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);
	gtk_main();

	system("leaks ./calculator");
	return 0;
}
