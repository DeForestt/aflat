#include "Parser/Parser.hpp"
#include "Parser/AST.hpp"
#include "Exceptions.hpp"

ast::Expr *prioritizeExpr(ast::Expr *expr);

parse::Parser::Parser(int mutability = 0) {
  this->typeList.foo = ast::Type::compair;
  this->mutability = mutability;

  // Int Type
  ast::Type Int = ast::Type();
  Int.typeName = "int";
  Int.opType = asmc::Hard;
  Int.size = asmc::DWord;

  // Sort Type
  ast::Type Short = ast::Type();
  Short.typeName = "short";
  Short.opType = asmc::Hard;
  Short.size = asmc::Word;

  // Char Type
  ast::Type Char = ast::Type();
  Char.typeName = "char";
  Char.opType = asmc::Hard;
  Char.size = asmc::Byte;

  // Long Type
  ast::Type Long = ast::Type();
  Long.typeName = "long";
  Long.opType = asmc::Hard;
  Long.size = asmc::QWord;

  // Pointer Type
  ast::Type Adr = ast::Type();
  Adr.typeName = "adr";
  Adr.opType = asmc::Hard;
  Adr.size = asmc::QWord;

  // Byte type
  ast::Type Byte = ast::Type();
  Byte.typeName = "byte";
  Byte.opType = asmc::Hard;
  Byte.size = asmc::Byte;

  // Float Type
  ast::Type Float = ast::Type();
  Float.typeName = "float";
  Float.opType = asmc::Float;
  Float.size = asmc::DWord;

  // Bool Type
  ast::Type Bool = ast::Type();
  Bool.typeName = "bool";
  Bool.opType = asmc::Hard;
  Bool.size = asmc::Byte;

  // Add Types to TypeList
  this->typeList << Int;
  this->typeList << Char;
  this->typeList << Byte;
  this->typeList << Adr;
  this->typeList << Float;
  this->typeList << Short;
  this->typeList << Long;
  this->typeList << Bool;
}

int getOpPriority(ast::Op op) {
  switch (op) {
  case ast::AndBit:
  case ast::OrBit:
  case ast::Great:
  case ast::Less:
    return 1;
  case ast::CompEqu:
  case ast::NotEqu:
  case ast::LessCmp:
  case ast::GreatCmp:
  case ast::Leq:
  case ast::Geq:
    return 2;
  case ast::Plus:
  case ast::Minus:
    return 3;
  case ast::Mul:
  case ast::Div:
    return 4;
  case ast::Mod:
    return 5;
  default:
    throw err::Exception("Unknown operator");
  }
};

/*
 * function name: Parser::parseStmt
 * description: Recursive function that parses a statement and returns the AST
 * as a Statment parameters: LinkedList<Token> &tokens - the list of tokens to
 * parse return: AST::Statement - the parsed statement
 */
