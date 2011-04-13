import module namespace init = "http://www.zorba-xquery.com/modules/store/static-collections/initialization";
import module namespace manip = "http://www.zorba-xquery.com/modules/store/static-collections/manipulation";
import module namespace ns = "http://example.org/datamodule/" at "collections.xqdata";

declare %sequential function local:create() 
{
  init:create-collection(xs:QName("ns:test1"));
  manip:insert-nodes-first(xs:QName("ns:test1"), for $i in 1 to 10 return <a> { $i } </a>);
  exit returning (fn:count(manip:collection(xs:QName("ns:test1"))) eq 10);
};

declare %sequential function local:insert() 
{
  manip:insert-nodes-last(xs:QName("ns:test1"), for $i in 11 to 13 return <b> { $i } </b>);
  exit returning manip:collection(xs:QName("ns:test1"));
};


declare variable $x := 0;

set $x := local:create();

if (fn:not($x)) then
  fn:false()
else 
  local:insert();
