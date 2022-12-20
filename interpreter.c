#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "value.h"
#include <string.h>
#include "talloc.h"
#include "linkedlist.h"
#include <ctype.h>
#include "parser.h"
#include "interpreter.h"

//print for quote statements
void printQuoteValue(Value *value){
    if (value->type == STR_TYPE){
        printf("\"%s\"", value->s);
    } else if (value->type == INT_TYPE){
       printf("%i",value->i);
    } else if (value->type == DOUBLE_TYPE){
        printf("%f", value->d);
    } else if (value->type == BOOL_TYPE){
        if (value->i == 0){
            printf("#f");
        } else if (value->i == 1){
            printf("#t");
        }
    } else if(value->type == NULL_TYPE){
        printf("()\n");
    } else if(value->type == SYMBOL_TYPE){
        printf("%s\n", value->s);
    } else if (value->type == CONS_TYPE){
        printf("(");
        Value *temp = value;
        while (temp->type != NULL_TYPE){
            if(temp->type != CONS_TYPE){
                printf(". ");
                printQuoteValue(temp);
                break;
            }
            printQuoteValue(car(temp));
            if (cdr(temp)->type != NULL_TYPE){
                printf(" ");
            }
            temp = cdr(temp);
        }
        printf(")\n");
    }
}

//prints out the value of our value object
void printValue(Value *value){
    if (value->type == STR_TYPE){
        printf("\"%s\"\n", value->s);
    } else if (value->type == INT_TYPE){
       printf("%i\n",value->i);
    } else if (value->type == DOUBLE_TYPE){
        printf("%f\n", value->d);
    } else if (value->type == BOOL_TYPE){
        if (value->i == 0){
            printf("#f\n");
        } else if (value->i == 1){
            printf("#t\n");
        }
    } else if(value->type == NULL_TYPE){
        printf("()\n");
    } else if(value->type == SYMBOL_TYPE){
        printf("%s\n", value->s);
    }  else if(value->type == CLOSURE_TYPE){
        printf("#<procedure>\n");
    } else if (value->type == CONS_TYPE){
        printf("(");
        Value *temp = value;
        while (temp->type != NULL_TYPE){
            if(temp->type != CONS_TYPE){
                printf(". ");
                printQuoteValue(temp);
                break;
            }
            printQuoteValue(car(temp));
            if (cdr(temp)->type != NULL_TYPE){
                printf(" ");
            }
            temp = cdr(temp);
        }
        printf(")\n");
    }
}

//primitive function to subtract two numbers together
Value *primitiveMinus(Value *numberList){
    bool isDouble = false;
    Value *number = reverse(numberList);
    double result = 0;
    if (car(number)->type == DOUBLE_TYPE){
            isDouble = true;
            result = car(number)->d;
    } else if (car(number)->type == INT_TYPE){
        result = car(number)->i;
    } else{
        printf("Evaluation error: wrong argument passed to + function\n");
        texit(0);
    }
    number = cdr(number);
    if (number->type == NULL_TYPE){
        Value *returnValue = talloc(sizeof(Value));
        if (!isDouble){
            returnValue->type = INT_TYPE;
            returnValue->i = (int) (-1 * result);
        } else{
            returnValue->type = DOUBLE_TYPE;
            returnValue->d = (-1 * result);
        }
        return returnValue;
        }
    while (number->type != NULL_TYPE){
        if (car(number)->type == DOUBLE_TYPE){
            isDouble = true;
            result = result - car(number)->d;
        } else if (car(number)->type == INT_TYPE){
            result = result - car(number)->i;
        } else{
            printf("Evaluation error: wrong argument passed to + function\n");
            texit(0);
        }
        number = cdr(number);
    }

    Value *returnValue = talloc(sizeof(Value));
    if (!isDouble){
        returnValue->type = INT_TYPE;
        returnValue->i = (int) result;
    } else{
        returnValue->type = DOUBLE_TYPE;
        returnValue->d = result;
    }
    return returnValue;
    
}

