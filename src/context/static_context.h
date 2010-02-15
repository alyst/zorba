/*
 * Copyright 2006-2008 The FLWOR Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ZORBA_CONTEXT_STATIC_CONTEXT_H
#define ZORBA_CONTEXT_STATIC_CONTEXT_H

#include <memory>
#include <list>
#include <set>

#include <zorba/api_shared_types.h>

#ifdef WIN32
#include "store/api/item.h" 
#include "compiler/expression/expr_base.h"
#endif

#include "context/static_context_consts.h"
#include "context/decimal_format.h"
#include "context/internal_uri_resolvers.h"

#include "types/typemanager.h"

#include "zorbaerrors/Assert.h"

#include "zorbautils/hashmap_strh.h"

#define ZORBA_HASHMAP_WITH_SERIALIZATION
#include "util/hashmap.h"
#undef ZORBA_HASHMAP_WITH_SERIALIZATION

#include "common/shared_types.h"


namespace zorba 
{

class expr;
class var_expr;
class function;
class XQType;
class ExternalModule;
class ValueIndex;
class QueryLoc;
class namespace_node;
class user_function;
class TypeManager;
class XQPCollator;

template <class V> class serializable_ItemPointerHashMap;

class ValueIndex;
typedef rchandle<ValueIndex> ValueIndex_t;

class ValueIC;
typedef rchandle<ValueIC> ValueIC_t;

class StaticallyKnownCollection;
typedef rchandle<StaticallyKnownCollection> StaticallyKnownCollection_t;


template <class V> class ItemPointerHashMap;


/***************************************************************************//**
  XQuery 1.1 static context
	[http://www.w3.org/TR/xquery/#id-xq-context-components]

  Class static_context implements a node in the hierarchy of static contextes.
  The root of this hierarchy is a global (singleton) sctx that is shared by all
  queries. This global root sctx is created during engine statrtup and is
  initialized with the default values specified by the W3C spec. After its 
  creation, it is accessible via the singleton GlobalEnvironment obj (see
  GENV_ROOT_STATIC_CONTEXT macro in src/system/globalenv.h). We call this the
  "zorba root sctx".

  An application can create sctx objs using the Zorba::createStaticContext()
  API method. The application can then set various components of such an sctx
  obj using the API methods provided by the StaticContext class (which is a
  wrapper of the internal static_context class). All application-created sctx
  objs are created as children of the zorba root sctx.

  For a particular query, its root sctx may be (a) a child of an application
  created sctx obj, or (b) if the query is a load-prolog query, the application
  created sctx obj itself, or (c) if the app did not provide any sctx, a child
  of zorba's root sctx. In cases (a) and (b) the application provides its sctx
  to the query via the Zorba:compileQuery() or the XQuery::compile() API methods.
  The query root sctx is populated with the declarations found in the prolog of
  the main module and the declarations that are imported from the modules that
  the main module imports.

  Each library module participating in a query has its a root sctx as well,
  which is a child of the zorba root sctx, or the application provided sctx
  (if any). (This implies that an application provided sctx is "visible" to
  all the modules of a query). A module root sctx contains the declarations
  found in the prolog of the associated module and the declarations that are
  imported from other modules.

  Additional sctx objs may be created under the query or module root sctxs. Such
  expression-level sctx objs provide scoping contextes for variables, namespace
  bindings, etc. The both inherit and hide information from their ancestor sctxs.

  Expression-level sctx objs are created and maintained only during the 
  translation of a module; afterwards they are discarded. In contrast, query
  and module root sctx objs stay around for the whole life of a query (including
  runtime). Such sctxs are registered in XQueryImpl::theSctxMap.


  Data Members of Class static_context:


  theParent :
  -----------
 
  Pointer to the parent sctx object in the sctx hierarchy.

  theQueryExpr :
  --------------

  If this sctx is the root sctx for a mainModule, theQueryExpr is the result
  of the translation of that mainModule. It is used in implementing the
  loadProlog api.

  theModuleNamespace :
  --------------------

  If this is the root sctx of a library module, theModuleNamespace stores the
  target namespace URI of that module. 

  theDocResolver :
  ----------------

  URI resolver used for retrieving documents (used by fn:doc and fn:doc-available).

  theColResolver :
  ----------------

  URI resolver used for retrieving W3C collections (used by fn:collection).

  theSchemaResolvers :
  -------------------

  Vector of URI resolvers used for retrieving a schema given the schema's target
  namespace and an optional list of URLs that may store the schema.

  theModuleResolvers :
  -------------------

  Vector of URI resolvers used for retrieving (a) the URIs of the components of
  a module given the module's target namespece and then (b) a module component
  given its URI. Actually, the URI resolvers registered in this vector are used
  by the (singleton) StandardModuleURIResolver, which implements the full module
  resolution protocol (see ./standard_uri_resolvers.cpp). Normally, the URI
  resolvers registered here are user-provided ones.

  theModulePaths :
  ----------------

  Vector of absolute directory pathnames for directories that contain module 
  and/or schema files. The Zorba root sctx stores a number of predefined paths.
  applications may register additional paths in their own sctx objs. 
 
  theTypemgr :
  ------------

  If non NULL, then "this" is the root sctx of a module, and theTypemgr stores
  the schemas that are imported by the associated module (in-scope element
  declarations, in-scope attribute declarations and in-scope type declarations).

  theNamespaceBindings :
  ----------------------

  A hash map whose entries map qname prefixes to namespace URIs.

  theDefaultElementNamespace :
  ----------------------------
  
  The namespace URI to be used for element and type qnames whose prefix is empty.

  theDefaultFunctionNamespace :
  -----------------------------

  The namespace URI to be used for function qnames whose prefix is empty.

  theFunctionMap :
  ----------------

  An entry of this hash map maps an expanded qname to the function object with
  that qname. The qname of a function does not fully identify the function; the
  arity is needed together with the qname for a full id. But, most functions 
  have a single arity, so for better performance, such functions are registered
  in this map. Functions that have multiple arities, have one of their associated
  function objs registered here and the rest are stored in theFunctionArityMap.
 
  theFunctionArityMap :
  ---------------------

  An entry of this hash map maps an expanded qname to a vector of function
  objects with that qname. The function ojbects in this vector correspond to
  different versions of a function that all share the same qname but have
  different arities. One of these versions is stored in the theFunctionMap,
  and the rest are regisreded in theFunctionArityMap.

  theCollectionMap : 
  ------------------

  A hash mash map mapping XQDDF collection qnames to the objs storing the info
  from the declaration of the associated collections.

  theIndexMap :
  -------------

  A hash mash map mapping XQDDF index qnames to the objs storing the info from
  the declaration of the associated index.

  theICMap :
  ----------

  A hash mash map mapping XQDDF integrity constraint qnames to the objs storing
  the info from the declaration of the associated integrity constraint.

  theDecimalFormats :
  -------------------

  Set of decimal formats. Each decimal format is identified by a qname. 
  theDecimalFormats must not contain two decimal formats with the same qname.
  If an ancestor sctx contains a decimal format with the same qname as a
  decimal format in "this" sctx, then the decimal format of the ancestor is
  hidden by the decimal format in "this". The set of in-scope decimal formats
  is the union of all decimal foramts in "this" and its ancestors, minus the
  decimal foramts that are hidden by inner-scope decimal formats.

  theTraceStream :
  ----------------

  Output stream that is used by the fn:trace function. std::cerr is the default
  if the user didn't provide one. For serialization: see note above.


  Note: All URI resolvers have standard implementations in the classes
  Standard*URIResolver. Optionally, the user can provide resolvers which are
  wrapped by the *URIResolverWrapper classes.

  Note: URI resolvers are not serialized if the plan is serialized. Instead,
  they are set again if the query is loaded. If the user has provided a resolver
  before, he needs to make sure that the resolver is available using the
  SerializationCallback class. The latter also yields for ExternalModules and
  the TraceStream.

********************************************************************************/

