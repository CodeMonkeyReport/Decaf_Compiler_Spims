/* File: ast.cc
 * ------------
 */

#include "ast.h"
#include "ast_type.h"
#include "ast_decl.h"
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

    if (name == NULL)
        return NULL;

    while (p != NULL)
    {
        currentScope = p->symbolTable;
        if (currentScope == NULL)
        {
            p = p->parent;
            continue;
        }
        result = currentScope->Lookup(name);
        if (result != NULL)
            return result;
        
        p = p->parent;
    }
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

