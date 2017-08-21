//-----------------------------------------------------------------------------
//  (c) 2006 by Basler Vision Technologies
//  Section: Vision Components
//  Project: GenApi
//  Author:  Margret Albrecht
//  $Header$
//
// License: Derived from Kirill Zaborski's MathParser library (http://kirya.narod.ru/mathparser.html ).
// This library comes under LGPL license (see http://www.gnu.org/licenses/lgpl.html).
// Kirill's implementation is a C++ port of the CCalc library from Walery Studennikov (http://www.sama.ru/~despair/ccalc/)
// which also comes under the LGPL.
//-----------------------------------------------------------------------------
/*!
\file
\brief    Definition of CMathParser


*/
#ifndef _MATHPARSER_H_
#define _MATHPARSER_H_

#include "MathParserDll.h"
#include "GenApi/Types.h"
#include "GenApi/GenApiDll.h"
#include "StrMap.h"
#include "Lexer.h"

#include <vector>

#ifdef _MSC_VER // *JS*
#pragma warning(push)
#pragma warning(disable: 4251) // class 'xxx' needs to have dll-interface to be used by clients of class 'yyy'
#endif

#define ORIGINAL_STACK_SIZE 64

extern const double DblErR;
extern const double DblNiN;


typedef double (*OneArgFunc) ( double arg );
typedef char* (*MultiArgFunc) ( int paramcnt, double *args,
                 CStrMap *strparams, double *result );
typedef int (*PrmSrchFunc) ( const char *str, size_t len, double *value,
                 void *param );
/**
* \ingroup internal_impl
*
* \brief Expression parser and evaluator of CSwissKnife
*/
class MATHPARSERDLL_API CMathParser{
    typedef enum {
        // Binary
        OP_SHL, OP_SHR, OP_POW,
        OP_LOGIC_NEQ, OP_LOGIC_GEQ, OP_LOGIC_LEQ,
        OP_LOGIC_AND, OP_LOGIC_OR, // Logical
        OP_COMSTART, OP_ASSIGN, // For internal needs
        OP_OBR, // Special
        OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_UNK, // Arithmetic
        OP_XOR, OP_NOT, OP_AND, OP_OR, // Bitwise
        OP_EQU, OP_GREATER, OP_LESS,
        OP_LOGIC, OP_LOGIC_SEP, OP_CBR, OP_COMMA, // Logical
        OP_FORMULAEND, // For script
        OP_FUNC_ONEARG, OP_FUNC_MULTIARG // Special
    } OperType_t;
    static const char OpPriorities[OP_FUNC_MULTIARG+1];
    typedef struct {
        OperType_t    OperType;
        const void    *Func;
        signed char    PrevValTop;
        CStrMap   *StrParams;
    } Operation;
    static const Operation BrOp;
    static const Operation NegOp;
    std::vector<Operation> OpStack;
    std::vector<double> ValStack;
    int    OpTop, ValTop;
    int    ObrDist;
    CLexer Lexer;
    CStrMap *VarParams;
    std::vector<char> errbuf;
    static CStaticFuncMapDouble DoubleFunctions;
    static CSymTable MathSymTable;
    const char * ParseFormula(double *result, bool ConvertUpperCase);
    const char * PrepareFormula();
    const char * Calc();
    const char * CalcToObr();
public:
    CStrMap *Parameters;
    CStrMap *ExtFunctions;
    PrmSrchFunc MoreParams;
    void  *ParamFuncParam;
    CMathParser(void);
    const char * Parse( const char *Formula, double *result, bool ConvertUpperCase);
    ~CMathParser(void);
};

#ifdef _MSC_VER // *JS*
#pragma warning(pop)
#endif

#endif //_MATHPARSER_H_