class static_context : public SimpleRCObject
{
  typedef ItemPointerHashMap<StaticallyKnownCollection_t> CollectionMap;

  typedef ItemPointerHashMap<ValueIndex_t> IndexMap;

  typedef ItemPointerHashMap<ValueIC_t> ICMap;

  typedef ItemPointerHashMap<function_t> FunctionMap;

  typedef ItemPointerHashMap<std::vector<function_t>* > FunctionArityMap;

  typedef HashMapStrHandle<xqpStringStore_t> NamespaceBindings;

  typedef std::map<std::string, XQPCollator*> CollationMap;

public:

  enum ctx_value_type
  {
    CTX_EXPR, CTX_INT, CTX_BOOL, CTX_XQTYPE, CTX_MODULE
  };

  typedef xqp_string (* str_param_t)();


  struct ctx_value_t : public ::zorba::serialization::SerializeBaseClass
  {
    enum ctx_value_type type;

    union
    {
      expr             * exprValue;
      int                intValue;
      bool               boolValue;
		  const XQType     * typeValue; ///do manual ref counting on this
    };

  public:
    SERIALIZABLE_CLASS(ctx_value_t)
    SERIALIZABLE_CLASS_CONSTRUCTOR(ctx_value_t)
    void serialize(::zorba::serialization::Archiver& ar);

