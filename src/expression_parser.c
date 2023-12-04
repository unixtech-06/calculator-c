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
 * File: expression_parser.c
 */


#include "../lib/expression_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* Get the precedence of an operator. Higher values for higher precedence. */
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

/* Apply an operator to two values and return the result. */
static double
apply_operator(double val1, double val2, char op)
{
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

/* Calculate the value of an arithmetic expression with parentheses. */
double
calculate_with_parentheses(const char *expression)
{
	double numbers[100]; // Stack for numbers
	char operators[100]; // Stack for operators
	int num_top = -1, op_top = -1; // Stack pointers

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
			if (op_top != -1) {
				op_top--; // Pop the '(' from the stack
			}
		} else {
			while (op_top != -1 && 
				   get_precedence(operators[op_top]) >= get_precedence(expression[i])) {
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