ast::Statment *parse::Parser::parseStmt(links::LinkedList<lex::Token *> &tokens,
                                        bool singleStmt = false) {
  ast::Statment *output = new ast::Statment;
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
    bool mask = false;
    // the default access modifier is public
    ast::ScopeMod scope = ast::Public;
    tokens.pop();

    bool isMutable = true;
    if (this->mutability == 0) isMutable = true;
    else isMutable = false;

    if (obj.meta == "const") {
      isMutable = false;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
      throw err::Exception("Expected statent after public on line " +
                          std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "mutable") {
      if (mutability == 2)
        throw err::Exception("Cannot use a mutable variable in safe mode, please set config mutability to promiscuous or strict");
      isMutable = true;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
      throw err::Exception("Expected statent after public on line " +
                          std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    };

    // check if the next token is a scope modifier
    if (obj.meta == "public") {
      // set the scope modifier to public
      scope = ast::Public;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statent after public on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "private") {
      // set the scope modifier to private
      scope = ast::Private;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statent after private one line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "static") {
      // set the scope modifier to static
      scope = ast::Static;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statent after static on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "export"){
            // set the scope modifier to static
      scope = ast::Export;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statent after static on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    }

    // Declare a variable
    if (typeList[obj.meta] != nullptr) {
      ast::Declare *dec = new ast::Declare();

      // ensures the the current token is an Ident
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        dec->Ident = Ident.meta;
        dec->type = *this->typeList[obj.meta];
        dec->mask = mask;
        dec->scope = scope;
        dec->mut = isMutable;
        output = dec;
        ast::Op overload = ast::Op::None;
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
          lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
          std::string scopeName = "global";
          if (sym.Sym == '<') {
            tokens.pop();
            lex::OpSym *next = dynamic_cast<lex::OpSym *>(tokens.peek());
            if (next != nullptr){
              if (next->Sym == '>') {
                overload = ast::Op::Less;
              } else if (next->Sym == '<') {
                overload = ast::Op::Great;
              } else if (next->Sym == '+') {
                overload = ast::Op::Plus;
              } else if (next->Sym == '-') {
                overload = ast::Op::Minus;
              } else if (next->Sym == '*') {
                overload = ast::Op::Mul;
              } else if (next->Sym == '/') {
                overload = ast::Op::Div;
              } else if (next->Sym == '%') {
                overload = ast::Op::Mod;
              } else if (next->Sym == '!') {
                overload = ast::Op::NotEqu;
              } else if (next->Sym == '&') {
                overload = ast::Op::AndBit;
              } else if (next->Sym == '|') {
                overload = ast::Op::OrBit;
              } else if (next->Sym == '=') {
                overload = ast::Op::Equ;
              } else {
              throw err::Exception("Expected and overloade operator on line " +
                                   std::to_string(obj.lineCount));
              }
            } else {
              lex::Symbol *nextSym = dynamic_cast<lex::Symbol *>(tokens.peek());
              if (nextSym == nullptr)
                throw err::Exception("Expected and overloade operator on line " +
                                     std::to_string(obj.lineCount));
              if (nextSym->meta == "=="){
                overload = ast::Op::CompEqu;
              } else if (nextSym->meta == "!="){
                overload = ast::Op::NotEqu;
              } else if (nextSym->meta == ">="){
                overload = ast::Op::Geq;
              } else if (nextSym->meta == "<="){
                overload = ast::Op::Leq;
              } else if (nextSym->meta == ">"){
                overload = ast::Op::GreatCmp;
              } else if (nextSym->meta == "<"){
                overload = ast::Op::LessCmp;
              } else {
                throw err::Exception("Expected and overloade operator on line " +
                                     std::to_string(obj.lineCount));
              }
            }
            tokens.pop();
            next = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (next == nullptr)
              throw err::Exception("Expected a close oporator " +
                                   std::to_string(obj.lineCount));
            if (next->Sym != '>')
              throw err::Exception("Expected a close oporator  " +
                                   std::to_string(obj.lineCount) + +" got " +
                                   next->Sym);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
              throw err::Exception("Expected a symbol on line " +
                                   std::to_string(obj.lineCount));
            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
          }
          if (sym.Sym == '@') {
            tokens.pop();
            if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
              lex::LObj scopeObj = *dynamic_cast<lex::LObj *>(tokens.pop());
              scopeName = scopeObj.meta;
            } else
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  "Expected Scope Name");
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Can Only Scope a Function");
            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
            if (sym.Sym != '(')
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Can Only Scope a Function");
          }
          // Checking for Perenth to see if it is a function
          if (sym.Sym == '(') {
            tokens.pop();
            ast::Function *func = new ast::Function();
            func->ident.ident = dec->Ident;
            func->type = dec->type;
            func->scopeName = scopeName;
            func->mask = mask;
            func->scope = scope;
            func->op = overload;
            func->req = 0;
            func->args = this->parseArgs(tokens, ',', ')', func->argTypes, func->req);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
              sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
              
              // Check for a ':'
              if (sym.Sym == ':'){
                // make the next word a decorator.
                tokens.pop();
                lex::LObj * decor = dynamic_cast<lex::LObj *>(tokens.pop());
                if (decor == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + 
                "Expected Identifier after ':'");
                func->decorator = decor->meta;
                
                if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount));
                sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                if (sym.Sym == '.') {
                  tokens.pop();
                  lex::LObj * lob = dynamic_cast<lex::LObj *>(tokens.pop());
                  if (lob == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + 
                  "Expected Identifier after '.'");
                  func->decNSP = func->decorator;
                  func->decorator = lob->meta;
                  if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount));
                  sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                };
              }
              if (sym.Sym == '{') {
                tokens.pop();
                func->statment = this->parseStmt(tokens);
                output = func;
                delete (dec);
              } else {
                func->statment = nullptr;
                output = func;
                delete (dec);
              };
            } else
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Need terminating symble or open symble");
          } else if (sym.Sym == '=') {
            tokens.pop();
            ast::DecAssign *assign = new ast::DecAssign;
            assign->declare = dec;
            assign->mute = isMutable;
            assign->expr = this->parseExpr(tokens);
            output = assign;
          }
        }

      } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        lex::OpSym * sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        links::LinkedList<ast::Expr *> indices;
        if (sym->Sym == '['){
          while (sym != nullptr && sym->Sym == '['){
            tokens.pop();
            indices.push(this->parseExpr(tokens));
            lex::OpSym * closeBracket = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (closeBracket == nullptr || closeBracket->Sym != ']')
              throw err::Exception("Expected a close bracket on line " +
                                   std::to_string(obj.lineCount));
            sym = dynamic_cast<lex::OpSym *>(tokens.peek());
          }
          ast::DecArr * arr = new ast::DecArr;
          lex::LObj * ident = dynamic_cast<lex::LObj *>(tokens.pop());
          if (ident == nullptr)
            throw err::Exception("Expected a name on line " +
                                 std::to_string(obj.lineCount));
          arr->ident = ident->meta;
          if (this->typeList[obj.meta] == nullptr)
          throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                                 " Unknown type " + obj.meta);
          arr->type = *this->typeList[obj.meta];
          arr->indices = indices;
          arr->mut = isMutable;
          arr->scope = scope;
          output = arr;

          // Check for equal sign
          lex::OpSym * eq = dynamic_cast<lex::OpSym *>(tokens.peek());
          if (eq != nullptr && eq->Sym == '='){
            tokens.pop();
            ast::DecAssignArr * assign = new ast::DecAssignArr;
            assign->declare = arr;
            assign->expr = this->parseExpr(tokens);
            assign->mute = isMutable;
            output = assign;
          }
        } else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                                    " Expected a symbol or a declaration");
      } else {
        throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                             "Unparsable token found");
      }
    } else if (obj.meta == "return") {
      ast::Return *ret = new ast::Return;
      ret->expr = this->parseExpr(tokens);
      output = ret;
    } else if (obj.meta == "push") {
      ast::Push *push = new ast::Push;
      push->expr = this->parseExpr(tokens);
      output = push;
    } else if (obj.meta == "pull") {
      ast::Pull *pull = new ast::Pull;
      pull->expr = this->parseExpr(tokens);
      output = pull;
      if (singleStmt)
        return output;
    } else if (obj.meta == "if") {
      ast::If *ifstmt = new ast::If;
      ifstmt->elseIf = nullptr;
      ifstmt->elseStatment = nullptr;
      ifstmt->expr = this->parseExpr(tokens);
      lex::OpSym *sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr) {
        if (sym->Sym == '{') {
          tokens.pop();
          ifstmt->statment = this->parseStmt(tokens);
          output = ifstmt;
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unopened If");
      } else if (dynamic_cast<lex::LObj *>(tokens.peek())) {
        ifstmt->statment = this->parseStmt(tokens, true);
        output = ifstmt;
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened If");
      // check for else
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        if (obj.meta == "else") {
          tokens.pop();
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (sym->Sym == '{') {
              ifstmt->elseStatment = this->parseStmt(tokens);
              output = ifstmt;
            } else
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Unopened Else");
          } else if (dynamic_cast<lex::LObj *>(tokens.peek())) {
            ifstmt->elseStatment = this->parseStmt(tokens, true);
            output = ifstmt;
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened Else");
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unclosed if");
      }
      if (singleStmt)
        return ifstmt;
    } else if (obj.meta == "while") {
      ast::While *loop = new ast::While;

      loop->expr = this->parseExpr(tokens);
      lex::OpSym *sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr) {
        if (sym->Sym == '{') {
          tokens.pop();
          loop->stmt = this->parseStmt(tokens);
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unopened loop");
      } else
        loop->stmt = this->parseStmt(tokens, true);
      output = loop;
    } else if (obj.meta == "for") {
      ast::For *loop = new ast::For;
      loop->declare = this->parseStmt(tokens, true);

      lex::OpSym *sym = dynamic_cast<lex::OpSym *>(tokens.peek());

      if (sym == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unterminated forloop initializer");
      if (sym->Sym != ';')
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            "unterminated forloop initializer");

      tokens.pop();

      loop->expr = this->parseExpr(tokens);

      sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unterminated forloop condition");
      if (sym->Sym != ';')
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            "unterminated forloop condition");
      tokens.pop();

      loop->increment = this->parseStmt(tokens, true);

      sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr) {
        if (sym->Sym == ';') {
          tokens.pop();
        };
        sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym != nullptr) {
          if (sym->Sym == '{') {
            tokens.pop();
            loop->Run = this->parseStmt(tokens);
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened forloop body");
        }
      } else
        loop->Run = this->parseStmt(tokens, true);
      output = loop;
    } else if (obj.meta == "struct") {
      ast::UDeffType *stc = new ast::UDeffType();
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        lex::LObj ident = *dynamic_cast<lex::LObj *>(tokens.pop());
        stc->ident.ident = ident.meta;
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " struct needs Ident");
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        lex::OpSym op = *dynamic_cast<lex::OpSym *>(tokens.pop());
        if (op.Sym != '{')
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unopened UDeffType");
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened UDeffType");
      stc->statment = this->parseStmt(tokens);
      ast::Type t = ast::Type();
      t.size = asmc::QWord;
      t.typeName = stc->ident.ident;
      this->typeList << t;
      output = stc;
    } else if (obj.meta == "class") {
      ast::Class *item = new ast::Class();

      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        lex::LObj ident = *dynamic_cast<lex::LObj *>(tokens.pop());
        item->ident.ident = ident.meta;
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " class needs Ident");

      // check for the word signs
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        lex::LObj sig = *dynamic_cast<lex::LObj *>(tokens.pop());
        if (sig.meta == "signs") {
          lex::LObj *ident = dynamic_cast<lex::LObj *>(tokens.pop());
          if (ident != nullptr) {
            if (typeList[ident->meta] == nullptr)
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Type " + ident->meta + " not found");
            item->base = ident->meta;
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " signs needs Ident");
        }
      } else {
        item->base = "";
      };

      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        lex::OpSym op = *dynamic_cast<lex::OpSym *>(tokens.pop());
        if (op.Sym != '{')
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unopened UDeffType");
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened UDeffType");

      // check if there is a contract
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        ast::Type t = ast::Type();
        t.size = asmc::QWord;
        t.typeName = item->ident.ident;
        // Check if the class is in the typeList
        if (typeList[item->ident.ident] != nullptr)
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) + " Class " +
              item->ident.ident + " already exists");
        this->typeList << t;

        lex::LObj contract = *dynamic_cast<lex::LObj *>(tokens.peek());
        if (contract.meta == "contract") {
          tokens.pop();
          lex::OpSym *sy = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (sy == nullptr)
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened Contract");
          if (sy->Sym != '{')
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened Contract");
          item->contract = this->parseStmt(tokens);
        } else
          item->contract = nullptr;
      } else
        item->contract = nullptr;

      item->statment = this->parseStmt(tokens);
      output = item;
    } else if (obj.meta == "import"){
      ast::Import *imp = new ast::Import();

      lex::OpSym * sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr){
        if (sym->Sym == '{'){
          do {
            tokens.pop();
            lex::LObj *obj = dynamic_cast<lex::LObj *>(tokens.pop());
            if (obj != nullptr){
              imp->imports.push_back(obj->meta);
            } else {
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Expected Ident");
            }
          } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                   dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
              dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '}'){
            tokens.pop();
          } else {
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected }");
          }
        } else if (sym->Sym == '*'){
          tokens.pop();
          imp->imports.push_back("*");
        } else throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unexpected Token");
      } else {
          bool first = true;
          do {
            if (!first)
              tokens.pop();
            else first = false;

            lex::LObj *nt = dynamic_cast<lex::LObj *>(tokens.peek());
            if (nt != nullptr){
              imp->imports.push_back(nt->meta);
              ast::Type t = ast::Type();
              t.size = asmc::QWord;
              t.typeName = nt->meta;
              t.opType = asmc::Hard;
              this->typeList << t;
              tokens.pop();
            } else {
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Expected Ident");
            }
          } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                   dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
      }

      // check from from keyword
      lex::LObj *from = dynamic_cast<lex::LObj *>(tokens.pop());
      if (from == nullptr || from->meta != "from"){
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected from");
      };
      
      ast::StringLiteral * str = dynamic_cast<ast::StringLiteral *>(this->parseExpr(tokens));
      if (str != nullptr){
        imp->path = str->val;
      } else {
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected StringLiteral");
      }
      
      // check for under keyword
      lex::LObj * under = dynamic_cast<lex::LObj *>(tokens.peek());
      if (under != nullptr){
        if (under->meta == "under"){
          tokens.pop();
          lex::LObj * ident = dynamic_cast<lex::LObj *>(tokens.peek());
          if (ident != nullptr){
            imp->nameSpace = ident->meta;
            tokens.pop();
          } else {
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected Ident");
          }
        }
      } else {
        std::string id = imp->path.substr(imp->path.find_last_of('/') + 1, imp->path.find_last_of('.'));
        imp->nameSpace = id;
      }
      
      output = imp;
    } else if (obj.meta == "delete"){
      ast::Delete *del = new ast::Delete();
      lex::LObj * ident = dynamic_cast<lex::LObj *>(tokens.pop());
      if (ident == nullptr) throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected Ident");
      
      del->ident = ident->meta;
      links::LinkedList<std::string> modList;
      lex::OpSym * sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      while (sym != nullptr && sym->Sym == '.') {
        tokens.pop();
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
          lex::LObj mod = *dynamic_cast<lex::LObj *>(tokens.pop());
          modList << mod.meta;
        } else
          throw err::Exception("Expected, Ident after dot. on line " +
                                std::to_string(sym->lineCount));
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
          sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        } else
          throw err::Exception("expected assignment oporator got on line " +
                                std::to_string(sym->lineCount) + " " + sym->Sym);
      }
      
      del->modList = modList;
      output = del;
    } else if (obj.meta == "else") {
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " floating else, may have an extra semicolon before the else");
    } else {
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
        links::LinkedList<std::string> modList;
        while (sym.Sym == '.') {
          if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            lex::LObj mod = *dynamic_cast<lex::LObj *>(tokens.pop());
            modList << mod.meta;
          } else
            throw err::Exception("Expected, Ident after dot. on line " +
                                 std::to_string(sym.lineCount));
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
          } else
            throw err::Exception("expected assignment oporator got on line " +
                                 std::to_string(sym.lineCount) + " " + sym.Sym);
        }
        
        links::LinkedList<ast::Expr *> indecies;
        if (sym.Sym == '[') {
          indecies << this->parseExpr(tokens);
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
              dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
            tokens.pop();
          } else throw err::Exception("Expected ']'");

          while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '[') {
            tokens.pop();
            indecies << this->parseExpr(tokens);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
              tokens.pop();
            } else {
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Expected ]");
            }
          }
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
            sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
          } else {
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected some symbol after array");
          }
        }

        if (sym.Sym == '=') {
          ast::Assign *assign = new ast::Assign();
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            lex::OpSym *s2 = dynamic_cast<lex::OpSym *>(tokens.peek());
            if (s2->Sym == ':') {
              assign->refrence = true;
              tokens.pop();
            };
          };

          assign->Ident = obj.meta;
          assign->modList = modList;
          assign->expr = this->parseExpr(tokens);
          assign->indices = indecies;
          output = assign;
        } else if (sym.Sym == '(') {
          ast::Call *call = new ast::Call();
          call->ident = obj.meta;
          call->modList = modList;
          bool pop = false;
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
              dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')') {
            lex::OpSym *symp = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (symp->Sym != ')')
              throw err::Exception("Expected closed perenth got " + symp->Sym);
          } else {
            do {
              if (pop)
                tokens.pop();
              call->Args.push(this->parseExpr(tokens));
              pop = true;
            } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                     dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
              lex::OpSym *symp = dynamic_cast<lex::OpSym *>(tokens.pop());
              if (symp->Sym != ')')
                throw err::Exception("Expected closed perenth got " +
                                     symp->Sym);
            }
          }
          output = call;
        } else if (sym.Sym == '+') {
          lex::OpSym *s = dynamic_cast<lex::OpSym *>(tokens.peek());
          if (s != nullptr && s->Sym == '+') {
            tokens.pop();
            ast::Inc *inc = new ast::Inc();
            inc->ident = obj.meta;
            output = inc;
          } else
            throw err::Exception("Expected ++ on line " +
                                 std::to_string(sym.lineCount));
        } else if (sym.Sym == '-') {
          lex::OpSym *s = dynamic_cast<lex::OpSym *>(tokens.peek());
          if (s != nullptr && s->Sym == '-') {
            tokens.pop();
            ast::Dec *inc = new ast::Dec();
            inc->ident = obj.meta;
            output = inc;
          } else
            throw err::Exception("Expected -- on line " +
                                 std::to_string(sym.lineCount));
        } else
          throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                               " expected assignment oporator after " +
                               obj.meta);
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " expected Asignment oporator after " + obj.meta);
    }
  }

  if (singleStmt)
    return output;
  if (tokens.head == nullptr) {
    this->Output = *output;
    return output;
  } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &
             tokens.head->next != nullptr) {
    lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
    tokens.pop();

    if (obj.Sym == ';') {
      ast::Sequence *s = new ast::Sequence;
      s->Statment1 = output;
      s->Statment2 = this->parseStmt(tokens);
      this->Output = *s;
      return s;
    } else if (obj.Sym == '}') {
      this->Output = *output;
      return output;
    } else if (obj.Sym == '!') {
      this->Output = *output;
      return output;
    }
  } else {
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &
        tokens.head->next == nullptr) {
      lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
      tokens.pop();
      if (obj.Sym == '}') {
        this->Output = *output;
        return nullptr;
      } else if (obj.Sym == ';') {
        this->Output = *output;
        return output;
      }
    }
  }
  return output;
}

