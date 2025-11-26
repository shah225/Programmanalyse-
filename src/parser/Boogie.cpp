#include "Boogie.h"

#include <ranges>
#include <SMT.h>

#include "ast/statements/Assert.h"
#include "ast/statements/Assignment.h"
#include "ast/programs/Sequence.h"
#include "ast/programs/IfElse.h"
#include "ast/Program.h"
#include "ast/FunctionDefinition.h"
#include "ast/programs/While.h"

using namespace ast;

// helper
template<typename T>
void append(std::vector<T> &a, const std::vector<T> &b) {
    a.insert(std::end(a), std::begin(b), std::end(b));
}

void newSymbols(const TypedIdentifierList &newSymbols) {
    for (const auto &[type, identifier]: newSymbols) { newSymbol(identifier, type); }
}

std::any BoogieProgramVisitor::visitMain(BoogieParser::MainContext *context) {
    const auto firstFncCtx = context->proc_decl().at(0);
    return visitProc_decl(firstFncCtx);
}

std::any BoogieProgramVisitor::visitProc_decl(BoogieParser::Proc_declContext *context) {
    auto signature = visitProc_sign(context->proc_sign());
    const auto &[identifier, argList, retList] = std::any_cast<Signature>(signature);
    newSymbols(argList);
    newSymbols(retList);
    auto bodyImpl = visitImpl_body(context->impl_body());
    const auto &[localVariables, body] = std::any_cast<std::pair<TypedIdentifierList, std::shared_ptr<
        Sequence> > >(bodyImpl);
    return std::make_shared<FunctionDefinition>(identifier, argList, body);
}

std::any BoogieProgramVisitor::visitProc_sign(BoogieParser::Proc_signContext *context) {
    std::string identifier = context->Ident()->getText();
    auto argList = context->proc_sign_in()
                       ? std::any_cast<TypedIdentifierList>(visitProc_sign_in(context->proc_sign_in()))
                       : TypedIdentifierList{};
    auto retList = context->proc_sign_out()
                       ? std::any_cast<TypedIdentifierList>(visitProc_sign_out(context->proc_sign_out()))
                       : TypedIdentifierList{};
    return std::tuple{identifier, argList, retList};
}

std::any BoogieProgramVisitor::visitImpl_body(BoogieParser::Impl_bodyContext *context) {
    TypedIdentifierList localVariables;
    for (const auto &localVariableDeclaration: context->local_vars()) {
        // localVariableDeclaration coud declare multiple variables in parallel
        auto localVars = std::any_cast<TypedIdentifierList>(visitLocal_vars(localVariableDeclaration));
        append(localVariables, localVars);
    }
    // update symbol table
    newSymbols(localVariables);
    auto body = std::any_cast<std::shared_ptr<ast::Sequence> >(visitStmt_list(context->stmt_list()));
    return std::pair{localVariables, body};
}

std::any BoogieProgramVisitor::visitStmt_list(BoogieParser::Stmt_listContext *context) {
    std::vector<std::shared_ptr<const Program> > statements;
    for (const auto &child: context->children) {
        if (const auto stmt = dynamic_cast<BoogieParser::Label_or_cmdContext *>(child)) {
            statements.push_back(std::any_cast<std::shared_ptr<const Program> >(visitLabel_or_cmd(stmt)));
            // TODO: flatten sequences?
            // auto newStatements = std::any_cast<std::shared_ptr<Sequence>>(visitLabel_or_cmd(stmt));
            // for (const auto &newStatement : newStatements->programs()) {
            //     statements.push_back(newStatement);
            // }
        }
        if (const auto stmt = dynamic_cast<BoogieParser::Transfer_cmdContext *>(child)) {
            statements.push_back(std::any_cast<std::shared_ptr<Program> >(visitTransfer_cmd(stmt)));
        }
        if (const auto stmt = dynamic_cast<BoogieParser::Structured_cmdContext *>(child)) {
            statements.push_back(std::any_cast<std::shared_ptr<Program> >(visitStructured_cmd(stmt)));
        }
    }
    return std::make_shared<Sequence>(statements);
}

