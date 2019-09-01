#ifndef ShuntYard_hpp
#define ShuntYard_hpp

#include <iostream>
#include <string>

#include <algorithm>
#include <iterator>

#include <vector>
#include <stack>
#include <queue>

#include <cstdlib>
#include <cmath>

#define CXX_BEGIN namespace cxx {
#define CXX_END }

CXX_BEGIN

enum token_t {
	EXP    = '^', MULT   = '*', DIV    = '/', PLUS   = '+', MINUS  = '-',
	P_OPEN = '(', P_CLOSE = ')',

	NUMBER = 0, BEGIN = 1, END = 2, UNKNOW = 3
};

enum precedence_t {
	N, GREATER, EQUAL, LESS
};

std::string number = {};
std::string expr   = {};

std::size_t len;
int index;


inline bool is_digit
(const char c) {
	return ('0' <= c) && (c <= '9');
}

inline bool is_whitespace
(const char c) {
	return (' ' == c) || (c == '\t') || (c == '\n') || ('\v' == c);
}


inline token_t get_token()
{
	while (index < len && is_whitespace(expr[index])) ++index;

	if (index >= len) return END;

	switch (expr[index++]) {
		case '('  : return P_OPEN  ;
		case ')'  : return P_CLOSE ;
		case '^'  : return EXP     ;
		case '*'  : return MULT    ;
		case '/'  : return DIV     ;
		case '+'  : return PLUS    ;
		case '-'  : return MINUS   ;
	}

	index -= 1;

	if (is_digit(expr[index]))
	{
		number.clear();

		do {
			number.push_back(expr[index]); ++index;
		}

		while (index != len && is_digit(expr[index]));
		return NUMBER;
	}

	++index; return UNKNOW;
}

const char* to_str(const token_t token)
{
	switch (token) {
		case P_OPEN   : return "("       ;
		case P_CLOSE  : return ")"       ;
		case EXP      : return "EXP"     ;
		case MULT     : return "MULT"    ;
		case DIV      : return "DIV"     ;
		case PLUS     : return "PLUS"    ;
		case MINUS    : return "MINUS"   ;

		case NUMBER   : return "NUMBER"  ;
		case UNKNOW   : return "UNKNOW"  ;
		case BEGIN    : return "BEGIN"   ;
		case END      : return "END"     ;
	}

	return "?(token_t)";
}

const char* to_str(precedence_t precedence) {
	switch (precedence) {
	case N        : return "N"       ;
	case GREATER  : return "GREATER" ;
	case EQUAL    : return "EQUAL"   ;
	case LESS     : return "LESS"    ;
	}

	return "?(precedence_t)";
}

void ostream(std::string expr) {
	int index = std::exchange(::cxx::index, 0);

	len = expr.length();
	::cxx::expr = std::move(std::exchange(expr, {}));

	token_t token = { BEGIN };

	while (token != END) {
		token = get_token();

		std::cout << to_str(token) << ' ';

		if (token == NUMBER) {
			std::cout << "[" << number << "] ";
		}
	}

	::cxx::index = index;
}

std::string to_str_expr(const token_t token) {
	switch (token) {
		case P_OPEN   : return "(" ;
		case P_CLOSE  : return ")" ;
		case EXP      : return "^" ;
		case MULT     : return "*" ;
		case DIV      : return "/" ;
		case PLUS     : return "+" ;
		case MINUS    : return "-" ;
	}

	return "";
}

inline bool match(const token_t lhd, const token_t rhd) {
	return (lhd == rhd);
}

template <std::size_t N>
inline bool match(const token_t lhd, token_t const(&rhd)[N]) {
	for (std::size_t i = 0; i < N; ++i) {
		if (match(lhd, rhd[i])) return true;
	}

	return false;
}

inline bool E_arithm(const token_t token) {
	return match(token, {EXP, MULT, DIV, PLUS, MINUS});
}

inline bool E_number(const char c) { return is_digit(c); }

