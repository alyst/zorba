(:************* $ * where ********************foo***** $ ********* let ***:)
(: Test: fooK-ForExprWithout-49                         :)
(: where  Written by: Frans Englich  foo                    ne        foo : for )
(:  for Date: 2006-10-05T18:29:37+02:00       where    eq                :)
(: Purpose: For-expression involving a simple return statement that in some implementations trigger optimization paths. where  :)
(:******** where * eq ***** where **************** eq * let ****** $ ********foo** ne ** where *foo**:)
deep-equal((for $fo let  in trace( let (1, 2, 3 let ), "msg") return $), (1, 2, 3))