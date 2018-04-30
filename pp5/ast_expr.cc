/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "mips.h"
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "stdio.h"
#include <string.h>



IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}
Location* IntConstant::Emit(CodeGenerator *codeGen)
{
    Location *result;

    result = codeGen->GenLoadConstant(this->value);
    result->type = intType;
    return result;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
Location* DoubleConstant::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}
Location* BoolConstant::Emit(CodeGenerator *codeGen)
{
    Location *result;

    result = codeGen->GenLoadConstant(this->value);
    result->type = intType;
    return result;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}
Location* StringConstant::Emit(CodeGenerator *codeGen)
{
    Location *result;

    result = codeGen->GenLoadConstant(this->value);
    result->type = strType;
    return result;
}

Location* NullConstant::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}
Location* Operator::Emit(CodeGenerator *codeGen)
{
    return NULL;
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
Location* CompoundExpr::Emit(CodeGenerator *codeGen)
{
    Location *left = NULL;
    Location *right = NULL;
    Location *result = NULL;

    if(this->left != NULL)
        left = this->left->Emit(codeGen);
    right = this->right->Emit(codeGen);

    Assert(right != NULL);

    
    if (left == NULL) // Unary
    {
        if (strcmp(this->op->tokenString, "!") == 0) // Unary not
        {
            Location *oneConstant = codeGen->GenLoadConstant(1);

            // Boolean not is handles by the sub operation,
            // 1 - 0 (false) = 1 (true)
            // 1 - 1  (true) = 0 (false)
            result = codeGen->GenBinaryOp("-", oneConstant, right);
        }
        else // Otherwise unary -
        {
            Location *zeroConstant = codeGen->GenLoadConstant(0);
            
            // Unary minus handled as 0 - target
            result = codeGen->GenBinaryOp("-", zeroConstant, right);
        }
    }
    else             // Binary
    {
        result = codeGen->GenBinaryOp(this->op->tokenString, left, right);
    }
    result->type = intType;
    return result;
}

Location* RelationalExpr::Emit(CodeGenerator *codeGen)
{
    Location *left = NULL;
    Location *right = NULL;
    Location *result = NULL;

    if (this->left != NULL)
        left = this->left->Emit(codeGen);
    right = this->right->Emit(codeGen);
    Assert(left != NULL && right != NULL);

    char *opString = this->op->tokenString;
    Assert(opString != NULL);

    if (strcmp(opString, "<") == 0)
    {
        result = codeGen->GenBinaryOp("<", left, right);
    }
    else if (strcmp(opString, ">") == 0)
    {
        result = codeGen->GenBinaryOp("<", right, left);
    }
    else if (strcmp(opString, "<=") == 0)
    {
        Location *lessResult = codeGen->GenBinaryOp("<", left, right);
        Location *eqResult = codeGen->GenBinaryOp("==", left, right);

        result = codeGen->GenBinaryOp("||", lessResult, eqResult);
    }
    else if (strcmp(opString, ">=") == 0)
    {
        Location *lessResult = codeGen->GenBinaryOp("<", right, left);
        Location *eqResult = codeGen->GenBinaryOp("==", left, right);

        result = codeGen->GenBinaryOp("||", lessResult, eqResult);
    }
    return result;
}

Location* AssignExpr::Emit(CodeGenerator *codeGen)
{
    // Create the tac objects
    Location *left = NULL;
    Location *right = NULL;

    if (this->left != NULL)
        left = this->left->Emit(codeGen);
    right = this->right->Emit(codeGen);

    Assert(left != NULL);
    Assert(right != NULL);

    if (right->structure == reference)
    {
        right = codeGen->GenLoad(right);
    }
    if (left->structure == reference)
    {
        codeGen->GenStore(left, right);
        return NULL;
    }

    codeGen->GenAssign(left, right);

    return NULL;
} 
  
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}
Location* ArrayAccess::Emit(CodeGenerator *codeGen)
{
    Location *subscript;
    Location *baseLocation;
    Location *elementSize;
    Location *baseOffset;
    Location *errorMessage;
    Location *isSmallerThanZero;
    Location *isGreaterThanSize;
    Location *isEqualToSize;
    Location *isGreaterOrEqualToSize;
    Location *isValidSubscript;
    Location *zero;
    Location *arraySize;
    Location *result;

    char *postLabel = codeGen->NewLabel();

    subscript = this->subscript->Emit(codeGen);
    subscript = subscript->structure == reference ? codeGen->GenLoad(subscript) : subscript;

    // Array base address
    baseLocation = base->Emit(codeGen);
    if (baseLocation->structure == reference)
    {
        baseLocation = codeGen->GenLoad(baseLocation);
    }
    
    // Check if subscript > 0
    zero = codeGen->GenLoadConstant(0);
    isSmallerThanZero = codeGen->GenBinaryOp("<", subscript, zero);

    // Check if subscript > size
    arraySize = codeGen->GenLoad(baseLocation, -4);
    isGreaterThanSize = codeGen->GenBinaryOp("<", arraySize, subscript);
    isEqualToSize = codeGen->GenBinaryOp("==", subscript, arraySize);

    isGreaterOrEqualToSize = codeGen->GenBinaryOp("||", isGreaterThanSize, isEqualToSize);

    isValidSubscript = codeGen->GenBinaryOp("||", isGreaterOrEqualToSize, isSmallerThanZero);

    codeGen->GenIfZ(isValidSubscript, postLabel);

    // Print error here
    errorMessage = codeGen->GenLoadConstant("Decaf runtime error: Array index out of bounds\\n");
    codeGen->GenBuiltInCall(PrintString, errorMessage);
    codeGen->GenBuiltInCall(Halt);

    codeGen->GenLabel(postLabel);
    elementSize = codeGen->GenLoadConstant(4);
    baseOffset = codeGen->GenBinaryOp("*", subscript, elementSize);

    result = codeGen->GenBinaryOp("+", baseLocation, baseOffset);
    result->structure = reference;
    return result;
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) base->SetParent(this); 
    (field=f)->SetParent(this);
}
Location* FieldAccess::Emit(CodeGenerator *codeGen)
{
    // TODO
    Location *fieldLocation = this->FindLocation(this->field->name);
    Assert(fieldLocation != NULL);
    return fieldLocation;
}