//returns true if arguments are increasing
Value *primitiveLess(Value *numberList){
    Value *number = reverse(numberList);
    double currNum = 0;
    if (car(number)->type == DOUBLE_TYPE){
        currNum = car(number)->d;
    } else if (car(number)->type == INT_TYPE){
        currNum = car(number)->i;
    } else{
        printf("Evaluation error\n");
        texit(0);
    }
    number = cdr(number);
    Value *returnBool = talloc(sizeof(Value));
    returnBool->type = BOOL_TYPE;
    while (number->type != NULL_TYPE){
        if (car(number)->type == DOUBLE_TYPE){
            double newNum = car(number)->d;
            if (newNum <= currNum){
                returnBool->i = 0;
                return returnBool;
            } else {
                currNum = newNum;
            }
        } else if (car(number)->type == INT_TYPE){
            double newNum = car(number)->i;
            if (newNum <= currNum){
                returnBool->i = 0;
                return returnBool;
            } else {
                currNum = newNum;
            }
        } else{
            printf("Evaluation error: wrong argument passed to + function\n");
            texit(0);
        }
        number = cdr(number);
    }
    returnBool->i = 1;
    return returnBool;
}

//returns true if arguments are all equal
Value *primitiveEqual(Value *numberList){
    Value *number = numberList;
    double currNum = 0;
    if (car(number)->type == DOUBLE_TYPE){
        currNum = car(number)->d;
    } else if (car(number)->type == INT_TYPE){
        currNum = car(number)->i;
    } else{
        printf("Evaluation error: wrong argument passed to + function\n");
        texit(0);
    }
    Value *returnBool = talloc(sizeof(Value));
    returnBool->type = BOOL_TYPE;
    while (number->type != NULL_TYPE){
        if (car(number)->type == DOUBLE_TYPE){
            double newNum = car(number)->d;
            if (newNum != currNum){
                returnBool->i = 0;
                return returnBool;
            }
        } else if (car(number)->type == INT_TYPE){
            double newNum = car(number)->i;
            if (newNum != currNum){
                returnBool->i = 0;
                return returnBool;
            }
        } else{
            printf("Evaluation error: wrong argument passed to + function\n");
            texit(0);
        }
        number = cdr(number);
    }
    returnBool->i = 1;
    return returnBool;
}

//returns true if arguments are decreasing
Value *primitiveGreater(Value *numberList){
    Value *number = reverse(numberList);
    double currNum = 0;
    if (car(number)->type == DOUBLE_TYPE){
        currNum = car(number)->d;
    } else if (car(number)->type == INT_TYPE){
        currNum = car(number)->i;
        //printf("%f", currNum);
    } else{
        printf("Evaluation error: wrong argument passed to + function\n");
        texit(0);
    }

    number = cdr(number);

    Value *returnBool = talloc(sizeof(Value));
    returnBool->type = BOOL_TYPE;
    while (number->type != NULL_TYPE){
        if (car(number)->type == DOUBLE_TYPE){
            double newNum = car(number)->d;
            if (newNum >= currNum){
                returnBool->i = 0;
                return returnBool;
            } else {
                currNum = newNum;
            }
        } else if (car(number)->type == INT_TYPE){
            double newNum = car(number)->i;
            if (newNum >= currNum){
                returnBool->i = 0;
                return returnBool;
            } else {
                currNum = newNum;
            }
        } else{
            printf("Evaluation error: wrong argument passed to + function\n");
            texit(0);
        }
        number = cdr(number);
    }
    returnBool->i = 1;
    return returnBool;
}

//primitive function to multiply several numbers together
Value *primitiveTimes(Value *numberList){
    if (numberList->type == NULL_TYPE || cdr(numberList)->type == NULL_TYPE){
        printf("Evaluation error: not enough arguments to * function\n");
        texit(0);
    }
    bool isDouble = false;
    double result = 1;
    Value *number = numberList;
    while (number->type != NULL_TYPE){
        if (car(number)->type == DOUBLE_TYPE){
            isDouble = true;
            result = result * car(number)->d;
        } else if (car(number)->type == INT_TYPE){
            result = result * car(number)->i;
        } else{
            printf("Evaluation error: wrong argument passed to + function\n");
            texit(0);
        }
        number = cdr(number);
    }

    Value *returnValue = talloc(sizeof(Value));
    if (!isDouble){
        returnValue->type = INT_TYPE;
        returnValue->i = (int) result;
    } else{
        returnValue->type = DOUBLE_TYPE;
        returnValue->d = result;
    }
    return returnValue;
    
}

