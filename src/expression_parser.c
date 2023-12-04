#include "../lib/expression_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static 
int get_precedence(char op) 
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

static 
double apply_operator(double val1, double val2, char op) 
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

double 
calculate_with_parentheses(const char *expression) 
{
    double numbers[100];
    char operators[100];
    int num_top = -1, op_top = -1;

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
                op_top--; // スタックから '(' をポップ
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