Location* LValue::Emit(CodeGenerator *codeGen)
{
    // This isn't real
    return NULL;
}

Location* This::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) base->SetParent(this);
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

Location* Call::Emit(CodeGenerator *codeGen)
{
    Location *result;
    // if base is not null it may need to be an ACall
    
    // TODO figure out how to get return value later
    for(int i = this->actuals->NumElements() - 1; i >= 0; i--)
    {
        codeGen->GenPushParam(this->actuals->Nth(i)->Emit(codeGen));
    }
    result = codeGen->GenLCall(this->field->name, true);

    codeGen->GenPopParams(this->actuals->NumElements() * 4);
    return result;
}

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}
Location* NewExpr::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}

NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}
Location* NewArrayExpr::Emit(CodeGenerator *codeGen)
{
    Location *result;
    Location *elementCount;
    Location *elementCountWithSize;
    Location *totalSize;
    Location *elementSize;
    Location *zero;
    Location *one;
    Location *isGreaterThanZero;
    Location *errorMessage;
    char *postLabel = codeGen->NewLabel();

    // Calc size and get loc
    elementCount = this->size->Emit(codeGen);

    // Check if size > 0
    zero = codeGen->GenLoadConstant(0);

    isGreaterThanZero = codeGen->GenBinaryOp("<", elementCount, zero);
    codeGen->GenIfZ(isGreaterThanZero, postLabel);
    
    // Print error here
    errorMessage = codeGen->GenLoadConstant("Decaf runtime error: Array size is <= 0\\n");
    codeGen->GenBuiltInCall(PrintString, errorMessage);
    codeGen->GenBuiltInCall(Halt);

    // Now past the error handling section
    codeGen->GenLabel(postLabel);
    one = codeGen->GenLoadConstant(1);
    elementCountWithSize = codeGen->GenBinaryOp("+", one, elementCount);
    elementSize = codeGen->GenLoadConstant(4);
    totalSize = codeGen->GenBinaryOp("*", elementCountWithSize, elementSize);
    result = codeGen->GenBuiltInCall(Alloc, totalSize);

    codeGen->GenStore(result, elementCount);

    result = codeGen->GenBinaryOp("+", result, elementSize);

    return result;
}
       
Location* ReadLineExpr::Emit(CodeGenerator *codeGen)
{
    // TODO
    return NULL;
}

Location* ReadIntegerExpr::Emit(CodeGenerator *codeGen)
{
    Location *result;

    result = codeGen->GenBuiltInCall(ReadInteger);
    return result;
}