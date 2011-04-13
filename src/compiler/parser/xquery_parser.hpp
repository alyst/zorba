
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef PARSER_HEADER_H
# define PARSER_HEADER_H

/* "%code requires" blocks.  */

/* Line 35 of lalr1.cc  */
#line 35 "/home/markos/zorba/zorba/src/compiler/parser/xquery_parser.y"


#include <list>
#include <string>
#include <vector>

#include "compiler/parsetree/parsenodes.h"
#include "zorbatypes/zstring.h"

#ifdef __GNUC__
    // disable a warning in location.hh which comes with bison
    // position.hh:141: warning: suggest parentheses around && within ||
#   pragma GCC diagnostic ignored "-Wparentheses"
#endif

#include "location.hh"

#ifdef __GNUC__
#  pragma GCC diagnostic warning "-Wparentheses"
#endif

typedef std::list<zorba::zstring> string_list_t;
typedef std::pair<zorba::zstring,zorba::zstring> string_pair_t;







/* Line 35 of lalr1.cc  */
#line 76 "/home/markos/zorba/zorba/build-gdb/src/compiler/parser/xquery_parser.hpp"


#include <string>
#include <iostream>
#include "stack.hh"


/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace zorba {

/* Line 35 of lalr1.cc  */
#line 90 "/home/markos/zorba/zorba/build-gdb/src/compiler/parser/xquery_parser.hpp"
  class position;
  class location;

/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // zorba

/* Line 35 of lalr1.cc  */
#line 100 "/home/markos/zorba/zorba/build-gdb/src/compiler/parser/xquery_parser.hpp"

#include "location.hh"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
do {							\
  if (N)						\
    {							\
      (Current).begin = (Rhs)[1].begin;			\
      (Current).end   = (Rhs)[N].end;			\
    }							\
  else							\
    {							\
      (Current).begin = (Current).end = (Rhs)[0].end;	\
    }							\
} while (false)
#endif


/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

namespace zorba {

/* Line 35 of lalr1.cc  */
#line 148 "/home/markos/zorba/zorba/build-gdb/src/compiler/parser/xquery_parser.hpp"

  /// A Bison parser.
  class xquery_parser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
    {

/* Line 35 of lalr1.cc  */
#line 150 "/home/markos/zorba/zorba/src/compiler/parser/xquery_parser.y"

    zorba::parsenode *node;
    zorba::exprnode *expr;
    zorba::FunctionSig *fnsig;
    zorba::VarNameAndType *varnametype;
    off_t sval;
    const char *strval;
    xs_integer *ival;
    xs_double *dval;
    xs_decimal *decval;
    ZorbaParserError *err;
    string_list_t *strlist;
    string_pair_t *strpair;
    std::vector<string_pair_t> *vstrpair;
    CatchExpr::NameTestList *name_test_list;
    FTStopWordOption::incl_excl_list_t *incl_excl_list;
    FTSelection::pos_filter_list_t *pos_filter_list;
    FTThesaurusOption::thesaurus_id_list_t *thesaurus_id_list;
    ft_anyall_mode::type ft_anyall_value;



/* Line 35 of lalr1.cc  */
#line 184 "/home/markos/zorba/zorba/build-gdb/src/compiler/parser/xquery_parser.hpp"
    };
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     _EOF = 0,
     UNRECOGNIZED = 258,
     APOS_ATTR_CONTENT = 260,
     CHAR_LITERAL = 261,
     CHAR_LITERAL_AND_CDATA_END = 262,
     CHAR_LITERAL_AND_PI_END = 263,
     CHAR_REF_LITERAL = 264,
     ELEMENT_CONTENT = 265,
     ELEM_WILDCARD = 266,
     ENTITY_REF = 267,
     EXPR_COMMENT_LITERAL = 268,
     PI_NCNAME_LBRACE = 269,
     NCNAME_SVAL = 270,
     PRAGMA_LITERAL_AND_END_PRAGMA = 271,
     QNAME_SVAL_AND_END_PRAGMA = 272,
     EQNAME_SVAL_AND_END_PRAGMA = 273,
     PREFIX_WILDCARD = 274,
     COMP_ELEMENT_QNAME_LBRACE = 275,
     COMP_ATTRIBUTE_QNAME_LBRACE = 276,
     COMP_PI_NCNAME_LBRACE = 277,
     QNAME_SVAL = 278,
     EQNAME_SVAL = 279,
     ANNOTATION_QNAME_SVAL = 280,
     QUOTE_ATTR_CONTENT = 281,
     STRING_LITERAL = 282,
     XML_COMMENT_LITERAL = 283,
     DECLARE = 286,
     MODULE = 287,
     BLANK = 288,
     ALLOWING = 289,
     BASE_URI = 290,
     BOUNDARY_SPACE = 291,
     COMMENT = 292,
     CONSTRUCTION = 293,
     COPY_NAMESPACES = 294,
     COUNT = 295,
     DETERMINISTIC = 296,
     DOCUMENT = 297,
     DOCUMENT_NODE = 298,
     ELEMENT = 299,
     EVAL = 300,
     FOR = 301,
     FUNCTION = 302,
     IF = 303,
     IMPORT = 304,
     INSTANCE = 305,
     LET = 306,
     MOST = 307,
     NEXT = 308,
     NO = 309,
     NONDETERMINISTIC = 310,
     ONLY = 311,
     OPTION = 312,
     ORDERING = 313,
     PREVIOUS = 314,
     PROCESSING_INSTRUCTION = 315,
     SCHEMA = 316,
     SCHEMA_ATTRIBUTE = 317,
     SCHEMA_ELEMENT = 318,
     SEQUENTIAL = 319,
     SET = 320,
     SIMPLE = 321,
     SLIDING = 322,
     SOME = 323,
     SPACE = 324,
     STABLE = 325,
     TEXT = 326,
     TUMBLING = 327,
     SWITCH = 328,
     TYPESWITCH = 329,
     UPDATING = 330,
     VALIDATE = 331,
     WHEN = 332,
     WORD = 333,
     DECIMAL_FORMAT = 334,
     DECIMAL_SEPARATOR = 335,
     GROUPING_SEPARATOR = 336,
     INFINITY_VALUE = 337,
     MINUS_SIGN = 338,
     NaN = 339,
     PERCENT = 340,
     PER_MILLE = 341,
     ZERO_DIGIT = 342,
     DIGIT = 343,
     PATTERN_SEPARATOR = 344,
     ANCESTOR = 345,
     ANCESTOR_OR_SELF = 346,
     AND = 347,
     APOS = 348,
     AS = 349,
     ASCENDING = 350,
     AT = 351,
     ATTRIBUTE = 352,
     AT_SIGN = 353,
     CASE = 354,
     CASTABLE = 355,
     CAST = 356,
     CDATA_BEGIN = 357,
     CDATA_END = 358,
     CHILD = 359,
     COLLATION = 360,
     COMMA = 361,
     COMMENT_BEGIN = 362,
     COMMENT_END = 363,
     DECIMAL_LITERAL = 364,
     CONTEXT = 365,
     VARIABLE = 366,
     DEFAULT = 367,
     DESCENDANT = 368,
     DESCENDANT_OR_SELF = 369,
     DESCENDING = 370,
     DIV = 371,
     DOLLAR = 372,
     DOT = 373,
     DOT_DOT = 374,
     COLON = 375,
     DOUBLE_COLON = 376,
     DOUBLE_LBRACE = 377,
     DOUBLE_LITERAL = 378,
     DOUBLE_RBRACE = 379,
     ELSE = 380,
     _EMPTY = 381,
     GREATEST = 382,
     LEAST = 383,
     EMPTY_TAG_END = 384,
     ENCODING = 385,
     EQUALS = 386,
     ESCAPE_APOS = 387,
     ESCAPE_QUOTE = 388,
     EVERY = 389,
     EXCEPT = 390,
     EXTERNAL = 391,
     FOLLOWING = 392,
     FOLLOWING_SIBLING = 393,
     FOLLOWS = 394,
     GE = 395,
     GETS = 396,
     GT = 397,
     HOOK = 398,
     HASH = 399,
     IDIV = 400,
     _IN = 401,
     INHERIT = 402,
     INTEGER_LITERAL = 403,
     INTERSECT = 404,
     IS = 405,
     ITEM = 406,
     LBRACE = 407,
     LBRACK = 408,
     LE = 409,
     LPAR = 410,
     LT_OR_START_TAG = 411,
     MINUS = 412,
     MOD = 413,
     NAMESPACE = 414,
     _NAN = 415,
     NE = 416,
     NODECOMP = 417,
     NOT_OPERATOR_KEYWORD = 418,
     NO_INHERIT = 419,
     NO_PRESERVE = 420,
     OR = 421,
     ORDER = 422,
     ORDERED = 423,
     BY = 424,
     GROUP = 425,
     PARENT = 426,
     PERCENTAGE = 427,
     PI_BEGIN = 428,
     PI_END = 429,
     PLUS = 430,
     PRAGMA_BEGIN = 431,
     PRAGMA_END = 432,
     PRECEDES = 433,
     PRECEDING = 434,
     PRECEDING_SIBLING = 435,
     PRESERVE = 436,
     QUOTE = 437,
     RBRACE = 438,
     RBRACK = 439,
     RETURN = 440,
     RPAR = 441,
     SATISFIES = 442,
     SELF = 443,
     SEMI = 444,
     SLASH = 445,
     SLASH_SLASH = 446,
     STAR = 447,
     START_TAG_END = 448,
     STRIP = 449,
     TAG_END = 450,
     THEN = 451,
     TO = 452,
     TREAT = 453,
     UNION = 454,
     UNORDERED = 455,
     VAL_EQ = 456,
     VAL_GE = 457,
     VAL_GT = 458,
     VAL_LE = 459,
     VAL_LT = 460,
     VAL_NE = 461,
     VALUECOMP = 462,
     VBAR = 463,
     EMPTY_SEQUENCE = 464,
     WHERE = 465,
     XML_COMMENT_BEGIN = 466,
     XML_COMMENT_END = 467,
     XQUERY = 468,
     VERSION = 469,
     START = 470,
     AFTER = 471,
     BEFORE = 472,
     REVALIDATION = 473,
     _STRICT = 474,
     LAX = 475,
     SKIP = 476,
     _DELETE = 477,
     NODE = 478,
     INSERT = 479,
     NODES = 480,
     RENAME = 481,
     REPLACE = 482,
     VALUE = 483,
     OF = 484,
     FIRST = 485,
     INTO = 486,
     LAST = 487,
     MODIFY = 488,
     COPY = 489,
     WITH = 490,
     BLOCK = 491,
     BREAK = 492,
     CONTINUE = 493,
     EXIT = 494,
     LOOP = 495,
     RETURNING = 496,
     WHILE = 497,
     TRY = 498,
     CATCH = 499,
     USING = 500,
     ALL = 501,
     ANY = 502,
     CONTAINS = 503,
     CONTENT = 504,
     DIACRITICS = 505,
     DIFFERENT = 506,
     DISTANCE = 507,
     END = 508,
     ENTIRE = 509,
     EXACTLY = 510,
     FROM = 511,
     FTAND = 512,
     FTNOT = 513,
     NOT = 514,
     FT_OPTION = 515,
     FTOR = 516,
     INSENSITIVE = 517,
     LANGUAGE = 518,
     LEVELS = 519,
     LOWERCASE = 520,
     OCCURS = 521,
     PARAGRAPH = 522,
     PARAGRAPHS = 523,
     PHRASE = 524,
     RELATIONSHIP = 525,
     SAME = 526,
     SCORE = 527,
     SENSITIVE = 528,
     SENTENCE = 529,
     SENTENCES = 530,
     STEMMING = 531,
     STOP = 532,
     THESAURUS = 533,
     TIMES = 534,
     UPPERCASE = 535,
     WEIGHT = 536,
     WILDCARDS = 537,
     WINDOW = 538,
     WITHOUT = 539,
     WORDS = 540,
     COLLECTION = 541,
     CONSTOPT = 542,
     APPEND_ONLY = 543,
     QUEUE = 544,
     MUTABLE = 545,
     READ_ONLY = 546,
     UNIQUE = 547,
     NON = 548,
     INDEX = 549,
     MANUALLY = 550,
     AUTOMATICALLY = 551,
     MAINTAINED = 552,
     ON = 553,
     RANGE = 554,
     EQUALITY = 555,
     GENERAL = 556,
     INTEGRITY = 557,
     CONSTRAINT = 558,
     CHECK = 559,
     KEY = 560,
     FOREACH = 561,
     FOREIGN = 562,
     KEYS = 563,
     BYTE_ORDER_MARK_UTF8 = 564,
     RANGE_REDUCE = 565,
     ADDITIVE_REDUCE = 566,
     MULTIPLICATIVE_REDUCE = 567,
     UNION_REDUCE = 568,
     INTERSECT_EXCEPT_REDUCE = 569,
     QVARINDECLLIST_REDUCE = 570,
     COMMA_DOLLAR = 571,
     UNARY_PREC = 572,
     SEQUENCE_TYPE_REDUCE = 573,
     OCCURS_STAR = 574,
     OCCURS_PLUS = 575,
     OCCURS_HOOK = 576,
     STEP_REDUCE = 577
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    xquery_parser (xquery_driver& driver_yyarg);
    virtual ~xquery_parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

  private:
    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Generate an error message.
    /// \param state   the state where the error occurred.
    /// \param tok     the lookahead token.
    virtual std::string yysyntax_error_ (int yystate, int tok);

#if YYDEBUG
    /// \brief Report a symbol value on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_value_print_ (int yytype,
					 const semantic_type* yyvaluep,
					 const location_type* yylocationp);
    /// \brief Report a symbol on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_print_ (int yytype,
				   const semantic_type* yyvaluep,
				   const location_type* yylocationp);
