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
 * File: main.c
 */

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define INITIAL_STACK_SIZE 100
#define STACK_RESIZE_FACTOR 2

static GtkWidget *entry;
static GtkWidget *expression_label;

static int 
get_precedence(char op) 
{
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

static double 
apply_operator(double val1, double val2, char op, int *error) 
{
	*error = 0; /* Reset error flag */

	switch (op) {
	case '+':
		return val1 + val2;
	case '-':
		return val1 - val2;
	case '*':
		return val1 * val2;
	case '/':
		if (val2 == 0) {
			fprintf(stderr, "Error: Division by zero.\n");
			*error = 1; /* Set error flag */
			return 0.0;
		}
		return val1 / val2;
	default:
		*error = 1; /* Set error flag for unknown operator */
		return 0.0;
	}
}

static void 
resize_stack(void **stack, int *size, size_t element_size) 
{
	const int new_size = *size * STACK_RESIZE_FACTOR;
	void *new_stack = realloc(*stack, new_size * element_size);
	if (new_stack == NULL) {
		fprintf(stderr, "Error: Stack memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	*stack = new_stack;
	*size = new_size;
}

static double 
calculate_with_parentheses(const char *expression) 
{
	int stack_size = INITIAL_STACK_SIZE;
	double *numbers = malloc(stack_size * sizeof(double)); /* Dynamic stack for numbers */
	char *operators = malloc(stack_size * sizeof(char)); /* Dynamic stack for operators */
	if (numbers == NULL || operators == NULL) {
		fprintf(stderr, "Error: Initial stack memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}

	int num_top = -1, op_top = -1; /* Stack pointers */
	int error_flag = 0;
	int open_parentheses = 0;

	for (int i = 0; expression[i] != '\0'; i++) {
		if (isspace(expression[i]))
			continue;

		if (isdigit(expression[i])) {
			double value = 0;
			while (isdigit(expression[i])) {
				value = value * 10 + (expression[i] - '0');
				i++;
			}
			i--;
			if (num_top == stack_size - 1) {
				resize_stack((void **)&numbers, &stack_size, sizeof(double));
			}
			numbers[++num_top] = value;
		} else if (expression[i] == '(') {
			if (op_top == stack_size - 1) {
				resize_stack((void **)&operators, &stack_size, sizeof(char));
			}
			operators[++op_top] = expression[i];
			open_parentheses++;
		} else if (expression[i] == ')') {
			while (op_top != -1 && operators[op_top] != '(') {
				const double val2 = numbers[num_top--];
				const double val1 = numbers[num_top--];
				const char op = operators[op_top--];

				if (num_top == stack_size - 1) {
					resize_stack((void **)&numbers, &stack_size, sizeof(double));
				}
				numbers[++num_top] = apply_operator(val1, val2, op, &error_flag);
				if (error_flag) {
					free(numbers);
					free(operators);
					fprintf(stderr, "Error: Invalid operation.\n");
					return 0.0;
				}
			}
			if (op_top != -1) {
				op_top--; /* Pop the '(' from the stack */
			}
			open_parentheses--;
		} else {
			while (op_top != -1 && 
				   get_precedence(operators[op_top]) >= get_precedence(expression[i])) {
				const double val2 = numbers[num_top--];
				const double val1 = numbers[num_top--];
				const char op = operators[op_top--];

				if (num_top == stack_size - 1) {
					resize_stack((void **)&numbers, &stack_size, sizeof(double));
				}
				numbers[++num_top] = apply_operator(val1, val2, op, &error_flag);
				if (error_flag) {
					free(numbers);
					free(operators);
					fprintf(stderr, "Error: Invalid operation.\n");
					return 0.0;
				}
			}
			if (op_top == stack_size - 1) {
				resize_stack((void **)&operators, &stack_size, sizeof(char));
			}
			operators[++op_top] = expression[i];
		}
	}

	if (open_parentheses != 0) {
		free(numbers);
		free(operators);
		fprintf(stderr, "Error: Unmatched parentheses.\n");
		return 0.0;
	}

	while (op_top != -1) {
		const double val2 = numbers[num_top--];
		const double val1 = numbers[num_top--];
		const char op = operators[op_top--];

		if (num_top == stack_size - 1) {
			resize_stack((void **)&numbers, &stack_size, sizeof(double));
		}
		numbers[++num_top] = apply_operator(val1, val2, op, &error_flag);
		if (error_flag) {
			free(numbers);
			free(operators);
			fprintf(stderr, "Error: Invalid operation.\n");
			return 0.0;
		}
	}

	const double result = numbers[num_top];
	free(numbers);
	free(operators);
	return result;
}

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

	/* Validate the input expression */
	for (int i = 0; current_text[i] != '\0'; i++) {
		if (!isdigit(current_text[i]) && !isspace(current_text[i]) &&
		    strchr("+-*/()", current_text[i]) == NULL) {
			fprintf(stderr, "Error: Invalid input character '%c'.\n", current_text[i]);
			return;
		}
	}

	if (strcmp(label, "=") == 0) {
		gtk_label_set_text(GTK_LABEL(expression_label), current_text);

		const double result = calculate_with_parentheses(current_text);
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
	const gchar *buttons[] = {
		"7", "8", "9", "/",
		"4", "5", "6", "*",
		"1", "2", "3", "-",
		"0", ".", "(", ")",
		"C", "=", "+", ""
	};

	gtk_init(&argc, &argv);

	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Calculator");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

	GtkWidget* grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(window), grid);

	expression_label = gtk_label_new("");
	gtk_label_set_justify(GTK_LABEL(expression_label), GTK_JUSTIFY_LEFT);
	gtk_grid_attach(GTK_GRID(grid), expression_label, 0, 0, 4, 1);

	entry = gtk_entry_new();
	gtk_grid_attach(GTK_GRID(grid), entry, 0, 1, 4, 1);

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (*buttons[i * 4 + j] == '\0') 
				continue;
			GtkWidget* button = gtk_button_new_with_label(buttons[i * 4 + j]);
			g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
			gtk_grid_attach(GTK_GRID(grid), button, j, i + 2, 1, 1);
		}
	}

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}



