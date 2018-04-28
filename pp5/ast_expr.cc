/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "mips.h"
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>



IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
void IntConstant::Emit(Mips *mipsContext)
{
    // TODO
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
void DoubleConstant::Emit(Mips *mipsContext)
{
    // TODO
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
void BoolConstant::Emit(Mips *mipsContext)
{
    // TODO
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}
void StringConstant::Emit(Mips *mipsContext)
{
    // TODO
}

void NullConstant::Emit(Mips *mipsContext)
{
    // TODO
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}
CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}
void CompoundExpr::Emit(Mips *mipsContext)
{

}

int CompoundExpr::GetSize()
{
    int totalSize = 0;

    // Add left and right values
    if (this->left != NULL)
        totalSize += this->left->GetSize();
    totalSize += this->right->GetSize();

    // Add 4 to save the new computed value
    totalSize += 4;

    return totalSize;
}

int AssignExpr::GetSize()
{
    int totalSize = 0;

    // Only the right value matters here
    totalSize += this->right->GetSize();

    return totalSize;
}
void AssignExpr::Emit(Mips *mipsContext)
{
    // Create the tac objects

} 
  
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}
void ArrayAccess::Emit(Mips *mipsContext)
{
    // TODO
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}
void FieldAccess::Emit(Mips *mipsContext)
{
    // TODO
    Location *fieldLocation = this->FindLocation(this->field->name);
}

void LValue::Emit(Mips *mipsContext)
{
    // TODO
}

void This::Emit(Mips *mipsContext)
{
    // TODO
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}
int Call::GetSize()
{
    int totalSize = 0;
    // Add 4 for each parameter passed
    totalSize += actuals->NumElements() * 4;

    // 4 for return value
    totalSize += 4;
    return totalSize;
}
void Call::Emit(Mips *mipsContext)
{
    // TODO
}

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}
void NewExpr::Emit(Mips *mipsContext)
{
    // TODO
}

NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}
void NewArrayExpr::Emit(Mips *mipsContext)
{
    // TODO
}
       
void ReadLineExpr::Emit(Mips *mipsContext)
{
    // TODO
}

void ReadIntegerExpr::Emit(Mips *mipsContext)
{
    // TODO
}