//primitive function to divide 2 numbers 
Value *primitiveDivide(Value *numberList){
    if (numberList->type == NULL_TYPE || cdr(numberList)->type == NULL_TYPE || cdr(cdr(numberList))->type != NULL_TYPE){
        printf("Evaluation error: not enough arguments to / function\n");
        texit(0);
    }
    double result = 0;
    Value *number = reverse(numberList);
    if (car(number)->type == DOUBLE_TYPE){
        result = car(number)->d;
    } else if (car(number)->type == INT_TYPE){
        result = car(number)->i;
    } else{
        printf("Evaluation error: wrong argument passed to + function\n");
        texit(0);
    }

    number = cdr(number);

    if (car(number)->type == DOUBLE_TYPE){
        result = result / car(number)->d;
    } else if (car(number)->type == INT_TYPE){
        result = result / car(number)->i;
    } else{
        printf("Evaluation error: wrong argument passed to + function\n");
        texit(0);
    }

    Value *returnValue = talloc(sizeof(Value));
    if (result == (int) result){
        returnValue->type = INT_TYPE;
        returnValue->i = (int) result;
    } else{
        returnValue->type = DOUBLE_TYPE;
        returnValue->d = result;
    }

    return returnValue;
}

//primitive function to mod 2 numbers 
Value *primitiveMod(Value *numberList){
    if (numberList->type == NULL_TYPE || cdr(numberList)->type == NULL_TYPE || cdr(cdr(numberList))->type != NULL_TYPE){
        printf("Evaluation error: not enough arguments to / function\n");
        texit(0);
    }
    
    int number2 = car(numberList)->i;;
    int number1 = car(cdr(numberList))->i;

    int result = number1 % number2;

    Value *returnValue = talloc(sizeof(Value));
    returnValue->type = INT_TYPE;
    returnValue->i = result;
    return returnValue;
}

//primitive function to add several numbers together
Value *primitivePlus(Value *numberList){
    bool isDouble = false;
    double result = 0;
    Value *number = numberList;
    while (number->type != NULL_TYPE){
        if (car(number)->type == DOUBLE_TYPE){
            isDouble = true;
            result = result + car(number)->d;
        } else if (car(number)->type == INT_TYPE){
            result = result + car(number)->i;
        } else{
            printf("Evaluation error: wrong argument passed to + function\n");
            texit(0);
        }
        number = cdr(number);
    }

    Value *returnValue = talloc(sizeof(Value));
    if (!isDouble){
        returnValue->type = INT_TYPE;
        returnValue->i = (int) result;
    } else{
        returnValue->type = DOUBLE_TYPE;
        returnValue->d = result;
    }
    return returnValue;
    
}

//primitive function that returns true if empty list
Value *primitiveNull(Value *input){
    if(input->type == NULL_TYPE){
        printf("Evaluation error: too few arguments for function null?\n");
        texit(0);
    }
    if (cdr(input)->type != NULL_TYPE){
        printf("Evaluation error: too many arguments for function null?\n");
        texit(0);
    }
    Value *param = talloc(sizeof(Value));
    param = car(input);
    Value *returnValue = talloc(sizeof(Value));
    returnValue->type = BOOL_TYPE;
    if(param->type == NULL_TYPE){
        returnValue->i = 1;
        return returnValue;
    } else{
        returnValue->i = 0;
        return returnValue;
    }
}

//primitive that gets first item in list
Value *primitiveCar(Value *input){
    if(input->type == NULL_TYPE){
        printf("Evaluation error: too few arguments for function car\n");
        texit(0);
    }
    if (cdr(input)->type != NULL_TYPE){
        printf("Evaluation error: too many arguments for function car\n");
        texit(0);
    }
    Value *list = car(input);

    if (list->type != CONS_TYPE){
        printf("Evaluation error: argument for function car is not a list\n");
        texit(0);
    }

    return car(list);
}

//primitive that gets all items but first of list
Value *primitiveCdr(Value *input){
    if(input->type == NULL_TYPE){
        printf("Evaluation error: too few arguments for function cdr\n");
        texit(0);
    }
    if (cdr(input)->type != NULL_TYPE){
        printf("Evaluation error: too many arguments for function cdr\n");
        texit(0);
    }
    Value *list = car(input);

    if (list->type != CONS_TYPE){
        printf("Evaluation error: argument for function cdr is not a list\n");
        texit(0);
    }

    return cdr(list);
}
    
