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
#include "codegen.h"
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
    virtual Location* Emit(CodeGenerator *codeGen);
    virtual void Declare(Hashtable<Location*> *symbolTable) = 0;
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
};

class VarDecl : public Decl 
{    
  public:
    Type *type;
    Location* Emit(CodeGenerator *codeGen);
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
    Location* Emit(CodeGenerator *codeGen);
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
    void Declare(Hashtable<Location*> *symbolTable);
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    Location* Emit(CodeGenerator *codeGen);
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
  public:
    Hashtable<Location*> *symbolTable;
    Location* Emit(CodeGenerator *codeGen);
    int frameSize;
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    void Declare(Hashtable<Location*> *symbolTable);
};

#endif
