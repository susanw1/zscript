lexer grammar ZscriptLexer;

TERM                : '\n' ;

RESPONSE            : '!' ;

AND                 : '&' ;
OR                  : '|' ;
LPAREN              : '(' ;
RPAREN              : ')' ;
PLUS                : '+' -> pushMode(BIGHEX);

LOCK                : '%' ;
ECHO                : '=' ;

fragment HEX        : [0-9a-f] ; 
fragment WSHEX      : WS* [0-9a-f] ; 
fragment ZERO       : WS* '0' ;
fragment HEXPAIR    : WSHEX WSHEX ;

KEY                 : [A-Z] ; 
VALUE               : ZERO* HEXPAIR? WSHEX? WSHEX ;
VALUEN              : ZERO* WSHEX* WSHEX ;

WS                  : [ \t\r] -> skip ;   

OPENQUOTE           : '"' -> pushMode(BIGSTRING) ;

fragment SEP        : AND | OR | LPAREN| RPAREN ;
fragment FIELD      : PLUS | OPENQUOTE | KEY ;

ADDR                : '@' -> pushMode(ADDRESSING); 

COMMENT             : '#' -> pushMode(COMMENTING); 


mode BIGSTRING;

CLOSEQUOTE          : '"' -> popMode ;
TEXTESC             : '=' -> pushMode(ESCAPING)  ;
TEXT                : ~[="\u000a]+ ;


mode ESCAPING;
ESCHEX2             : HEX HEX -> popMode;

 
mode BIGHEX;
BIGHEXVALUE         : (HEXPAIR)+ -> popMode;
BHWS                : [ \t\r] -> skip ;   

mode ADDRESSING;
ADDRCONT            : '.' ; 
AVALUE              : ZERO* HEXPAIR? WSHEX? WSHEX ;
ADDRTEXT            : WS* (SEP|FIELD|ADDR) ADDRCHAR+ ;
ADDRCHAR            : ~[\n] ;
ADDRTERM            : '\n' -> popMode ;


mode COMMENTING;
COMMTEXT            : ~[\u000a]+ ;
COMMTERM            : '\n' -> popMode ;