//primitive that combines two object
Value *primitiveCons(Value *input){
    if(input->type == NULL_TYPE || cdr(input)->type == NULL_TYPE){
        printf("Evaluation error: too few arguments for function cons\n");
        texit(0);
    }
    if (cdr(cdr(input))->type != NULL_TYPE){
        printf("Evaluation error: too many arguments for function cons\n");
        texit(0);
    }
    Value *input1 = car(input);
    Value *input2 = car(cdr(input));
    
    Value *newPair = talloc(sizeof(Value));
    newPair->type = CONS_TYPE;
    newPair->c.car = input2;
    newPair->c.cdr = input1;

    //printValue(newPair);
    //texit(0);
    return newPair;
}    


//adds new binding to frame
void addNewBinding(Frame *frame, Value *varName, Value *value){
    Value *newBinding = talloc(sizeof(Value));
    newBinding->type = CONS_TYPE;
    Value *varNameContainer = talloc(sizeof(Value));
    varNameContainer->type = CONS_TYPE;
    varNameContainer->c.car = varName;
    Value *varValueContainer = talloc(sizeof(Value));
    varValueContainer->type = CONS_TYPE;
    varValueContainer->c.car = value;
    varValueContainer->c.cdr = makeNull();
    varNameContainer->c.cdr = varValueContainer;
    newBinding->c.car = varNameContainer;
    newBinding->c.cdr = frame->bindings;
    frame->bindings = newBinding;
}

//binds a primitive to its function
void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    // Add primitive functions to top-level bindings list
    Value *value = talloc(sizeof(Value));
    value->type = PRIMITIVE_TYPE;
    value->pf = function;
    Value *nameContainer = talloc(sizeof(Value));
    nameContainer->type = SYMBOL_TYPE;
    nameContainer->s = name;
    addNewBinding(frame, nameContainer, value);
}


//searches the frame for the symbol that is passed in and returns the value of it that is stored there
Value *lookUpSymbol(Value *tree, Frame *frame){
    char *name = tree->s;
    Frame *currFrame = frame;
    bool hasParent;
    if (currFrame->parent){
        hasParent = true;
    } else {
        hasParent = false; 
    }
    bool runAgain = true;
    while (runAgain){
        Value *currBinding = currFrame->bindings;
        while (currBinding->type != NULL_TYPE){
            char *frameName = car(car(currBinding))->s;
            if (strcmp(frameName, name) == 0){
                Value *value = car(cdr(car(currBinding)));
                if (value->type == UNSPECIFIED_TYPE){
                    printf("Evaluation error: symbol has unspecified type.\n");
                    texit(0);
                }
                return value;
            }
            currBinding = currBinding->c.cdr;
        }
        if (hasParent){
            currFrame = currFrame->parent;
            if (currFrame->parent){
                hasParent = true;
            } else {
                hasParent = false; 
            }
        } else {
            runAgain = false;
        }
   }
    printf("Evaluation error: symbol 'x' not found.\n");
    texit(0);
    return makeNull();
}

//evaluates a cond statement, returning void type if no output
Value *evalCond(Value *args, Frame *frame){
    Value *test = args;
    while (test->type != NULL_TYPE){
        Value *boolExpr = car(car(test));
        Value *expr = car(cdr(car(test)));
        if (boolExpr->type == SYMBOL_TYPE){
            if (strcmp(boolExpr->s, "else") == 0){
                return eval(expr, frame);
            } else{
                printf("Evaluation error: else?\n");
                texit(0);
            }
        }
        boolExpr = eval(boolExpr, frame);
        if (boolExpr->i == 1){
            return eval(expr, frame);
        }
        test = cdr(test);
    }
    Value *returnVoid = talloc(sizeof(Value));
    returnVoid->type = VOID_TYPE;
    return returnVoid; 
}

