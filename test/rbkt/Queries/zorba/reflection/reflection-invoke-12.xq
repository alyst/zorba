import module namespace reflection = "http://www.zorba-xquery.com/modules/reflection";
import module namespace client = "http://pilman.ch/ns/blubb" at "client.xqlib";

reflection:invoke-sequential(xs:QName("client:tweets2")) 