    ctx_value_t(enum ctx_value_type type = (enum ctx_value_type)-1)
    {
      this->type = type;
    }

    virtual ~ctx_value_t() {}
  };


  struct ctx_module_t : public ::zorba::serialization::SerializeBaseClass
  {
    ExternalModule * module;
    bool             dyn_loaded_module;

  public:
    SERIALIZABLE_CLASS(ctx_module_t)
    SERIALIZABLE_CLASS_CONSTRUCTOR(ctx_module_t)
    void serialize(::zorba::serialization::Archiver& ar);

    ctx_module_t() : module(0), dyn_loaded_module(false) {}

    virtual ~ctx_module_t() {}
  };

protected:
  static_context                       * theParent;

	serializable_hashmap<ctx_value_t>   keymap;     // maps strings to ctx_values
	serializable_hashmap<ctx_module_t>  modulemap;  // uris to external modules
	serializable_hashmap<xqp_string>    str_keymap; // maps strings to strings

  expr_t                                 theQueryExpr;

  std::string                            theModuleNamespace;

  InternalDocumentURIResolver          * theDocResolver;
  InternalCollectionURIResolver        * theColResolver;

  std::vector<InternalModuleURIResolver*> theModuleResolvers;
  std::vector<InternalSchemaURIResolver*> theSchemaResolvers;

  checked_vector<std::string>            theModulePaths;

  rchandle<TypeManager>                  theTypemgr;
  
  NamespaceBindings                    * theNamespaceBindings;
  xqpStringStore_t                       theDefaultElementNamespace;
  xqpStringStore_t                       theDefaultFunctionNamespace;

  FunctionMap                          * theFunctionMap;
  FunctionArityMap                     * theFunctionArityMap;

  CollectionMap                        * theCollectionMap;
  IndexMap                             * theIndexMap;
  ICMap                                * theICMap;

  CollationMap                         * theCollationMap;
  std::string                          * theDefaultCollation;

  StaticContextConsts::xquery_version_t      theXQueryVersion;

  StaticContextConsts::xpath_compatibility_t theXPathCompatibility;

  StaticContextConsts::construction_mode_t   theConstructionMode;

  StaticContextConsts::inherit_mode_t        theInheritMode;

  StaticContextConsts::preserve_mode_t       thePreserveMode;

  StaticContextConsts::ordering_mode_t       theOrderingMode;

  StaticContextConsts::empty_order_mode_t    theEmptyOrderMode;

  StaticContextConsts::boundary_space_mode_t theBoundarySpaceMode;

  StaticContextConsts::validation_mode_t     theValidationMode;

  std::vector<DecimalFormat_t>             * theDecimalFormats;

  std::ostream                       * theTraceStream;

public:
  SERIALIZABLE_CLASS(static_context);

  void serialize_resolvers(serialization::Archiver& ar);

  void serialize_tracestream(serialization::Archiver& ar);

  void serialize(serialization::Archiver& ar);

public:
  static_context(::zorba::serialization::Archiver& ar);

  ~static_context();

  static_context* create_child_context();

  bool is_global_root_sctx() const;

  expr_t get_query_expr() const;

  void set_query_expr(expr_t expr);

  static_context* get_parent() const { return theParent; }

  void set_module_namespace(std::string& ns) { theModuleNamespace = ns; }

  const std::string& get_module_namespace() const { return theModuleNamespace; }

  //
  // URI Resolution 
  //
  void set_document_uri_resolver(InternalDocumentURIResolver*);

  InternalDocumentURIResolver* get_document_uri_resolver();

  void set_collection_uri_resolver(InternalCollectionURIResolver*);