//evaluates an if statement of scheme, requiring exactly 3 arguments
Value *evalIf(Value *args, Frame *frame){
    if (args->type == NULL_TYPE || cdr(args)->type == NULL_TYPE || cdr(cdr(args))->type == NULL_TYPE){
        printf("Evaluation error: incorrect params in if function\n");
        texit(0);
    }

    Value *cond = car(args);
    Value *trueExpr = car(cdr(args));
    Value *falseExpr = car(cdr(cdr(args)));
    
    int condResult = (eval(cond, frame))->i;
    if (condResult == 0){
       return eval(falseExpr, frame);
    } else if (condResult == 1){
       return eval(trueExpr, frame);
    } else {
        printf("Evaluation error: if function\n");
        texit(0);
        return makeNull();
    }
    
}

//evaluates a let statement, with variables beign declared as the first argument and then returning the value of the last exression
Value *evalLet(Value *args,Frame *frame, bool isStar){
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE){
        printf("Evaluation error: incorrect params in let function\n");
        texit(0);
    }

    Frame *f = talloc(sizeof(Frame));
    f->bindings = makeNull();
    f->parent = frame;
    
    
    Value *currArg = car(args);

    if (currArg->type != CONS_TYPE && currArg->type != NULL_TYPE){
        printf("Evaluation error: let function\n");
        texit(0);
    }

    while (currArg->type != NULL_TYPE){
        if (currArg->type != CONS_TYPE){
            printf("Evaluation error: let function\n");
            texit(0);
        }
        if (car(currArg)->type != CONS_TYPE || car(car(currArg))->type != SYMBOL_TYPE || cdr(car(currArg))->type != CONS_TYPE){
            printf("Evaluation error: let function\n");
            texit(0);
        }
        //add checks here
        Value *expr = car(cdr(car(currArg)));
        Value *var = car(car(currArg));
        if (!isStar){
            Value * tempBinding = f->bindings;
            while (tempBinding->type != NULL_TYPE){
                if (strcmp(car(car(tempBinding))->s, var->s) == 0){
                    printf("Evaluation error: duplicate variables\n");
                    texit(0);
                }
                tempBinding = cdr(tempBinding);
            }
        }
        Value *exprVal = NULL;
        if (isStar){
            Frame *tempFrame = talloc(sizeof(Value));
            tempFrame->bindings = f->bindings;
            tempFrame->parent = f->parent;
            exprVal = eval(expr, tempFrame);
        } else{
            exprVal = eval(expr, frame);
        }
        
        Value *newBinding = talloc(sizeof(Value));
        newBinding->type = CONS_TYPE;
        Value *varNameContainer = talloc(sizeof(Value));
        varNameContainer->type = CONS_TYPE;
        varNameContainer->c.car = var;
        Value *varValueContainer = talloc(sizeof(Value));
        varValueContainer->type = CONS_TYPE;
        varValueContainer->c.car = exprVal;
        varValueContainer->c.cdr = makeNull();
        varNameContainer->c.cdr = varValueContainer;
        newBinding->c.car = varNameContainer;
        newBinding->c.cdr = f->bindings;
        f->bindings = newBinding;
        currArg = currArg->c.cdr;
    }
    Value *execute = cdr(args);
    Value *result;
    Value *body = car(cdr(args));
    while (execute->type != NULL_TYPE){
        result = eval(car(execute), f);
        execute = cdr(execute);
    }
    return result;
}

//evaluates a begin statement (returns last expression)
Value *evalBegin(Value *args, Frame *frame){
    Value *result = talloc(sizeof(Value));
    result->type = VOID_TYPE;
    Value *expr = args;
    while (expr->type != NULL_TYPE){
        result = eval(car(expr), frame);
        expr = cdr(expr);
    }
    return result;
}

