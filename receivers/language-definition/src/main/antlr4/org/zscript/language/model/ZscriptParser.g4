parser grammar ZscriptParser;

options {
    tokenVocab = ZscriptLexer;
}

sequence
:
    (
        responseMarker? preamble? commandExpr TERM
    )*
    |
    (
        responseMarker? locks? addressExpr ADDRTERM
    )*
    |
    (
        commentExpr COMMTERM
    )*
;

responseMarker
:
    RESPONSE VALUE?
;

preamble
:
    locks echo?
    | echo locks?
;

locks
:
    LOCK VALUEN?
;

echo
:
    ECHO VALUE?
;

addressExpr
:
    ADDR address addressPayload
;

address
:
    AVALUE?
    (
        ADDRCONT AVALUE?
    )*
;

addressPayload
:
    ADDRTEXT
;

commentExpr
:
    COMMTEXT
;

commandExpr
:
    singleCommand
    | commandExpr AND commandExpr
    | commandExpr OR commandExpr
    | LPAREN commandExpr RPAREN
;

singleCommand
:
    (
        field
        | OPENQUOTE quoteField CLOSEQUOTE
        | bighexField
    )+
;

field
:
    KEY VALUE?
;

quoteField
:
    (
        TEXT
        | escapeText
    )*
;

escapeText
:
    (
        TEXTESC ESCHEX2
    )
;

bighexField
:
    PLUS BIGHEXVALUE
;
    
    