std::any BoogieProgramVisitor::visitLocal_vars(BoogieParser::Local_varsContext *context) {
    if (!context->attr().empty()) {
        throw std::runtime_error("Attributes not implemented for local variables");
    }
    return visitTyped_idents_wheres(context->typed_idents_wheres());
}

std::any BoogieProgramVisitor::visitLabel_or_cmd(BoogieParser::Label_or_cmdContext *context) {
    if (context->assign_cmd() != nullptr) {
        return visitAssign_cmd(context->assign_cmd());
    }
    if (context->call_cmd() != nullptr) {
        return visitCall_cmd(context->call_cmd());
    }
    if (context->assert_cmd() != nullptr) {
        return visitAssert_cmd(context->assert_cmd());
    }
	if (context->assume_cmd() != nullptr) {
        return visitAssume_cmd(context->assume_cmd());
    }
    throw std::runtime_error("not implemented");
}

std::any BoogieProgramVisitor::visitTransfer_cmd(BoogieParser::Transfer_cmdContext *context) {
    if (context->break_cmd() != nullptr) {
        throw std::runtime_error("not implemented");
    }
    if (context->while_cmd() != nullptr) {
        return visitWhile_cmd(context->while_cmd());
    }
    if (context->if_cmd() != nullptr) {
        return visitIf_cmd(context->if_cmd());
    }
    throw std::runtime_error("unknown transfer command");
}

std::any BoogieProgramVisitor::visitStructured_cmd(BoogieParser::Structured_cmdContext *context) {
    throw std::runtime_error("not implemented");
}

std::any BoogieProgramVisitor::visitAssert_cmd(BoogieParser::Assert_cmdContext *context) {
    const auto condition = std::any_cast<std::shared_ptr<ast::Expression> >(visitProposition(context->proposition()));
    const auto assert = std::make_shared<ast::Assert>(condition);
    return std::shared_ptr<const Program>(assert);
}

std::any BoogieProgramVisitor::visitAssign_cmd(BoogieParser::Assign_cmdContext *context) {
    IdentifierList lValues;
    for (const auto &identifierNode: context->Ident()) {
        auto identifier = identifierNode->getText();
        lValues.push_back(identifier);
    }
    const auto rValues = std::any_cast<std::vector<std::shared_ptr<ast::Expression> > >(
        visitDef_body(context->def_body()));
    // split into multiple assignments
    if (lValues.size() != rValues.size()) {
        throw std::runtime_error(
            "Number of lvalues is not equal to number of rvalues for assignment: " + context->getText());
    }
    std::vector<std::shared_ptr<const ast::Program> > assignments;
    for (int i = 0; i < lValues.size(); i++) {
        auto lhs = lValues.at(i);
        auto rhs = rValues.at(i);
        assignments.emplace_back(std::make_shared<ast::Assignment>(lhs, rhs));
    }
    const auto assign = std::make_shared<ast::Sequence>(assignments);
    return std::shared_ptr<const Program>(assign);
}

std::any BoogieProgramVisitor::visitDef_body(BoogieParser::Def_bodyContext *context) {
    return visitExprs(context->exprs());
}

std::any BoogieProgramVisitor::visitAssume_cmd(BoogieParser::Assume_cmdContext *context) {
    const auto condition = std::any_cast<std::shared_ptr<ast::Expression>>(visitProposition(context->proposition()));
    auto assume = std::make_shared<ast::Assume>(condition);
    return std::shared_ptr<const Program>(assume);
}


std::any BoogieProgramVisitor::visitCall_cmd(BoogieParser::Call_cmdContext *context) {
    throw std::runtime_error("not implemented");
}

