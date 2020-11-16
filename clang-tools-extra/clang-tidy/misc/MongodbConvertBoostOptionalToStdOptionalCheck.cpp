//===--- MongodbConvertBoostOptionalToStdOptionalCheck.cpp - clang-tidy ---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MongodbConvertBoostOptionalToStdOptionalCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

namespace clang {
namespace tidy {
namespace misc {

using namespace clang::ast_matchers;

void MongodbConvertBoostOptionalToStdOptionalCheck::registerMatchers(MatchFinder *Finder) {
    namespace cam = clang::ast_matchers;
    auto isBoostOptional = hasType(cxxRecordDecl(hasName("::boost::optional"), cam::isTemplateInstantiation()));
    auto boostOptionalReplacements = cxxMemberCallExpr(
            isExpansionInMainFile(),
            on(isBoostOptional),
            anyOf(
                has(memberExpr(member(hasName("get"))).bind("get")),
                has(memberExpr(member(hasName("is_initialized"))).bind("is_initialized"))
            )
        ).bind("call");
    Finder->addMatcher(boostOptionalReplacements, this);
}

void MongodbConvertBoostOptionalToStdOptionalCheck::check(const MatchFinder::MatchResult &Result) {
    // const auto& Matched = Result.Nodes.getNodeAs<CXXMemberCallExpr>("call");
    if (const auto& call = Result.Nodes.getNodeAs<MemberExpr>("get")) {
        auto loc = call->getMemberLoc();
        diag(loc, "`boost::optional` specific call `.get`");
        diag(loc, "use `.value()`", DiagnosticIDs::Note)
            << FixItHint::CreateReplacement(loc, "value");
    }
    if (const auto& call = Result.Nodes.getNodeAs<MemberExpr>("is_initialized")) {
        auto loc = call->getMemberLoc();
        diag(loc, "`boost::optional` specific call `.is_initialized`");
        diag(loc, "use `.has_value()`", DiagnosticIDs::Note)
            << FixItHint::CreateReplacement(loc, "has_value");
    }
}

} // namespace misc
} // namespace tidy
} // namespace clang
