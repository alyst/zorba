jsoniq version "1.0";
import module namespace http="http://zorba.io/modules/http-client";

variable $result := http:post("http://zorbatest.lambda.nu:8080/cgi-bin/test-text", "poststring","text/plain");
$result.body.content

