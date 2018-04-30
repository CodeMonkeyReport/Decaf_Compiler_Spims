/* File: ast_stmt.h
 * ----------------
 * The Stmt class and its subclasses are used to represent
 * statements in the parse tree.  For each statment in the
 * language (for, if, return, etc.) there is a corresponding
 * node class for that construct. 
 *
 * pp5: You will need to extend the Stmt classes to implement
 * code generation for statements.
 */


#ifndef _H_ast_stmt
#define _H_ast_stmt

#include "list.h"
#include "ast.h"
#include "codegen.h"

class Decl;
class VarDecl;
class Expr;
  
class Program : public Node
{
  protected:
     List<Decl*> *decls;
     
  public:
     Program(List<Decl*> *declList);
     static Hashtable<Location*> *globalSymbolTable;
     void Check();
     Location* Emit();
};

class Stmt : public Node
{
  public:
     Stmt() : Node() {}
     Stmt(yyltype loc) : Node(loc) {}
     virtual Location* Emit(CodeGenerator  *codeGen) { return NULL; };
     
};

class StmtBlock : public Stmt 
{
    public:
    List<Stmt*> *stmts;
    List<VarDecl*> *decls;
    StmtBlock(List<VarDecl*> *variableDeclarations, List<Stmt*> *statements);
    Location* Emit(CodeGenerator  *codeGen);
    
};

  
class ConditionalStmt : public Stmt
{
  protected:
    Expr *test;
    Stmt *body;
  
  public:
    ConditionalStmt(Expr *testExpr, Stmt *body);
    Location* Emit(CodeGenerator  *codeGen);
    
};

class LoopStmt : public ConditionalStmt 
{
  public:
    LoopStmt(Expr *testExpr, Stmt *body)
            : ConditionalStmt(testExpr, body) {}
    Location* Emit(CodeGenerator  *codeGen);
    
};

class ForStmt : public LoopStmt 
{
  protected:
    Expr *init, *step;
  
  public:
    ForStmt(Expr *init, Expr *test, Expr *step, Stmt *body);
    Location* Emit(CodeGenerator  *codeGen);
    
};

class WhileStmt : public LoopStmt 
{
  public:
    WhileStmt(Expr *test, Stmt *body) : LoopStmt(test, body) {}
    Location* Emit(CodeGenerator  *codeGen);
    
};

class IfStmt : public ConditionalStmt 
{
  protected:
    Stmt *elseBody;
  
  public:
    IfStmt(Expr *test, Stmt *thenBody, Stmt *elseBody);
    Location* Emit(CodeGenerator  *codeGen);
    
};

class BreakStmt : public Stmt 
{
  public:
    BreakStmt(yyltype loc) : Stmt(loc) {}
    Location *Emit(CodeGenerator *codeGen);
};

class ReturnStmt : public Stmt  
{
  protected:
    Expr *expr;
  
  public:
    ReturnStmt(yyltype loc, Expr *expr);
    Location* Emit(CodeGenerator  *codeGen);
    
};

class PrintStmt : public Stmt
{
  protected:
    List<Expr*> *args;
    
  public:
    PrintStmt(List<Expr*> *arguments);
    Location* Emit(CodeGenerator  *codeGen);
    
};


#endif
