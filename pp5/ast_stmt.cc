/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "mips.h"

Hashtable<Location*> *Program::globalSymbolTable = new Hashtable<Location*>();

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::Check() {
    /* You can use your pp3 semantic analysis or leave it out if
     * you want to avoid the clutter.  We won't test pp5 against 
     * semantically-invalid programs.
     */
}
void Program::Emit() {
    /* pp5: here is where the code generation is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, generating instructions as you go.
     *      Each node can have its own way of translating itself,
     *      which makes for a great use of inheritance and
     *      polymorphism in the node classes.
     */
    Mips *mipsContext = new Mips();
    
    mipsContext->EmitPreamble();
    int globalVarCount = 0;
    for(int i = 0; i < this->decls->NumElements(); i++)
    {
        // fill global symbol table here
        Decl* declare = this->decls->Nth(i);
        VarDecl* varDeclare = dynamic_cast<VarDecl*>(declare);

        if (varDeclare == NULL)
            continue;
        
        Program::globalSymbolTable->Enter(varDeclare->id->name, new Location(gpRelative, globalVarCount * 4, varDeclare->id->name));
        globalVarCount++;
    }

    for(int i = 0; i < this->decls->NumElements(); i++)
    {
        this->decls->Nth(i)->Emit(mipsContext);
    }
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}
void StmtBlock::Emit(Mips *mipsContext)
{
    // TODO
    FnDecl* parentFunction = this->FindFunctionDeclare();
}
int StmtBlock::GetSize()
{
    // Start off at 0
    int totalSize = 0;

    // Each decl should add 4 bytes to the size
    totalSize += this->decls->NumElements() * 4;

    for(int i = 0; i < this->stmts->NumElements(); i++)
    {
        totalSize += this->stmts->Nth(i)->GetSize();
    }

    return totalSize;
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}
void ConditionalStmt::Emit(Mips *mipsContext)
{
    // TODO
}
int ConditionalStmt::GetSize()
{
    // TODO
    return 0;
}

void LoopStmt::Emit(Mips *mipsContext)
{
    // TODO
}
int LoopStmt::GetSize()
{
    // TODO
    return 0;
}

void WhileStmt::Emit(Mips *mipsContext)
{
    // TODO
}
int WhileStmt::GetSize()
{
    // TODO
    return 0;
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}
void ForStmt::Emit(Mips *mipsContext)
{
    // TODO   
}
int ForStmt::GetSize()
{
    // TODO
    return 0;
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}
void IfStmt::Emit(Mips *mipsContext)
{
    // TODO
}
int IfStmt::GetSize()
{
    // TODO
    return 0;
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
void ReturnStmt::Emit(Mips *mipsContext)
{
    // TODO
}
int ReturnStmt::GetSize()
{
    return expr->GetSize();
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}
void PrintStmt::Emit(Mips *mipsContext)
{
    // TODO
}
int PrintStmt::GetSize()
{
    // TODO
    return 0;
}