precedence_t get_precedence(token_t lhd, token_t rhd)
{
	precedence_t precedence = N;

	switch (lhd)
	{
		case EXP    :
			switch (rhd)
			{
			case EXP    :
				precedence = EQUAL;
			break;

			case MULT: case DIV: case PLUS: case MINUS:
				precedence = GREATER;
			break;
			}
		break;

		case MULT   :
		case DIV    :
			switch (rhd)
			{
			case EXP:
				precedence = LESS;
				break;

			case MULT: case DIV:
				precedence = EQUAL;
				break;

			case PLUS: case MINUS:
				precedence = GREATER;
				break;
			}
		break;

		case PLUS   :
		case MINUS  :
			switch (rhd)
			{
			case EXP: case MULT: case DIV:
				precedence = LESS;
				break;

			case PLUS: case MINUS:
				precedence = EQUAL;
				break;
			}
		break;
	}

	return precedence;
}

#define BEGIN_ELSE {
#define END_ELSE }

#define BEGIN_SWITCH {
#define END_SWITCH }

#define Case case

void compute(std::string &&expr)
{
	index = 0;
	len = expr.length();

	if (!len) { return; }

	::cxx::expr = std::move(std::exchange(expr, {}));

	token_t token = { get_token() };

	std::stack <std::string> operators;
	std::vector <std::string> operands;

	while (index <= len && token != END)
	{
		switch (token)
		{
			case NUMBER   : operands.emplace_back(std::move(number));
			break;

			case P_OPEN   : operators.emplace(std::move(to_str_expr(token)));
			break;

			case P_CLOSE  :
				while (!operators.empty() && operators.top() != "(") {
					operands.emplace_back(std::move(operators.top()));
					operators.pop();
				}

				if (operators.empty()) {
					std::cerr << "\tsyntax error: missing '('\n";
					exit(EXIT_FAILURE);
				}

				else
					operators.pop();
			break;

			default:
				if (E_arithm(token) && (operators.empty() || operators.top() == "(")) {
					operators.emplace(std::move(to_str_expr(token)));
				}

				else

				BEGIN_ELSE
				precedence_t precedence = get_precedence(token, (token_t) operators.top()[0]);


				switch (token)

				BEGIN_SWITCH
					case EXP:
						if (precedence == EQUAL || precedence == GREATER) {
							operators.emplace(std::move(to_str_expr(token)));
						}

						else {
							operands.emplace_back(std::move(to_str_expr(token)));
						}
					break;

					case MULT: case DIV: case PLUS: case MINUS:
						if (precedence == GREATER) {
							operators.emplace(std::move(to_str_expr(token)));
						}

						else

						if (precedence == EQUAL || precedence == LESS)
						{
							do {
								if (!operators.empty()) {
									operands.emplace_back(std::move(operators.top()));

									precedence = get_precedence(token, (token_t)operators.top()[0]);
									operators.pop();
								}
							}

							while ((precedence == EQUAL) || (precedence == LESS));

							operators.emplace(std::move(to_str_expr(token)));
						}
					break;
				END_SWITCH

				END_ELSE
		}

		token = get_token();
	}

	while (!operators.empty()) {
		operands.emplace_back(std::move(operators.top()));

		operators.pop();
	}

	//

	std::stack <float> VAL;

	for (const auto& element : operands)
	{
		if (E_number(element[0])) {
			VAL.push(std::stof(element));
		}

		else {
			float N1 = VAL.top(); VAL.pop();
			float N2 = VAL.top(); VAL.pop();

			switch (element[0]) {
				case '+': VAL.push(N2 + N1); break;
				case '-': VAL.push(N2 - N1); break;
				case '*': VAL.push(N2 * N1); break;
				case '/': VAL.push(N2 / N1); break;

				case '^':
					VAL.push(std::pow(N2, N1));
											 break;

				default : std::cout << "\tsyntax error: token'" << element[0] << "'\n";
			}
		}
	}

	std::cout << ": " << VAL.top() << "\n\n";
}

CXX_END

#endif
