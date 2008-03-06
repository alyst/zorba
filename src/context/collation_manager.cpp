
#include "collation_manager.h"
#include <sstream>
#include "errors/error_factory.h"

namespace zorba
{

	const CollationManager::COLLATION_DESCR		CollationManager::default_collations [] =
	{
		///URI, locale
    {"http://www.w3.org/2005/xpath-functions/collation/codepoint","root",::Collator::TERTIARY},
    {"http://flowrfound.ethz.ch/FLWOR1/collations/English", "en",::Collator::TERTIARY},
    {"http://flowrfound.ethz.ch/FLWOR1/collations/German", "de",::Collator::TERTIARY},
    {"http://flowrfound.ethz.ch/FLWOR1/collations/French", "fr",::Collator::TERTIARY},
    {"http://flowrfound.ethz.ch/FLWOR1/collations/Spanish", "es",::Collator::TERTIARY},
    {"http://flowrfound.ethz.ch/FLWOR1/collations/Swedish", "sv",::Collator::TERTIARY},
    {"http://flowrfound.ethz.ch/FLWOR1/collations/Turkish", "tr",::Collator::TERTIARY}
	};


CollationManager::~CollationManager()
{
	///free all collations
	std::map<std::string, COLL_ELEM*>::iterator		it;
	for(it = coll_map.begin();it != coll_map.end();it++)
	{
		//delete the value
		delete it->second->coll;
		delete it->second;
	}
}

std::string
CollationManager::computeKey(std::string  coll_string,
														::Collator::ECollationStrength coll_strength)
{
	std::ostringstream		oss;

	oss << coll_string << ":" << (int)coll_strength;
	return oss.str();
}

::Collator*
CollationManager::getCollation(std::string  coll_string,
															::Collator::ECollationStrength coll_strength)
{
	std::string		key;
	std::map<std::string, COLL_ELEM*>::iterator		it;

	key = computeKey(coll_string, coll_strength);
	it = coll_map.find(key);
	if(it != coll_map.end())
	{
		it->second->refcount++;
		return it->second->coll;
	}

	///if it does not exist, then create one

	//create the collator object
	UErrorCode status = U_ZERO_ERROR;
	COLL_ELEM	*coll_elem = new COLL_ELEM;

	coll_elem->refcount = 0;
	coll_elem->coll = NULL;

	//NOTE For default: By passing "root" as a locale parameter the root locale is used.
	//Root locale implements the UCA rules
	//(see DUCET from http://www.unicode.org/Public/UCA/5.0.0/allkeys.txt)
	coll_elem->coll = ::Collator::createInstance(Locale(coll_string.c_str()), status);

	if(U_FAILURE(status)) 
	{
		//ZORBA_ERROR_ALERT( ZorbaError::XQST0076, NULL);//only for FLWOR 
		delete coll_elem;
		return NULL;
	}

	//by default set level 1 comparison for the collator
	coll_elem->coll->setStrength(coll_strength);
	coll_elem->refcount++;
	
	coll_map[key] = coll_elem;
	return coll_elem->coll;
}

void
CollationManager::removeReference(std::string  coll_string,
																::Collator::ECollationStrength coll_strength)
{
	std::string		key;
	std::map<std::string, COLL_ELEM*>::iterator		it;

	key = computeKey(coll_string, coll_strength);
	it = coll_map.find(key);
	if(it == coll_map.end())
	{
		return;//it does not exist anyway
	}
	it->second->refcount--;
	if(it->second->refcount <= 0)
	{
		delete it->second->coll;
		delete it->second;
		coll_map.erase(it);
	}
}


const CollationManager::COLLATION_DESCR*
CollationManager::getHardcodedCollator(std::string URI)
{
	////blind search
	if(URI.empty())
		return NULL;
	for(unsigned int i=0;i<sizeof(default_collations)/sizeof(COLLATION_DESCR);i++)
		if(URI == default_collations[i].coll_uri)
			return &default_collations[i];
	return NULL;
}


}//end namespace zorba