std::any BoogieProgramVisitor::visitIf_cmd(BoogieParser::If_cmdContext *context) {
    auto guard = std::any_cast<std::shared_ptr<ast::Expression> >(visitGuard(context->guard()));
    auto thenBody = std::any_cast<std::shared_ptr<Sequence> >(visitStmt_list(context->stmt_list(0)));
    std::shared_ptr<Sequence> elseBody = nullptr;

    if (context->stmt_list().size() == 1) {
        const auto ifElse = std::make_shared<IfElse>(guard, thenBody);
        return std::shared_ptr<Program>(ifElse);
    }

    if (context->stmt_list().size() > 1) {
        // else
        elseBody = std::any_cast<std::shared_ptr<Sequence> >(visitStmt_list(context->stmt_list(1)));
    } else if (context->if_cmd() != nullptr) {
        // else if
        const auto ifElseStmt = std::any_cast<std::shared_ptr<ast::Statement> >(visitIf_cmd(context->if_cmd()));
        std::vector<std::shared_ptr<const Program> > ifElseStmtWrapped{ifElseStmt};
        elseBody = std::make_shared<Sequence>(ifElseStmtWrapped);
    }
    const auto ifElse = std::make_shared<IfElse>(guard, thenBody, elseBody);
    return std::shared_ptr<Program>(ifElse);
}


std::any BoogieProgramVisitor::visitWhile_cmd(BoogieParser::While_cmdContext *context) {
    auto guard = std::any_cast<std::shared_ptr<ast::Expression> >(visitGuard(context->guard()));
    auto body = std::any_cast<std::shared_ptr<Sequence> >(visitStmt_list(context->stmt_list()));

    auto &z3Ctx = guard->expression.ctx();
    z3::expr invariantExpr = z3Ctx.bool_val(true);
    for (const auto &invariantCtx: context->invariant()) {
        const auto partialInvariant = std::any_cast<std::shared_ptr<ast::Expression> >(visitInvariant(invariantCtx));
        invariantExpr = invariantExpr && partialInvariant->expression;
    }
    auto invariant = std::make_shared<ast::Expression>(invariantExpr);
    const auto whileCmd = std::make_shared<While>(guard, body, invariant);
    return std::shared_ptr<Program>(whileCmd);
}

std::any BoogieProgramVisitor::visitInvariant(BoogieParser::InvariantContext *context) {
    return visitExpr(context->expr());
}

std::any BoogieProgramVisitor::visitGuard(BoogieParser::GuardContext *context) {
    if (context->Ast()) {
        throw std::logic_error("not implemented");
    }
    return visitExpr(context->expr());
}

std::any BoogieProgramVisitor::visitType(BoogieParser::TypeContext *context) {
    const std::string type = context->type_atom()->getText();
    if (type == "int") { return integer; }
    if (type == "bool") { return boolean; }
    throw std::logic_error("not implemented");
}

std::any BoogieProgramVisitor::visitExprs(BoogieParser::ExprsContext *context) {
    std::vector<std::shared_ptr<ast::Expression> > expressionList;
    for (const auto &expr: context->expr()) {
        expressionList.push_back(std::any_cast<std::shared_ptr<ast::Expression> >(visitExpr(expr)));
    }
    return expressionList;
}

std::any BoogieProgramVisitor::visitExpr(BoogieParser::ExprContext *context) {
    if (context->implies_expr().size() != 1) {
        // Equivalences not supported
        throw std::logic_error("not implemented");
    }
    return visitImplies_expr(context->implies_expr().at(0));
}

std::any BoogieProgramVisitor::visitImplies_expr(BoogieParser::Implies_exprContext *context) {
    if (context->logical_expr().size() != 1) {
        // Implications not supported
        throw std::logic_error("not implemented");
    }
    return visitLogical_expr(context->logical_expr().at(0));
}

