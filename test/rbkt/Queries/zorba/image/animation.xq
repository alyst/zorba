(:~
 : Simple test module for the animation functions of the image library.
 : 
 : @author Daniel Thomas
 :)
import module namespace file = 'http://www.zorba-xquery.com/modules/file';
import module namespace ani = 'http://www.zorba-xquery.com/modules/image/animation';
import schema namespace image = 'http://www.zorba-xquery.com/modules/image/image';




(:~
 : Outputs a nice error message to the screen ...
 :
 : @param $messsage is the message to be displayed
 : @return The passed message but really very, very nicely formatted.
 :)
declare function local:error($messages as xs:string*) as xs:string* {
  "
************************************************************************
ERROR:
  Location:", file:path-to-full-path("."), "
  Cause:",
  $messages,
  "
************************************************************************
"
};
(:~
 : @return true if the ani:create-animated-gif function works.
 :)
declare function local:test-create-animated-gif() as xs:boolean {
    let $gif1 := file:read("images/bird.gif")
    let $gif2 := file:read("images/bird2.gif")
    let $animatedGif := ani:create-animated-gif(($gif1, $gif2), xs:unsignedInt(10), xs:unsignedInt(0))
    let $animatedRef := file:read("images/animation/simple.gif")
    return ($animatedGif eq $animatedRef)
};

(:~
 : @return true if the ani:create-morphed-gif function works.
 :)
declare function local:test-create-morphed-gif() as xs:boolean {
    let $gif1 := file:read("images/bird.gif")
    let $gif2 := file:read("images/bird2.gif")
    let $animatedGif := ani:create-morphed-gif(($gif1, $gif2), xs:unsignedInt(10), xs:unsignedInt(0), xs:unsignedInt(2  ))
    let $animatedRef := file:read("images/animation/morph.gif")
    return ($animatedGif eq $animatedRef)
};




declare sequential function local:main() as xs:string* {

  let $a := local:test-create-animated-gif()
  return
    if (fn:not($a)) then
      exit returning local:error(("Creating simple animated gif failed"))
    else ();
  
  let $b := local:test-create-morphed-gif()
  return
    if (fn:not($b)) then
      exit returning local:error(("Creating simple morphed gif failed"))
    else ();
  (: If all went well ... make sure the world knows! :)  
  "SUCCESS";

    


};

local:main();



