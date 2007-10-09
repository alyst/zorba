/**
 *
 * @copyright
 * ========================================================================
 *	Copyright 2007 FLWOR Foundation
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 * ========================================================================
 *
 * @author David Graf (david.graf@28msec.com)
 * @file types/casting.h
 *
 */

#ifndef XQP_CASTING_H
#define XQP_CASTING_H

#include "util/rchandle.h"
#include "types/sequence_type.h"

namespace xqp
{
	typedef rchandle<class Item> Item_t;
	
	/**
	 * Class which implements casting of items.
	 */
	class GenericCast
	{
		private:
			TypeCode targetType;
		public:
			GenericCast();
			~GenericCast() {}
			
			/**
			 * Sets the target types of the following casts
			 * @param targetType 
			 */
			void setTarget ( TypeCode targetType );
			
			/**
			 * Executes the casting of the passed item
			 * @param item 
			 * @return resutling item
			 */
			Item_t cast ( Item_t item );
	}; /* class GenericCast */

} /* namespace xqp */

#endif	/* XQP_CASTING_H */
