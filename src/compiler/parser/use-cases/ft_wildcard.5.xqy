for $book in doc("http://bstore1.example.com/full-text.xml")
   /books/book
let $cont := $book//content[. ftcontains "test.{3,4}" 
   with wildcards]
where count($cont)>0
return $book/@number, $cont
