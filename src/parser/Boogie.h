#pragma once
#include <BoogieBaseVisitor.h>
#include <BoogieLexer.h>
#include <BoogieParser.h>

#include "ast/Program.h"

class BoogieProgramVisitor final : BoogieBaseVisitor {
public:
    std::any visitMain(BoogieParser::MainContext *context) override;

    // std::any visitAxiom_decl(BoogieParser::Axiom_declContext *context) override;
    // std::any visitConst_decl(BoogieParser::Const_declContext *context) override;
    // std::any visitFunc_decl(BoogieParser::Func_declContext *context) override;
    // std::any visitReturn_var_or_type(BoogieParser::Return_var_or_typeContext *context) override;
    // std::any visitImpl_decl(BoogieParser::Impl_declContext *context) override;
    std::any visitProc_decl(BoogieParser::Proc_declContext *context) override;

    // std::any visitType_decl(BoogieParser::Type_declContext *context) override;
    // std::any visitVar_decl(BoogieParser::Var_declContext *context) override;
    // std::any visitOrder_spec(BoogieParser::Order_specContext *context) override;
    // std::any visitVar_or_type(BoogieParser::Var_or_typeContext *context) override;
    std::any visitProc_sign(BoogieParser::Proc_signContext *context) override;

    // std::any visitProc_sign_in(BoogieParser::Proc_sign_inContext *context) override;
    // std::any visitProc_sign_out(BoogieParser::Proc_sign_outContext *context) override;
    std::any visitImpl_body(BoogieParser::Impl_bodyContext *context) override;

    std::any visitStmt_list(BoogieParser::Stmt_listContext *context) override;

    std::any visitLocal_vars(BoogieParser::Local_varsContext *context) override;

    // std::any visitSpec(BoogieParser::SpecContext *context) override;
    // std::any visitModifies_spec(BoogieParser::Modifies_specContext *context) override;
    // std::any visitRequires_spec(BoogieParser::Requires_specContext *context) override;
    // std::any visitEnsures_spec(BoogieParser::Ensures_specContext *context) override;
    std::any visitLabel_or_cmd(BoogieParser::Label_or_cmdContext *context) override;

    std::any visitTransfer_cmd(BoogieParser::Transfer_cmdContext *context) override;

    std::any visitStructured_cmd(BoogieParser::Structured_cmdContext *context) override;

    std::any visitAssert_cmd(BoogieParser::Assert_cmdContext *context) override;

    std::any visitAssign_cmd(BoogieParser::Assign_cmdContext *context) override;

    std::any visitDef_body(BoogieParser::Def_bodyContext *context) override;

    std::any visitAssume_cmd(BoogieParser::Assume_cmdContext *context) override;

    // std::any visitBreak_cmd(BoogieParser::Break_cmdContext *context) override;
    std::any visitCall_cmd(BoogieParser::Call_cmdContext *context) override;

    // std::any visitGoto_cmd(BoogieParser::Goto_cmdContext *context) override;
    // std::any visitHavoc_cmd(BoogieParser::Havoc_cmdContext *context) override;
    std::any visitIf_cmd(BoogieParser::If_cmdContext *context) override;



    std::any visitWhile_cmd(BoogieParser::While_cmdContext *context) override;

    std::any visitInvariant(BoogieParser::InvariantContext *context) override;

    // std::any visitYield_cmd(BoogieParser::Yield_cmdContext *context) override;
    // std::any visitCall_params(BoogieParser::Call_paramsContext *context) override;
    std::any visitGuard(BoogieParser::GuardContext *context) override;

    std::any visitType(BoogieParser::TypeContext *context) override;

    // std::any visitType_args(BoogieParser::Type_argsContext *context) override;
    // std::any visitType_atom(BoogieParser::Type_atomContext *context) override;
    // std::any visitMap_type(BoogieParser::Map_typeContext *context) override;
    std::any visitExprs(BoogieParser::ExprsContext *context) override;

    // std::any visitProposition(BoogieParser::PropositionContext *context) override;
    std::any visitExpr(BoogieParser::ExprContext *context) override;

    std::any visitImplies_expr(BoogieParser::Implies_exprContext *context) override;