std::any BoogieProgramVisitor::visitLogical_expr(BoogieParser::Logical_exprContext *context) {
    auto relExpr = std::any_cast<std::shared_ptr<ast::Expression> >(visitRel_expr(context->rel_expr()));
    if (context->and_op()) {
        const auto andExpr = std::any_cast<std::shared_ptr<ast::Expression> >(visitAnd_expr(context->and_expr()));
        return std::make_shared<ast::Expression>(relExpr->expression && andExpr->expression);
    }
    if (context->or_op()) {
        const auto orExpr = std::any_cast<std::shared_ptr<ast::Expression> >(visitOr_expr(context->or_expr()));
        return std::make_shared<ast::Expression>(relExpr->expression || orExpr->expression);
    }
    return relExpr;
}

std::any BoogieProgramVisitor::visitAnd_expr(BoogieParser::And_exprContext *context) {
    const auto firstOperand = std::any_cast<std::shared_ptr<
        ast::Expression> >(visitRel_expr(context->rel_expr().at(0)));
    z3::expr expression = firstOperand->expression;
    std::vector<std::shared_ptr<ast::Expression> > operands;
    for (const auto &expr: context->rel_expr() | std::views::drop(1)) {
        const auto operand = std::any_cast<std::shared_ptr<ast::Expression> >(visitRel_expr(expr));
        expression = expression && operand->expression;
    }
    return std::make_shared<ast::Expression>(expression);
}

std::any BoogieProgramVisitor::visitOr_expr(BoogieParser::Or_exprContext *context) {
    const auto firstOperand = std::any_cast<std::shared_ptr<
        ast::Expression> >(visitRel_expr(context->rel_expr().at(0)));
    z3::expr expression = firstOperand->expression;
    std::vector<std::shared_ptr<ast::Expression> > operands;
    for (const auto &expr: context->rel_expr() | std::views::drop(1)) {
        const auto operand = std::any_cast<std::shared_ptr<ast::Expression> >(visitRel_expr(expr));
        expression = expression || operand->expression;
    }
    return std::make_shared<ast::Expression>(expression);
}

std::any BoogieProgramVisitor::visitRel_expr(BoogieParser::Rel_exprContext *context) {
    if (context->bv_term().size() == 1) {
        return std::any_cast<std::shared_ptr<ast::Expression> >(visitBv_term(context->bv_term().at(0)));
    }
    if (context->bv_term().size() == 2) {
        const auto leftOperand = std::any_cast<std::shared_ptr<ast::Expression> >(
            visitBv_term(context->bv_term().at(0)));
        const auto rightOperand = std::any_cast<std::shared_ptr<ast::Expression> >(
            visitBv_term(context->bv_term().at(1)));
        const auto lExpr = leftOperand->expression;
        const auto rExpr = rightOperand->expression;
        const std::string operation = context->rel_op().at(0)->getText();
        if (operation == "==") { return std::make_shared<ast::Expression>(lExpr == rExpr); }
        if (operation == "!=") { return std::make_shared<ast::Expression>(lExpr != rExpr); }
        if (operation == "<=") { return std::make_shared<ast::Expression>(lExpr <= rExpr); }
        if (operation == ">=") { return std::make_shared<ast::Expression>(lExpr >= rExpr); }
        if (operation == "<") { return std::make_shared<ast::Expression>(lExpr < rExpr); }
        if (operation == ">") { return std::make_shared<ast::Expression>(lExpr > rExpr); }
        throw std::logic_error("no matching operator for relational expression");
    }
    throw std::logic_error("not implemented");
}

std::any BoogieProgramVisitor::visitBv_term(BoogieParser::Bv_termContext *context) {
    if (context->term().size() != 1) {
        // Concatenations not supported
        throw std::logic_error("not implemented");
    }
    return visitTerm(context->term().at(0));
}

