(:~
 : Simple test module for the paint functions of the image library.
 : 
 : @author Daniel Thomas
 :)
import module namespace basic = 'http://www.zorba-xquery.com/modules/image/basic';
import module namespace file = 'http://www.zorba-xquery.com/modules/file';
import module namespace paint = 'http://www.zorba-xquery.com/modules/image/paint';
import schema namespace image = 'http://www.zorba-xquery.com/modules/image/image';

declare variable $local:gif as xs:base64Binary := basic:create(xs:unsignedInt(100), xs:unsignedInt(100), image:imageType("GIF"));


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
 : @return true if the man:draw-rectangle function works.
 :)
declare function local:test-draw-rectangle() as xs:boolean {
    let $draw := paint:draw-rectangle($local:gif, 20, 20, 50, 50, (), (), (), false())
    let $draw-ref := file:read("images/paint/rectangle.gif")
    return($draw eq $draw-ref)
};

(:~
 : @return true if the man:draw-rectangle function works.
 :)
declare function local:test-draw-rectangle-green() as xs:boolean {
    let $draw := paint:draw-rectangle($local:gif, 20, 20, 50, 50, image:colorType("#00AF00"), (), (), false())
    let $draw-ref := file:read("images/paint/rectangleGreen.gif")
    return($draw eq $draw-ref)
};


(:~
 : @return true if the man:draw-rectangle function works.
 :)
declare function local:test-draw-rectangle-green-red() as xs:boolean {
    let $draw := paint:draw-rectangle($local:gif, 20, 20, 50, 50, image:colorType("#00AF00"), image:colorType("#A10000"), (), false())
    let $draw-ref := file:read("images/paint/rectangleGreenRed.gif")
    return($draw eq $draw-ref)
};

(:~
 : @return true if the man:draw-rectangle function works.
 :)
declare function local:test-draw-rectangle-green-red-wide() as xs:boolean {
    let $draw := paint:draw-rectangle($local:gif, 20, 20, 50, 50, image:colorType("#00AF00"), image:colorType("#A10000"), 5, false())
    let $draw-ref := file:read("images/paint/rectangleGreenRedWide.gif")
    return($draw eq $draw-ref)
};

(:~
 : @return true if the man:draw-rectangle function works.
 :)
declare function local:test-draw-rectangle-anti-aliased() as xs:boolean {
    let $draw := paint:draw-rectangle($local:gif, 20, 20, 50, 50, image:colorType("#00AF00"), image:colorType("#A10000"), 5, true())
    let $draw-ref := file:read("images/paint/rectangleAntiAliased.gif")
    return($draw eq $draw-ref)
};

(:~
 : @return true if the man:draw-rounded-rectangle function works.
 :)
declare function local:test-draw-rounded-rectangle() as xs:boolean {
    let $draw := paint:draw-rounded-rectangle($local:gif, 20, 20, 50, 50, 10, 10, (), (), (), ())
    let $draw-ref := file:read("images/paint/rectangleRounded.gif")
    return($draw eq $draw-ref)
};

(:~
 : @return true if the man:draw-rounded-rectangle function works.
 :)
declare function local:test-draw-rounded-rectangle-blue() as xs:boolean {
    let $draw := paint:draw-rounded-rectangle($local:gif, 20, 20, 50, 50, 10, 10, image:colorType("#0000FF"), (), (), ())
    let $draw-ref := file:read("images/paint/rectangleRoundedBlue.gif")
    return($draw eq $draw-ref)
};


(:~
 : @return true if the man:draw-rounded-rectangle function works.
 :)
declare function local:test-draw-rounded-rectangle-blue-green() as xs:boolean {
    let $draw := paint:draw-rounded-rectangle($local:gif, 20, 20, 50, 50, 10, 10, image:colorType("#0000FF"), image:colorType("#00FF00"), (), ())
    let $draw-ref := file:read("images/paint/rectangleRoundedBlueGreen.gif")
    return($draw eq $draw-ref)
};

(:~
 : @return true if the man:draw-rounded-rectangle function works.
 :)
declare sequential function local:test-draw-rounded-rectangle-wide() as xs:boolean {
    let $draw := paint:draw-rounded-rectangle($local:gif, 20, 20, 50, 50, 10, 10, image:colorType("#0000FF"), image:colorType("#00FF00"), 3, ())
    let $draw-ref := file:read("images/paint/rectangleRoundedWide.gif")
    return($draw eq $draw-ref)
};

(:~
 : @return true if the man:draw-rounded-rectangle function works.
 :)
declare function local:test-draw-rounded-rectangle-anti-aliased() as xs:boolean {
    let $draw := paint:draw-rounded-rectangle($local:gif, 20, 20, 50, 50, 10, 10, image:colorType("#0000FF"), image:colorType("#00FF00"), (), true())
    let $draw-ref := file:read("images/paint/rectangleRoundedAntiAliased.gif")
    return($draw eq $draw-ref)
};


declare sequential function local:main() as xs:string* {

  let $a := local:test-draw-rectangle()
  return
    if (fn:not($a)) then
      exit returning local:error(("Drawing a rectangle on an image failed."))
    else ();
    
  let $c := local:test-draw-rectangle-green()
  return
    if (fn:not($c)) then
      exit returning local:error(("Drawing a green rectangle on an image failed."))
    else ();    
  
  let $d := local:test-draw-rectangle-green-red-wide()
  return
    if (fn:not($d)) then
      exit returning local:error(("Drawing a wide green rectangle filled with red on an image failed."))
    else ();    
   
  let $e := local:test-draw-rectangle-anti-aliased()
  return
    if (fn:not($e)) then
      exit returning local:error(("Drawing a anti-aliased wide green rectangle filled with red on an image failed."))
    else ();    
   
    
  let $f := local:test-draw-rounded-rectangle()
  return
    if (fn:not($f)) then
      exit returning local:error(("Drawing a rounded rectangle on an image failed."))
    else ();    
  
      
  let $g := local:test-draw-rounded-rectangle-blue()
  return
    if (fn:not($g)) then
      exit returning local:error(("Drawing a blue rounded rectangle on an image failed."))
    else ();          
    
  let $h := local:test-draw-rounded-rectangle-blue-green()
  return
    if (fn:not($h)) then
      exit returning local:error(("Drawing a blue rounded rectangle filled with green on an image failed."))
    else ();               
           
  let $i := local:test-draw-rounded-rectangle-wide()
  return
    if (fn:not($i)) then
      exit returning local:error(("Drawing a blue rounded rectangle with wide stroke filled with green on an image failed."))
    else ();     
    
    
             
  let $j := local:test-draw-rounded-rectangle-anti-aliased()
  return
    if (fn:not($j)) then
      exit returning local:error(("Drawing a blue rounded rectangle anti-aliased filled with green on an image failed."))
    else ();               
           
              
           
  
    
  (: If all went well ... make sure the world knows! :)  
  "SUCCESS";

    


};

local:main();