    std::any visitLogical_expr(BoogieParser::Logical_exprContext *context) override;

    std::any visitAnd_expr(BoogieParser::And_exprContext *context) override;

    std::any visitOr_expr(BoogieParser::Or_exprContext *context) override;

    // std::any visitAnd_op(BoogieParser::And_opContext *context) override;
    // std::any visitOr_op(BoogieParser::Or_opContext *context) override;
    std::any visitRel_expr(BoogieParser::Rel_exprContext *context) override;

    // std::any visitRel_op(BoogieParser::Rel_opContext *context) override;
    std::any visitBv_term(BoogieParser::Bv_termContext *context) override;

    std::any visitTerm(BoogieParser::TermContext *context) override;

    // std::any visitAdd_op(BoogieParser::Add_opContext *context) override;
    std::any visitFactor(BoogieParser::FactorContext *context) override;

    // std::any visitMul_op(BoogieParser::Mul_opContext *context) override;
    std::any visitPower(BoogieParser::PowerContext *context) override;

    std::any visitUnary_expr(BoogieParser::Unary_exprContext *context) override;

    std::any visitMinus_expr(BoogieParser::Minus_exprContext *context) override;

    std::any visitNeg_expr(BoogieParser::Neg_exprContext *context) override;

    // std::any visitNeg_op(BoogieParser::Neg_opContext *context) override;
    std::any visitCoercion_expr(BoogieParser::Coercion_exprContext *context) override;

    std::any visitArray_expr(BoogieParser::Array_exprContext *context) override;

    std::any visitAtom_expr(BoogieParser::Atom_exprContext *context) override;

    // std::any visitBv_expr(BoogieParser::Bv_exprContext *context) override;
    std::any visitInt_expr(BoogieParser::Int_exprContext *context) override;

    std::any visitVar_expr(BoogieParser::Var_exprContext *context) override;

    // std::any visitFun_expr(BoogieParser::Fun_exprContext *context) override;
    std::any visitBool_lit(BoogieParser::Bool_litContext *context) override;

    // std::any visitDec(BoogieParser::DecContext *context) override;
    // std::any visitOld_expr(BoogieParser::Old_exprContext *context) override;
    // std::any visitArith_coercion_expr(BoogieParser::Arith_coercion_exprContext *context) override;
    std::any visitParen_expr(BoogieParser::Paren_exprContext *context) override;

    // std::any visitForall_expr(BoogieParser::Forall_exprContext *context) override;
    // std::any visitExists_expr(BoogieParser::Exists_exprContext *context) override;
    // std::any visitLambda_expr(BoogieParser::Lambda_exprContext *context) override;
    // std::any visitQuant_body(BoogieParser::Quant_bodyContext *context) override;
    // std::any visitBound_vars(BoogieParser::Bound_varsContext *context) override;
    // std::any visitIf_then_else_expr(BoogieParser::If_then_else_exprContext *context) override;
    // std::any visitCode_expr(BoogieParser::Code_exprContext *context) override;
    // std::any visitSpec_block(BoogieParser::Spec_blockContext *context) override;
    std::any visitAttr_typed_idents_wheres(BoogieParser::Attr_typed_idents_wheresContext *context) override;

    std::any visitAttr_typed_idents_where(BoogieParser::Attr_typed_idents_whereContext *context) override;

    std::any visitTyped_idents_wheres(BoogieParser::Typed_idents_wheresContext *context) override;

    std::any visitTyped_idents_where(BoogieParser::Typed_idents_whereContext *context) override;

    std::any visitTyped_idents(BoogieParser::Typed_identsContext *context) override;

    std::any visitIdents(BoogieParser::IdentsContext *context) override;

    // std::any visitType_params(BoogieParser::Type_paramsContext *context) override;
    // std::any visitAttr(BoogieParser::AttrContext *context) override;
    // std::any visitAttr_or_trigger(BoogieParser::Attr_or_triggerContext *context) override;
    // std::any visitAttr_param(BoogieParser::Attr_paramContext *context) override;
};

namespace parser {
    std::shared_ptr<const ast::Program> parseBoogie(const std::string &filePath);
}
