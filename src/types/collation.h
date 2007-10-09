/* -*- mode: c++; indent-tabs-mode: nil; tab-width: 2 -*-
 *
 *  $Id: collation.h,v 1.1 2006/10/09 07:07:59 Paul Pedersen Exp $
 *
 *	Copyright 2006-2007 FLWOR Foundation.
 *
 */

#ifndef XQP_COLLATION_H
#define XQP_COLLATION_H

#include <string>

namespace xqp {

class collation : public rcobject
{
protected:
	std::string name;

public:
	collation( std::string const& _name) : name(_name) { }
	collation() : name("") { }
	~collation() {}

public:
	std::string get_name() const { return name; }

};

}	/* namespace xqp*/
#endif	/* XQP_COLLATION_H */
