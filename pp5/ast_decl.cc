/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "codegen.h"
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}
Location* Decl::Emit(CodeGenerator *codeGen)
{
    return NULL;
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
void VarDecl::Declare(Hashtable<Location*> *symbolTable)
{

}
Location* VarDecl::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
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
Location* ClassDecl::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}
void InterfaceDecl::Declare(Hashtable<Location*> *symbolTable)
{
    // TODO
}
Location* InterfaceDecl::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}
	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    this->localVariableCount = 0;
    this->frameSize = 0;
    this->symbolTable = new Hashtable<Location*>();
    body = NULL;
}
Location* FnDecl::Emit(CodeGenerator *codeGen)
{
    // Null body means this is a prototype, no need to emit anything
    if (this->body == NULL)
        return NULL;

    // If body is not a StmtBlock we have problems with the parser

    // Do the stuff, add the declares
    codeGen->GenLabel(this->id->name);
    BeginFunc *begin = codeGen->GenBeginFunc();

    // Start off the frame offset at -8 for the first local.
    codeGen->stackFrameOffset = codeGen->OffsetToFirstLocal;
    this->body->Emit(codeGen);

    begin->SetFrameSize( -1 * (codeGen->stackFrameOffset + 8 ));
    codeGen->GenEndFunc();
    return NULL;
}
void FnDecl::Declare(Hashtable<Location*> *symbolTable)
{
    int paramCount = this->formals->NumElements();

    for (int i = 0; i < this->formals->NumElements(); i++)
    {
        VarDecl *decl = dynamic_cast<VarDecl*>(this->formals->Nth(i));
        Assert(decl != NULL); // Should always be a VarDecl

        char *varName = decl->id->name;
        int currentParameterOffset = (i + 1) * 4;
        Location *newLocation = new Location(fpRelative, currentParameterOffset, varName);
        newLocation->type = decl->type == Type::intType ? intType : strType;
        this->symbolTable->Enter(varName, newLocation);
    }
}
void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}