ast::Statment *parse::Parser::parseArgs(links::LinkedList<lex::Token *> &tokens,
                                        char delimn, char close,
                                        std::vector<ast::Type> &types, int &requiered) {
  ast::Statment *output = new ast::Statment();
  lex::OpSym * sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym == nullptr){
    requiered++;
  } else if (sym->Sym == '*') tokens.pop();
  
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
    bool isMutable;
    if (this->mutability == 0) isMutable = true;
    else isMutable = false;
    tokens.pop();
     if (obj.meta == "const") {
      isMutable = false;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
      throw err::Exception("Expected statent after public on line " +
                          std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "mutable") {
      if (mutability == 2)
        throw err::Exception("Cannot use a mutable variable in safe mode, please set config mutability to promiscuous or strict");
      isMutable = true;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
      throw err::Exception("Expected statent after public on line " +
                          std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    };

    if (typeList[obj.meta] != nullptr) {
      ast::Declare *dec = new ast::Declare();
      // ensures the the current token is an Ident
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        dec->Ident = Ident.meta;
        dec->type = *typeList[obj.meta];
        dec->mut = isMutable;
        output = dec;
        types.push_back(dec->type);
      }
    } else
      throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                           " expected type got " + obj.meta);
  }

  if (tokens.head == nullptr) {
    throw err::Exception("unterminated function call");
  } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &
             tokens.head->next != nullptr) {
    lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
    tokens.pop();

    if (obj.Sym == delimn) {
      ast::Sequence *s = new ast::Sequence;
      s->Statment1 = output;
      s->Statment2 = this->parseArgs(tokens, delimn, close, types, requiered);
      return s;
    } else if (obj.Sym == close) {
      return output;
    }
  }
  return output;
}