//evaluates a set bang, changing value of symbol in current frame
void setBang(Value *args, Frame *frame){
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE || car(args)->type == NULL_TYPE || car(cdr(args))->type == NULL_TYPE){
        printf("Evaluation error: bad params for define function\n");
        texit(0);
    } 
    Value *expr = car(cdr(args));
    Value *var = car(args);
    if (var->type != SYMBOL_TYPE){
        printf("Evaluation error: must define to a symbol\n");
        texit(0);
    }
    
    Value *tempBinding = frame->bindings;
    while (tempBinding->type != NULL_TYPE){
        if (strcmp(car(car(tempBinding))->s, var->s) == 0){
            Value *exprVal = eval(expr, frame);
            tempBinding->c.car->c.cdr->c.car = exprVal;
            return;
        }
        tempBinding = cdr(tempBinding);
    }

    Frame *oldFrame = frame;

    while (frame->parent != NULL){
        frame = frame->parent;

        Value *tempBinding = frame->bindings;
        while (tempBinding->type != NULL_TYPE){
            if (strcmp(car(car(tempBinding))->s, var->s) == 0){
                Value *exprVal = eval(expr, oldFrame);
                tempBinding->c.car->c.cdr->c.car = exprVal;
                return;
            }
            tempBinding = cdr(tempBinding);
        }
    }
    printf("Evaluation error: no symbol found for set!\n");
    texit(0);
}

//evaluates a let statement, with variables beign declared as the first argument and then returning the value of the last exression
Value *evalLetRec(Value *args,Frame *frame){
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE){
        printf("Evaluation error: incorrect params in let function\n");
        texit(0);
    }

    Frame *f = talloc(sizeof(Frame));
    f->bindings = makeNull();
    f->parent = frame;
    
    
    Value *currArg = car(args);

    if (currArg->type != CONS_TYPE && currArg->type != NULL_TYPE){
        printf("Evaluation error: let function\n");
        texit(0);
    }

    while (currArg->type != NULL_TYPE){
        if (currArg->type != CONS_TYPE){
            printf("Evaluation error: let function\n");
            texit(0);
        }
        if (car(currArg)->type != CONS_TYPE || car(car(currArg))->type != SYMBOL_TYPE || cdr(car(currArg))->type != CONS_TYPE){
            printf("Evaluation error: let function\n");
            texit(0);
        }
        
        Value *var = car(car(currArg));

        Value *newBinding = talloc(sizeof(Value));
        newBinding->type = CONS_TYPE;
        Value *varNameContainer = talloc(sizeof(Value));
        varNameContainer->type = CONS_TYPE;
        varNameContainer->c.car = var;
        Value *varValueContainer = talloc(sizeof(Value));
        varValueContainer->type = CONS_TYPE;
        Value *unspec = talloc(sizeof(Value));
        unspec->type = UNSPECIFIED_TYPE;
        varValueContainer->c.car = unspec;
        varValueContainer->c.cdr = makeNull();
        varNameContainer->c.cdr = varValueContainer;
        newBinding->c.car = varNameContainer;
        newBinding->c.cdr = f->bindings;
        f->bindings = newBinding;
        currArg = currArg->c.cdr;
    }
    
    currArg = car(args);
    while (currArg->type != NULL_TYPE){
        Value *expr = car(cdr(car(currArg)));
        Value *exprVal = eval(expr, f);
        currArg = cdr(currArg);
    }

    currArg = car(args);
    while (currArg->type != NULL_TYPE){
        Value *expr = car(cdr(car(currArg)));
        Value *var = car(car(currArg));
        Value *setBangArgs = talloc(sizeof(Value));
        setBangArgs->type = CONS_TYPE;
        setBangArgs->c.car = var;
        Value *exprContainer = talloc(sizeof(Value));
        exprContainer->type = CONS_TYPE;
        exprContainer->c.car = expr;
        exprContainer->c.cdr = makeNull();
        setBangArgs->c.cdr = exprContainer;
        setBang(setBangArgs, f);
        currArg = cdr(currArg);
    }

    Value *execute = cdr(args);
    Value *result;
    Value *body = car(cdr(args));
    while (execute->type != NULL_TYPE){
        result = eval(car(execute), f);
        execute = cdr(execute);
    }
    return result;
}

//evaluates the define function
void evalDefine(Value *args, Frame *frame){
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE || car(args)->type == NULL_TYPE || car(cdr(args))->type == NULL_TYPE){
        printf("Evaluation error: bad params for define function\n");
        texit(0);
    } 
    Value *expr = car(cdr(args));
    Value *var = car(args);
    if (var->type != SYMBOL_TYPE){
        printf("Evaluation error: must define to a symbol\n");
        texit(0);
    }
    Value *tempBinding = frame->bindings;
    while (tempBinding->type != NULL_TYPE){
        if (strcmp(car(car(tempBinding))->s, var->s) == 0){
            printf("Evaluation error: duplicate variables\n");
            texit(0);
        }
        tempBinding = cdr(tempBinding);
    }
    Value *exprVal = eval(expr, frame);

    addNewBinding(frame, var, exprVal);
}

