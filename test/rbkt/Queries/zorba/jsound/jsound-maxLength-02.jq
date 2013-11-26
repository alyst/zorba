import module namespace jsd = "http://jsound.io/modules/jsound"; 

let $jsd :=
  {
    "$namespace" : "http://www.example.com/my-schema",
    "$types" : [
      {
        "$kind" : "array",
        "$name" : "foo",
        "$content" : [ "integer" ],
        "$maxLength" : 2
      }
    ]
  }

let $instance := [ 1, 2 ]

return jsd:jsd-validate( $jsd, "foo", $instance )

(: vim:set syntax=xquery et sw=2 ts=2: :)
