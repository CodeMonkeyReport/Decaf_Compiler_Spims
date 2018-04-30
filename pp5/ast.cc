/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_stmt.h"
#include "tac.h"
#include <string.h> // strdup
#include <stdio.h>  // printf

Node::Node(yyltype loc) {
    location = new yyltype(loc);
    parent = NULL;
}

Node::Node() {
    location = NULL;
    parent = NULL;
}

Location *Node::FindLocation(const char *name)
{
    Location *result = NULL;
    Hashtable<Location*> *currentScope;
    Node *p = this;
    Program *prog = NULL;
    if (name == NULL)
        return NULL;

    while (p != NULL)
    {
        //currentScope = p->symbolTable;
        prog = dynamic_cast<Program*>(p);
        if (prog != NULL)
        {
            return prog->globalSymbolTable->Lookup(name);
        }

        FnDecl *function = dynamic_cast<FnDecl*>(p);
        if (function == NULL)
        {
            p = p->parent;
            continue;
        }
        result = function->symbolTable->Lookup(name);
        if (result != NULL)
            return result;

    
        p = p->parent;
    }
    // if we reach the top it must be the global symbol table
    
    return NULL;
}
FnDecl* Node::FindFunctionDeclare()
{
    Node* p;
    for (p = this->parent; p != NULL; p = p->parent)
    {
        FnDecl* foundFuncDeclare = dynamic_cast<FnDecl*>(p);
        if (foundFuncDeclare != NULL)
        {
            return foundFuncDeclare;
        }
    }
    return NULL;
}


Identifier::Identifier(yyltype loc, const char *n) : Node(loc) {
    name = strdup(n);
} 