std::any BoogieProgramVisitor::visitTerm(BoogieParser::TermContext *context) {
    std::vector<std::shared_ptr<ast::Expression> > operands;
    for (const auto &expr: context->factor()) {
        operands.push_back(std::any_cast<std::shared_ptr<ast::Expression> >(visitFactor(expr)));
    }
    const auto operators = context->add_op();

    z3::expr expression = operands.at(0)->expression;
    for (int i = 0; i < operators.size(); ++i) {
        z3::expr operand = operands.at(i + 1)->expression;
        std::string op = operators.at(i)->getText();
        if (op == "+") { expression = expression + operand; } else if (
            op == "-") { expression = expression + operand; } else {
            throw std::logic_error("no matching operator for add expression");
        }
    }
    return std::make_shared<ast::Expression>(expression);
}

std::any BoogieProgramVisitor::visitFactor(BoogieParser::FactorContext *context) {
    std::vector<std::shared_ptr<ast::Expression> > operands;
    for (const auto &expr: context->power()) {
        operands.push_back(std::any_cast<std::shared_ptr<ast::Expression> >(visitPower(expr)));
    }
    const auto operators = context->mul_op();

    z3::expr expression = operands.at(0)->expression;
    for (int i = 0; i < operators.size(); ++i) {
        z3::expr operand = operands.at(i + 1)->expression;
        std::string op = operators.at(i)->getText();
        if (op == "*") { expression = expression * operand; } else if (
            op == "/" || op == "div") { expression = expression / operand; } else if (op == "%" || op == "mod") {
            expression = expression % operand;
        } else { throw std::logic_error("no matching operator for mul expression"); }
    }
    return std::make_shared<ast::Expression>(expression);
}

std::any BoogieProgramVisitor::visitPower(BoogieParser::PowerContext *context) {
    if (!context->power().empty()) {
        // Power operator not supported
        throw std::logic_error("not implemented");
    }
    return visitUnary_expr(context->unary_expr());
}

std::any BoogieProgramVisitor::visitUnary_expr(BoogieParser::Unary_exprContext *context) {
    if (context->minus_expr() != nullptr) {
        return visitMinus_expr(context->minus_expr());
    }
    if (context->neg_expr() != nullptr) {
        return visitNeg_expr(context->neg_expr());
    }
    if (context->coercion_expr() != nullptr) {
        return visitCoercion_expr(context->coercion_expr());
    }
    throw std::runtime_error("unknown unary expression");
}

std::any BoogieProgramVisitor::visitMinus_expr(BoogieParser::Minus_exprContext *context) {
    const auto expr = std::any_cast<std::shared_ptr<ast::Expression> >(visitUnary_expr(context->unary_expr()));
    return std::make_shared<ast::Expression>(-expr->expression);
}

std::any BoogieProgramVisitor::visitNeg_expr(BoogieParser::Neg_exprContext *context) {
    const auto expr = std::any_cast<std::shared_ptr<ast::Expression> >(visitUnary_expr(context->unary_expr()));
    return std::make_shared<ast::Expression>(!expr->expression);
}

std::any BoogieProgramVisitor::visitCoercion_expr(BoogieParser::Coercion_exprContext *context) {
    if (!context->Colon().empty()) {
        // Coercion not supported
        throw std::logic_error("not implemented");
    }
    return visitArray_expr(context->array_expr());
}

std::any BoogieProgramVisitor::visitArray_expr(BoogieParser::Array_exprContext *context) {
    if (!context->LBracket().empty()) {
        // Arrays not supported
        throw std::logic_error("not implemented");
    }
    return visitAtom_expr(context->atom_expr());
}

std::any BoogieProgramVisitor::visitAtom_expr(BoogieParser::Atom_exprContext *context) {
    if (context->bool_lit() != nullptr) { return visitBool_lit(context->bool_lit()); }
    if (context->dec() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->int_expr() != nullptr) { return visitInt_expr(context->int_expr()); }
    if (context->bv_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->var_expr() != nullptr) { return visitVar_expr(context->var_expr()); }
    if (context->fun_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->old_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->arith_coercion_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->paren_expr() != nullptr) { return visitParen_expr(context->paren_expr()); }
    if (context->forall_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->exists_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->lambda_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->if_then_else_expr() != nullptr) { throw std::logic_error("not implemented"); }
    if (context->code_expr() != nullptr) { throw std::logic_error("not implemented"); }
    throw std::runtime_error("unknown atom expression");
}

