/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "stdio.h"
#include "errors.h"
#include "codegen.h"

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
Location* Program::Emit() {
    /* pp5: here is where the code generation is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, generating instructions as you go.
     *      Each node can have its own way of translating itself,
     *      which makes for a great use of inheritance and
     *      polymorphism in the node classes.
     */
    CodeGenerator *codeGen = new CodeGenerator();
    
    int globalVarCount = 0;
    bool foundMain = false;
    for(int i = 0; i < this->decls->NumElements(); i++)
    {
        // fill global symbol table here
        Decl* declare = this->decls->Nth(i);
        VarDecl* varDeclare = dynamic_cast<VarDecl*>(declare);

        if (varDeclare == NULL)
        {
            declare->Declare(Program::globalSymbolTable);
        }
        else
        {   
            Location *newLocation = new Location(gpRelative, globalVarCount * 4, varDeclare->id->name);
            newLocation->type = varDeclare->type == Type::intType ? intType : strType;
            Program::globalSymbolTable->Enter(varDeclare->id->name, newLocation);
            globalVarCount++;
        }
        if (strcmp(declare->id->name, "main") == 0)
            foundMain = true;
    }
    if (foundMain == false)
    {
        ReportError::NoMainFound();
        exit(1);
    }

    for(int i = 0; i < this->decls->NumElements(); i++)
    {
        this->decls->Nth(i)->Emit(codeGen);
    }
    codeGen->DoFinalCodeGen();
    return NULL;
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}
Location* StmtBlock::Emit(CodeGenerator *codeGen)
{
    // If we cant find the parent function then somthing else went wrong
    FnDecl* parentFunction = this->FindFunctionDeclare();
    Assert(parentFunction != NULL);

    // Make space for locals in frame
    parentFunction->frameSize += this->decls->NumElements() * 4;

    for(int i = 0; i < this->decls->NumElements(); i++)
    {
        VarDecl *decl = dynamic_cast<VarDecl*>(this->decls->Nth(i));
        Assert(decl != NULL); // Should always be a VarDecl

        char *varName = decl->id->name;

        Location * newLocation = new Location(fpRelative, codeGen->stackFrameOffset, varName);
        newLocation->type = decl->type == Type::intType ? intType : strType;
        // Insert at the current offset, this handles the case where this is not the main function body
        parentFunction->symbolTable->Enter(varName, newLocation);

        // Update the current offset
        codeGen->stackFrameOffset -= 4;
    }
    for(int i = 0; i < this->stmts->NumElements(); i++)
    {
        this->stmts->Nth(i)->Emit(codeGen);
    }
    return NULL;
}


ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}
Location* ConditionalStmt::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}


Location* LoopStmt::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}

Location *BreakStmt::Emit(CodeGenerator *codeGen)
{
    codeGen->GenGoto(codeGen->breakLabel);
    return NULL;
}

Location* WhileStmt::Emit(CodeGenerator *codeGen)
{
    char *prevLabelName = codeGen->NewLabel();
    char *postLabelName = codeGen->NewLabel();
    strcpy(codeGen->breakLabel, postLabelName);
    Location *test;

    // Label before test to jump back
    codeGen->GenLabel(prevLabelName);
    test = this->test->Emit(codeGen);

    codeGen->GenIfZ(test, postLabelName);
    this->body->Emit(codeGen);

    codeGen->GenGoto(prevLabelName);
    codeGen->GenLabel(postLabelName);

    return NULL;
}


ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}
Location* ForStmt::Emit(CodeGenerator *codeGen)
{
    
    Location *test;
    Location *update;

    char *prevLabel = codeGen->NewLabel();
    char *postLabel = codeGen->NewLabel();
    strcpy(codeGen->breakLabel, postLabel);

    // Init
    this->init->Emit(codeGen);

    // Test
    codeGen->GenLabel(prevLabel);
    test = this->test->Emit(codeGen);
    codeGen->GenIfZ(test, postLabel);

    // Body
    this->body->Emit(codeGen);

    // Step
    this->step->Emit(codeGen);
    codeGen->GenGoto(prevLabel);
    codeGen->GenLabel(postLabel);

    return NULL;   
}


IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}
Location* IfStmt::Emit(CodeGenerator *codeGen)
{
    // Pre processing
    Location *test = this->test->Emit(codeGen);
    char *elseLabelName;
    if (this->elseBody != NULL)
        elseLabelName = codeGen->NewLabel();
    char *postLabelName = codeGen->NewLabel();

    if (this->elseBody != NULL)
    {
        codeGen->GenIfZ(test, elseLabelName);
        this->body->Emit(codeGen);
        codeGen->GenGoto(postLabelName);
        codeGen->GenLabel(elseLabelName);
        this->elseBody->Emit(codeGen);
        codeGen->GenLabel(postLabelName);
    }
    else
    {
        codeGen->GenIfZ(test, postLabelName);
        this->body->Emit(codeGen);
        codeGen->GenLabel(postLabelName);
    }
    return NULL;
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
Location* ReturnStmt::Emit(CodeGenerator *codeGen)
{
    // TODO
    Location *result;

    result = this->expr->Emit(codeGen);
    codeGen->GenReturn(result);
    return result;
}

  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}
Location* PrintStmt::Emit(CodeGenerator *codeGen)
{
    // TODO
    Location *arg;
    for(int i = 0; i < this->args->NumElements(); i++)
    {
        arg = this->args->Nth(i)->Emit(codeGen);
        BuiltIn whichBuiltin = arg->type == intType ? PrintInt : PrintString;

        codeGen->GenBuiltInCall(whichBuiltin, arg);
    }
    return NULL;
}


