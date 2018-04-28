/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "mips.h"
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}
void Decl::Emit(Mips *mipsContext)
{

}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
void VarDecl::Declare(Hashtable<Location*> *symbolTable)
{

}
void VarDecl::Emit(Mips *mipsContext)
{
    // TODO
}


ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
}
void ClassDecl::Declare(Hashtable<Location*> *symbolTable)
{
    // TODO
}
void ClassDecl::Emit(Mips *mipsContext)
{
    // TODO
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}
void InterfaceDecl::Declare(Hashtable<Location*> *symbolTable)
{
    // TODO
}
void InterfaceDecl::Emit(Mips *mipsContext)
{
    // TODO
}
	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    this->localVariableCount = 0;
    this->symbolTable = new Hashtable<Location*>();
    body = NULL;
}
void FnDecl::Emit(Mips *mipsContext)
{
    // Null body means this is a prototype, no need to emit anything
    if (this->body == NULL)
        return;

    // If body is not a StmtBlock we have problems with the parser
    Label *funcLabel = new Label(this->id->name);
    BeginFunc *begin = new BeginFunc();

    // Do the stuff, add the declares
    begin->SetFrameSize(body->GetSize());

    EndFunc *end = new EndFunc();

    funcLabel->Emit(mipsContext);
    begin->Emit(mipsContext);
    body->Emit(mipsContext);
    end->Emit(mipsContext);
}
void FnDecl::Declare(Hashtable<Location*> *symbolTable)
{
    Location* previousDeclare = symbolTable->Lookup(this->id->name);
    int paramCount = this->formals->NumElements();

    for (int i = 0; i < this->formals->NumElements(); i++)
    {
        char *varName = this->formals->Nth(i)->id->name;
        int currentParameterOffset = (i + 1) * 4;
        symbolTable->Enter(varName, new Location(fpRelative, currentParameterOffset, varName));
    }
}
void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}




