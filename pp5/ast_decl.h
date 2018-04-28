/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp5: You will need to extend the Decl classes to implement 
 * code generation for declarations.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "mips.h"
#include "ast.h"
#include "list.h"
#include "hashtable.h"

class Type;
class NamedType;
class Identifier;
class Stmt;

class Decl : public Node 
{
 
  public:
    Identifier *id;
    Decl(Identifier *name);
    virtual void Emit(Mips *mipsContext);
    virtual void Declare(Hashtable<Location*> *symbolTable) = 0;
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    void Emit(Mips *mipsContext);
    VarDecl(Identifier *name, Type *type);
    void Declare(Hashtable<Location*> *symbolTable);
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;

  public:
    void Emit(Mips *mipsContext);
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
    void Declare(Hashtable<Location*> *symbolTable);
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    void Emit(Mips *mipsContext);
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    void Declare(Hashtable<Location*> *symbolTable);
};

class FnDecl : public Decl 
{
  protected:
    int localVariableCount;
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    Hashtable<Location*> *symbolTable;
  public:
    void Emit(Mips *mipsContext);
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    void Declare(Hashtable<Location*> *symbolTable);
};

#endif