  InternalCollectionURIResolver* get_collection_uri_resolver();

  void add_schema_uri_resolver(InternalSchemaURIResolver*);

  void get_schema_uri_resolvers(std::vector<InternalSchemaURIResolver*>& resolvers);

  void remove_schema_uri_resolver(InternalSchemaURIResolver*);

  void add_module_uri_resolver(InternalModuleURIResolver*);

  void remove_module_uri_resolver(InternalModuleURIResolver*);

  void get_module_uri_resolvers(std::vector<InternalModuleURIResolver*>& resolvers) const;

  void set_module_paths(const std::vector<std::string>& aModulePaths);

  void get_module_paths(std::vector<std::string>& aModulePaths) const;

  void get_full_module_paths(std::vector<std::string>& aFullModulePaths) const;

  //
  // Type management
  //
  TypeManager* get_typemanager() const;

  TypeManager* get_local_typemanager() const;

  void set_typemanager(rchandle<TypeManager>);

  //
  // Namespace Bindings
  //
  const xqpStringStore_t& default_elem_type_ns() const;

  void set_default_elem_type_ns(const xqpStringStore_t& ns, const QueryLoc& loc);

  const xqpStringStore_t& default_function_ns() const;

  void set_default_function_ns(const xqpStringStore_t& ns, const QueryLoc& loc);

  void bind_ns(
        xqpStringStore_t& prefix,
        xqpStringStore_t& ns,
        const QueryLoc& loc,
        const XQUERY_ERROR& err = XQST0033);

  bool lookup_ns(
        xqpStringStore_t& ns,
        const xqpStringStore_t& prefix,
        const QueryLoc& loc,
        const XQUERY_ERROR& err = XPST0081) const;

  void expand_qname(
        store::Item_t& qname,
        const xqpStringStore_t& default_ns,
        const xqpStringStore_t& pfx,
        const xqpStringStore_t& local,
        const QueryLoc& loc) const;

  //
  // Functions
  //
  void bind_fn(
        const store::Item* qname,
        function_t& f,
        ulong arity,
        const QueryLoc& loc);

  void unbind_fn(const store::Item* qname, ulong arity);

  function* lookup_fn(const store::Item* qname, ulong arity);

  void find_functions(
        const store::Item* qname,
        std::vector<function*>& functions) const;

  // bind a module registered by the user
  // this module can be used to retrieve external functions defined
  // in the namespace of the module
  bool bind_external_module(
        ExternalModule* aModule,
        bool aDynamicallyLoaded = false);

  StatelessExternalFunction* lookup_stateless_external_function(
        const xqp_string& prefix,
        const xqp_string& local);

  //
  // XQDDF Collections
  //
  void bind_collection(StaticallyKnownCollection_t& aCollection, const QueryLoc& aLoc);

  const StaticallyKnownCollection* lookup_collection(const store::Item* aName) const;

  store::Iterator_t collection_names() const;

  //
  // XQDDF Indexes
  //
  void bind_index(const store::Item* qname, ValueIndex_t& vi, const QueryLoc& loc);

  ValueIndex* lookup_index(const store::Item* qname) const;

  store::Iterator_t index_names() const;


  //
  // XQDDF Integrity Constraints
  //
  void bind_ic(const store::Item* qname, ValueIC_t& vic, const QueryLoc& loc);

  ValueIC_t lookup_ic(const store::Item* qname) const;

  store::Iterator_t ic_names() const;


  //
  // Collations
  //
	void add_collation(const std::string& uri, const QueryLoc& loc);

  bool is_known_collation(const std::string& uri);

  XQPCollator* get_collator(const std::string& uri, const QueryLoc& loc);

  void set_default_collation(const std::string& uri, const QueryLoc& loc);

  const std::string& get_default_collation(const QueryLoc& loc) const;

  XQPCollator* get_default_collator(const QueryLoc& loc);


  //
  //  Misc
  //
  StaticContextConsts::xquery_version_t xquery_version() const;

  void set_xquery_version(StaticContextConsts::xquery_version_t v);

  void set_xquery_version(const std::string& v);

	StaticContextConsts::xpath_compatibility_t xpath_compatibility() const;

	void set_xpath_compatibility(StaticContextConsts::xpath_compatibility_t v);

	StaticContextConsts::construction_mode_t construction_mode() const;