Value *evalAnd(Value *args, Frame *frame){
    Value *currArg = args;
    Value *returnBool = talloc(sizeof(Value));
    returnBool->type = BOOL_TYPE;
    while (currArg->type != NULL_TYPE){
        Value *expr = eval(car(currArg), frame);
        if (expr->type != BOOL_TYPE){
            printf("Evaluation error: not a boolean\n");
            texit(0);
        }
        if (expr->i == 0){
            returnBool->i = 0;
            return returnBool;
        }
        currArg = cdr(currArg);
    }
    returnBool->i = 1;
    return returnBool;
}

//evaluates an or conditional statement
Value *evalOr(Value *args, Frame *frame){
    Value *currArg = args;
    Value *returnBool = talloc(sizeof(Value));
    returnBool->type = BOOL_TYPE;
    while (currArg->type != NULL_TYPE){
        Value *expr = eval(car(currArg), frame);
        if (expr->type != BOOL_TYPE){
            printf("Evaluation error: not a boolean\n");
            texit(0);
        }
        if (expr->i == 1){
            returnBool->i = 1;
            return returnBool;
        }
        currArg = cdr(currArg);
    }
    returnBool->i = 0;
    return returnBool;
}

//applioes a primitive to its function with arguments
Value *applyPrimitive(Value *function, Value *args){
    return (function->pf)(args);
}

//applies function to all of the arguments
Value *apply(Value *function, Value *args){
    if (function->type != CLOSURE_TYPE){
        printf("Evaluation error: not a function\n");
        texit(0);
    }

    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->parent = function->cl.frame;
    newFrame->bindings = makeNull();

    Value *currArg = args;
    Value *paramName = function->cl.paramNames;
    while (currArg->type != NULL_TYPE){
        if (paramName->type == NULL_TYPE){
            printf("Evaluation error: too many arguments in function\n");
            texit(0);
        }
        addNewBinding(newFrame, car(paramName), car(currArg));

        currArg = cdr(currArg);
        paramName = cdr(paramName);
    }

    if (paramName->type != NULL_TYPE){
        printf("Evaluation error: not enough arguments in function\n");
        texit(0);
    }

    return eval(function->cl.functionCode, newFrame);
}

//evaluates a lambda function
Value *evalLambda(Value *args, Frame *frame){
    if (args->type != CONS_TYPE || cdr(args)->type != CONS_TYPE){
        printf("Evaluation error: bad formatting\n");
        texit(0);
    }

    Value *closure = talloc(sizeof(Value));
    closure->type = CLOSURE_TYPE;

    Value *closureParams = makeNull();
    Value *currArg = car(args);
    while (currArg->type != NULL_TYPE){
        if (car(currArg)->type != SYMBOL_TYPE){
            printf("Evaluation error: not a symbol\n");
            texit(0);
        }

        Value *tempParam = closureParams;
        while (tempParam->type != NULL_TYPE){
            if (strcmp(car(tempParam)->s, car(currArg)->s) == 0){
                printf("Evaluation error: repeated symbol\n");
                texit(0);
            }
    
            tempParam = cdr(tempParam);
        }

        Value *varContainer = talloc(sizeof(Value));
        varContainer->type = CONS_TYPE;
        varContainer->c.car = car(currArg);
        varContainer->c.cdr = closureParams;
        closureParams = varContainer;
        
        currArg = cdr(currArg);
    }

    closure->cl.paramNames = closureParams;
    closure->cl.functionCode = car(cdr(args));
    closure->cl.frame = frame;

    return closure;
}

//loops thorugh our s expressions and evaluates each of them
void interpret(Value *tree){
    Frame *frame = talloc(sizeof(Frame));
    frame->bindings = makeNull();
    frame->parent = NULL;
    Value *currS = tree;

    bind("+", primitivePlus, frame);
    bind("-", primitiveMinus, frame);
    bind("<", primitiveLess, frame);
    bind(">", primitiveGreater, frame);
    bind("=", primitiveEqual, frame);
    bind("*", primitiveTimes, frame);
    bind("/", primitiveDivide, frame);
    bind("modulo", primitiveMod, frame);
    bind("null?", primitiveNull, frame);
    bind("car", primitiveCar, frame);
    bind("cdr", primitiveCdr, frame);
    bind("cons", primitiveCons, frame);
    
    while (currS->type != NULL_TYPE){
        printValue(eval(car(currS), frame));
        currS = cdr(currS);
    }
}

