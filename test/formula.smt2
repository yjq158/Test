; 
(set-info :status unknown)
(declare-fun X0 () Int)
(assert
 (= (< 0 X0) true))
(assert
 (= (< (+ 0 1) X0) true))
(assert
 (let ((?x16 (+ 0 1)))
 (let ((?x21 (+ ?x16 1)))
 (let (($x23 (< ?x21 X0)))
 (= $x23 false)))))
(assert
 (let ((?x18 (* 1 2)))
 (let ((?x22 (* ?x18 2)))
 (let ((?x27 (mod ?x22 8)))
 (let (($x28 (= ?x27 0)))
 (= $x28 false))))))
(check-sat)