	void set_construction_mode(StaticContextConsts::construction_mode_t v);

	StaticContextConsts::inherit_mode_t inherit_mode() const;

  void set_inherit_mode(StaticContextConsts::inherit_mode_t v);

	StaticContextConsts::preserve_mode_t preserve_mode() const;

  void set_preserve_mode(StaticContextConsts::preserve_mode_t v);

	StaticContextConsts::ordering_mode_t ordering_mode() const;

	void set_ordering_mode(StaticContextConsts::ordering_mode_t v);

	StaticContextConsts::empty_order_mode_t empty_order_mode() const;

	void set_empty_order_mode(StaticContextConsts::empty_order_mode_t v);

	StaticContextConsts::boundary_space_mode_t boundary_space_mode() const;

	void set_boundary_space_mode(StaticContextConsts::boundary_space_mode_t v);

  StaticContextConsts::validation_mode_t validation_mode() const;

  void set_validation_mode(StaticContextConsts::validation_mode_t v);

  void add_decimal_format(const DecimalFormat_t& format, const QueryLoc& loc);

  DecimalFormat_t get_decimal_format(const store::Item_t& qname);

  // Returns all the keys in the str_keymap hashtable, used by instrospection
  std::vector<xqp_string>* get_all_str_keys() const;

  // Returns all the keys in the keymap hashtable, used by instrospection
  std::vector<xqp_string>* get_all_keymap_keys() const;

  //
  // Normalized qname construction
  //
  static xqp_string
  qname_internal_key(const store::Item* qname);

  xqp_string
  qname_internal_key(xqp_string default_ns, xqp_string prefix, xqp_string local) const;

  xqp_string
  qname_internal_key(xqp_string default_ns, xqp_string qname) const;

  static std::pair<xqp_string /*local*/, xqp_string /*uri*/>
  decode_qname_internal_key (xqp_string key);


  //
  // Var QName --> Var Expr
  //
  bool
  bind_var(const store::Item* qname, expr* expr)
  {
    return bind_expr2 ("var:", qname_internal_key (qname), expr);
  }

  expr*
  lookup_var(xqp_string qname) const
  {
    return lookup_expr2("var:", qname_internal_key("", qname));
  }

  expr*
  lookup_var(xqp_string ns, xqp_string localname) const
  {
    return lookup_expr2("var:", qname_internal_key(ns, "", localname));
  }

  expr*
  lookup_var(const store::Item* qname)
  {
    return lookup_expr2("var:", qname_internal_key(qname));
  }

  expr*
  lookup_var_nofail (xqp_string qname) const
  {
    expr* e = lookup_var(qname);
    ZORBA_ASSERT (e != NULL);
    return e;
  }

  //
  // Type Management : Entity Name --> Type
  // where entity may be variable, the context item, function, document, collection
  //
  void
  bind_type( xqp_string key, xqtref_t t);

  xqtref_t
  lookup_type (xqp_string key);

  xqtref_t
  lookup_type2(const char* key1, xqp_string key2);

  xqtref_t
  lookup_type2_no_assert(const char* key1, xqp_string key2);

	void
  set_context_item_static_type(xqtref_t t);

	xqtref_t
	context_item_static_type();

	void
  set_document_type(xqp_string docURI, xqtref_t t);

  xqtref_t
  get_document_type(const xqp_string);

	void
  set_default_collection_type(xqtref_t t);

	xqtref_t
  default_collection_type();

	void
  set_collection_type(xqp_string collURI, xqtref_t t);

  xqtref_t
  get_collection_type(const xqp_string);


  //
  // Base uri
  //
  xqp_string
  final_baseuri ();

  xqp_string
  current_absolute_baseuri() const;

  void
  set_current_absolute_baseuri(xqp_string);

	void
  compute_current_absolute_baseuri();

  xqp_string
  baseuri() const;

  void
  set_baseuri(xqp_string, bool from_prolog = true);

  xqp_string
  encapsulating_entity_baseuri() const;

  void
  set_encapsulating_entity_baseuri(xqp_string);

  xqp_string
  entity_retrieval_url() const;

  void
  set_entity_retrieval_url(xqp_string);

  xqp_string
  implementation_baseuri () const { return "http://www.zorba-xquery.com/"; }

  // TODO: move this method to URI class in zorbatypes
  static xqp_string
  make_absolute_uri(xqp_string uri, xqp_string base_uri, bool validate = true);