std::any BoogieProgramVisitor::visitInt_expr(BoogieParser::Int_exprContext *context) {
    return std::make_shared<ast::Expression>(z3Ctx.int_val(context->getText().c_str()));
}

std::any BoogieProgramVisitor::visitVar_expr(BoogieParser::Var_exprContext *context) {
    return std::make_shared<ast::Expression>(*symbolTable.at(context->getText()));
}

std::any BoogieProgramVisitor::visitBool_lit(BoogieParser::Bool_litContext *context) {
    const std::string boolLit = context->getText();
    if (boolLit == "false") { return std::make_shared<ast::Expression>(z3Ctx.bool_val(false)); }
    if (boolLit == "true") { return std::make_shared<ast::Expression>(z3Ctx.bool_val(true)); }
    throw std::runtime_error("unknown bool literal");
}

std::any BoogieProgramVisitor::visitParen_expr(BoogieParser::Paren_exprContext *context) {
    return visitExpr(context->expr());
}

std::any BoogieProgramVisitor::visitAttr_typed_idents_wheres(BoogieParser::Attr_typed_idents_wheresContext *context) {
    TypedIdentifierList typedIdentifiers;
    for (const auto &ctx: context->attr_typed_idents_where()) {
        auto newTypedIdentifiers = std::any_cast<TypedIdentifierList>(visitAttr_typed_idents_where(ctx));
        append(typedIdentifiers, newTypedIdentifiers);
    }
    return typedIdentifiers;
}

std::any BoogieProgramVisitor::visitAttr_typed_idents_where(BoogieParser::Attr_typed_idents_whereContext *context) {
    return visitTyped_idents_where(context->typed_idents_where());
}

std::any BoogieProgramVisitor::visitTyped_idents_wheres(BoogieParser::Typed_idents_wheresContext *context) {
    TypedIdentifierList typedIdentifiers;
    for (const auto &ctx: context->attr_typed_idents_where()) {
        auto newTypedIdentifiers = std::any_cast<TypedIdentifierList>(visitAttr_typed_idents_where(ctx));
        append(typedIdentifiers, newTypedIdentifiers);
    }
    return typedIdentifiers;
}

std::any BoogieProgramVisitor::visitTyped_idents_where(BoogieParser::Typed_idents_whereContext *context) {
    return std::any_cast<TypedIdentifierList>(visitTyped_idents(context->typed_idents()));
}

std::any BoogieProgramVisitor::visitTyped_idents(BoogieParser::Typed_identsContext *context) {
    const auto type = std::any_cast<PrimitiveType>(visitType(context->type()));
    const auto identifiers = std::any_cast<IdentifierList>(visitIdents(context->idents()));

    TypedIdentifierList typedIdentifierList;
    typedIdentifierList.reserve(identifiers.size());
    for (const auto &identifier: identifiers) {
        typedIdentifierList.emplace_back(type, identifier);
    }
    return typedIdentifierList;
}

std::any BoogieProgramVisitor::visitIdents(BoogieParser::IdentsContext *context) {
    IdentifierList identifierList;
    for (const auto &identifier: context->Ident()) {
        identifierList.push_back(identifier->getText());
    }
    return identifierList;
}

std::shared_ptr<const Program> parser::parseBoogie(const std::string &filePath) {
    std::ifstream stream;
    stream.open(filePath);
    antlr4::ANTLRInputStream input(stream);

    BoogieLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    BoogieParser parser(&tokens);

    auto *context = parser.main();
    BoogieProgramVisitor visitor;
    auto functionDefinition = std::any_cast<std::shared_ptr<ast::FunctionDefinition> >(visitor.visitMain(context));

    auto program = functionDefinition->body;
    return program;
}