#endif


    /// State numbers.
    typedef int state_type;
    /// State stack type.
    typedef stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef stack<location_type> location_stack_type;

    /// The state stack.
    state_stack_type yystate_stack_;
    /// The semantic value stack.
    semantic_stack_type yysemantic_stack_;
    /// The location stack.
    location_stack_type yylocation_stack_;

    /// Internal symbol numbers.
    typedef unsigned short int token_number_type;
    /* Tables.  */
    /// For a state, the index in \a yytable_ of its portion.
    static const short int yypact_[];
    static const short int yypact_ninf_;

    /// For a state, default rule to reduce.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned short int yydefact_[];

    static const short int yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const short int yytable_[];
    static const short int yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned short int yystos_[];

    /// For a rule, its LHS.
    static const unsigned short int yyr1_[];
    /// For a rule, its RHS length.
    static const unsigned char yyr2_[];

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif

#if YYERROR_VERBOSE
    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    virtual std::string yytnamerr_ (const char *n);
#endif

#if YYDEBUG
    /// A type to store symbol numbers and -1.
    typedef short int rhs_number_type;
    /// A `-1'-separated list of the rules' RHS.
    static const rhs_number_type yyrhs_[];
    /// For each rule, the index of the first RHS symbol in \a yyrhs_.
    static const unsigned short int yyprhs_[];
    /// For each rule, its source line number.
    static const unsigned short int yyrline_[];
    /// For each scanner token number, its symbol number.
    static const unsigned short int yytoken_number_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /* Debugging.  */
    int yydebug_;
    std::ostream* yycdebug_;
#endif

    /// Convert a scanner token number \a t to a symbol number.
    token_number_type yytranslate_ (int t);

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg        Why this token is reclaimed.
    /// \param yytype       The symbol type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    inline void yydestruct_ (const char* yymsg,
			     int yytype,
			     semantic_type* yyvaluep,
			     location_type* yylocationp);

    /// Pop \a n symbols the three stacks.
    inline void yypop_ (unsigned int n = 1);

    /* Constants.  */
    static const int yyeof_;
    /* LAST_ -- Last index in TABLE_.  */
    static const int yylast_;
    static const int yynnts_;
    static const int yyempty_;
    static const int yyfinal_;
    static const int yyterror_;
    static const int yyerrcode_;
    static const int yyntokens_;
    static const unsigned int yyuser_token_number_max_;
    static const token_number_type yyundef_token_;

    /* User arguments.  */
    xquery_driver& driver;
  };

/* Line 35 of lalr1.cc  */
#line 1 "[Bison:b4_percent_define_default]"

} // zorba

/* Line 35 of lalr1.cc  */
#line 690 "/home/markos/zorba/zorba/build-gdb/src/compiler/parser/xquery_parser.hpp"



#endif /* ! defined PARSER_HEADER_H */