//gets the value of an expression based on its type
Value *eval(Value *tree, Frame *frame) {
   switch (tree->type)  {
     case INT_TYPE: {
        Value *val = talloc(sizeof(Value));
        val->type = INT_TYPE;
        val->i = tree->i;
        return val;
      }

      case NULL_TYPE: {
        return makeNull();
      }

      case UNSPECIFIED_TYPE: {
        printf("Evaluation error: unspecified type\n");
        texit(0);
      }

      case BOOL_TYPE: {
        Value *val = talloc(sizeof(Value));
        val->type = BOOL_TYPE;
        val->i = tree->i;
        return val;
      }

      case DOUBLE_TYPE: {
         Value *val = talloc(sizeof(Value));
         val->type = DOUBLE_TYPE;
         val->d = tree->d;
         return val;
      }
    
     case STR_TYPE: {
        Value *val = talloc(sizeof(Value));
        val->type = STR_TYPE;
        val->s = tree->s;
        return val;
     }  
     case SYMBOL_TYPE: {
        return lookUpSymbol(tree, frame);
        break;
     }  
     case PRIMITIVE_TYPE: {
        return lookUpSymbol(tree, frame);
        break;
     }
     case CONS_TYPE: {
        Value *first = car(tree);
        Value *args = cdr(tree);

        if (strcmp(first->s, "if") == 0) {
            Value *result = evalIf(args,frame);
            return result;
        } else if (strcmp(first->s, "cond") == 0){
            Value *result = evalCond(args, frame);
            return result;
        } else if (strcmp(first->s, "let") == 0){
            Value *result = evalLet(args, frame, false);
            return result;
        } else if (strcmp(first->s, "and") == 0){
            Value *result = evalAnd(args, frame);
            return result;
        } else if (strcmp(first->s, "or") == 0){
            Value *result = evalOr(args, frame);
            return result;
        } else if (strcmp(first->s, "begin") == 0){
            Value *result = evalBegin(args, frame);
            return result;
        } 
        else if (strcmp(first->s, "let*") == 0){
            Value *result = evalLet(args, frame, true);
            return result;
        }
        else if (strcmp(first->s, "letrec") == 0){
            Value *result = evalLetRec(args,frame);
            return result;
        } else if (strcmp(first->s, "quote") == 0){
            if (args->type != CONS_TYPE || cdr(args)->type != NULL_TYPE){
                printf("Evaluation error: incorrect formatting for function quote\n");
                texit(0);
            }
            return car(args);
        } else if (strcmp(first->s, "define") == 0){
            evalDefine(args,frame);
            Value *result = talloc(sizeof(Value));
            result->type = VOID_TYPE;
            return result;
        }  else if (strcmp(first->s, "set!") == 0){
            setBang(args,frame);
            Value *result = talloc(sizeof(Value));
            result->type = VOID_TYPE;
            return result;
        } else if (strcmp(first->s, "lambda") == 0){
            return evalLambda(args,frame);
        } else {
            // If not a special form, evaluate the first, evaluate the args, then
            // apply the first to the args.
            Value *evaledOperator = eval(first, frame);
            Value *evaledArgs = makeNull();
            Value *currArg = args;
            while (currArg->type != NULL_TYPE){
                Value *evalArg = eval(car(currArg), frame);
                Value *newCons = talloc(sizeof(Value));
                newCons->type = CONS_TYPE;
                newCons->c.car = evalArg;
                newCons->c.cdr = evaledArgs;
                evaledArgs = newCons;

                currArg = cdr(currArg);
            }
            if (evaledOperator->type == PRIMITIVE_TYPE){
                return applyPrimitive(evaledOperator, evaledArgs);
            }
            return apply(evaledOperator, evaledArgs);
        }
        break;
     }

        default: {
         return tree;
     }

      
    }    
      return makeNull();
}