  xqp_string resolve_relative_uri(
        xqp_string uri,
        xqp_string abs_base_uri,
        bool validate = true);

  //
  // Options
  //
  bool
  lookup_option(const xqp_string& prefix, const xqp_string& localname, xqp_string& option) const;

  bool
  bind_option(const xqp_string& prefix, const xqp_string& localname, const xqp_string& option);

  //
  // Merge in the static context of a module
  //
  bool import_module (const static_context* module, const QueryLoc& loc);

  //
  //
  //
  void set_trace_stream(std::ostream&);

  std::ostream* get_trace_stream() const;

  /**
  * @brief This method gets all variable names from this static context
  * and its parents.
  *
  * This method gets all variables in the scope of this static context.
  * Therefore it gets recursively all variables names from its parent,
  * adds them to aVariableList and then adds the variables saved in the
  * asked static context to aVariableList.
  *
  * @sa getVariables::getVariables(std::vector<var_expr_t>& aVarialeList)
  * @param aVariableList a vector of strings, where the variable names
  *  are put into.
  * @post aVariableList contains all variable names reachable from the
  *  scope from the current static context.
  */
  void getVariables(std::vector<std::string>& aVarialeList) const;

  /**
  * @brief This method gets all variable expressions from this static
  * context and its parents.
  *
  * This method gets all variables in the scope of this static context.
  * Therefore it gets recursively all variables expressions from its parent,
  * adds them to aVariableList and then adds the variables saved in the
  * asked static context to aVariableList.
  *
  * @sa getVariables::getVariables(std::vector<std::string>& aVarialeList)
  * @param aVariableList a vector of variable expressions, where the
  *  variable names are put into.
  * @post aVariableList contains all variable expressions reachable from the
  *  scope from the current static context.
  */
  void getVariables(std::vector<var_expr_t>& aVarialeList) const;

protected:
  static_context();

  static_context(static_context* parent);

  CollectionMap* collection_map() const { return theCollectionMap; }

  IndexMap* index_map() const { return theIndexMap; }

  ICMap* ic_map() const { return theICMap; }


  bool lookup_once(xqp_string key, xqp_string& val) const
  {
    return str_keymap.get(key, val);
  }

  bool lookup_once2(const char* key1, xqp_string key2, xqp_string& val) const
  {
    return str_keymap.get2(key1, key2, val);
  }

  bool lookup_once (xqp_string key, ctx_value_t &val) const
  {
    return keymap.get (key, val);
  }

  bool lookup_once2 (const char *key1, xqp_string key2, ctx_value_t& val) const
  {
    return keymap.get2 (key1, key2, val);
  }

	template<class V> bool context_value(xqp_string key, V &val) const; 

	template<class V> bool context_value2(const char *key1, xqp_string key2, V& val) const
	{
		if (lookup_once2 (key1, key2, val))
      return true;
    else
      return theParent == NULL ? false : theParent->context_value2 (key1, key2, val);
	}

  bool lookup_module(xqp_string key, ctx_module_t& val) const
  {
    if (modulemap.get (key, val))
      return true;
    else
      return theParent == NULL ? false : theParent->lookup_module (key, val);
  }

  expr* lookup_expr (xqp_string key) const
  {
    ctx_value_t val(CTX_EXPR);
    return context_value (key, val) ? val.exprValue : NULL;
  }

  expr* lookup_expr2 (const char *key1, xqp_string key2) const
  {
    ctx_value_t val(CTX_EXPR);
    return context_value2 (key1, key2, val) ? val.exprValue : NULL;
  }

  void bind_str(xqp_string key, xqp_string v, XQUERY_ERROR err = XQP0019_INTERNAL_ERROR);

  bool bind_expr (xqp_string key, expr *e);

  bool bind_expr2 (const char *key1, xqp_string key2, expr *e);

  bool bind_module(xqp_string uri, ExternalModule* m, bool dyn_loaded = false);

  //serialization helpers
  bool check_parent_is_root();

  void set_parent_as_root();
};


/* Debugging purposes */
std::ostream& operator<<(std::ostream& stream, const static_context::ctx_value_t& object);


}
#endif /*	ZORBA_CONTEXT_STATIC_CONTEXT_H */

/*
 * Local variables:
 * mode: c++
 * End:
 */
