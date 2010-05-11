/* The Butterfly Effect
 * This file copyright (C) 2010  Klaas van Gend
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation
 * applicable version is GPL version 2 only.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef LOCALSTRING_H
#define LOCALSTRING_H

#include <QString>
#include <QDomNode>

/// this QString-on-steroids only copies a string it is in the right locale
class LocalString
{
public:
	LocalString();

	/** checks if string aValue is in the right locale
	  *  (we get that locale from the QLocale::system() )
	  *  we follow these rules:
	  *    1) if nothing set and we find a <title> without a language - let's use it
	  *    2) if we find a title with the corresponding language - let's use it instead of rule 1
	  *    3) if we find a title with a full corresponding "language_country", let's use that one instead of 1 or 2
	  *
	  *  Note: you'll probably use multiple calls to check() if you are using the SAX model
	  *  @param aValue   string to be copied if one of the above rules matches
	  *  @param aLangCode  the locale code - either empty, or "nl" or "nl_BE" format
	  */
	void check(const QString& aValue, const QString& aLangCode);

	/** read within the DomNode Dom to find all tags with a lang="lang"
	  * and find the string which fits the current locale best
	  * example:
	  *         <toolboxitem count="9" name="Bowling Pin">
	  *				<name lang="nl">Kegel</name>
	  *				<name lang="pt_BR">###</name>
	  *             <object type="Bowling Pin" />
	  *			</toolboxitem>
	  * will find "###" if lang is "pt_BR" and "Bowling Pin" if lang is "de_AT".
	  * see class LocalString for more explanation on the matching algorithms
	  *
	  *  Note: you'll probably use one call to fillFromDom() if you are using the DOM model
	  * @param myNode		the DOM Node that we're analysing (in the above example 'toolboxitem')
	  * @param aTagString	the tag to look for (in the above example 'name')
	  * @param aDefault		the value for the default locale (in the above example 'Bowling Pin')
	  *                     (if there is a <name> without a lang tag, that one
	  *						will be used as default, so you won't need to
	  *						supply it as a programmer)
	  */
	void	fillFromDOM(const QDomNode& myNode,
						const QString& aTagString, const QString& aDefault="");

	bool isEmpty(void)
	{ return theString.isEmpty(); }

	const QString & result() const
	{ return theString; }

private:
	/// constructor will initialize this with the 5-character language+country code.
	QString the5Char;
	/// contains the already detected language
	QString theLang;
	/// contains the current value - in theLang language
	QString theString;
};

#endif // LOCALSTRING_H
