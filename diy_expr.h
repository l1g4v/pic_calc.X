#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIZE 60 //max size of the string to evaluate

// Stack data structure
typedef struct {
    int top;
    float items[MAX_SIZE];
} Stack;

// Function declarations
void push(Stack *stack, float value);
float pop(Stack *stack);
float evaluateExpression(const char *expression);


void push(Stack *stack, float value) {
    if (stack->top < MAX_SIZE - 1) {
        stack->items[++stack->top] = value;
    } else {
        //stack overflow, the pic will reset if CONFIG4L.STVREN is ON
        return;
    }
}

float pop(Stack *stack) {
    if (stack->top >= 0) {
        return stack->items[stack->top--];
    } else {
        //stack underflow, the pic will reset if CONFIG4L.STVREN is ON
        return -1;
    }
}

float evaluateExpression(const char *expression) {
    Stack operandStack;
    operandStack.top = -1;

    Stack operatorStack;
    operatorStack.top = -1;

    for (int i = 0; expression[i] != '\0'; ++i) {
        if (isdigit(expression[i]) || (expression[i] == '.' && isdigit(expression[i + 1]))) {
            // If the character is a digit or a decimal point, parse the number and push it to the operand stack
            char *endptr;
            float number = strtod(&expression[i], &endptr);
            push(&operandStack, number);
            i = endptr - expression - 1;
        } else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
            // If the character is an operator, pop and evaluate operators from the operator stack
            while (operatorStack.top >= 0 &&
                   ((expression[i] == '+' || expression[i] == '-') &&
                    (operatorStack.items[operatorStack.top] == '+' || operatorStack.items[operatorStack.top] == '-' ||
                     operatorStack.items[operatorStack.top] == '*' || operatorStack.items[operatorStack.top] == '/')) &&
                   (expression[i] == '*' || expression[i] == '/') &&
                   (operatorStack.items[operatorStack.top] == '*' || operatorStack.items[operatorStack.top] == '/')) {
                float operand2 = pop(&operandStack);
                float operand1 = pop(&operandStack);
                char op = pop(&operatorStack);

                if (op == '+') {
                    push(&operandStack, operand1 + operand2);
                } else if (op == '-') {
                    push(&operandStack, operand1 - operand2);
                } else if (op == '*') {
                    push(&operandStack, operand1 * operand2);
                } else if (op == '/') {
                    if (operand2 == 0) {
                        return 0;
                    }
                    push(&operandStack, operand1 / operand2);
                }
            }
            push(&operatorStack, expression[i]);
        }
    }

    // Pop and evaluate any remaining operators from the operator stack
    while (operatorStack.top >= 0) {
        float operand2 = pop(&operandStack);
        float operand1 = pop(&operandStack);
        char op = pop(&operatorStack);

        if (op == '+') {
            push(&operandStack, operand1 + operand2);
        } else if (op == '-') {
            push(&operandStack, operand1 - operand2);
        } else if (op == '*') {
            push(&operandStack, operand1 * operand2);
        } else if (op == '/') {
            if (operand2 == 0) {
                return 0;
                
            }
            push(&operandStack, operand1 / operand2);
        }
    }

    // The final result is on top of the operand stack
    return operandStack.items[operandStack.top];
}