ast::ConditionalExpr *
parse::Parser::parseCondition(links::LinkedList<lex::Token *> &tokens) {

  ast::ConditionalExpr *output = new ast::ConditionalExpr();

  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
    if (sym.Sym != '(')
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           "\'" + sym.Sym + "\"" +
                           "unOpened Condition.  Please open with: (");
  } else
    throw err::Exception("Line: " + '\'' +
                         dynamic_cast<lex::LObj *>(tokens.peek())->meta + '\'' +
                         " unOpened Condition.  Please open with: (");

  output->expr1 = this->parseExpr(tokens);

  if (dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr) {
    lex::Symbol sym = *dynamic_cast<lex::Symbol *>(tokens.pop());
    if (sym.meta == "==") {
      output->op = ast::Equ;
    } else if (sym.meta == "!=") {
      output->op = ast::NotEqu;
    } else if (sym.meta == "<") {
      output->op = ast::Less;
    } else if (sym.meta == ">") {
      output->op = ast::Great;
    }
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Condition with now conditional Oporator");

  output->expr2 = this->parseExpr(tokens);

  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
    if (sym.Sym != ')')
      throw err::Exception(
          "Line: " + std::to_string(tokens.peek()->lineCount) +
          " unTerminated Condition.  Please terminate with: )");
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " unTerminated Condition.  Please terminate with: )");

  return output;
}

