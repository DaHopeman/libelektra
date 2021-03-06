/**
 * @file
 *
 * @brief This file contains the implementation of a basic YAML lexer.
 *
 * @copyright BSD License (see LICENSE.md or https://www.libelektra.org)
 */

// -- Imports ------------------------------------------------------------------------------------------------------------------------------

#include <stdexcept>

#include <kdblogger.h>

#include "lexer.hpp"

using std::ifstream;
using std::make_pair;
using std::runtime_error;
using std::string;
using std::unique_ptr;

using yawn::Location;
using yawn::Token;

// -- Functions ----------------------------------------------------------------------------------------------------------------------------

namespace
{

/**
 * @brief Create a token with the given attributes.
 *
 * @param type This number specifies the type of the created token.
 * @param location This number specifies the location of the token in the
 *                 scanned text.
 * @param text This variable specifies the content that should be stored
 *             in the token.
 *
 * @return A token storing the data provided as arguments to this function
 */
unique_ptr<Token> createToken (int const type, Location const & location, string const & text = "")
{
	return unique_ptr<Token>{ new Token{ type, location, text } };
}

} // namespace

// -- Class --------------------------------------------------------------------------------------------------------------------------------

namespace yawn
{

// ===========
// = Private =
// ===========

/**
 * @brief This method checks if the input at the specified offset starts a key
 *        value token.
 *
 * @param offset This parameter specifies an offset to the current position,
 *               where this function will look for a key value token.
 *
 * @retval true If the input matches a key value token
 * @retval false Otherwise
 */
bool Lexer::isValue (size_t const offset) const
{
	return (input.LA (offset) == ':') && (input.LA (offset + 1) == '\n' || input.LA (offset + 1) == ' ');
}

/**
 * @brief This method checks if the current input starts a list element.
 *
 * @retval true If the input matches a list element token
 * @retval false Otherwise
 */
bool Lexer::isElement () const
{
	return (input.LA (1) == '-') && (input.LA (2) == '\n' || input.LA (2) == ' ');
}

/**
 * @brief This method checks if the input at the specified offset starts a line
 *        comment.
 *
 * @param offset This parameter specifies an offset to the current position,
 *               where this function will look for a comment token.
 *
 * @retval true If the input matches a comment token
 * @retval false Otherwise
 */
bool Lexer::isComment (size_t const offset) const
{
	return (input.LA (offset) == '#') && (input.LA (offset + 1) == '\n' || input.LA (offset + 1) == ' ');
}

/**
 * @brief This method consumes characters from the input stream keeping
 *        track of line and column numbers.
 *
 * @param characters This parameter specifies the number of characters the
 *                   the function should consume.
 */
void Lexer::forward (size_t const characters = 1)
{
	ELEKTRA_LOG_DEBUG ("Forward %zu characters", characters);

	for (size_t charsLeft = characters; charsLeft > 0; charsLeft--)
	{
		if (input.LA (1) == 0)
		{
			ELEKTRA_LOG_DEBUG ("Hit EOF!");
			return;
		}

		location += 1;
		if (input.LA (1) == '\n')
		{
			location.end.column = 1;
			location.lines ();
		}
		input.consume ();
	}
}

/**
 * @brief This method adds block closing tokens to the token queue, if the
 *        indentation decreased.
 *
 * @param lineIndex This parameter specifies the column (indentation in number
 *                  of spaces) for which this method should add block end
 *                  tokens.
 */
void Lexer::addBlockEnd (size_t const lineIndex)
{
	while (lineIndex < levels.top ().indent)
	{
		ELEKTRA_LOG_DEBUG ("Add block end");
		tokens.push_back (levels.top ().type == Level::Type::MAP ? createToken (Token::MAP_END, location) :
									   createToken (Token::SEQUENCE_END, location));
		levels.pop ();
	}
}

/**
 * @brief This function adds an indentation value if the given value is smaller
 *        than the current indentation.
 *
 * @param lineIndex This parameter specifies the indentation value that this
 *                  function compares to the current indentation.
 *
 * @param type This value specifies the block collection type that
 *             `lineIndex` might start.
 *
 * @retval true If the function added an indentation value
 *         false Otherwise
 */
bool Lexer::addIndentation (size_t const lineIndex, Level::Type type)
{
	if (lineIndex > levels.top ().indent)
	{
		ELEKTRA_LOG_DEBUG ("Add indentation %zu", lineIndex);
		levels.push (Level{ lineIndex, type });
		return true;
	}
	return false;
}

/**
 * @brief This method saves a token for a simple key candidate located at the
 *        current input position.
 */
void Lexer::addSimpleKeyCandidate ()
{
	size_t position = tokens.size () + emitted.size ();
	simpleKey = make_pair (createToken (Token::KEY, location), position);
}

/**
 * @brief This method checks if the lexer needs to scan additional tokens.
 *
 * @retval true If the lexer should fetch additional tokens
 * @retval false Otherwise
 */
bool Lexer::needMoreTokens () const
{
	if (done)
	{
		return false;
	}

	bool keyCandidateExists = simpleKey.first != nullptr;
	return keyCandidateExists || tokens.empty ();
}

/**
 * @brief This method removes uninteresting characters from the input.
 */
void Lexer::scanToNextToken ()
{
	ELEKTRA_LOG_DEBUG ("Scan to next token");
	bool found = false;
	while (!found)
	{
		while (input.LA (1) == ' ')
		{
			forward ();
		}
		ELEKTRA_LOG_DEBUG ("Skipped whitespace");
		if (input.LA (1) == '\n')
		{
			forward ();
			ELEKTRA_LOG_DEBUG ("Skipped newline");
		}
		else
		{
			found = true;
			ELEKTRA_LOG_DEBUG ("Found next token");
		}
	}
}

/**
 * @brief This method adds new tokens to the token queue.
 */
void Lexer::fetchTokens ()
{
	scanToNextToken ();
	location.step ();
	addBlockEnd (location.begin.column);
	ELEKTRA_LOG_DEBUG ("Fetch new token at location: %zu:%zu", location.begin.line, location.begin.column);

	if (input.LA (1) == 0)
	{
		scanEnd ();
		return;
	}
	else if (isValue ())
	{
		scanValue ();
		return;
	}
	else if (isElement ())
	{
		scanElement ();
		return;
	}
	else if (input.LA (1) == '"')
	{
		scanDoubleQuotedScalar ();
		return;
	}
	else if (input.LA (1) == '\'')
	{
		scanSingleQuotedScalar ();
		return;
	}
	else if (input.LA (1) == '#')
	{
		scanComment ();
		return;
	}

	scanPlainScalar ();
}

/**
 * @brief This method adds the token for the start of the YAML stream to
 *        the token queue.
 */
void Lexer::scanStart ()
{
	ELEKTRA_LOG_DEBUG ("Scan start token");
	tokens.push_back (createToken (Token::STREAM_START, location));
}

/**
 * @brief This method adds the token for the end of the YAML stream to
 *        the token queue.
 */
void Lexer::scanEnd ()
{
	ELEKTRA_LOG_DEBUG ("Scan end token");
	addBlockEnd (0);
	tokens.push_back (createToken (Token::STREAM_END, location));
	tokens.push_back (createToken (-1, location));
	done = true;
}

/**
 * @brief This method scans a single quoted scalar and adds it to the token
 *        queue.
 */
void Lexer::scanSingleQuotedScalar ()
{
	ELEKTRA_LOG_DEBUG ("Scan single quoted scalar");

	size_t start = input.index ();
	// A single quoted scalar can start a simple key
	addSimpleKeyCandidate ();

	forward (); // Include initial single quote
	while (input.LA (1) != '\'' || input.LA (2) == '\'')
	{
		forward ();
	}
	forward (); // Include closing single quote
	tokens.push_back (createToken (Token::SINGLE_QUOTED_SCALAR, location, input.getText (start)));
}

/**
 * @brief This method scans a double quoted scalar and adds it to the token
 *        queue.
 */
void Lexer::scanDoubleQuotedScalar ()
{
	ELEKTRA_LOG_DEBUG ("Scan double quoted scalar");
	size_t start = input.index ();

	// A double quoted scalar can start a simple key
	addSimpleKeyCandidate ();

	forward (); // Include initial double quote
	while (input.LA (1) != '"')
	{
		forward ();
	}
	forward (); // Include closing double quote
	tokens.push_back (createToken (Token::DOUBLE_QUOTED_SCALAR, location, input.getText (start)));
}

/**
 * @brief This method scans a plain scalar and adds it to the token queue.
 */
void Lexer::scanPlainScalar ()
{
	ELEKTRA_LOG_DEBUG ("Scan plain scalar");
	// A plain scalar can start a simple key
	addSimpleKeyCandidate ();

	size_t lengthSpace = 0;
	size_t lengthNonSpace = 0;
	size_t start = input.index ();

	while (true)
	{
		lengthNonSpace = countPlainNonSpace (lengthSpace);
		if (lengthNonSpace == 0)
		{
			break;
		}
		forward (lengthSpace + lengthNonSpace);
		lengthSpace = countPlainSpace ();
	}

	tokens.push_back (createToken (Token::PLAIN_SCALAR, location, input.getText (start)));
}

/**
 * @brief This method counts the number of non space characters that can be part
 *        of a plain scalar at position `offset`.
 *
 * @param offset This parameter specifies an offset to the current input
 *               position, where this function searches for non-space
 *               characters.
 *
 * @return The number of non-space characters at the input position `offset`
 */
size_t Lexer::countPlainNonSpace (size_t const offset) const
{
	ELEKTRA_LOG_DEBUG ("Scan non space characters");
	string const stop = " \n";

	size_t lookahead = offset + 1;
	while (stop.find (input.LA (lookahead)) == string::npos && input.LA (lookahead) != 0 && !isValue (lookahead) &&
	       !isComment (lookahead))
	{
		lookahead++;
	}

	ELEKTRA_LOG_DEBUG ("Found %zu non-space characters", lookahead - offset - 1);
	return lookahead - offset - 1;
}

/**
 * @brief This method counts the number of space characters that can be part
 *        of a plain scalar at the current input position.
 *
 * @return The number of space characters at the current input position
 */
size_t Lexer::countPlainSpace () const
{
	ELEKTRA_LOG_DEBUG ("Scan spaces");
	size_t lookahead = 1;
	while (input.LA (lookahead) == ' ')
	{
		lookahead++;
	}
	ELEKTRA_LOG_DEBUG ("Found %zu space characters", lookahead - 1);
	return lookahead - 1;
}

/**
 * @brief This method scans a comment and adds it to the token queue.
 */
void Lexer::scanComment ()
{
	ELEKTRA_LOG_DEBUG ("Scan comment");
	size_t start = input.index ();
	while (input.LA (1) != '\n' && input.LA (1) != 0)
	{
		forward ();
	}
	tokens.push_back (createToken (Token::COMMENT, location, input.getText (start)));
}

/**
 * @brief This method scans a mapping value token and adds it to the token
 *        queue.
 */
void Lexer::scanValue ()
{
	ELEKTRA_LOG_DEBUG ("Scan value");
	forward (1);
	tokens.push_back (createToken (Token::VALUE, location, input.getText (input.index () - 1)));
	forward (1);
	if (simpleKey.first == nullptr)
	{
		throw runtime_error ("Unable to locate key for value");
	}
	size_t offset = simpleKey.second - emitted.size ();
	auto key = move (simpleKey.first);
	auto start = key->getLocation ().begin;
	tokens.insert (tokens.begin () + offset, move (key));
	simpleKey.first = nullptr; // Remove key candidate
	if (addIndentation (start.column, Level::Type::MAP))
	{
		location.begin = start;
		tokens.insert (tokens.begin () + offset, createToken (Token::MAP_START, location));
	}
}

/**
 * @brief This method scans a list element token and adds it to the token
 *        queue.
 */
void Lexer::scanElement ()
{
	ELEKTRA_LOG_DEBUG ("Scan element");
	if (addIndentation (location.end.column, Level::Type::SEQUENCE))
	{
		tokens.push_back (createToken (Token::SEQUENCE_START, location));
	}
	forward (1);
	tokens.push_back (createToken (Token::ELEMENT, location, input.getText (input.index () - 1)));
	forward (1);
}

// ==========
// = Public =
// ==========

/**
 * @brief This constructor initializes a lexer with the given input.
 *
 * @param stream This stream specifies the text which this lexer analyzes.
 */
Lexer::Lexer (ifstream & stream) : input{ stream }
{
	ELEKTRA_LOG_DEBUG ("Init lexer");

	scanStart ();
	fetchTokens ();
}

/**
 * @brief This method returns the next token produced by the lexer.
 *
 * If the lexer found the end of the input, then this function returns `-1`.
 *
 * @param attribute The parser uses this parameter to store auxiliary data for
 *                  the returned token.
 *
 * @return A number specifying the type of the first token the parser has not
 *         emitted yet
 */
int Lexer::nextToken (void ** attribute)
{
	while (needMoreTokens ())
	{
		fetchTokens ();
	}
#ifdef HAVE_LOGGER
	ELEKTRA_LOG_DEBUG ("Tokens:");
	for (auto const & token : tokens)
	{
		ELEKTRA_LOG_DEBUG ("\t%s\n", to_string (*token).c_str ());
	}
#endif

	if (tokens.size () <= 0)
	{
		tokens.push_front (createToken (-1, location));
	}

	emitted.push_back (move (tokens.front ()));
	tokens.pop_front ();

	*attribute = &emitted.back ();
	return emitted.back ()->getType ();
}

/**
 * @brief This method returns the current input of the lexer
 *
 * @return A UTF-8 encoded string version of the parser input
 */
string Lexer::getText ()
{
	return input.toString ();
}

} // namespace yawn