ast::Expr *parse::Parser::parseExpr(links::LinkedList<lex::Token *> &tokens) {
  ast::Expr *output = new ast::Expr();
  if (dynamic_cast<lex::StringObj *>(tokens.peek()) != nullptr) {
    lex::StringObj stringObj = *dynamic_cast<lex::StringObj *>(tokens.peek());
    tokens.pop();
    ast::StringLiteral *slit = new ast::StringLiteral();
    slit->val = stringObj.value;
    output = slit;
  } else if (dynamic_cast<lex::INT *>(tokens.peek()) != nullptr) {
    lex::INT intObj = *dynamic_cast<lex::INT *>(tokens.pop());
    ast::IntLiteral *ilit = new ast::IntLiteral();
    ilit->val = std::stoi(intObj.value);
    output = ilit;
  } else if (dynamic_cast<lex::FloatLit *>(tokens.peek()) != nullptr) {
    lex::FloatLit floatObj = *dynamic_cast<lex::FloatLit *>(tokens.pop());
    ast::FloatLiteral *flit = new ast::FloatLiteral();
    flit->val = floatObj.value;
    output = flit;
  } else if (dynamic_cast<lex::Long *>(tokens.peek()) != nullptr) {
    lex::Long intObj = *dynamic_cast<lex::Long *>(tokens.pop());
    ast::LongLiteral *ilit = new ast::LongLiteral();
    ilit->val = std::stoi(intObj.value);
    output = ilit;
  } else if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.pop());
    links::LinkedList<std::string> modList;
    links::LinkedList<ast::Expr *> indecies;

    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      while (sym.Sym == '.') {
        tokens.pop();
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
          lex::LObj mod = *dynamic_cast<lex::LObj *>(tokens.pop());
          modList << mod.meta;
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Expected, Ident after dot.");
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
          sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
        } else
          break;
      }

      if (sym.Sym == '[') {
          tokens.pop();
          indecies << this->parseExpr(tokens);
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
              dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
            tokens.pop();
          } else throw err::Exception("Expected ']'");
          while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
            dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '[') {
            tokens.pop();
            indecies << this->parseExpr(tokens);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
              tokens.pop();
            } else {
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Expected ]");
            }
          }
        }

    }
        
    ast::Var *var = new ast::Var();
    var->Ident = obj.meta;
    var->modList = modList;
    var->indecies = indecies;
    output = var;
    if (obj.meta == "new") {
      ast::NewExpr *newExpr = new ast::NewExpr();
      lex::LObj *typeName = dynamic_cast<lex::LObj *>(tokens.pop());
      if (typeName == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected, Ident after new.");
      ast::Type *nType = this->typeList[typeName->meta];
      if (nType == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) + ": " +
            typeName->meta + " is not a valid type.");
      newExpr->type = *nType;
      lex::OpSym *sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr && sym->Sym == '(') {
        tokens.pop();
        lex::OpSym *testSym = dynamic_cast<lex::OpSym *>(tokens.peek());
        bool pop = false;
        newExpr->args = links::LinkedList<ast::Expr *>();
        if (testSym != nullptr && testSym->Sym != '[') {
          lex::OpSym *symp = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (symp->Sym != ')')
            throw err::Exception("Expected closed perenth got " + symp->Sym);
        } else {
          do {
            if (pop)
              tokens.pop();
            newExpr->args.push(this->parseExpr(tokens));
            pop = true;
          } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                   dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');

          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            lex::OpSym *symp = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (symp->Sym != ')')
              throw err::Exception("Expected closed perenth got " + symp->Sym);
          }
          output = newExpr;
        }
      }
      output = newExpr;
    } else if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
      lex::LObj aobj = *dynamic_cast<lex::LObj *>(tokens.peek());
      if (aobj.meta == "as") {
        ast::DeRefence *deRef = new ast::DeRefence();
        tokens.pop();
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
          lex::LObj view = *dynamic_cast<lex::LObj *>(tokens.pop());
          deRef->Ident = obj.meta;
          deRef->modList = modList;
          if (this->typeList[view.meta] == nullptr)
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unknown Type " + view.meta);
          deRef->type = *this->typeList[view.meta];
          output = deRef;
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " No dereffrens type given with as");
      }
    } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym.Sym == '(') {
        tokens.pop();
        ast::Call *call = new ast::Call();
        call->ident = obj.meta;
        call->modList = modList;
        bool pop = false;
        lex::OpSym *testSym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (testSym != nullptr && testSym->Sym != '[') {
          lex::OpSym *symp = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (symp->Sym != ')')
            throw err::Exception("Expected closed perenth got " + symp->Sym);
        } else {
          do {
            if (pop)
              tokens.pop();
            call->Args.push(this->parseExpr(tokens));
            pop = true;
          } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                   dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');

          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            lex::OpSym *symp = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (symp->Sym != ')')
              throw err::Exception("Expected closed perenth got " + symp->Sym);
          }
        }

        ast::CallExpr *callExpr = new ast::CallExpr;
        callExpr->call = call;

        output = callExpr;
      } else {
        ast::Var *var = new ast::Var();
        var->Ident = obj.meta;
        var->indecies = indecies;
        var->modList = modList;
        output = var;
      }
    } else {
      ast::Var *var = new ast::Var();
      var->Ident = obj.meta;
      var->modList = modList;
      output = var;
    }
  } else if (dynamic_cast<lex::CharObj *>(tokens.peek()) != nullptr) {
    lex::CharObj obj = *dynamic_cast<lex::CharObj *>(tokens.pop());
    ast::CharLiteral *charlit = new ast::CharLiteral();
    charlit->value = obj.value;
    output = charlit;
  } else if (dynamic_cast<lex::Ref *>(tokens.peek()) != nullptr) {
    tokens.pop();
    ast::Refrence *ref = new ast::Refrence();
    if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
      lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.pop());
      ref->Ident = obj.meta;

      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        lex::OpSym *sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        while (sym->Sym == '.') {
          tokens.pop();
          if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.pop());
            ref->modList.push(obj.meta);
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected identifier after dot");
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = dynamic_cast<lex::OpSym *>(tokens.peek());
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected identifier after dot");
        }
      }

      output = ref;
    } else
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " No object given to refrece");
  } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    lex::OpSym eq = *dynamic_cast<lex::OpSym *>(tokens.peek());
    if (eq.Sym == '[') {
      tokens.pop();
      ast::Lambda *lambda = new ast::Lambda();
      lambda->function = new ast::Function();
      lambda->function->req = 0;
      lambda->function->args =
          this->parseArgs(tokens, ',', ']', lambda->function->argTypes, lambda->function->req);
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an > to start lambda not a symbol");
      if ((dynamic_cast<lex::OpSym *>(tokens.pop())->Sym != '='))
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym +
            " Need an > to start lambda");

      if (dynamic_cast<lex::Symbol *>(tokens.peek()) == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an > to start lambda not a symbol");
      if ((dynamic_cast<lex::Symbol *>(tokens.pop())->meta != ">"))
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym +
            " Need an > to start lambda");
      lex::OpSym *symp = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '{') {
        tokens.pop();
        lambda->function->statment = this->parseStmt(tokens);
      } else
        lambda->function->statment = this->parseStmt(tokens, true);

      ast::Type Adr = ast::Type();
      Adr.typeName = "any";
      Adr.opType = asmc::Hard;
      Adr.size = asmc::QWord;
      lambda->function->type = Adr;

      output = lambda;
    } else if (eq.Sym == '(') {
      tokens.pop();
      ast::ParenExpr *paren = new ast::ParenExpr();
      paren->expr = this->parseExpr(tokens);
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an ) to end parenth not a symbol");
      if ((dynamic_cast<lex::OpSym *>(tokens.pop())->Sym != ')'))
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym +
            " Need an ) to end parenth");
      output = paren;
    } else if (eq.Sym == '!') {
      tokens.pop();
      ast::Not *notExpr = new ast::Not();
      notExpr->expr = this->parseExpr(tokens);
      output = notExpr;
    } else if (eq.Sym == '{') {
      ast::StructList *structList = new ast::StructList();
      lex::OpSym *symp;
      do {
        tokens.pop();
        structList->args.push(this->parseExpr(tokens));
        symp = dynamic_cast<lex::OpSym *>(tokens.peek());
      } while (symp != nullptr && symp->Sym == ',');
      
      if (symp == nullptr || symp->Sym != '}')
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an } to end struct not a symbol");
      tokens.pop();
      output = structList;
    }
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unknown Expr");

  ast::Compound *compound;
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());

    compound = new ast::Compound();
    if (sym.Sym == '+') {
      tokens.pop();
      compound->op = ast::Plus;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '-') {
      tokens.pop();
      compound->op = ast::Minus;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '*') {
      tokens.pop();
      compound->op = ast::Mul;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '/') {
      tokens.pop();
      compound->op = ast::Div;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '%') {
      tokens.pop();
      compound->op = ast::Mod;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '&') {
      tokens.pop();
      compound->op = ast::AndBit;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '>') {
      tokens.pop();
      compound->op = ast::Great;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '<') {
      tokens.pop();
      compound->op = ast::Less;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.Sym == '|') {
      tokens.pop();
      compound->op = ast::OrBit;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    }
  } else if (dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr) {
    compound = new ast::Compound();
    lex::Symbol sym = *dynamic_cast<lex::Symbol *>(tokens.peek());
    if (sym.meta == "==") {
      tokens.pop();
      compound->op = ast::CompEqu;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.meta == "!=") {
      tokens.pop();
      compound->op = ast::NotEqu;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.meta == ">") {
      tokens.pop();
      compound->op = ast::GreatCmp;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.meta == "<") {
      tokens.pop();
      compound->op = ast::LessCmp;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.meta == ">=") {
      tokens.pop();
      compound->op = ast::Geq;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    } else if (sym.meta == "<=") {
      tokens.pop();
      compound->op = ast::Leq;
      compound->expr1 = output;
      compound->expr2 = this->parseExpr(tokens);
      return prioritizeExpr(compound);
    }
  }
  return output;
}

ast::Expr *prioritizeExpr(ast::Expr *expr) {
  ast::Compound *compound = dynamic_cast<ast::Compound *>(expr);

  ast::Compound *root = compound;

  if (compound != nullptr) {
    ast::Compound *right = dynamic_cast<ast::Compound *>(compound->expr2);
    int i = 0;
    while (right != nullptr) {
      if (getOpPriority(compound->op) >= getOpPriority(right->op)) {
        compound->expr2 = right->expr1;
        right->expr1 = compound;
        if (i == 0)
          root = right;
        compound = right;
        right = dynamic_cast<ast::Compound *>(compound->expr2);
      } else
        break;
      i++;
    }
  }
  return root;
};